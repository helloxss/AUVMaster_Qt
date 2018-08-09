#ifndef OPENCVPANE_H
#define OPENCVPANE_H

#include <QWidget>
#include <QLabel>
#include<opencv2\opencv.hpp>

class OpenCVPane : public QWidget
{
	Q_OBJECT

public:
	explicit OpenCVPane(QWidget *parent = 0);
	~OpenCVPane();

signals:

public slots:
	void ImgDispSlot(cv::Mat img);

private:
	QLabel *pOpencvArea;
	void displayMat(cv::Mat image);
};

#endif // OPENCVPANE_H
