#ifndef SWITCHLED_H
#define SWITCHLED_H

#include <QObject>
#include <QString>
#include <QList>
#include <QColor>
#include <QTimer>
#include "logpane.h"
#include "structs.h"

/*
 * LED说明：
 * 1.list为空时：R - G - B循环，1秒间隔
 * 2.开启推进器：0,255,0 - 0,0,0 循环，0.333秒间隔，循环3次；关闭推进器：0,255,0 - 0,0,0 循环，0.5秒间隔，循环2次
 * 3.开启相机：0,0,255 - 0,0,0 循环，0.333秒间隔，循环3次；关闭相机：0,0,255 - 0,0,0 循环，0.5秒间隔，循环2次
 * 4.AUV任务流程开始：255,255,255 - 0,0,0 循环，1秒间隔，循环5次；AUV任务流程结束：255,255,255 - 0,0,0 循环，0.5秒间隔，循环3次
 * 5.AUV任务项目结束：255,255,255 - 0,0,0 循环，1秒间隔，循环3次
 * 6.漏液检测：255,0,0 - 0,0,0 循环，0.2秒间隔，持续
 */

class SwitchLED : public QObject
{
	Q_OBJECT
public:
	explicit SwitchLED(QObject *parent = 0);
	~SwitchLED();

signals:
	void operateLog(QString, LogPane::WarnLevel);
	void switchOrder(QString);
	void LEDOrder(QString);

public slots:
	void propSwitchSlot(bool isOn);
	void camSwitchSlot(bool isOn);
	/*
	 * 按照colorList的列表，循环每隔timeInMS毫秒变换一次，闪烁flashTimes次
	 * flashTimes 为 0 则闪烁一次timeInMS毫秒并持续闪烁，下一个指示灯信号传来后切换
	*/
	void setLED(LEDSetting s);

private slots:
	void LEDTimerUpdate();

private:
	QList<LEDSetting> list;//存储setLED函数设置的LED颜色，为空则循环R - G - B三色
	LEDSetting emptyListSetting;
	QTimer *LEDTimer;
	unsigned int listIndex = 0;

	void sendColorOrder(QColor c);

};

#endif // SWITCHLED_H
