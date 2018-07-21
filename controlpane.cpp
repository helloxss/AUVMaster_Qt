#include "controlpane.h"
#include "ui_controlpane.h"
#include "logpane.h"
#include "defines.h"
#include "assert.h"
#include <QDateTime>

ControlPane::ControlPane(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ControlPane)
{
	ui->setupUi(this);
	ui->btnStdCtrl->setChecked(true);
	ui->stackedWidget->setCurrentIndex(0);
	timeDispTimer = new QTimer (this);//定义定时器
	if(timeDispTimer)
	{
		connect(timeDispTimer,SIGNAL(timeout()),this,SLOT(timeDispTimerUpdate()));
		timeDispTimer->start(1000);
	}

	ui->LEDColor->setStyleSheet(QSL("background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(255, 255, 255, 255));"));
	if(ui->btnDepthClosedLoop->isChecked())
		ui->depthVal->setStyleSheet("background-color: rgb(190, 250, 250);");
	else
		ui->depthVal->setStyleSheet("background-color: rgb(255, 220, 170);");

	if(ui->btnCourseClosedLoop->isChecked())
		ui->courseVal->setStyleSheet("background-color: rgb(190, 250, 250);");
	else
		ui->courseVal->setStyleSheet("background-color: rgb(255, 220, 170);");
}

ControlPane::~ControlPane()
{
	if(timeDispTimer)
		delete timeDispTimer;
	delete ui;
}

void ControlPane::setLEDColor(QColor c)
{
//	ui->LEDColor->setText(QSL("%1,%2,%3").arg((bool)(c.red())).arg((bool)(c.green())).arg((bool)(c.blue())));
	ui->LEDColor->setStyleSheet(QSL("background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(%1, %2, %3, 255), stop:1 rgba(255, 255, 255, 255));")\
								.arg(c.red()).arg(c.green()).arg(c.blue()));
}

void ControlPane::setPosture(PostureData p)
{
	ui->txtYaw->setText(QSL("%1°(%2°/s)").arg(p.yaw).arg(p.yawSpd));
	ui->txtPitch->setText(QSL("%1°(%2°/s)").arg(p.pitch).arg(p.pitSpd));
	ui->txtRoll->setText(QSL("%1°(%2°/s)").arg(p.roll).arg(p.rolSpd));
	curYaw = p.yaw;
}

void ControlPane::setDepth(double d)
{
	ui->txtDepth->setText(QSL("%1cm").arg(d));
}

void ControlPane::setThrusterDisp(int t1, int t2, int t3, int t4, int t5, int t6)
{
	ui->txtThr1->setText(QString::number(t1));
	ui->txtThr2->setText(QString::number(t2));
	ui->txtThr3->setText(QString::number(t3));
	ui->txtThr4->setText(QString::number(t4));
	ui->txtThr5->setText(QString::number(t5));
	ui->txtThr6->setText(QString::number(t6));
}

void ControlPane::keyReaction(unsigned short k)
{
	if (k & 0x1)//W
	{
		ui->sldGo->setValue(3);
	}
	else if (k & 0x2)//S
	{
		ui->sldGo->setValue(-3);
	}
	else
	{
		ui->sldGo->setValue(0);
	}

	if (k & 0x4)//A
	{
		ui->sldTurn->setValue(-3);
	}
	else if (k & 0x8)//D
	{
		ui->sldTurn->setValue(3);
	}
	else
	{
		ui->sldTurn->setValue(0);
	}

	if (k & 0x10)//Q
	{
		ui->sldOffet->setValue(-3);
	}
	else if (k & 0x20)//E
	{
		ui->sldOffet->setValue(3);
	}
	else
	{
		ui->sldOffet->setValue(0);
	}
}

void ControlPane::timeDispTimerUpdate()
{
	static long long int StartTime(0);
	StartTime++;
	QString str=QString("%1m - %2s").arg((int)(StartTime/60)).arg(StartTime-(int)(StartTime/60)*60);
	ui->txtElapsedTime->setText(str);
	ui->txtElapsedTime->adjustSize();

	QDateTime time = QDateTime::currentDateTime();
	QString sysTimeStr = time.toString(" hh:mm:ss");
	ui->txtTime->setText(sysTimeStr);
	ui->txtTime->adjustSize();

	sysTimeStr = time.toString("yyyy-MM-dd ");
	ui->txtDate->setText(sysTimeStr);
	ui->txtDate->adjustSize();
}

