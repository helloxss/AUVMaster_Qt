#include "propeller.h"
#include "defines.h"

Propeller::Propeller(QObject *parent) :
	QObject(parent)
{
	orderTimer = new QTimer(this);
	if(orderTimer)
		connect(orderTimer, &QTimer::timeout, this, &Propeller::orderTimerUpdate);

}

Propeller::~Propeller()
{

}

void Propeller::mannualGoDelta(int m)
{
	mannualGo = m;

	if(!isAutomatic)
	{
		leftMain = mannualGo;
		rightMain = mannualGo;
		backVert = 0;
		backHori = -mannualTurn + mannualOffset;
		frontHori = mannualTurn + mannualOffset;
		frontVert = 0;

		checkLimit();
		emit order(propOrderStr());
		emit relativeThrust(leftMain, rightMain, backVert, backHori, frontHori, frontVert);
	}
}

void Propeller::mannualTurnDelta(int h)
{
	mannualTurn = h;

	if(!isAutomatic)
	{
		leftMain = mannualGo;
		rightMain = mannualGo;
		backVert = 0;
		backHori = -mannualTurn + mannualOffset;
		frontHori = mannualTurn + mannualOffset;
		frontVert = 0;

		checkLimit();
		emit order(propOrderStr());
		emit relativeThrust(leftMain, rightMain, backVert, backHori, frontHori, frontVert);
	}
}

void Propeller::mannualOffsetDelta(int h)
{
	mannualOffset = h;

	if(!isAutomatic)
	{
		leftMain = mannualGo;
		rightMain = mannualGo;
		backVert = 0;
		backHori = -mannualTurn + mannualOffset;
		frontHori = mannualTurn + mannualOffset;
		frontVert = 0;

		checkLimit();
		emit order(propOrderStr());
		emit relativeThrust(leftMain, rightMain, backVert, backHori, frontHori, frontVert);
	}
}

void Propeller::autoCtrlRunning(unsigned char b)
{
	isAutomatic = b;

	if((b & 0x1) == 0)//定深未开启
	{
		autoDepthVert = 0;
	}
	if((b & 0x2) == 0)//定俯仰未开启
	{
		autoPitchVertLv = 0;
	}
	if((b & 0x4) == 0)
	{
		autoHeadingHoriLv = 0;
	}
	if((b & 0x8) == 0)
	{
		taskGo = taskDive = taskTurn = taskOffset = 0;
	}

	if(orderTimer)
	{
		if(b)
		{
			if(!orderTimer->isActive())
				orderTimer->start(100);
		}
		else
		{
			orderTimer->stop();

			leftMain = mannualGo;
			rightMain = mannualGo;
			backVert = 0;
			backHori = -mannualTurn + mannualOffset;
			frontHori = mannualTurn + mannualOffset;
			frontVert = 0;

			checkLimit();
			emit order(propOrderStr());
			emit relativeThrust(leftMain, rightMain, backVert, backHori, frontHori, frontVert);
		}
	}
}

void Propeller::orderTimerUpdate()
{
	leftMain = taskGo + mannualGo;
	rightMain = taskGo + mannualGo;
	backVert = taskDive + autoDepthVert - autoPitchVertLv;
	backHori = - taskTurn + taskOffset - autoHeadingHoriLv - mannualTurn + mannualOffset;
	frontHori = taskTurn + taskOffset + autoHeadingHoriLv + mannualTurn + mannualOffset;
	frontVert = taskDive + autoDepthVert + autoPitchVertLv ;

	checkLimit();
	emit order(propOrderStr());
	emit relativeThrust(leftMain, rightMain, backVert, backHori, frontHori, frontVert);
}

QString Propeller::propOrderStr()
{
	QString orderStr;
	if(leftMain > 0)
		orderStr += QString("#1P%1").arg(leftMainHighDZ + leftMain);
	else if(leftMain < 0)
		orderStr += QString("#1P%1").arg(leftMainLowDZ + leftMain);
	else
		orderStr += QString("#1P1500");

	if(rightMain > 0)
		orderStr += QString("#2P%1").arg(rightMainHighDZ + rightMain);
	else if(rightMain < 0)
		orderStr += QString("#2P%1").arg(rightMainLowDZ + rightMain);
	else
		orderStr += QString("#2P1500");

	if(backVert > 0)
		orderStr += QString("#3P%1").arg(backVertHighDZ + backVert);
	else if(backVert < 0)
		orderStr += QString("#3P%1").arg(backVertLowDZ + backVert);
	else
		orderStr += QString("#3P1500");

	if(backHori > 0)
		orderStr += QString("#4P%1").arg(backHoriHighDZ + backHori);
	else if(backHori < 0)
		orderStr += QString("#4P%1").arg(backHoriLowDZ + backHori);
	else
		orderStr += QString("#4P1500");

	if(frontHori > 0)
		orderStr += QString("#5P%1").arg(frontHoriHighDZ + frontHori);
	else if(frontHori < 0)
		orderStr += QString("#5P%1").arg(frontHoriLowDZ + frontHori);
	else
		orderStr += QString("#5P1500");

	if(frontVert > 0)
		orderStr += QString("#6P%1").arg(frontVertHighDZ + frontVert);
	else if(frontVert < 0)
		orderStr += QString("#6P%1").arg(frontVertLowDZ + frontVert);
	else
		orderStr += QString("#6P1500");

	orderStr += "T200\r\n";

//	qDebug()<<orderStr;

	return orderStr;
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
