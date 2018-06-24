#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QTimer>
#include <QChartView>
#include "controlpane.h"
#include "logpane.h"
#include "opencvpane.h"
#include "chartpane.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

signals:
	void test(double d);

protected:
	void resizeEvent(QResizeEvent *event);

private:
	Ui::MainWindow *ui;

	QTimer *timer;

	ControlPane *pControlPane = nullptr;
	LogPane *pLogPane = nullptr;
//	ChartPane *pChartPane = nullptr;
	ChartPane *pChartPane = nullptr;
	OpenCVPane *pOpenCVPane = nullptr;

	QSplitter *verSplter;	//水平分隔条123
	QSplitter *hozSplter1;	//竖直分隔条
	QSplitter *hozSplter2;	//竖直分隔条

	void splitWindow();
	void constructConnect();

	//测试
private slots:
	void timerUpdate();
	//测试
	void on_actChartAniSwitch_triggered(bool checked);
	void on_act10Points_triggered(bool checked);
	void on_act20Points_triggered(bool checked);
	void on_act50Points_triggered(bool checked);
	void on_act100Points_triggered(bool checked);
};

#endif // MAINWINDOW_H
