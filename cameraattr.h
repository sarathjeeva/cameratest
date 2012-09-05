#ifndef __CameraAttr_H
#define __CameraAttr_H

#include <QDialog>
#include "ui_cameraattr.h"
#include <vector>

class Camera;
class CameraAttr: public QDialog, private Ui::UCameraAttr
{
	Q_OBJECT

public:
	CameraAttr(QWidget *parent = 0);
	~CameraAttr();


public slots:
	void changeDevice(int index);
	void changePixelFormat(int index);
	void changeFrameSize(int index);

private:
	int findVideoDevices(void);
	void showCapability(struct v4l2_capability *cap);
	void showPixelFormat();

	Camera *camera;
	QStringList devList;
	std::vector<struct v4l2_fmtdesc> mvecFmt;
};

#endif
