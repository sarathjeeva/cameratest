#include "videowidget.h"
#include <QtCore>
#include <QSocketNotifier>
#include <QPainter>
#include <QMessageBox>

#include <assert.h>

#include "tabconvert.h"
#include "convert/convertor.h"
#include "camera.h"

//static TabConvert *__pConvert = NULL;
static convertor*__pConvert = NULL;

VideoWidget::VideoWidget(Camera *camera, QWidget *parent) 
	: QWidget(parent), camera_(camera)
{
	assert(camera != NULL);
	camera->getImageFormat(&fmt_);
	this->image_ = new QImage(fmt_.width, fmt_.height, QImage::Format_RGB888);
}
VideoWidget::~VideoWidget()
{
	camera_->stopCapture();
	camera_->freeBuffers();
	delete this->image_;
}

int VideoWidget::initCamera()
{
	if (!camera_->getImageFormat(&fmt_)) {
		qDebug() << "Camera::getImageFormat failed";
		goto err;
	}
	if (__pConvert == NULL) {
#if 0
		__pConvert = new TabConvert(fmt_.width, fmt_.height);
		__pConvert->setYUVFormat(fmt_.pixelformat);
		__pConvert->setRGBFormat(V4L2_PIX_FMT_RGB24);
#endif
		__pConvert = convertor_registry::get_convertor(fmt_.pixelformat, V4L2_PIX_FMT_RGB24, fmt_.width);
	}
	if (camera_->allocBuffers(3) < 0) {
		qDebug() << "allocBuffers failed";
		goto err;
	}
	if (camera_->qbufs() < 0)
		goto err;

	notifier_ = new QSocketNotifier(camera_->getFd(), QSocketNotifier::Read, this);
	connect(notifier_, SIGNAL(activated(int)), 
			this, SLOT(readImage(int)));

	camera_->startCapture();
	return 0;
err:
	qDebug() << "initCamera failed";
	delete camera_;
	return -1;
}

void VideoWidget::readImage(int socket)
{
	unsigned char *ptr, *dup;
	unsigned int l;

	(void)socket;

	dup = ptr = camera_->dqbuf();
	for (l = 0; l < fmt_.height; l++) {
		unsigned char *dest = (unsigned char *)image_->scanLine(l);
		__pConvert->convert(ptr, dest, 1);
		ptr += fmt_.bytesperline;
#if 0
		unsigned short *nextline = (unsigned short *)this->image_->scanLine(l*2+1);
		memcpy(nextline, dest, 720*(this->image_->depth()/8));
#endif
	}
	camera_->qbuf(dup);
	repaint();
}

void VideoWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter;

	(void)event;
	painter.begin(this);
	painter.drawImage(10, 10, *this->image_);
	painter.end();
}

