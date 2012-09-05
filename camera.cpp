#include "camera.h"

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <assert.h>
#include <linux/videodev2.h>

#include <iostream>

struct CameraPriv {
	CameraPriv(const char *dev = NULL);
	~CameraPriv();

	bool open(int flag = 0);
	bool close(void);
	bool init(void);
	bool isOpen(void) { return eStatus >= OPEN; }

	int allocBuffers(unsigned int cnt);
	void freeBuffers(void);

	bool qbuf(unsigned char *ptr);
	bool qbuf(unsigned int idx);
	unsigned char *dqbuf(void);

	bool streamon(void);
	bool streamoff(void);

	bool setInput(const int *i);
	bool getInput(int *i);

	int enumerateImageFormats(std::vector<struct v4l2_fmtdesc> *vecFmt);
	int enumerateInputs(std::vector<struct v4l2_input> *vecInput);
	int enumerateFrameSizes(unsigned int pixelformat, std::vector<struct v4l2_frmsizeenum> *pvecFrmsize);

	bool getImageFormat(struct v4l2_pix_format *pix);
	bool setImageFormat(const struct v4l2_pix_format *pix);

	bool queryCapabilities(void);
	bool getCapabilities(struct v4l2_capability *vcap);

	enum { MAX_BUFS = 5, };
	enum { CLOSE = 0, OPEN = 1, ALLOCBUF = 2, STREAMON = 3 };

	const char			*sDevName;
	int					iFd;
	unsigned int		iBufTotal;
	struct v4l2_capability mCap;
	std::vector<struct v4l2_buffer> vecV4l2Buf;
	std::vector<unsigned char *>	vecUserBuf;

    struct v4l2_format	mFmt;

	std::vector<struct v4l2_fmtdesc> vecFmtdesc;
	std::vector<struct v4l2_input> vecInput;
	std::vector<struct v4l2_frmsizeenum> frmsizeVector_;
	unsigned int iCurInput;

	/* status */
	int eStatus;
};

CameraPriv::CameraPriv(const char *dev)
	: sDevName(dev), iFd(-1), eStatus(0)
{

}
CameraPriv::~CameraPriv()
{
	this->close();
}

bool CameraPriv::open(int flag)
{
	if (iFd >= 0) {
		std::cerr << "Warning: " << sDevName << " had been open" << std::endl;
		return false;
	}
	iFd = ::open(sDevName, O_RDWR | O_CLOEXEC | flag);
	if (iFd < 0) {
		std::cerr << "Error: " << sDevName << " failed: " << strerror(errno) << std::endl;
		return false;
	}
	eStatus = OPEN;
	//enumerateInput();
	//enumerateImageFormat();
	return true;
}

bool CameraPriv::close()
{
	switch (eStatus) {
	case STREAMON:
		streamoff();
	case ALLOCBUF:
		freeBuffers();
	case OPEN:
		::close(iFd);
	}
	return true;
}

void vidioc_error(std::string str)
{
	std::cerr << "ERROR: " << str << "failed: " << strerror(errno) << std::endl;
}

int CameraPriv::allocBuffers(unsigned int cnt)
{
    struct v4l2_requestbuffers  req;
	struct v4l2_buffer vbuf;

	unsigned int i;

	if (cnt > MAX_BUFS) {
		std::cerr << "request " << cnt << " > max buffers " << MAX_BUFS << std::endl;
		cnt = MAX_BUFS;
	}
	iBufTotal = cnt;
    req.count  = cnt;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    /* Allocate buffers in the capture device driver */
    if (ioctl(iFd, VIDIOC_REQBUFS, &req) == -1) {
        return -ENOMEM;
    }
    if (req.count < cnt || !req.count) {
        return -ENOMEM;
    }
    for (i = 0; i < cnt; i++) {
		memset(&vbuf, 0, sizeof(vbuf));
        vbuf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        vbuf.memory = V4L2_MEMORY_MMAP;
        vbuf.index  = i;

        if (ioctl(iFd, VIDIOC_QUERYBUF, &vbuf) == -1) {
			std::cerr << "iocto VIDIOC_QUERYBUF failed" << std::endl;
            return -ENOMEM;
        }

        /* Map the driver buffer to user space */
        unsigned char *_userptr = (unsigned char *)::mmap(NULL,
                       vbuf.length,
                       PROT_READ | PROT_WRITE,
                       MAP_SHARED,
                       iFd,
                       vbuf.m.offset);

		std::cout << "video buf " << i << " address: " 
			<< static_cast<const void *>(_userptr) << std::endl;
        if (_userptr == MAP_FAILED) {
			std::cerr << "mmap video buf " << i << " failed" << std::endl;
			for (int j = i--; j >= 0; j--) {
				munmap(vecUserBuf[j], vecV4l2Buf[j].length);
			}
            return -ENOMEM;
        }
		vecV4l2Buf.push_back(vbuf);
		vecUserBuf.push_back(_userptr);
    }
	eStatus = ALLOCBUF;
	return 0;
}

