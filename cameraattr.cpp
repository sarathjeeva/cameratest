#include <QtGui>
#include "cameraattr.h"
#include "camera.h"

CameraAttr::CameraAttr(QWidget *parent)
	: QDialog(parent), camera(NULL)
{
	setupUi(this);

	QDir dir("/sys/class/video4linux");
	devList = dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs);

	cmbDevice->addItems(devList);

	connect(cmbDevice, SIGNAL(currentIndexChanged(int)), this, SLOT(changeDevice(int)));
	connect(cmbPixelFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(changePixelFormat(int)));
	connect(cmbFrmSize, SIGNAL(currentIndexChanged(int)), this, SLOT(changeFrameSize(int)));
	if (devList.size() > 0)
		changeDevice(0);
}

CameraAttr::~CameraAttr()
{
	if (camera)
		delete camera;
}

void CameraAttr::showCapability(struct v4l2_capability *cap)
{
	labDriver->setText((char *)cap->driver);
	labCard->setText((char *)cap->card);
	labBusInfo->setText((char *)cap->bus_info);
	labVersion->setText(QString("0x%1").arg(cap->version, 5, 16));
	labCap->setText(QString("0x%1").arg(cap->capabilities, 5, 16));
}

void CameraAttr::changeDevice(int index)
{
	struct v4l2_capability vcap;

	if (index == -1)
		return;
	if (camera != NULL) {
		delete camera;
	}

	QString dev("/dev/"+ devList.at(index));

	camera = new Camera(dev.toLocal8Bit().constData());
	if (!camera->open())
		return;
	camera->getCapabilities(&vcap);
	showCapability(&vcap);
	camera->enumerateImageFormats(&mvecFmt);
	for (unsigned int i = 0; i < mvecFmt.size(); i++) {
		cmbPixelFormat->addItem((char *)mvecFmt[i].description);
	}
	changePixelFormat(0);
}

void CameraAttr::changePixelFormat(int index)
{
	std::vector<struct v4l2_frmsizeenum> vecFrm;
	camera->enumerateFrameSizes(mvecFmt[index].pixelformat, &vecFrm);
	if (vecFrm.size() > 0) {
		if (vecFrm[0].type == V4L2_FRMSIZE_TYPE_DISCRETE) {
			for (unsigned int i = 0; i < vecFrm.size(); i++) {
				struct v4l2_frmsize_discrete *dis = &vecFrm[i].discrete;
				cmbFrmSize->addItem(QString("%1*%2").arg(dis->width).arg(dis->height));
			}
		}
	}
}

void CameraAttr::changeFrameSize(int index)
{

}
int CameraAttr::findVideoDevices(void)
{
	QDir dir("/sys/class/video4linux");
	devList = dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs);
	return devList.size();
}
