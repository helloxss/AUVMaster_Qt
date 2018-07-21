#ifndef PROPELLER_H
#define PROPELLER_H

#include <QObject>
#include <QTimer>
#include "logpane.h"

class Propeller : public QObject
{
	Q_OBJECT
public:
	explicit Propeller(QObject *parent = 0);
	~Propeller();

signals:
	void order(QString);
	void relativeThrust(int t1, int t2, int t3, int t4, int t5, int t6);
	void operateLog(QString, LogPane::WarnLevel);

public slots:
	void autoDepthDelta(int lv)
	{
		autoDepthVert = lv;
	}

	void autoHeadingDelta(int lv)
	{
		autoHeadingHoriLv = lv;
	}

	void autoPitchDelta(int lv)
	{
		autoPitchVertLv = lv;
	}

	void mannualGoDelta(int m)
	{
		mannualGo = m;
	}

	void mannualTurnDelta(int h)
	{
		mannualTurn = h;
	}

	void mannualOffsetDelta(int h)
	{
		mannualOffset = h;
	}

	void taskDelta(int t[6])
	{
		memcpy(task, t, sizeof(int)*6);
	}

	void autoCtrlRunning(bool autoDepth, bool autoPitch)
	{
//		isTaskRunning = isStart;
	}

private slots:
	void orderTimerUpdate();

private:
	int autoDepthVert = 0;		//定深调整量
	int autoHeadingHoriLv = 0;	//定航调整量
	int autoPitchVertLv = 0;	//定俯仰前垂调整量
	int task[6];				//任务推进器调整量
	int mannualGo = 0;
	int mannualTurn = 0;
	int mannualOffset = 0;

	//以下：推进器值变量
	int leftMain = 0;
	int rightMain = 0;
	int backVert = 0;
	int backHori = 0;
	int frontHori = 0;
	int frontVert = 0;

	//以下：推进器死区值
	int leftMainLowDZ = 1488;
	int leftMainHighDZ = 1527;

	int rightMainLowDZ = 1492;
	int rightMainHighDZ = 1531;

	int backVertLowDZ = 1490;
	int backVertHighDZ = 1529;

	int backHoriLowDZ = 1487;
	int backHoriHighDZ = 1534;

	int frontHoriLowDZ = 1493;
	int frontHoriHighDZ = 1535;

	int frontVertLowDZ = 1491;
	int frontVertHighDZ = 1530;

//	int CThrusters::frontVDeadZone[2] = { 1530, 1491 };//
//	int CThrusters::backVDeadZone[2] = { 1529, 1490 };//
//	int CThrusters::frontHDeadZone[2] = { 1535, 1493 };//
//	int CThrusters::backHDeadZone[2] = { 1534, 1487 };//
//	int CThrusters::rightMDeadZone[2] = { 1531, 1492 };
//	int CThrusters::leftMDeadZone[2] = { 1527, 1488 };

	//以下：推进器限幅值
	int Vdmax = 200;
	int Vdmin = 200;
	int Hdmax = 200;
	int Hdmin = 200;
	int Mdmax = 200;
	int Mdmin = 200;

	bool isTaskRunning = false;

	QTimer *orderTimer = nullptr;

	void checkLimit();
//	void setLimit()
};

#endif // PROPELLER_H
