#ifndef TASK_CRUISE_H
#define TASK_CRUISE_H

#include "task.hpp"

class Task_Cruise : public Task
{
public:
	Task_Cruise(int period, QObject *parent = 0);
	virtual ~Task_Cruise();
	static QString taskName;

	const unsigned int securityTimerConst = 300;//安保定时器定时时间 in second
	struct cruiseSettings
	{
		unsigned int runTime;//in second
		unsigned int depth;
		int goLevel;
		bool isRelativeTurn;
		double heading;//in degree
	};

	QList<cruiseSettings> settingsList;

public slots:
	virtual void setTaskPara() Q_DECL_OVERRIDE;
	virtual void taskSetUp() Q_DECL_OVERRIDE;//虚开始线程函数
	virtual QString getTaskName() Q_DECL_OVERRIDE { return taskName; }

private slots:
	void ctrlTimerUpdate();

private:
	void * (Task_Cruise:: * pCtrlFunc)(void *) = nullptr;//返回值是void *，参数是void *的task_curise成员函数的指针
	void * pCtrlFuncPara = 0;

	inline QString infoStr();
	QString extraInfo;
	QString taskStatus;
	QTime elapseTime;

	//控制目标函数
	void *ctrlSetDepthYaw(void *settings);
	double lastHeading = 0;

	void *ctrlWaitForAdjust(void *settings);
	unsigned int adjustOkTime = 0;

	void *ctrlGo(void *settings);
	unsigned int goTime = 0;
};

#endif // TASK_CRUISE_H
