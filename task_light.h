#ifndef TASK_COLOR_H
#define TASK_COLOR_H

#include "task.hpp"
#include "normalPID.h"
#include "MVGigE.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

//寻光任务类
/* 灯光：
 * 等待定深、等待寻找目标：0,0,255 - 0,255,0 0.5秒，持续
 * 找到目标：0,0,255 - 0,255,0 0.25秒，持续
*/
class Task_Light : public Task
{
public:
	Task_Light(int period, QObject *parent = 0);
	virtual ~Task_Light();

	//新建任务时，以下内容按原样复制================
public:
	static QString taskName;

	const unsigned int securityTimerConst = 300;//安保定时器定时时间 in second

public slots:
	virtual void setTaskPara() Q_DECL_OVERRIDE;
	virtual void taskSetUp() Q_DECL_OVERRIDE;//虚开始线程函数
	virtual QString getTaskName() Q_DECL_OVERRIDE { return taskName; }//通过task指针访问子类名称时使用

private slots:
	void ctrlTimerUpdate();

private:
	QString infoStr() { return QSL("任务时间：%1s\n任务状态：%2\n%3\n").arg((double)elapseTime.elapsed() / 1000).arg(taskStatus).arg(extraInfo); }
	QString extraInfo;
	QString taskStatus;
	QTime elapseTime;
	//以上内容在新建任务时原样复制，函数在cpp中添加定义

	void * (Task_Light:: *pCtrlFunc)(void *) = nullptr;
	void * pCtrlFuncPara = 0;
	bool isCamThreadRun = false;
	volatile bool isDataUpdated = false;

	//任务参数=====================================================
	const int findTargetTurnLvConst = 50;

	//控制目标函数=================================================
	void *ctrlSetSearchDepth(void *);//定深
	double searchDepth = 20;

	void *ctrlWaitForDepthOK(void *);
	unsigned int depthOKTime = 0;

	void *ctrlFindTarget(void *);
	double startYaw = 0;

	void *ctrlWaitForFindTarget(void *);
	unsigned char roundDetect = 0;

	void *ctrlChaseTarget(void *);
	NormalPID GoCtrl;//前进控制PID
	NormalPID XAdjustCtrl;//横向控制PID
	unsigned int shockTime = 0;
	unsigned int depthCtrlAreaTime = 0;
	bool isCheasingLEDSet = false;
	bool isShockingLEDSet = false;
	bool isOutDepthAreaOkDepthEmitted = false;//深度持续控制区外，画面中满足条件的深度已发送的标志

	//与图像线程共享的变量=========================================
	volatile bool isLightInSight = false;
	volatile bool isWholeInSight = false;
	volatile double posX = 0;
	volatile double posY = 0;
	volatile double distance = 0;
	volatile double deltaDepth = 0;

private slots:
	//视觉处理函数
	void camIdentifyTarget(cv::Mat src);
	double SL[5];
	unsigned int medianCnt = 0;
};

#endif // TASK_LIGHT_H