void CameraPriv::freeBuffers(void)
{
	for (unsigned int i = 0; i < iBufTotal; i++) {
		if (vecUserBuf[i] == NULL)
			continue;
		if (-1 == ::munmap(vecUserBuf[i], vecV4l2Buf[i].length)) {
			std::cerr << "munmap failed: videobuf " << i 
				<< " address " << static_cast<const void *>(vecUserBuf[i]) << std::endl;
		} else {
			vecUserBuf[i] = NULL;
		}
	}
}

bool CameraPriv::qbuf(unsigned int idx)
{
	if (idx >= iBufTotal)
		return false;
	return ::ioctl(iFd, VIDIOC_QBUF, &vecV4l2Buf[idx]) == 0;
}
bool CameraPriv::qbuf(unsigned char *ptr)
{
	unsigned int i;
	for (i = 0; i < iBufTotal; i++)
		if (vecUserBuf[i] == ptr)
			break;
	return qbuf(i);
}
unsigned char *CameraPriv::dqbuf(void) 
{
	struct v4l2_buffer v4l2buf;

	memset(&v4l2buf, 0, sizeof(v4l2buf));
	v4l2buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4l2buf.memory = V4L2_MEMORY_MMAP;
	if (::ioctl(iFd, VIDIOC_DQBUF, &v4l2buf) < 0) {
		std::cerr << "VIDEO_DQBUF failed" << std::endl;
		return NULL;
	}
	return vecUserBuf[v4l2buf.index];
}

bool CameraPriv::streamon(void) 
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (::ioctl(iFd, VIDIOC_STREAMON, &type) == -1) {
		std::cerr << "VIDIOC_STREAMON failed on device" << std::endl;
		return false;
	}
	eStatus = STREAMON;
	return true;
}

bool CameraPriv::streamoff(void) 
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (::ioctl(iFd, VIDIOC_STREAMOFF, &type) == -1) {
		std::cerr << "ioctl VIDIOC_STREAMOFF failed" << std::endl;
		return false;
	}
	eStatus = CLOSE;
	return true;
}

int CameraPriv::enumerateImageFormats(std::vector<struct v4l2_fmtdesc> *pvecFmt)
{
	struct v4l2_fmtdesc fmtdesc;
	int i = 0;

	for (i = 0; ; i++) {
		memset(&fmtdesc, 0, sizeof(fmtdesc));
		fmtdesc.index = i;
		fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (ioctl(iFd, VIDIOC_ENUM_FMT, &fmtdesc) == -1) {
			if (errno == EINVAL)
				break;
			i--;
			continue;
		}
		pvecFmt->push_back(fmtdesc);
		std::cout << "format: " << fmtdesc.description << std::endl;
	}
	return i;
}

bool CameraPriv::setInput(const int *i)
{
	assert(i != NULL);
	if (::ioctl(iFd, VIDIOC_S_INPUT, i) == -1) {
		vidioc_error("VIDIOC_S_INPUT");
		return false;
	}
	iCurInput = *i;
	return true;
}

bool CameraPriv::getInput(int *i)
{
	assert(i != NULL);
	if (::ioctl(iFd, VIDIOC_G_INPUT, i) == -1) {
		vidioc_error("VIDIOC_G_INPUT");
		return false;
	}
	return true;
}

int CameraPriv::enumerateInputs(std::vector<struct v4l2_input> *pvecInput)
{
	struct v4l2_input input;
	int i;

	for (i = 0; ;i++) {
		memset(&input, 0, sizeof(input));
		input.index = i;
		if (ioctl(iFd, VIDIOC_ENUMINPUT, &input) == -1) {
			if (errno == EINVAL)
				break;
			i--;
			continue;
		}
		pvecInput->push_back(input);
		std::cout << "V4L2 input name: " << input.name << std::endl;
	}
	return i;
}

