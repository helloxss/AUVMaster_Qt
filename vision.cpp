#include "vision.h"
#include "defines.h"
#include "task.hpp"
#include <QDebug>
#include <QThread>
#include <vector>
using namespace std;

HANDLE Vision::fCamHdl = NULL;//前视摄像头句柄
HANDLE Vision::dCamHdl = NULL;//下视摄像头句柄
MV_PixelFormatEnums Vision::PixelFormat;//像素格式对象
MVImage Vision::image;//定义图像对象

Vision::Vision(QObject *parent) : QObject(parent)
{

}

Vision::~Vision()
{

}

QString Vision::initFCam()
{
	//初始化
	MVSTATUS_CODES r;
	r = MVInitLib();
	if (r != MVST_SUCCESS)
		return QSL("MVInitLib() 失败，错误代码%1").arg(r);

	//查找连接到计算机上的相机
	r = MVUpdateCameraList();
	if (r != MVST_SUCCESS)
		return QSL("MVUpdateCameraList() 失败，错误代码%1").arg(r);

	//获取相机数量
	int NumCameras = 0;
	MVGetNumOfCameras(&NumCameras);
	if (NumCameras == 0)
		return QSL("识别的相机数量等于0");

	//得到第idx个相机信息
	unsigned char idx = 0;
	MVCamInfo CamInfo;
	MVGetCameraInfo(idx, &CamInfo);

	//打开第idx个相机
	Vision::fCamHdl = NULL;
	//MVOpenCamByIndex(idx,&hCam);

	//打开指定IP的相机
	MVOpenCamByIP((char*)FCAM_IP, &Vision::fCamHdl);
	if (Vision::fCamHdl == NULL)
		return QSL("fCamHdl == NULL");

	//设置曝光模式
	//ExposureAutoEnums ExposureAuto = ExposureAuto_Continuous;//ExposureAuto_Once;//一次自动曝光(效果一般)
	//MVSetExposureAuto(Vision::fCamHdl, ExposureAuto);

	//设置图像像素格式
	PixelFormat = PixelFormat_BayerRG8;
	MVSetPixelFormat(Vision::fCamHdl, PixelFormat);

	//设置当前帧率
	MVSetFrameRate(Vision::fCamHdl, 39.9);

	//设置期望亮度
	MVSetAutoTargetValue(Vision::fCamHdl, FCAM_AUTO_TGT);

	//设置曝光时间范围
	MVSetAutoExposureTimeLowerLimit(Vision::fCamHdl, 30);
	MVSetAutoExposureTimeUpperLimit(Vision::fCamHdl, 400000);

	//设置曝光时间
	MVSetExposureTime(Vision::fCamHdl, FCAM_EXPO_TME);

	//设置增益
	MVSetGain(Vision::fCamHdl, FCAM_GAIN);

	//设置Gamma值
	MVSetGamma(Vision::fCamHdl, FCAM_GAMMA);

	//设置白平衡
//	MVSetGainBalance(totalinfo.Vision::fCamHdl,1);

	//设置亮度容差
	//MVSetAutoThreshold(Vision::fCamHdl, 0);

	//设置触发方式
	MVSetTriggerMode(Vision::fCamHdl, TriggerMode_Off);

	//设置及获取图像宽高
	int ww = 1600, hh = 1200;
	MVSetWidth(Vision::fCamHdl, ww);
	MVSetHeight(Vision::fCamHdl, hh);
	MVGetWidth(Vision::fCamHdl, &ww);
	MVGetHeight(Vision::fCamHdl, &hh);

	//获取图像格式
	MVGetPixelFormat(Vision::fCamHdl, &PixelFormat);
	image.CreateByPixelFormat(ww, hh, PixelFormat);
	return QString();
}

