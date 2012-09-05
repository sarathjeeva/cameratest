#ifndef __VIDEOWIDGET_H
#define __VIDEOWIDGET_H

#include <QWidget>
#include <linux/videodev2.h>

class QSocketNotifier;
class QImage;
class Camera;
class VideoWidget: public QWidget
{
	Q_OBJECT;
public:
	VideoWidget(Camera *camera, QWidget *parent = 0);
	~VideoWidget();
	int initCamera();

public slots:
	void readImage(int socket);

protected:
	void paintEvent(QPaintEvent *event);

private:
	QSocketNotifier *notifier_;
	QImage *image_;
	Camera *camera_;
	struct v4l2_pix_format fmt_;
	
};
#endif
