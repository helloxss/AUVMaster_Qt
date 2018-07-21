#include "switchled.h"
#include "QDebug"

SwitchLED::SwitchLED(QObject *parent) : QObject(parent)
{
	emptyListSetting.colorList.append(QColor(255,0,0));
	emptyListSetting.colorList.append(QColor(0,255,0));
	emptyListSetting.colorList.append(QColor(0,0,255));

	LEDTimer = new QTimer(this);
	connect(LEDTimer, &QTimer::timeout, this, &SwitchLED::LEDTimerUpdate);
	LEDTimer->start(1000);
}

SwitchLED::~SwitchLED()
{

}

void SwitchLED::propSwitchSlot(bool isOn)
{
	if(isOn)
	{
		emit switchOrder("D7@1#");
	}
	else
	{
		emit switchOrder("D7@0#");
	}
}

void SwitchLED::camSwitchSlot(bool isOn)
{
	if(isOn)
	{
		emit switchOrder("D8@1#");
	}
	else
	{
		emit switchOrder("D8@0#");
	}
}

void SwitchLED::setLED(LEDSetting s)
{
	listIndex = 0;
	if(list.isEmpty())//如果为空，马上响应
	{
		list.append(s);
		LEDTimerUpdate();
	}
	else
		list.append(s);
}

void SwitchLED::LEDTimerUpdate()
{
	if(list.isEmpty())
	{
		LEDTimer->setInterval(1000);
		if(listIndex >= (unsigned int/*消警告*/)emptyListSetting.colorList.size())
			listIndex = 0;

		qDebug()<<"emptyList:"<<emptyListSetting.colorList.at(listIndex);
		sendColorOrder(emptyListSetting.colorList.at(listIndex));
		++listIndex;
	}
	else
	{
		static unsigned int flashedTimes = 0;
		LEDTimer->setInterval(list.at(0).changeTime);
		if(listIndex >= (unsigned int/*消警告*/)list.at(0).colorList.size())
		{
			listIndex = 0;
			++flashedTimes;
		}
		if(flashedTimes > list.at(0).flashTimes - 1)
		{
			list.removeFirst();
			flashedTimes = 0;
			LEDTimerUpdate();
			return;
		}

		qDebug()<<"flashedTimes:"<<flashedTimes<<"interval:"<<LEDTimer->interval()<<list.at(0).colorList.at(listIndex);
		sendColorOrder(list.at(0).colorList.at(listIndex));
		++listIndex;
	}
}

void SwitchLED::sendColorOrder(QColor c)
{
	if(c.red())
		emit LEDOrder("L@R1#");
	else
		emit LEDOrder("L@R0#");

	if(c.green())
		emit LEDOrder("L@G1#");
	else
		emit LEDOrder("L@G0#");

	if(c.blue())
		emit LEDOrder("L@B1#");
	else
		emit LEDOrder("L@B0#");
}