QString Vision::initDCam()
{
	//初始化
	MVSTATUS_CODES r;
	r = MVInitLib();
	if (r != MVST_SUCCESS)
		return QSL("MVInitLib() 失败，错误代码%1").arg(r);
	//查找连接到计算机上的相机
	r = MVUpdateCameraList();
	if (r != MVST_SUCCESS)
		return QSL("MVUpdateCameraList() 失败，错误代码%1").arg(r);
	//获取相机数量
	int NumCameras = 0;
	MVGetNumOfCameras(&NumCameras);
	if (0 == NumCameras)
		return QSL("识别的相机数量等于0");
	//得到第idx个相机信息
	unsigned char idx = 0;
	MVCamInfo CamInfo;
	MVGetCameraInfo(idx, &CamInfo);

	//打开第idx个相机
	Vision::dCamHdl = NULL;
	//MVOpenCamByIndex(idx,&hCamD);

	//打开指定IP的相机
	char IP[20] = "192.168.1.201";
	MVOpenCamByIP(IP, &Vision::dCamHdl);
	if (NULL == Vision::dCamHdl)
		return QSL("dCamHdl == NULL");

	//设置图像像素格式
	PixelFormat = PixelFormat_BayerRG8;
	MVSetPixelFormat(Vision::dCamHdl, PixelFormat);

	//设置当前帧率
	double fps = 30;
	MVSetFrameRate(Vision::dCamHdl, fps);

	//设置期望亮度
	//MVSetAutoTargetValue(Vision::dCamHdl, 120);

	//设置曝光时间范围
	MVSetAutoExposureTimeLowerLimit(Vision::dCamHdl, 10);
	MVSetAutoExposureTimeUpperLimit(Vision::dCamHdl, 400000);

	//设置曝光模式
	//ExposureAutoEnums ExposureAuto = ExposureAuto_Continuous ;//ExposureAuto_Once;//一次自动曝光(效果一般)
	//MVSetExposureAuto(hCamD, ExposureAuto);

	//设置曝光时间
	MVSetExposureTime(Vision::dCamHdl, DCAM_EXPO_TIM);

	//设置增益
	MVSetGain(Vision::dCamHdl, DCAM_GAIN);

	//设置Gamma值
	MVSetGamma(Vision::dCamHdl, DCAM_GAMMA);

	//设置亮度容差
	//MVSetAutoThreshold(Vision::dCamHdl, 0.1);

	//设置触发方式
	MVSetTriggerMode(Vision::dCamHdl, TriggerMode_Off);

	//设置及获取图像宽高
	int ww = 1280, hh = 960;
	MVSetWidth(Vision::dCamHdl, ww);
	MVSetHeight(Vision::dCamHdl, hh);
	MVGetWidth(Vision::dCamHdl, &ww);
	MVGetHeight(Vision::dCamHdl, &hh);

	//获取图像格式
	MVGetPixelFormat(Vision::dCamHdl, &PixelFormat);
	image.CreateByPixelFormat(ww, hh, PixelFormat);
	return true;
}

void Vision::destoryFCam()
{
	if(fCamHdl)
	{
		MVStopGrab(fCamHdl);
		MVSetTriggerMode(fCamHdl,TriggerMode_Off);
		MVCloseCam(fCamHdl);
		fCamHdl = 0;
	}
}

void Vision::destoryDCam()
{
	if(dCamHdl)
	{
		MVStopGrab(dCamHdl);
		MVSetTriggerMode(dCamHdl,TriggerMode_Off);
		MVCloseCam(dCamHdl);
		dCamHdl = 0;
	}
}

int Vision::microVisionCallBack(MV_IMAGE_INFO *pInfo, ULONG_PTR nUserVal)
{
	//获取调用者指针
	Task *caller = (Task *)nUserVal;
	if (!caller)
		return 0;

	//获取图像
	MVInfo2Image(Vision::fCamHdl, pInfo, &(Vision::image));

	//获取图像宽
	int w = Vision::image.GetWidth();
	//获取图像宽
	int h = Vision::image.GetHeight();
	Mat src;
	src.create(h, w, CV_8UC3);
	//将彩色图像数据转换为OpenCV的Mat数据格式
	MVSTATUS_CODES r;//错误枚举对象
	r = MVBayerToBGR(Vision::fCamHdl, pInfo->pImageBuffer, src.data, w * 3, w, h, Vision::PixelFormat);
	if (r != MVST_SUCCESS)
	{
		qDebug() << "convert failed:" << r << endl;
	}

	resize(src, src, Size(src.cols / 4, src.rows / 4), (0, 0), (0, 0), 3);
	emit caller->videoImgIn(src);
	return 0;
}

