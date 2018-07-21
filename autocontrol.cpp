#include "autocontrol.h"
#include <QThread>
#include "defines.h"

AutoControl::AutoControl(QObject *parent) : \
    QObject(parent), \
    autoDepthPID(autoDepthKp, autoDepthTi, autoDepthTd, autoDepthThreshould), \
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
	//获取两次接受数据的间隔时间
	double duration = (double)autoDepthTime.elapsed() / 1000;
	autoDepthTime.restart();

	//计算控制并发出信号
	if(isAutoDepth)
	{
//		qDebug()<<"depth control in "<<duration<<"s";
		emit autoDepthProp((int)(autoDepthPID.PIDCtrl(autoDepthTgt - d, duration)));
	}
	else
		emit autoDepthProp(0);
}

void AutoControl::postureData(PostureData p)
{
	if(isAutoPitch)
		emit autoPitchProp((int)(autoPitchPID.PDCtrlGivenDiff(p.pitch, p.pitSpd)));
	else
		emit autoPitchProp(0);

	if(isAutoHeading)
		emit autoHeadingProp(
				(int)autoPitchPID.PDCtrlGivenDiff(signedDeltaAngle(autoHeadingTgt, p.yaw), p.yawSpd));
	else
		emit autoHeadingProp(0);

}

double AutoControl::signedDeltaAngle(double tgt, double base)
{
	while (tgt < 0)
		tgt += 360;
	while (base < 0)
		base += 360;

	double delta = tgt - base;

	while (delta > 180)
		delta -= 360;
	while (delta < -180)
		delta += 360;
	return delta;
}

