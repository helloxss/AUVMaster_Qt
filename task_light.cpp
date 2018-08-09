#include <QThread>
#include "task_light.h"
#include "vision.h"
#include "funcs.h"

QString Task_Light::taskName = QSL("光标追踪");

Task_Light::Task_Light(int period, QObject *parent) :
	Task(period, parent),
	GoCtrl(1.05, 0, 0.8),
	XAdjustCtrl(0.8, 10000, 0.3)
{
	connect(this, &Task_Light::videoImgIn, this, &Task_Light::camIdentifyTarget);
	memset(SL, 0, sizeof(double) * 5);
}

Task_Light::~Task_Light()
{
	MVStopGrab(Vision::fCamHdl);
	if (timer)
		delete timer;
}

void Task_Light::setTaskPara()
{

}

void Task_Light::taskSetUp()
{
	//检查一下相机
	if(Vision::fCamHdl)
	{
		if (!isCamThreadRun)
		{
			MVSetExposureTime(Vision::fCamHdl, 400);
			MVSTATUS_CODES r = MVStartGrab(Vision::fCamHdl, &Vision::microVisionCallBack, (long long)this);
			if(r != MVST_SUCCESS)
			{
				emit operateLog(QSL("%1：相机打开失败，代码%2").arg(taskName).arg(r), LogPane::err);
				emit taskEndSgl(nextOne);
				return;
			}
			else
				isCamThreadRun = true;
		}
	}
	else
	{
		emit operateLog(QSL("%1：相机没开").arg(taskName), LogPane::err);
		emit taskEndSgl(nextOne);
		return;
	}

	//第一个目标函数
	pCtrlFunc = &Task_Light::ctrlSetSearchDepth;
	pCtrlFuncPara = nullptr;

	//控制定时器
	timer = new QTimer(this);
	if(!timer)
	{
		emit operateLog(QSL("%1：控制定时器创建失败").arg(taskName), LogPane::err);
		emit taskEndSgl(Task::noMore);
		return;
	}
	connect( timer, &QTimer::timeout, this, &Task_Light::ctrlTimerUpdate, Qt::QueuedConnection);
	timer->start(ctrlPeriod);

	//开始计时
	elapseTime.start();

	//安保定时器
	QTimer::singleShot(securityTimerConst*1000, this, \
					   [this]{
		pCtrlFunc = nullptr;
		timer->stop();
		emit operateLog((QSL("%1：安保定时时间(%2s)到")).arg(taskName).arg(securityTimerConst), LogPane::warn);
		emit taskEndSgl(Task::noMore);
		MVStopGrab(Vision::fCamHdl);
		isCamThreadRun = false;
	});
}

void Task_Light::ctrlTimerUpdate()
{
	if(pCtrlFunc)
		(this->*pCtrlFunc)(pCtrlFuncPara);
	emit taskInfos(infoStr());
}

void *Task_Light::ctrlSetSearchDepth(void *)
{
	taskStatus = QSL("%1").arg((QString)__FUNCTION__);

	emit autoPitchSwitch(true);
	emit autoDepthSwitch(true);
	emit setAutoDepth(searchDepth);
	emit taskTurnSgl(0);
	emit operateLog(QSL("%1：当前搜索深度 %2 cm，正在调整中...").arg(taskName).arg(searchDepth), LogPane::info);

	pCtrlFunc = &Task_Light::ctrlWaitForDepthOK;

	//设定灯
	LEDSetting s;
	s.colorList<<QColor(0,0,255)<<QColor(0,255,0);
	s.changeTime = 250;s.flashTimes = 0;
	emit LEDFlash(s);

	return nullptr;
}