void Vision::detectColor(const Mat &img_input, Mat &img_H, Mat &img_S, Mat &img_output, int Hmin, int Hmax, int Smin, int Smax, int H_delta)
{
	Mat img_V;
	Mat img_out1,imgout2;
	Mat img_hsv;
	Mat img_H_temp, img_S_temp,img_V_temp;
	Mat img_H1, img_H2;
	Mat img_rangedH, img_rangedS,img_rangedV;
	vector<Mat> channels;
	int Vmin=160;
	int Vmax=255;
	//将色彩空间从BGR转换到HSV
	cvtColor(img_input, img_hsv, COLOR_BGR2HSV);
	//分离成H、S、V三个通道
	split(img_hsv, channels);
	//只取出H和S两个通道
	img_H_temp = channels.at(0);
	img_S_temp = channels.at(1);
	img_V_temp=channels.at(2);

	//考虑到H通道的本质是一个环形，取值从0到180，因此，应当区分H是否出现“溢出”的情况
	if (Hmin + H_delta <= 180)
	{
		//对H通道和S通道框定上下阈值
		inRange(img_H_temp, Scalar(Hmin, 0.0, 0.0), Scalar(Hmax, 0.0, 0, 0), img_rangedH);
		inRange(img_S_temp, Scalar(Smin, 0.0, 0.0), Scalar(Smax, 0.0, 0, 0), img_rangedS);
		inRange(img_V_temp, Scalar(Vmin, 0.0, 0.0), Scalar(Vmax, 0.0, 0, 0), img_rangedV);
		img_rangedH.copyTo(img_H);
		img_rangedS.copyTo(img_S);
		img_rangedV.copyTo(img_V);
		//对H通道和S通道分别做腐蚀、膨胀操作
		erode(img_rangedH, img_rangedH, Mat(), Point(-1, -1), 1);
		dilate(img_rangedH, img_rangedH, Mat(), Point(-1, -1), 5);
		erode(img_rangedS, img_rangedS, Mat(), Point(-1, -1), 1);
		dilate(img_rangedS, img_rangedS, Mat(), Point(-1, -1), 5);
		erode(img_rangedV, img_rangedV, Mat(), Point(-1, -1), 1);
		dilate(img_rangedV, img_rangedV, Mat(), Point(-1, -1), 5);
		//将H通道和S通道按位取与，得到结果图像
		bitwise_and(img_rangedH, img_rangedS,img_out1);
		bitwise_and(img_out1, img_rangedV,img_output);
	}
	else
	{
		//对H通道和S通道框定上下阈值
		inRange(img_H_temp, Scalar(Hmin, 0, 0), 180, img_H1);
		inRange(img_H_temp, 0, Scalar(Hmin + H_delta - 180), img_H2);
		bitwise_or(img_H1, img_H2, img_rangedH);
		inRange(img_S_temp, Scalar(Smin, 0.0, 0.0), Scalar(Smax, 0.0, 0, 0), img_rangedS);
		inRange(img_V_temp, Scalar(Vmin, 0.0, 0.0), Scalar(Vmax, 0.0, 0, 0), img_rangedV);
		img_rangedH.copyTo(img_H);
		img_rangedS.copyTo(img_S);
		img_rangedV.copyTo(img_V);
		//对H通道和S通道分别做腐蚀、膨胀操作
		erode(img_rangedH, img_rangedH, Mat(), Point(-1, -1), 1);
		dilate(img_rangedH, img_rangedH, Mat(), Point(-1, -1), 5);
		erode(img_rangedS, img_rangedS, Mat(), Point(-1, -1), 1);
		dilate(img_rangedS, img_rangedS, Mat(), Point(-1, -1), 5);
		erode(img_rangedV, img_rangedV, Mat(), Point(-1, -1), 1);
		dilate(img_rangedV, img_rangedV, Mat(), Point(-1, -1), 5);
		//将H通道和S通道按位取与，得到结果图像
		bitwise_and(img_rangedH, img_rangedS,img_out1);
		bitwise_and(img_out1, img_rangedV,img_output);
	}
}

