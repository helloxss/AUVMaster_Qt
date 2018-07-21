#ifndef STRUCTS_H
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
	unsigned int changeTime;//LED变换一次的时间
	unsigned int flashTimes;//LED变换次数
};

#endif // STRUCTS_H