void *Task_Light::ctrlWaitForDepthOK(void *)
{
	taskStatus = QSL("%1").arg((QString)__FUNCTION__);

	if(isLightInSight)//等待定深的过程中就找到了
	{
		if(depth + deltaDepth > 0)
		{
			operateLog(QSL("%1：在定深过程中（深度：%2cm）找到目标，估计目标深度：%3").arg(taskName).arg(depth).arg(depth + deltaDepth), LogPane::info);
			emit setAutoDepth(depth + deltaDepth);//定为当前深度
		}
		else
		{
			operateLog(QSL("%1：在定深过程中（深度：%2cm）找到目标，未估计目标深度").arg(taskName).arg(depth), LogPane::info);
			emit setAutoDepth(depth);//定为当前深度

		}
		pCtrlFunc = &Task_Light::ctrlChaseTarget;
		//找到目标后不设定灯，在ctrlChaseTarge t设定灯色
	}
	emit taskTurnSgl(0);

	//深度达标计时
	depthOKTime = (abs(depth - searchDepth) <= 3) ? depthOKTime + ctrlPeriod : 0;
//	depthOKTime += ctrlPeriod;//！！！！！！！测试
	extraInfo = QSL("深度达标时间：%1").arg((double)depthOKTime/1000);

	if(depthOKTime >= 5000)
	{
		depthOKTime = 0;
		pCtrlFunc = &Task_Light::ctrlFindTarget;
		emit operateLog(QSL("%1：搜索深度已达到，开始搜索目标").arg(taskName), LogPane::info);
	}
	return nullptr;
}

void *Task_Light::ctrlFindTarget(void *)
{
	taskStatus = QSL("%1").arg((QString)__FUNCTION__);

	emit taskTurnSgl(findTargetTurnLvConst);//较低角速度寻找目标
	startYaw = pos.yaw;
	pCtrlFunc = &Task_Light::ctrlWaitForFindTarget;
	emit operateLog(QSL("%1：正在搜索目标，搜索深度%2cm").arg(taskName).arg(searchDepth), LogPane::info);

	return nullptr;
}

void *Task_Light::ctrlWaitForFindTarget(void *)
{
	taskStatus = QSL("%1").arg((QString)__FUNCTION__);

	while(!isDataUpdated)	//等待图像回调的数据更新
		;

	if(isLightInSight)//寻到光
	{
		taskTurnSgl(0);
		pCtrlFunc = &Task_Light::ctrlChaseTarget;
		roundDetect = 0;
		emit operateLog(QSL("%1：寻找到目标").arg(taskName), LogPane::info);
	}
	else//转一圈检测
	{
		//在起始角度向右0~90度范围内、roundDetect为0，则将roundDetect第1位置1
		//在起始角度向右90~180度范围内、roundDetect为0x1,则将roundDetect第2位置1
		//在起始角度向右180~270度范围内、roundDetect为0x3,则将roundDetect第3位置1
		//在起始角度向右270~360度范围内、roundDetect为0x7,则将roundDetect第4位置1
		//在起始角度向右0~90度范围内、roundDetect为0xf，则转过一圈
		double deltaAngle = signedDeltaAngle(pos.yaw, startYaw);

		switch(roundDetect)
		{
		case 0x0:
			roundDetect = (deltaAngle > 0 && deltaAngle <= 90) ? roundDetect | 0x1 : roundDetect;
			break;
		case 0x1:
			roundDetect = (deltaAngle > 90 && deltaAngle <= 180) ? roundDetect | 0x2 : roundDetect;
			break;
		case 0x3:
			roundDetect = (deltaAngle > -180 && deltaAngle <= -90) ? roundDetect | 0x4 : roundDetect;
			break;
		case 0x7:
			roundDetect = (deltaAngle > -90 && deltaAngle <= 0) ? roundDetect | 0x8 : roundDetect;
			break;
		case 0xf:
			if(deltaAngle > 0 && deltaAngle <= 90)
			{
				roundDetect = 0;
				emit operateLog(QSL("%1：已经转过一圈而未找到目标。").arg(taskName), LogPane::warn);
				if(searchDepth >= 60)
				{
					emit operateLog(QSL("%1：搜索深度超过限定，退出").arg(taskName), LogPane::warn);
					emit taskEndSgl(noMore);
				}
				else
				{
					searchDepth += 20;
					pCtrlFunc = &Task_Light::ctrlSetSearchDepth;
				}
			}
			break;
		default:
			qDebug()<<(QString)__FUNCTION__<< "wrong roundDetect";
			break;
		}

		extraInfo = QSL("尚未找到目标\n开始角度：%1°\n转过百分比：%2%").arg(startYaw).arg((deltaAngle < 0 ? deltaAngle + 360 : deltaAngle) / 3.6);
	}

	return nullptr;
}

