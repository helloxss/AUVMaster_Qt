#include "autocontrol.h"
#include <QThread>
#include "defines.h"

AutoControl::AutoControl(QObject *parent) : \
    QObject(parent), \
	autoDepthPID(autoDepthKp, autoDepthTi, autoDepthTd/*, autoDepthThreshould*/), \
	autoHeadingPID(autoHeadingKp, autoHeadingTi, autoHeadingTd), \
	autoPitchPID(autoPitchKp, autoPitchTi, autoPitchTd)
{

}

AutoControl::~AutoControl()
{

}

void AutoControl::create()
{
	autoDepthTime.start();
}

void AutoControl::depthData(double d)
{
	//计算控制并发出信号
	static double lastDepth = 0;
	if(isAutoDepth)
	{
//		qDebug()<<(QString)__FUNCTION__<<(d-lastDepth) / 0.1;
		emit autoDepthProp((int)(autoDepthPID.PIDCtrlGivenDiff(autoDepthTgt - d, (lastDepth - d) / 0.1, 0.1)));
//		emit autoDepthProp((int)(autoDepthPID.PIDCtrl(autoDepthTgt - d, 0.1)));
		lastDepth = d;
	}
	else
	{
		lastDepth = 0;
		emit autoDepthProp(0);
	}
}

void AutoControl::postureData(PostureData p)
{
	if(isAutoPitch)
		emit autoPitchProp((int)(autoPitchPID.PDCtrlGivenDiff(p.pitch, p.pitSpd)));
	else
		emit autoPitchProp(0);

	if(isAutoHeading)
		emit autoHeadingProp(
				(int)autoHeadingPID.PDCtrlGivenDiff(signedDeltaAngle(autoHeadingTgt, p.yaw), p.yawSpd));
	else
		emit autoHeadingProp(0);
}