bool CameraPriv::getImageFormat(struct v4l2_pix_format* pix)
{
	struct v4l2_format fmt;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (::ioctl(iFd, VIDIOC_G_FMT, &fmt) == -1) {
		std::cerr << "Unable to get VIDIOC_G_FMT" << std::endl;
		return false;
	}
	*pix = fmt.fmt.pix;
	return true;
}
bool CameraPriv::setImageFormat(const struct v4l2_pix_format* pix)
{
	struct v4l2_format fmt;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix = *pix;
	if (::ioctl(iFd, VIDIOC_S_FMT, &fmt) == -1) {
		std::cerr << "VIDIOC_S_FMT failed: " << strerror(errno) << std::endl;
		return false;
	}
	return true;
}
int CameraPriv::enumerateFrameSizes(unsigned int pixelformat, std::vector<struct v4l2_frmsizeenum> *pvecFrm)
{
	int i;
	struct v4l2_frmsizeenum frmsize;
	assert(pvecFrm != NULL);
	for (i = 0; ; i++) {
		frmsize.index = i;
		frmsize.pixel_format = pixelformat;
		if (::ioctl(iFd, VIDIOC_ENUM_FRAMESIZES, &frmsize) == -1) {
			if (errno == EINVAL)
				break;
			pvecFrm->clear();
			return -1;
		}
		pvecFrm->push_back(frmsize);
	}
	return i;
}

bool CameraPriv::getCapabilities(struct v4l2_capability *vcap)
{
	assert(vcap != NULL);
	if (!queryCapabilities())
		return false;
	*vcap = mCap;
	return true;
}
bool CameraPriv::queryCapabilities(void)
{
	if (!isOpen())
		open();

	if (ioctl(iFd, VIDIOC_QUERYCAP, &mCap) < 0)
		return false;
	std::cout << "*************** Capabilities *****************" << std::endl;
	std::cout << "*    driver:        '" << mCap.driver << "'" << std::endl;
	std::cout << "*    card:          '" << mCap.card << "'" << std::endl;
	std::cout << "*    bus_info:      '" << mCap.bus_info << "'" << std::endl;
	std::cout << "*    version:       '" << mCap.version << "'" << std::endl;
	std::cout << "*    capabilities:  '" << mCap.capabilities << "'" << std::endl;
	std::cout << "**********************************************" << std::endl;
	return true;
}
/*--------------------- cut here -----------------------------*/

Camera::Camera(const char *name)
{
	priv = new CameraPriv(name);
}

Camera::~Camera()
{
	delete priv;
}

bool Camera::open(int flag)
{
	return priv->open(flag);
}

int Camera::enumerateInputs(std::vector<struct v4l2_input> *pvecInput)
{
#if 0
	unsigned int i;
	for (unsigned int i = 0; i < priv->vecInput.size(); i++)
		vecInput.push_back(priv->vecInput[i]);
	return (int)i;
#endif
	return priv->enumerateInputs(pvecInput);
}

int Camera::enumerateImageFormats(std::vector<struct v4l2_fmtdesc> *pvecFmt)
{
#if 0
	unsigned int i;
	for (unsigned int i = 0; i < priv->vecFmtdesc.size(); i++)
		vecInput.push_back(priv->vecFmtdesc[i]);
	return (int)i;
#endif
	assert(pvecFmt != NULL);
	return priv->enumerateImageFormats(pvecFmt);
}

void printFrameSize(struct v4l2_frmsizeenum *frmsize)
{
	if (frmsize->type == V4L2_FRMSIZE_TYPE_DISCRETE) {
		std::cout << "        Width : " << frmsize->discrete.width << std::endl;
		std::cout << "        Height: " << frmsize->discrete.height << std::endl << std::endl;
	}
	if (frmsize->type == V4L2_FRMSIZE_TYPE_STEPWISE) {
		std::cout << "        Min Width  : " << frmsize->stepwise.min_width  << std::endl;
		std::cout << "        Max Width  : " << frmsize->stepwise.max_width  << std::endl;
		std::cout << "        Step Width : " << frmsize->stepwise.step_width << std::endl;
		std::cout << "        Min Height : " << frmsize->stepwise.min_height << std::endl;
		std::cout << "        Max Height : " << frmsize->stepwise.max_height << std::endl;
		std::cout << "        Step Height: " << frmsize->stepwise.step_height<< std::endl;
	}
}

