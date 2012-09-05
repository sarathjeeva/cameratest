#ifndef __CAMERA_H
#define __CAMERA_H

#include <vector>
#include <linux/videodev2.h>

class CameraPriv;
class Camera {
public:
	Camera(const char *name = "/dev/video0");

	~Camera();

	bool open(int flag=0);
	bool init(void);
	int deinit(void);

	int allocBuffers(unsigned int cnt);
	void freeBuffers(void);

	bool qbuf(unsigned char *ptr);
	bool qbuf(unsigned int idx);
	unsigned char *dqbuf(void);

	bool qbufs(void);

	bool startCapture(void);

	bool stopCapture(void);

	int getFd(void);

	int enumerateImageFormats(std::vector<struct v4l2_fmtdesc> *vecFmt);
	int enumerateInputs(std::vector<struct v4l2_input> *vecInput);
	int enumerateFrameSizes(unsigned int pixelformat, std::vector<struct v4l2_frmsizeenum> *vecFrm);

	bool getCapabilities(struct v4l2_capability *cap);

	bool getImageFormat(struct v4l2_pix_format* pPix);
	bool setImageFormat(const struct v4l2_pix_format* pix);

	enum { MAX_BUFS = 5, };
	enum { CLOSE = 0, OPEN = 1, ALLOCBUF = 2, STREAMON = 3 };

private:
	CameraPriv	*priv;
};

#endif
