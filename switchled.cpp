#include "switchled.h"
#include "QDebug"

SwitchLED::SwitchLED(QObject *parent) : QObject(parent)
{
	emptyListSetting.colorList << (QColor(255,0,0)) << (QColor(0,255,0)) << (QColor(0,0,255));
	emptyListSetting.changeTime = 1000;
	emptyListSetting.flashTimes = 0;
	LEDTimer = new QTimer(this);
	connect(LEDTimer, &QTimer::timeout, this, &SwitchLED::LEDTimerUpdate);
	LEDTimer->start(emptyListSetting.changeTime);
}

SwitchLED::~SwitchLED()
{

}

void SwitchLED::propSwitchSlot(bool isOn)
{
	if(isOn)
	{
		isSwitchOn |= 0x1;
		emit switchOrder("D7@1#");
	}
	else
	{
		isSwitchOn &= ~0x1;
		emit switchOrder("D7@0#");
	}
}

void SwitchLED::camSwitchSlot(bool isOn)
{
	if(isOn)
	{
		isSwitchOn |= 0x2;
		emit switchOrder("D8@1#");
	}
	else
	{
		isSwitchOn &= ~0x2;
		emit switchOrder("D8@0#");
	}
}

void SwitchLED::setLED(LEDSetting s)
{
	listIndex = 0;
	if(list.isEmpty())//如果列表为空，马上响应
	{
		list.append(s);
		LEDTimerUpdate();
	}
	else
		list.append(s);
}

void SwitchLED::LEDTimerUpdate()
{
	static bool isFirstTimeEmpty = true;//是否第一次列表为空

	if(list.isEmpty())//空列表时：闪烁emptyListSetting
	{
		if(isFirstTimeEmpty)
		{
			isFirstTimeEmpty = false;
			emit LEDListEmpty();
		}

		LEDTimer->setInterval(emptyListSetting.changeTime);
		if(listIndex >= (unsigned int/*消警告*/)emptyListSetting.colorList.size())
			listIndex = 0;

//		qDebug()<<"emptyList:"<<emptyListSetting.colorList.at(listIndex);
		sendColorOrder(emptyListSetting.colorList.at(listIndex));
		++listIndex;
	}
	else//列表非空：根据列表首元素的flashTimes进行闪烁
	{
		isFirstTimeEmpty = true;
		static unsigned int flashedTimes = 0;

		if(list.at(0).flashTimes != 0)
		{

			LEDTimer->setInterval(list.at(0).changeTime);

			//重置游标
			if(listIndex >= (unsigned int/*消警告*/)list.at(0).colorList.size())
			{
				listIndex = 0;
				++flashedTimes;
				//删除首元素
				if(flashedTimes >= list.at(0).flashTimes)
				{
					list.removeFirst();
					flashedTimes = 0;
					listIndex = 0;
					return;
				}
			}

			//发送指令
//			qDebug()<<"flashTimes:"<<list.at(0).flashTimes<<"flashedTimes:"<<flashedTimes<<"interval:"<<LEDTimer->interval()<<list.at(0).colorList.at(listIndex);
			sendColorOrder(list.at(0).colorList.at(listIndex));
			++listIndex;


		}//if(list.at(0).flashTimes != 0)
		else
		{
			LEDTimer->setInterval(list.at(0).changeTime);

			//重置游标
			if(listIndex >= (unsigned int/*消警告*/)list.at(0).colorList.size())
			{
				listIndex = 0;
				++flashedTimes;
			}

			//发送指令
//			qDebug()<<"0-flashTimes:"<<list.at(0).flashTimes<<"flashedTimes:"<<flashedTimes<<"interval:"<<LEDTimer->interval()<<list.at(0).colorList.at(listIndex);
			sendColorOrder(list.at(0).colorList.at(listIndex));
			++listIndex;

			//删除首元素
			if(list.size() > 1 && flashedTimes)
			{
				list.removeFirst();
				flashedTimes = 0;
				listIndex = 0;
			}
		}//if(list.at(0).flashTimes == 0)
	}//if(!list.isemtpy())
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
