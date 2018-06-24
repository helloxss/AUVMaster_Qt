#ifndef CONTROLPANE_H
#define CONTROLPANE_H

#include "logpane.h"
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

signals:
	void operateLog(QString, LogPane::WarnLevel);
	void order(QString);

private:
	Ui::ControlPane *ui;
	QTimer * timeDispTimer;

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

protected:
	void keyPressEvent(QKeyEvent * e);

};

#endif // CONTROLPANE_H
