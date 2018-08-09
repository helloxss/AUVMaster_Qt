﻿#ifndef STRUCTS_H
#define STRUCTS_H

#include <QColor>
#include <QList>

struct PostureData
{
	double yaw;
	double pitch;
	double roll;

	double yawSpd;
	double pitSpd;
	double rolSpd;
};

struct LEDSetting
{
	QList<QColor> colorList;
	unsigned int changeTime;//changeTime毫秒后，切换到colorlist的下一个颜色
	unsigned int flashTimes;//LED变换次数
};

#endif // STRUCTS_H
