#include "opencvpane.h"
#include "defines.h"
#include <QDebug>

OpenCVPane::OpenCVPane(QWidget *parent) :
	QWidget(parent)
{
	pOpencvArea = new QLabel(this);
	pOpencvArea->setText(QSL("OpenCV displays here"));
}

OpenCVPane::~OpenCVPane()
{

}

void OpenCVPane::CamDispSlot(cv::Mat img)
{
	//rows：长		cols：宽
	double hTemp = (double)img.cols * (double)this->height() / (double)img.rows;
	if (hTemp > this->height())
	{
		double wTemp = (double)img.rows * (double)this->width() / (double)img.cols;
		cv::resize(img, img, cv::Size(this->width(), wTemp));
	}
	else
	{
		cv::resize(img, img, cv::Size(hTemp, this->height()));
	}
	displayMat(img);
}

void OpenCVPane::displayMat(cv::Mat image)
{
	cv::Mat rgb;
	QImage img;
	if (image.channels() == 3)
	{
		//cvt Mat BGR 2 QImage RGB
		cvtColor(image, rgb, CV_BGR2RGB);
		img = QImage((const unsigned char*)(rgb.data),
		             rgb.cols, rgb.rows,
		             rgb.cols*rgb.channels(),
		             QImage::Format_RGB888);
	}
	else
	{
		img = QImage((const unsigned char*)(image.data),
		             image.cols, image.rows,
		             image.cols*image.channels(),
		             QImage::Format_RGB888);
	}
	pOpencvArea->setPixmap(QPixmap::fromImage(img));
	pOpencvArea->resize(pOpencvArea->pixmap()->size());
}
