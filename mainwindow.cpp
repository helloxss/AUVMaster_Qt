#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "defines.h"
#include "time.h"
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->menuBar->setStyleSheet("QMenuBar{background-color:#646464;} QMenuBar::item {background: #808080;color:#F0F0F0}");
	setMinimumSize(1032, 654);
	setWindowState(Qt::WindowMaximized);

	splitWindow();
	constructConnect();

	//测试
	srand(time(0));
	timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &MainWindow::timerUpdate);
	connect(this, &MainWindow::test, pChartPane, &ChartPane::addData);
	timer->start(1000);
	//测试

	ui->actChartAniSwitch->setChecked(false);
	ui->act10Points->setChecked(true);
	pLogPane->addString(QString::fromLocal8Bit("系统启动于：%1，开始记录日志。")\
	                    .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy年MM月dd日 hh:mm:ss"))));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event);
	pChartPane->setFixedHeight(height()/5*2);
}

void MainWindow::splitWindow()
{
	verSplter = new QSplitter(Qt::Horizontal, this);
	setCentralWidget(verSplter);
	verSplter->setStyleSheet("QSplitter::handle{background-color:#646464}");

	//第一列：水平分隔线将窗口分为竖直的两部分
	hozSplter1 = new QSplitter(Qt::Vertical, verSplter);

	//第一列第一行 controlpane
	pControlPane = new ControlPane(hozSplter1);
	QPalette ControlPal(pControlPane->palette());
	ControlPal.setColor(QPalette::Background, QColor(255,255,255));
	pControlPane->setAutoFillBackground(true);
	pControlPane->setPalette(ControlPal);
	pControlPane->setFixedSize(pControlPane->width(),pControlPane->height());

	//第一列第二行 logPane
	pLogPane = new LogPane(hozSplter1);
	pLogPane->setFixedWidth(pControlPane->width());

	//第二列
	hozSplter2 = new QSplitter(Qt::Vertical, verSplter);

	//第二列第一行 opencvPane
	pOpenCVPane = new OpenCVPane(hozSplter2);
	QPalette openCVPal(pOpenCVPane->palette());
	openCVPal.setColor(QPalette::Background, QColor(255,255,255));
	pOpenCVPane->setAutoFillBackground(true);
	pOpenCVPane->setPalette(openCVPal);

	//第二列第二行 chartPane

	pChartPane = new ChartPane(hozSplter2);
//	QPalette curvePal(pChartPane->palette());
//	curvePal.setColor(QPalette::Background, QColor(255,255,255));
//	pChartPane->setAutoFillBackground(true);
//	pChartPane->setPalette(curvePal);

	//设定拖动因数
	verSplter->setStretchFactor(0, 0);
	verSplter->setStretchFactor(1, 1);
	hozSplter1->setStretchFactor(0, 0);
	hozSplter1->setStretchFactor(1, 1);
	hozSplter2->setStretchFactor(0, 3);
	hozSplter2->setStretchFactor(1, 2);

	//禁用竖直分割线的拖动功能和第一个水平分隔线的拖动功能
	QSplitterHandle *pSplterHdl = verSplter->handle(1);
	if (pSplterHdl) pSplterHdl->setDisabled(true);
	pSplterHdl = verSplter->handle(2);
	if (pSplterHdl) pSplterHdl->setDisabled(true);
	pSplterHdl = hozSplter1->handle(1);
	if (pSplterHdl) pSplterHdl->setDisabled(true);
	pSplterHdl = hozSplter1->handle(2);
	if (pSplterHdl) pSplterHdl->setDisabled(true);
	pSplterHdl = hozSplter2->handle(1);
	if (pSplterHdl) pSplterHdl->setDisabled(true);
	pSplterHdl = hozSplter2->handle(2);
	if (pSplterHdl) pSplterHdl->setDisabled(true);
}

void MainWindow::constructConnect()
{
	connect(pControlPane, &ControlPane::operateLog, pLogPane, &LogPane::addString);	//控制面板->日志面板：日志
}

//测试
void MainWindow::timerUpdate()
{
	emit test(rand() % (50 - (-5) + 1) + (-5));
//	static int i = 0;
//	emit test(i++);
}
//测试

void MainWindow::on_actChartAniSwitch_triggered(bool checked)
{
	pChartPane->animation(checked);
}

void MainWindow::on_act10Points_triggered(bool checked)
{
	if(checked)
	{
		ui->act20Points->setChecked(false);
		ui->act50Points->setChecked(false);
		ui->act100Points->setChecked(false);
		timer->setInterval(1000);
		pChartPane->setDispCount(11);
	}
	else
	{
		ui->act10Points->setChecked(true);
	}
}

void MainWindow::on_act20Points_triggered(bool checked)
{
	if(checked)
	{
		ui->act10Points->setChecked(false);
		ui->act50Points->setChecked(false);
		ui->act100Points->setChecked(false);
		timer->setInterval(500);
		pChartPane->setDispCount(21);
	}
	else
	{
		ui->act20Points->setChecked(true);
	}
}

void MainWindow::on_act50Points_triggered(bool checked)
{
	if(checked)
	{
		ui->act10Points->setChecked(false);
		ui->act20Points->setChecked(false);
		ui->act100Points->setChecked(false);
		timer->setInterval(200);
		pChartPane->setDispCount(51);
	}
	else
	{
		ui->act50Points->setChecked(true);
	}
}

void MainWindow::on_act100Points_triggered(bool checked)
{
	if(checked)
	{
		ui->act10Points->setChecked(false);
		ui->act20Points->setChecked(false);
		ui->act50Points->setChecked(false);
		timer->setInterval(100);
		pChartPane->setDispCount(101);
	}
	else
	{
		ui->act100Points->setChecked(true);
	}
}