int Camera::enumerateFrameSizes(unsigned int pixelformat, std::vector<struct v4l2_frmsizeenum> *vecFrm)
{
	return priv->enumerateFrameSizes(pixelformat, vecFrm);
}
void printV4L2Format(struct v4l2_pix_format *pix)
{
	const char *fstr;

	std::cout << "width : " << pix->width << std::endl;
	std::cout << "height: " << pix->height << std::endl;
	switch (pix->pixelformat) {
		case V4L2_PIX_FMT_YUYV:
			fstr = "YUYV";
			break;
		case V4L2_PIX_FMT_YYUV:
			fstr = "YYUV";
			break;
		case V4L2_PIX_FMT_YVYU:
			fstr = "YVYU";
			break;
		case V4L2_PIX_FMT_UYVY:
			fstr = "UYVY";
			break;
		case V4L2_PIX_FMT_VYUY:
			fstr = "VYUY";
			break;
	}
	std::cout << "format: " << fstr << std::endl;
}
bool Camera::init(void)
{
	unsigned int i;
	int idx;

	/* open camera device */
	priv->open();

	priv->enumerateInputs(&priv->vecInput);
	for (i = 0; i < priv->vecInput.size(); i++) {
		if (priv->vecInput[i].type == V4L2_INPUT_TYPE_CAMERA)
			break;
	}
	if (i == priv->vecInput.size()) {
		std::cerr << "There isn't a camera input device" << std::endl;
		goto clean;
	}
	idx = i;
	if (!priv->setInput(&idx)) {
		std::cerr << "Failed set video input" << std::endl;
        goto clean;
	}
	if (!priv->getInput(&idx)) {
		std::cerr << "Failed GET_INPUT. No inputs connected?" << std::endl;
		goto clean;
	}
	std::cout << " Current Input: " << priv->vecInput[idx].name << std::endl;

	if (!priv->queryCapabilities())
		std::cerr << "getCapabilities failed" << std::endl;

    if (!(priv->mCap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		std::cerr << "Device does not support capturing" << std::endl;
        goto clean;
    }

    if (!(priv->mCap.capabilities & V4L2_CAP_STREAMING)) {
		std::cerr << "Device does not support streaming" << std::endl;
        goto clean;
    }
	enumerateImageFormats(&priv->vecFmtdesc);

	struct v4l2_pix_format pix;
	if (!priv->getImageFormat(&pix)) {
        goto clean;
    }
	printV4L2Format(&pix);

	return true;
clean:
	priv->close();
	return false;
}

int Camera::deinit(void)
{
	priv->close();
	return 0;
}

bool Camera::qbuf(unsigned int i)
{
	return priv->qbuf(i);
}

bool Camera::qbuf(unsigned char *ptr)
{
	return priv->qbuf(ptr);
}

bool Camera::qbufs(void) 
{
	for (unsigned int i = 0; i < priv->vecV4l2Buf.size(); i++)
		if (priv->qbuf(i) < 0) {
			std::cerr << "qbuf %d failed" << std::endl;
			return false;
		}
	return true;
}
unsigned char *Camera::dqbuf(void) 
{
	return priv->dqbuf();
}
int Camera::allocBuffers(unsigned int cnt)
{
	return priv->allocBuffers(cnt);
}

void Camera::freeBuffers(void)
{
	priv->freeBuffers();
}

bool Camera::startCapture(void) 
{
	return priv->streamon();
}

bool Camera::stopCapture(void)
{
	return priv->streamoff();
}

bool Camera::getCapabilities(struct v4l2_capability *vcap)
{
	assert(vcap != NULL);
	return priv->getCapabilities(vcap);
}

bool Camera::getImageFormat(struct v4l2_pix_format* pix)
{
	assert(pix != NULL);
	return priv->getImageFormat(pix);
}

bool Camera::setImageFormat(const struct v4l2_pix_format* pix)
{
	return priv->setImageFormat(pix);
}

int Camera::getFd()
{
	return priv->iFd;
}

