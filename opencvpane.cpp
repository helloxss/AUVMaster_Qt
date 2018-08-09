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

void OpenCVPane::ImgDispSlot(cv::Mat img)
{
	//cols：长		rows：宽
	double hTemp = (double)img.rows * (double)this->width() / (double)img.cols;//当r变为w时，c变成的值
	if (hTemp > this->height())//r变为w时，c变成的值太大：c变成h，算r的变化值
	{
		double wTemp = (double)img.cols * (double)this->height() / (double)img.rows;//当c变为h时，r变成的值
		cv::resize(img, img, cv::Size(wTemp, this->height()));
	}
	else
	{
		cv::resize(img, img, cv::Size(this->width(), hTemp));
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
