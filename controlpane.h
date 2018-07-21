#ifndef CONTROLPANE_H
#define CONTROLPANE_H

#include "logpane.h"
#include "structs.h"
#include <QWidget>
#include <QTimer>
#include <QKeyEvent>

namespace Ui {
class ControlPane;
}

class ControlPane : public QWidget
{
	Q_OBJECT

public:
	explicit ControlPane(QWidget *parent = 0);
	~ControlPane();

public slots:
	void setLEDColor(QColor c);
	void setPosture(PostureData p);
	void setDepth(double d);
	void setThrusterDisp(int t1, int t2, int t3, int t4, int t5, int t6);

signals:
	void operateLog(QString, LogPane::WarnLevel);
	void LEDFlash(LEDSetting);
	void mannualGoSgl(int Lv);
	void mannualTurnSgl(int Lv);
	void mannualOffsetSgl(int Lv);
	void autoDepthSwitch(bool);
	void setAutoDepth(double);
	void autoHeadingSwitch(bool);
	void setAutoHeading(double);
	void propSwitch(bool);
	void camSwitch(bool);

private:
	Ui::ControlPane *ui;
	QTimer * timeDispTimer;
	unsigned short PressedKeys = 0;
	double curYaw = 0;
	void keyReaction(unsigned short k);

private slots:
	void timeDispTimerUpdate();

	void on_btnStdCtrl_clicked();
	void on_btnAdvCtrl_clicked();
	void on_btnTaskCtrl_clicked();
	void on_btnParaSet_clicked();
	void on_btnRefreshDepth_clicked();
	void on_btnRefreshCourse_clicked();
	void on_btnDepthClosedLoop_clicked(bool checked);
	void on_btnCourseClosedLoop_clicked(bool checked);
	void on_sldTurn_valueChanged(int value);
	void on_btnTurnZero_clicked();
	void on_btnTurnLeft_clicked();
	void on_btnTurnRight_clicked();
	void on_sldOffet_valueChanged(int value);
	void on_btnOffsetZero_clicked();
	void on_btnOffsetLeft_clicked();
	void on_btnOffertRight_clicked();
	void on_sldGo_valueChanged(int value);
	void on_btnGoZero_clicked();
	void on_btnForward_clicked();
	void on_btnBackward_clicked();

	void on_btnSet0Depth_clicked();

	void on_btnUseCurYaw_clicked();

	void on_depthVal_valueChanged(double arg1);

	void on_courseVal_valueChanged(double arg1);

	void on_btnThrsSwitch_clicked(bool checked);

	void on_btnCamSwitch_clicked(bool checked);

protected:
	void keyPressEvent(QKeyEvent * e);
	void keyReleaseEvent(QKeyEvent *e);
//	void

};

#endif // CONTROLPANE_H