void *Task_Light::ctrlChaseTarget(void *)
{
	taskStatus = QSL("%1，目标点：%2").arg((QString)__FUNCTION__).arg(isLightInSight);

	while(!isDataUpdated)	//等待图像回调的数据更新
		;

	if(isLightInSight)//有目标点：根据距离控制taskTurn, taskGo, 并设定深度
	{
		if(!isCheasingLEDSet)
		{
			//设定灯：目标出现在视野中则闪绿灯
			isCheasingLEDSet = true;
			isShockingLEDSet = false;
			LEDSetting s;
			s.colorList<<QColor(0,255,0)<<QColor(0,0,0);
			s.changeTime = 100;s.flashTimes = 0;
			emit LEDFlash(s);
		}

		shockTime = 0;

		//左右航向控制
		int taskTurn = 0;
		if(abs(posX) > 3)
		{
			taskTurn = XAdjustCtrl.PDCtrlGivenDiff(posX, pos.yawSpd);
			emit taskTurnSgl(taskTurn);
		}

		//前进控制
		static double duration = 0.1;
		int taskGo = -70;
		int tMax = 0;
		if(distance > 20)
		{
			taskGo = GoCtrl.PIDCtrl(distance - 50, (double)duration);
			tMax = posX > 0 ? (200 - posX) : (200 + posX);
			tMax = tMax < 0 ? 0 : tMax;

			if (taskGo > tMax)
				taskGo = tMax;
			else if (taskGo < -tMax)
				taskGo = -tMax;

			duration = 0.1;
		}
		else
		{
			duration += (double)ctrlPeriod/1000;
		}
		emit taskGoSgl(taskGo);

		//深度控制
		if(distance <= 150)//在此距离内？是，持续的深度控制 ： 否，当目标偏离画面中心100像素后才估算深度
		{
			depthCtrlAreaTime += ctrlPeriod;
			if(depthCtrlAreaTime >= 200)//当处在深度控制距离内超过0.2秒后才开始控制，防止图像第一次返回的距离为0导致的错误深度估计
			{
				double setDepth = (depth + deltaDepth > 0) ? depth + deltaDepth : depth;
				emit setAutoDepth(setDepth);
				extraInfo += QSL("深度估算：（控制区内） %7\n").arg(setDepth);
			}//if(depthCtrlAreaTime >= 200)
		}//if(distance <= 150)
		else
		{
			depthCtrlAreaTime = 0;
			if(abs(posY) >= 100)//目标偏离画面中心大于这些像素？是：估算深度 ； 否：保持当前深度
			{
				isOutDepthAreaOkDepthEmitted = false;
				double setDepth = (depth + deltaDepth > 0) ? depth + deltaDepth : depth;
				emit setAutoDepth(setDepth);
				extraInfo += QSL("深度估算：（控制区外） %7\n").arg(setDepth);
			}
			else
			{
				if(false == isOutDepthAreaOkDepthEmitted)//只发一次
				{
					emit setAutoDepth(depth);
					isOutDepthAreaOkDepthEmitted = true;
				}
			}
		}//if(distance <= 150) else

		//显示信息
		extraInfo = QSL("有目标点\n目标位置：(%1,%2)\n左右控制：%3\n目标距离：%4\n前进控制：%5 (max %6)\n")
				.arg(posX).arg(posY).arg(taskTurn).arg(distance).arg(taskGo).arg(tMax);

	}//if(isLightInSight)
	else//丢失了目标点：在10秒内按照最后一次出现的位置发送推进器指令，超过10秒则重新进入寻找模式
	{
		taskGoSgl(0);
		shockTime += ctrlPeriod;
		if(shockTime >= 10000)//仍然没能找到：重新进入寻找模式
		{
			searchDepth = depth;
			pCtrlFunc = &Task_Light::ctrlFindTarget;
			emit operateLog(QSL("%1：丢失目标超过10s，重新寻找目标").arg(taskName), LogPane::info);
			shockTime = 0;
			//设定灯：寻找，闪蓝绿灯，250ms
			LEDSetting s;
			s.colorList<<QColor(0,0,255)<<QColor(0,255,0);
			s.changeTime = 250;s.flashTimes = 0;
			emit LEDFlash(s);
		}
		else if(!isShockingLEDSet)
		{
			//设定灯：震惊，则闪蓝灯
			isShockingLEDSet = true;
			isCheasingLEDSet = false;
			LEDSetting s;
			s.colorList<<QColor(0,0,255)<<QColor(0,0,0);
			s.changeTime = 100;s.flashTimes = 0;
			emit LEDFlash(s);
		}
		extraInfo = QSL("丢失目标\n丢失时间：%1\n")
					.arg((double)shockTime/1000);
	}//if(isLightInSight) else

	return nullptr;
}

