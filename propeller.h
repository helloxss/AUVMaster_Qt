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
	void autoDepthDelta(int lv) { autoDepthVert = lv; }
	void autoHeadingDelta(int lv) { autoHeadingHoriLv = lv; }
	void autoPitchDelta(int lv) { autoPitchVertLv = lv; }
	void mannualGoDelta(int m);
	void mannualTurnDelta(int h);
	void mannualOffsetDelta(int h);
	void taskTurnDelta(int lv) { taskTurn = lv; }
	void taskOffsetDelta(int lv) {taskOffset = lv; }
	void taskGoDelta(int lv) { taskGo = lv; }
	void taskDiveDelta(int lv){ taskDive = lv; }
	void autoCtrlRunning(unsigned char b);//自动控制开启后，propeller自动地发送推进器指令。未开启自动控制，则手动操作后发送推进器指令

	private slots:
	void orderTimerUpdate();

private:
	int autoDepthVert = 0;		//定深调整量
	int autoHeadingHoriLv = 0;	//定航调整量
	int autoPitchVertLv = 0;	//定俯仰前垂调整量
	int taskTurn = 0;
	int taskOffset = 0;
	int taskGo = 0;
	int taskDive = 0;
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
	const int leftMainLowDZ = 1488;
	const int leftMainHighDZ = 1527;

	const int rightMainLowDZ = 1492;
	const int rightMainHighDZ = 1531;

	const int backVertLowDZ = 1490;
	const int backVertHighDZ = 1529;

	const int backHoriLowDZ = 1487;
	const int backHoriHighDZ = 1534;

	const int frontHoriLowDZ = 1493;
	const int frontHoriHighDZ = 1535;

	const int frontVertLowDZ = 1491;
	const int frontVertHighDZ = 1530;

	//以下：推进器限幅值
	int Vdmax = 200;
	int Vdmin = 200;
	int Hdmax = 200;
	int Hdmin = 200;
	int Mdmax = 200;
	int Mdmin = 200;

	bool isAutomatic = false;

	QTimer *orderTimer = nullptr;

	QString propOrderStr();
	void checkLimit();
	//	void setLimit()
};

#endif // PROPELLER_H
