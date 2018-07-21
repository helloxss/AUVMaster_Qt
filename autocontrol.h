#ifndef AUTOCONTROL_H
#define AUTOCONTROL_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include "sepintegralPID.h"
#include "structs.h"
#include "logpane.h"

class AutoControl : public QObject
{
	Q_OBJECT
public:
	explicit AutoControl(QObject *parent = 0);
	~AutoControl();

signals:
	void autoDepthProp(int lv);
	void autoDepthOK();
	void autoHeadingProp(int lv);
	void autoHeadingOK();
	void autoPitchProp(int lv);
	void operateLog(QString, LogPane::WarnLevel level);

public slots:
	void create();

	void depthData(double d);
	void postureData(PostureData p);

	void autoDepthSwitch(bool b) {
		isAutoDepth = b;
		if(b)
		{
			autoDepthPID.reset();
		}
	}
	void autoHeadingSwitch(bool b) {
		isAutoHeading = b;
		if(b)
			autoHeadingPID.reset();
	}
	void autoPitchSwitch(bool b) {
		isAutoPitch = b;
		if(b)
			autoPitchPID.reset();
	}

	void setDepthTgt(double d) {autoDepthTgt = d;}
	void setHeadingTgt(double h) {autoHeadingTgt = h;}
	void setPitchTgt(double p) {autoPitchTgt = p;}

private:
	bool isAutoDepth = false;
	double autoDepthTgt = 0;
	double autoDepthKp = 4, autoDepthTi = 10, autoDepthTd = 0.8, autoDepthThreshould = 100;
	SepIntegralPID autoDepthPID;

	bool isAutoHeading = false;
	double autoHeadingTgt = 0;
	double autoHeadingKp = 6, autoHeadingTi = 10000, autoHeadingTd = 0.33;
	NormalPID autoHeadingPID;

	bool isAutoPitch = false;
	double autoPitchTgt = 0;
	double autoPitchKp = 8, autoPitchTi = 10000, autoPitchTd = 0.16;
	NormalPID autoPitchPID;

	QTime autoDepthTime;

	double signedDeltaAngle(double tgt, double base);
};

#endif // AUTOCONTROL_H