void Vision::rect_boundary_light(const Mat &img_input, Mat &img_output, bool &flag, float &maxarea, int &Foundtime, int &P_x_b, int &P_x_e, int &P_y_b, int &P_y_e)
{
	float area_max = 0.0;
	float area_max2 = 0.0;
	float area_max1 = 0.0;
	int i_marked = 0, i_marked1 = 0, i_marked2 = 0;
	Mat img_contours = Mat::zeros(img_input.size(), CV_8UC1);	//findContours函数只支持8UC1或者32FC1的图像
	RNG rng(12345);

	//定义findContours()需要的vector容器
	vector<vector<Point>> contours(1000);
	vector<Vec4i> hierarchy(1000);

	//寻找轮廓
	findContours(img_input, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	//创建容器用于寻找矩形
	vector<RotatedRect> minRect(contours.size() + 1);
	for (int i = 0; i < contours.size(); i++)
	{
		minRect[i] = minAreaRect(contours[i]);
	}
	for (int i = 0; i < contours.size(); i++)//随机色画出所有矩形
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));//定义颜色变量
		drawContours(img_contours, contours, i, color, 2, 8, vector<Vec4i>(), 3, Point());//画轮廓
		//画旋转矩形，与画没有旋转角度的矩形方法不一样
		Point2f rect_points[4];					//定义一个含有四个成员、每个成员都是一个浮点数点类型的数组，代表矩形的四个顶点
		minRect[i].points(rect_points);			//类函数，返回矩形的四个顶点
		for (int j = 0; j < 4; j++)
		{
			line(img_output, rect_points[j], rect_points[(j + 1) % 4], color, 2, 8);		//画出矩形的四条边
		}
	}

	//判断拟合后矩形的个数
	if (contours.size() <= 1){			//只有一个矩形或没有
		if (contours.size()){
			area_max = minRect[0].size.area();
			i_marked = 0;
		}
	}//if (contours.size() <= 1)
	else{								//多于一个矩形
		//选出面积最大的两个
		int i(2);
		area_max1 = minRect[0].size.area(); i_marked1 = 0;
		area_max2 = minRect[1].size.area(); i_marked2 = 1;
		while (i <contours.size()){
			if (minRect[i].size.area() > area_max1){
				area_max1 = minRect[i].size.area();
				i_marked1 = i;
				continue;
			}
			if (minRect[i].size.area() > area_max2){
				area_max2 = minRect[i].size.area();
				i_marked2 = i;
				continue;
			}
			i++;
		}
		//计算最大两个的面积之比
		float ratio = area_max1 / area_max2;
		if (0.5 < ratio && ratio < 2){	//比值符合条件则看中心点的y值
			Point2f rect_points1[4];
			minRect[i_marked1].points(rect_points1);
			Point2f rect_points2[4];
			minRect[i_marked2].points(rect_points2);
			if (rect_points1[1].y > rect_points2[1].y)
				i_marked = i_marked1;
			if (rect_points1[2].y < rect_points2[2].y)
				i_marked = i_marked2;
		}
		else{							//比值不符合条件则取面积最大者
			(area_max1 > area_max2) ? (area_max = area_max1, i_marked = i_marked1) : (area_max = area_max2, i_marked = i_marked2);
		}
	}

	//画面积最大的那个矩形，边框颜色为黄色Scalar( 255, 255, 0)
	Point2f rect_points[4];
	minRect[i_marked].points(rect_points);
	int i = 0;
	P_x_b = rect_points[1].x;
	for (i = 0; i < 4; i++)
	{
		if (rect_points[i].x < P_x_b)
			P_x_b = rect_points[i].x;
		if (P_x_b < 0)
			P_x_b = 1;
	}

	P_x_e = rect_points[3].x;
	for (i = 0; i < 4; i++)
	{
		if (rect_points[i].x>P_x_e)
			P_x_e = rect_points[i].x;
		if (P_x_e > 400)
			P_x_e = 400;
	}

	P_y_b = rect_points[1].y;

	for (i = 0; i < 4; i++)
	{
		if (rect_points[i].y < P_y_b)
			P_y_b = rect_points[i].y;
		if (P_y_b < 0)
			P_y_b = 1;
	}
	P_y_e = rect_points[3].y;
	for (i = 0; i < 4; i++)
	{
		if (rect_points[i].y>P_y_e)
			P_y_e = rect_points[i].y;
		if (P_y_e > 300)
			P_y_e = 300;
	}
	for (int j = 0; j < 4; j++)
	{
		line(img_output, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 255, 255), 4, 8);
	}

	//输出最大矩形的面积
	maxarea = minRect[i_marked].size.area();

	//对最大矩形的面积做一个下阈值限定。这样做的目的是，如果图像受到较大的干扰（噪点过多），使得像素点满足条件，
	//而图像中没有一个主导矩形的话，航行器会保持直航而不受影响。
	if (minRect[i_marked].size.area() > 3)
	{
		flag = 1;
	}
	else
	{
		flag = 0;
	}
}
