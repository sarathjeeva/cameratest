#include <QtGui>
#include "videowidget.h"
#include "camera.h"
#include "cameraattr.h"

#define WIDTH	320
#define HEIGHT	240
int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
#if 1
	struct v4l2_pix_format pix;
	Camera *camera = new Camera("/dev/video0");
	camera->init();

	camera->getImageFormat(&pix);
	pix.width = WIDTH;
	pix.height = HEIGHT;
	if (camera->setImageFormat(&pix) && 
			pix.width == WIDTH && pix.height == HEIGHT)
		qDebug() << "Camera Change Image size to " << WIDTH << "*" << HEIGHT;
	else
		qDebug() << "Camera don't support Change Image size";

	VideoWidget *video = new VideoWidget(camera);
	video->setFixedSize(730, 600);
	if (video->initCamera() < 0) {
		QMessageBox::information(NULL, QString("Camera"), 
				QString("Connect to video device failed"), 
				QMessageBox::Close);
		return 1;
	}
	video->show();

	app.exec();

	delete video;
	delete camera;
#else
	CameraAttr *cam_attr = new CameraAttr;
	cam_attr->show();
	app.exec();
#endif

	return 0;
}
