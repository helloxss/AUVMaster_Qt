#ifndef VISION_H
#define VISION_H

#include <QObject>
#include "MVGigE.h"
#include "GigECamera_Types.h"
#include "MVImage.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace cv;

/* 类说明：
 * 类名：Vision
 * 继承于：QObject
 * 功能：提供维视相机的初始化方法、句柄、公用像素格式对象、公用图像对象（以避免代码中的全局对象）（其实内存里没差……）
 *		以及公共图像处理函数方法的存储
*/

#define FCAM_AUTO_TGT 120		//前视期望亮度
#define FCAM_EXPO_TME 1200		//前视曝光时间
#define FCAM_GAIN 5				//前视增益
#define FCAM_GAMMA 1			//前视Gamma值
#define FCAM_IP "192.168.1.103"

#define DCAM_EXPO_TIM 7000
#define DCAM_GAIN 1				//前视增益
#define DCAM_GAMMA 1			//前视Gamma值
#define DCAM_IP "192.168.1.102"

class Vision : public QObject
{
	Q_OBJECT
public:
	explicit Vision(QObject *parent = 0);
	~Vision();

	static HANDLE fCamHdl;//前视摄像头句柄
	static HANDLE dCamHdl;//下视摄像头句柄
	static MV_PixelFormatEnums PixelFormat;//像素格式对象
	static MVImage image;//定义图像对象

signals:

public slots:
	static QString initFCam();
	static QString initDCam();
	static void destoryFCam();
	static void destoryDCam();

public:
	static int microVisionCallBack(MV_IMAGE_INFO *pInfo, ULONG_PTR nUserVal);
	static void detectColor(const Mat &img_input, Mat &img_H, Mat &img_S, Mat &img_output, int Hmin, int Hmax, int Smin, int Smax, int H_delta);

	static void rect_boundary_light(const Mat &img_input, Mat &img_output, bool &flag, float &maxarea, int &Foundtime, int &P_x_b, int &P_x_e, int &P_y_b, int &P_y_e);
};

#endif // VISION_H