void Task_Light::camIdentifyTarget(Mat src)
{
	GaussianBlur(src, src, Size(3, 3), 0.1, 0, BORDER_DEFAULT);
	Mat g_grayImage;//输入的灰度图
	cvtColor(src, g_grayImage, CV_RGB2GRAY);
	inRange(g_grayImage, 255, 255, g_grayImage);

	int P_x_b = 0, P_x_e = 0, P_y_b = 0, P_y_e = 0;
	bool flag = false;
	float maxarea = 0;
	int Foundtime = 0;

	//==================================debug模式下注释掉这句===========================================
	Vision::rect_boundary_light(g_grayImage, src, flag, maxarea, Foundtime, P_x_b, P_x_e, P_y_b, P_y_e);
	//=================================否则造成程序崩溃(opencv + vs2015 bug)============================

	emit videoImg(src);

	Point2f A;
	A.x = (P_x_b + P_x_e) / 2;
	A.y = (P_y_b + P_y_e) / 2;

	double dest_x = A.x - g_grayImage.cols / 2;
	double dest_y = (A.y - g_grayImage.rows / 2);

	if (P_x_b < 3 || P_x_e > 397 || P_y_b < 3 || P_y_e > 297)//图像x最小值小于1，距离不可信
		flag = 0;
	else
		flag = 1;

	//中值滤波，获取过去5个光源的面积的中值
	double area = 0;
	if(flag)
	{
		memcpy(SL, SL+1, sizeof(double)*4);
		SL[4] = ((double)P_x_e - (double)P_x_b)*((double)P_y_e - (double)P_y_b) / 100;
		medianCnt = medianCnt <= 4 ? medianCnt+1 : medianCnt;
		if(medianCnt)
		{
			if(medianCnt > 5)
				medianCnt = 5;
			area = median(SL + 5 - medianCnt, medianCnt);
//			qDebug()<<"SL:"<<*SL<<*(SL+1)<<*(SL+2)<<*(SL+3)<<*(SL+4)<<"area:"<<area;
		}
	}
	else
	{
		medianCnt = 0;
	}
	double distance = 0;
	if(area)
		distance = sqrt(33 / area) * 50;
	double wp = ((double)P_x_e - (double)P_x_b);//光源在图像中的宽度
	double lp = dest_y;//光源中心距离图像中心的距离

	this->isDataUpdated = true;
//	caller->deltaDepth = distance * sin(caller->pos.pitch) - lp * 6 * cos(caller->pos.pitch) / wp;
	if(wp)
		this->deltaDepth = lp * 6 / wp;
	else
		this->deltaDepth = 0;
	this->isLightInSight = flag;
	this->posX = dest_x;
	this->posY = dest_y;
	this->distance = distance;
}
