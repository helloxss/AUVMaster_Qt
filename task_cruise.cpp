#include "task_cruise.h"
#include "funcs.h"

QString Task_Cruise::taskName = QSL("巡航任务");

Task_Cruise::Task_Cruise(int period, QObject *parent): Task(period, parent)
{

}

Task_Cruise::~Task_Cruise()
{
	if(timer)
		delete timer;
}

void Task_Cruise::setTaskPara()
{

}

void Task_Cruise::taskSetUp()
{	
	//巡航任务目标列表
	cruiseSettings s;
	s.depth = 50; s.isRelativeTurn = false; s.heading = pos.yaw; s.goLevel = 100; s.runTime = 10;//保持当前航向，在50cm深度，以100主推跑10s
	settingsList.append(s);
	s.depth = 70; s.isRelativeTurn = true; s.heading = 90; s.goLevel = 100; s.runTime = 10;//在上一个航向的基础上右转90度，在70cm深度，以100主推跑10s
	settingsList.append(s);
	s.depth = 100; s.isRelativeTurn = true; s.heading = 90; s.goLevel = 100; s.runTime = 10;//在上一个航向的基础上右转90度，在100cm深度，以100主推跑10s
	settingsList.append(s);
	s.depth = 30; s.isRelativeTurn = true; s.heading = 90; s.goLevel = 100; s.runTime = 10;//在上一个航向的基础上右转90度，在30cm深度，以100主推跑10s
	settingsList.append(s);


	//第一个要执行的函数
	if(settingsList.isEmpty())
	{
		emit operateLog(QSL("%1：空的巡航任务目标列表").arg(taskName), LogPane::err);
		emit taskEndSgl(nextOne);
		return;
	}
	pCtrlFunc = &Task_Cruise::ctrlSetDepthYaw;
	pCtrlFuncPara = (void*)(&(settingsList.first()));

	//控制定时器
	timer = new QTimer(this);
	if(!timer)
	{
		emit operateLog(QSL("%1：控制定时器创建失败").arg(taskName), LogPane::err);
		emit taskEndSgl(Task::noMore);
		return;
	}
	connect( timer, &QTimer::timeout, this, &Task_Cruise::ctrlTimerUpdate );
	timer->start(ctrlPeriod);

	//开始计时
	elapseTime.start();

	//安保定时器
	unsigned int allTaskTime = 0;
	foreach (cruiseSettings s, settingsList) {
		allTaskTime += s.runTime;
	}
	if(allTaskTime > securityTimerConst)
		emit operateLog(QSL("%1：安保定时器时间(%2s)小于所有目标运行时间之和(%3s)").arg(taskName).arg(securityTimerConst).arg(allTaskTime), LogPane::warn);
	QTimer::singleShot(securityTimerConst*1000, this, \
					   [this]{ emit operateLog((QSL("%1：安保定时时间到")).arg(taskName), LogPane::warn); emit taskEndSgl(Task::nextOne);});
}

void Task_Cruise::ctrlTimerUpdate()
{
	(this->*pCtrlFunc)(pCtrlFuncPara);
	emit taskInfos(infoStr());
}

QString Task_Cruise::infoStr()
{
	return	QSL("任务时间：%1s\n任务状态：%2\n%3\n").arg((double)elapseTime.elapsed() / 1000).arg(taskStatus).arg(extraInfo);
}

void *Task_Cruise::ctrlSetDepthYaw(void *settings)
{
	cruiseSettings *s = (cruiseSettings *)settings;
	taskStatus = QSL("%1\n目标深度%2cm，航向%3°，运行时间%4s").arg((QString)__FUNCTION__).arg(s->depth).arg(s->heading).arg(s->runTime);

	if(settings)
	{
		if(s->isRelativeTurn)
			emit operateLog(QSL("%1：目标深度%2cm，航向相对于上个目标%3°，运行时间%4s").arg(taskName).arg(s->depth).arg(s->heading).arg(s->runTime), LogPane::info);
		else
			emit operateLog(QSL("%1：目标深度%2cm，航向%3°，运行时间%4s").arg(taskName).arg(s->depth).arg(s->heading).arg(s->runTime), LogPane::info);

		emit taskGoSgl(0);
		emit autoDepthSwitch(true);
		emit autoPitchSwitch(true);
		emit setAutoDepth(s->depth);
		emit autoHeadingSwitch(true);
		if(s->isRelativeTurn)
		{
			s->heading += lastHeading;
		}
		emit setAutoHeading(s->heading);
		lastHeading = s->heading;
		pCtrlFunc = &Task_Cruise::ctrlWaitForAdjust;
		LEDSetting l; l.flashTimes = 0; l.changeTime = 250;
		l.colorList<<QColor(0, 0, 255)<<QColor(0, 0, 0);
		emit LEDFlash(l);
	}
	return nullptr;
}

void *Task_Cruise::ctrlWaitForAdjust(void *settings)
{
	cruiseSettings *s = (cruiseSettings *)settings;
	taskStatus = QSL("%1\n目标深度%2cm，航向%3°，运行时间%4s").arg((QString)__FUNCTION__).arg(s->depth).arg(s->heading).arg(s->runTime);

	if(settings)
	{
		(abs(depth - s->depth) <= 5 && abs(signedDeltaAngle(pos.yaw, s->heading)) <= 5) ? adjustOkTime += ctrlPeriod : adjustOkTime = 0;
//		adjustOkTime += ctrlPeriod;

		extraInfo = QSL("深度航向达标时间：%1s(/%2s)").arg((double)adjustOkTime/1000). arg(5);

		if(adjustOkTime >= 5000)
		{
			emit operateLog(QSL("%1：深度、航向达标时间达到5s，开始前进%2s").arg(taskName).arg(s->runTime), LogPane::info);
			adjustOkTime= 0;
			pCtrlFunc = &Task_Cruise::ctrlGo;
			LEDSetting s; s.flashTimes = 0; s.changeTime = 250;
			s.colorList<<QColor(0, 255, 0)<<QColor(0, 0, 0);
			emit LEDFlash(s);
		}
	}
	return nullptr;
}

void *Task_Cruise::ctrlGo(void *settings)
{
	cruiseSettings *s = (cruiseSettings *)settings;
	taskStatus = QSL("%1\n目标深度%2cm，航向%3°，运行时间%4s").arg((QString)__FUNCTION__).arg(s->depth).arg(s->heading).arg(s->runTime);

	if(settings)
	{
		emit taskGoSgl(s->goLevel);
		goTime += ctrlPeriod;

		extraInfo = QSL("前进时间：%1s").arg((double)goTime/1000);

		if(goTime >= s->runTime *1000)
		{
			goTime = 0;
			if(settingsList.count() > 1)//如果设置列表中还有剩余项，重新执行ctrlSetDepthYaw
			{
				settingsList.removeFirst();
				pCtrlFuncPara = &settingsList.first();
				pCtrlFunc = &Task_Cruise::ctrlSetDepthYaw;
			}
			else
			{
				emit taskGoSgl(0);
				emit taskEndSgl((TaskEndStatus)(nextOne | keepDepth));
			}
		}
	}
	return nullptr;
}
