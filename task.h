#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <QString>
#include <QTime>
#include <QTimer>
#include "defines.h"
#include "structs.h"
#include "logpane.h"

/*
 * 类Task：所有任务的基类
 * 定义了若干所有任务都会用到的信号和槽
 * 因为继承关系，所以只有在这个类中定义的信号和槽才能与外部（主线程）连接，子类中的不可以与外部（主线程）连接
 * 子类中应当重写本类中的虚函数以实现运行时的动态联编
 * */
class Task : public QObject
{
	Q_OBJECT
public:
	explicit Task(int period, QObject *parent = 0);
	virtual ~Task();

//	TaskType whichTask;
	QString TaskName;
	QTime elapseTime;
	QTimer *timer;
	unsigned int ctrlPeriod;//in ms

protected:
	bool isCamThreadRun = false;
	volatile bool isDataUpdated = false;
	QMutex mutex;

	void taskCompleteEmit(bool isSucceed);//在需要结束当前任务时调用

signals:
	void taskCompletedSignal(bool);	//该信号请求主线程销毁当前任务子线程。true参数请求主线程执行下一个任务，false请求主线程停止。
	void operateLog(QString, LogPane::WarnLevel);		//该信号记录操作日志
	void updateUIData(QString);		//该信号在任务面板上显示相应的任务信息
	void order(QString);			//该信号向下位机发送指令
//	void videoImg(cv::Mat);			//该信号在opencvPanel上显示Mat的图像

protected slots:
	virtual void beginTaskControl();//虚开始线程函数
	virtual void depthSensorSlot(double d);//虚深度传感器数据槽函数，等待子类重写
	virtual void postureSensorSlot(PostureData p);//虚姿态传感器数据槽函数

};

#endif // TASK_H
