#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <QString>
#include <QTime>
#include <QTimer>
#include "defines.h"
#include "structs.h"
#include "logpane.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

/*
 * 类Task：所有任务的基类
 * 定义了若干所有任务都会用到的信号和槽
 * 因为继承关系，所以只有在这个类中定义的信号和槽才能与外部（主线程）连接，子类中的不可以与外部（主线程）连接
 * 子类中应当重写本类中的虚函数以实现运行时的动态联编
 *
 * 如果涉及拷贝操作，记得要写拷贝构造函数，否则timer指针悬挂！
 * */
class Task : public QObject
{
	Q_OBJECT
public:
	explicit Task(int period, QObject *parent = 0): QObject(parent), ctrlPeriod(period)
	{
		pos.pitch = 0; pos.pitSpd = 0;
		pos.roll = 0; pos.rolSpd = 0;
		pos.yaw = 0; pos.yawSpd = 0;
	}
	virtual ~Task() {}
	virtual QString getTaskName() = 0;

	enum TaskEndStatus
	{
		nextOne = 0x0,
		thisAgain = 0x1,
		keepDepth = 0x2,
		keepHeading = 0x4,
		noMore = 0x8,
	};

	QTimer *timer = nullptr;
	unsigned int ctrlPeriod;//in ms
	double depth = 0;
	PostureData pos;

public slots:
	virtual void taskSetUp() = 0;
	virtual void setTaskPara() = 0;
	void depthSlot(double d){ depth = d; }
	void postureSlot(PostureData p){ pos = p; }

signals:
	void taskEndSgl(Task::TaskEndStatus);	//请求主线程销毁当前任务子线程
	void taskInfos(QString);						//在任务面板上显示相应的任务信息

	void operateLog(QString, LogPane::WarnLevel);	//操作日志（需要被转发）
	void taskGoSgl(int);
	void taskTurnSgl(int);
	void taskOffsetSgl(int);
	void taskDiveSgl(int);
	void autoPitchSwitch(bool);
	void autoDepthSwitch(bool);
	void setAutoDepth(double);
	void autoHeadingSwitch(bool);
	void setAutoHeading(double);
	void LEDFlash(LEDSetting);
	void videoImg(cv::Mat);							//在opencvPanel上显示Mat的图像（需要被转发）
	void videoImgIn(cv::Mat);

};

#endif // TASK_H