void ControlPane::on_btnStdCtrl_clicked()
{
	ui->stackedWidget->setCurrentIndex(0);
	ui->btnStdCtrl->setChecked(true);
	ui->btnAdvCtrl->setChecked(false);
	ui->btnTaskCtrl->setChecked(false);
	ui->btnParaSet->setChecked(false);
}

void ControlPane::on_btnAdvCtrl_clicked()
{
	ui->stackedWidget->setCurrentIndex(1);
	ui->btnStdCtrl->setChecked(false);
	ui->btnAdvCtrl->setChecked(true);
	ui->btnTaskCtrl->setChecked(false);
	ui->btnParaSet->setChecked(false);
}

void ControlPane::on_btnTaskCtrl_clicked()
{
	ui->stackedWidget->setCurrentIndex(2);
	ui->btnStdCtrl->setChecked(false);
	ui->btnAdvCtrl->setChecked(false);
	ui->btnTaskCtrl->setChecked(true);
	ui->btnParaSet->setChecked(false);
}

void ControlPane::on_btnParaSet_clicked()
{
	ui->stackedWidget->setCurrentIndex(3);
	ui->btnStdCtrl->setChecked(false);
	ui->btnAdvCtrl->setChecked(false);
	ui->btnTaskCtrl->setChecked(false);
	ui->btnParaSet->setChecked(true);
}

void ControlPane::on_btnDepthClosedLoop_clicked(bool checked)
{
	emit autoDepthSwitch(checked);
	if(checked)
	{
		emit operateLog(QSL("控制面板：开启深度闭环，目标%1cm").arg(ui->depthVal->value()), LogPane::info);
		ui->btnRefreshDepth->setEnabled(true);
		ui->depthVal->setStyleSheet("background-color: rgb(190, 250, 250);");
		emit setAutoDepth(ui->depthVal->value());//更新自动控制的目标深度
	}
	else
	{
		emit operateLog(QSL("控制面板：关闭深度闭环"), LogPane::info);
		ui->btnRefreshDepth->setEnabled(false);
		ui->depthVal->setStyleSheet("background-color: rgb(255, 220, 170);");
		emit setAutoDepth(0);
	}
}

void ControlPane::on_btnCourseClosedLoop_clicked(bool checked)
{
	emit autoHeadingSwitch(checked);
	if(checked)
	{
		emit operateLog(QSL("控制面板：开启航向闭环，目标%1°").arg(ui->courseVal->value()), LogPane::info);
		ui->btnRefreshCourse->setEnabled(true);
		ui->courseVal->setStyleSheet("background-color: rgb(190, 250, 250);");
		emit setAutoHeading(ui->courseVal->value());
	}
	else
	{
		emit operateLog(QSL("控制面板：关闭航向闭环"), LogPane::info);
		ui->btnRefreshCourse->setEnabled(false);
		ui->courseVal->setStyleSheet("background-color: rgb(255, 220, 170);");
		emit setAutoDepth(0);
	}
}

void ControlPane::on_sldTurn_valueChanged(int value)
{
	//	qDebug()<<"on_sldTurn_valueChanged";
	ui->btnTurnZero->setText(QString("%1").arg(value));
	emit mannualTurnSgl(value * 30);
}

void ControlPane::on_btnTurnZero_clicked()
{
	//此处不应该发送指令，接下来还会调用on_sldTurn_valueChanged
	//	qDebug()<<"on_btnTurnZero_clicked";
	ui->sldTurn->setValue(0);
}

void ControlPane::on_btnTurnLeft_clicked()
{
	ui->sldTurn->setValue(ui->sldTurn->value()-1);
}

void ControlPane::on_btnTurnRight_clicked()
{
	ui->sldTurn->setValue(ui->sldTurn->value()+1);
}

void ControlPane::on_sldOffet_valueChanged(int value)
{
	ui->btnOffsetZero->setText(QString("%1").arg(value));
	emit mannualOffsetSgl(value * 30);
}

void ControlPane::on_btnOffsetZero_clicked()
{
	ui->sldOffet->setValue(0);
}

void ControlPane::on_btnOffsetLeft_clicked()
{
	ui->sldOffet->setValue(ui->sldOffet->value()-1);
}

void ControlPane::on_btnOffertRight_clicked()
{
	ui->sldOffet->setValue(ui->sldOffet->value()+1);
}

void ControlPane::on_sldGo_valueChanged(int value)
{
	ui->btnGoZero->setText(QString("%1").arg(value));
	emit mannualGoSgl(value * 30);
}

