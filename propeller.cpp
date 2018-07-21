#include "propeller.h"
#include "defines.h"

Propeller::Propeller(QObject *parent) :
	QObject(parent)
{
	memset(task, 0, sizeof(int)*6);
	orderTimer = new QTimer(this);
	if(orderTimer)
	{
		connect(orderTimer, &QTimer::timeout, this, &Propeller::orderTimerUpdate);
		orderTimer->start(100);
	}
}

Propeller::~Propeller()
{

}

void Propeller::orderTimerUpdate()
{
	leftMain = task[0] + mannualGo;
	rightMain = task[1] + mannualGo;
	backVert = task[2] + autoDepthVert + autoPitchVertLv;
	backHori = task[3] + autoHeadingHoriLv + mannualTurn + mannualOffset;
	frontHori = task[4] - autoHeadingHoriLv - mannualTurn + mannualOffset;
	frontVert = task[5] + autoDepthVert - autoPitchVertLv;

	checkLimit();

	QString orderStr;
	if(leftMain > 0)
		orderStr += QSL("#1P%1").arg(leftMainHighDZ + leftMain);
	else if(leftMain < 0)
		orderStr += QSL("#1P%1").arg(leftMainLowDZ + leftMain);
	else
		orderStr += QSL("#1P1500");

	if(rightMain > 0)
		orderStr += QSL("#2P%1").arg(rightMainHighDZ + rightMain);
	else if(rightMain < 0)
		orderStr += QSL("#2P%1").arg(rightMainLowDZ + rightMain);
	else
		orderStr += QSL("#2P1500");

	if(backVert > 0)
		orderStr += QSL("#3P%1").arg(backVertHighDZ + backVert);
	else if(backVert < 0)
		orderStr += QSL("#3P%1").arg(backVertLowDZ + backVert);
	else
		orderStr += QSL("#3P1500");

	if(backHori > 0)
		orderStr += QSL("#4P%1").arg(backHoriHighDZ + backHori);
	else if(backHori < 0)
		orderStr += QSL("#4P%1").arg(backHoriLowDZ + backHori);
	else
		orderStr += QSL("#4P1500");

	if(frontHori > 0)
		orderStr += QSL("#5P%1").arg(frontHoriHighDZ + frontHori);
	else if(frontHori < 0)
		orderStr += QSL("#5P%1").arg(frontHoriLowDZ + frontHori);
	else
		orderStr += QSL("#5P1500");

	if(frontVert > 0)
		orderStr += QSL("#6P%1").arg(frontVertHighDZ + frontVert);
	else if(frontVert < 0)
		orderStr += QSL("#6P%1").arg(frontVertLowDZ + frontVert);
	else
		orderStr += QSL("#6P1500");

	emit order(orderStr);
	emit relativeThrust(leftMain, rightMain, backVert, backHori, frontHori, frontVert);
}

void Propeller::checkLimit()
{
	frontVert > Vdmax ?  frontVert = Vdmax : (frontVert < -Vdmin ? frontVert = -Vdmin : 0);
	frontHori > Hdmax ? frontHori =Hdmax : (frontHori < - Hdmin ? frontHori = - Hdmin : 0);
	backHori > Hdmax ? backHori = Hdmax : (backHori < - Hdmin ? backHori = - Hdmin : 0);
	backVert > Vdmax ? backVert = Vdmax : (backVert < - Vdmin ? backVert = - Vdmin : 0);
	leftMain > Mdmax ? leftMain = Mdmax : (leftMain < - Mdmin ? leftMain = - Mdmin : 0);
	rightMain > Mdmax ? rightMain = Mdmax : (rightMain < - Mdmin ? rightMain = - Mdmin : 0);
}