void ControlPane::on_btnGoZero_clicked()
{
	ui->sldGo->setValue(0);
}

void ControlPane::on_btnForward_clicked()
{
	ui->sldGo->setValue(ui->sldGo->value()+1);
}

void ControlPane::on_btnBackward_clicked()
{
	ui->sldGo->setValue(ui->sldGo->value()-1);
}

void ControlPane::on_btnRefreshDepth_clicked()
{
	assert(ui->btnDepthClosedLoop->isEnabled());
	emit operateLog(QSL("控制面板：设定深度闭环目标%1cm").arg(ui->depthVal->value()), LogPane::info);
	emit setAutoDepth(ui->depthVal->value());
	ui->depthVal->setStyleSheet("background-color: rgb(190, 250, 250);");
}

void ControlPane::on_btnRefreshCourse_clicked()
{
	assert(ui->btnCourseClosedLoop->isEnabled());
	emit operateLog(QSL("控制面板：设定航向闭环目标%1°").arg(ui->courseVal->value()), LogPane::info);
	emit setAutoHeading(ui->courseVal->value());
	ui->courseVal->setStyleSheet("background-color: rgb(190, 250, 250);");
}

/*================================================================*/
/*********************下面为键盘控制指令*******************************/
void ControlPane::keyPressEvent(QKeyEvent *e )
{
	bool isWSADQE = true;
	switch (e->key()) {
	case Qt::Key_W:
		PressedKeys |= 0x1; break;
	case Qt::Key_S:
		PressedKeys |= 0x2; break;
	case Qt::Key_A:
		PressedKeys |= 0x4; break;
	case Qt::Key_D:
		PressedKeys |= 0x8; break;
	case Qt::Key_Q:
		PressedKeys |= 0x10; break;
	case Qt::Key_E:
		PressedKeys |= 0x20; break;
	default:
		isWSADQE = false; break;
	}
	if(isWSADQE)
		keyReaction(PressedKeys);
}

void ControlPane::keyReleaseEvent(QKeyEvent *e)
{
	bool isWSADQE = true;
	switch (e->key()) {
	case Qt::Key_W:
		PressedKeys &= ~0x1; break;
	case Qt::Key_S:
		PressedKeys &= ~0x2; break;
	case Qt::Key_A:
		PressedKeys &= ~0x4; break;
	case Qt::Key_D:
		PressedKeys &= ~0x8; break;
	case Qt::Key_Q:
		PressedKeys &= ~0x10; break;
	case Qt::Key_E:
		PressedKeys &= ~0x20; break;
	default:
		isWSADQE = false; break;
	}
	if(isWSADQE)
		keyReaction(PressedKeys);
}

void ControlPane::on_btnSet0Depth_clicked()
{
	ui->depthVal->setValue(0);
}

void ControlPane::on_btnUseCurYaw_clicked()
{
	ui->courseVal->setValue(curYaw);
}

void ControlPane::on_depthVal_valueChanged(double arg1)
{
	Q_UNUSED(arg1);
	ui->depthVal->setStyleSheet("background-color: rgb(255, 220, 170);");
}

void ControlPane::on_courseVal_valueChanged(double arg1)
{
	Q_UNUSED(arg1);
	ui->courseVal->setStyleSheet("background-color: rgb(255, 220, 170);");

}

void ControlPane::on_btnThrsSwitch_clicked(bool checked)
{
	emit propSwitch(checked);
	if(checked)
	{
		LEDSetting s;
		s.changeTime = 333;
		s.colorList << QColor(0,255,0) << QColor(0,0,0);
		s.flashTimes = 3;
		emit LEDFlash(s);
	}
	else
	{
		LEDSetting s;
		s.changeTime = 500;
		s.colorList << QColor(0,255,0) << QColor(0,0,0);
		s.flashTimes = 2;
		emit LEDFlash(s);
	}
}

void ControlPane::on_btnCamSwitch_clicked(bool checked)
{
	emit camSwitch(checked);
	if(checked)
	{
		LEDSetting s;
		s.changeTime = 333;
		s.colorList << QColor(0,0,255) << QColor(0,0,0);
		s.flashTimes = 3;
		emit LEDFlash(s);
	}
	else
	{
		LEDSetting s;
		s.changeTime = 500;
		s.colorList << QColor(0,0,255) << QColor(0,0,0);
		s.flashTimes = 2;
		emit LEDFlash(s);
	}
}
