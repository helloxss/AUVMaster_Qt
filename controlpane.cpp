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
}

ControlPane::~ControlPane()
{
	if(timeDispTimer)
		delete timeDispTimer;
	delete ui;
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
	if(checked)
	{
		emit operateLog(QSL("控制面板：开启深度闭环"), LogPane::info);
		ui->btnRefreshDepth->setEnabled(true);
		//emit order(Communication::depthCloseLoop(true));
		//emit order(Communication::depthOrder(ui->depthVal->value()));
	}
	else
	{
		emit operateLog(QSL("控制面板：关闭深度闭环"), LogPane::info);
		//emit order(Communication::depthCloseLoop(false));
		ui->btnRefreshDepth->setEnabled(false);
	}
}

void ControlPane::on_btnCourseClosedLoop_clicked(bool checked)
{
	if(checked)
	{
		emit operateLog(QSL("控制面板：开启航向闭环"), LogPane::info);
		ui->btnRefreshCourse->setEnabled(true);
		//emit order(Communication::courseCloseLoop(true));
		//emit order(Communication::courseOrder(ui->depthVal->value()));
	}
	else
	{
		emit operateLog(QSL("控制面板：关闭航向闭环"), LogPane::info);
		ui->btnRefreshCourse->setEnabled(false);
		//emit order(Communication::courseCloseLoop(false));
	}
}

void ControlPane::on_sldTurn_valueChanged(int value)
{
	//	qDebug()<<"on_sldTurn_valueChanged";
	ui->btnTurnZero->setText(QString("%1").arg(value));
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
	//emit order(Communication::offsetLevelOrder(value));
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
	//emit order(Communication::goLevelOrder(value));
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
	//emit order(Communication::depthOrder(ui->depthVal->value()));
}

void ControlPane::on_btnRefreshCourse_clicked()
{
	assert(ui->btnCourseClosedLoop->isEnabled());
	//emit order(Communication::courseOrder(ui->courseVal->value()));
}

/*================================================================*/
/*********************下面为键盘控制指令*******************************/
void ControlPane::keyPressEvent(QKeyEvent *e )
{
	/*  键盘触发事件
	*  "W",/*Qt::Key_W 0x57
	*  "A",/*Qt::Key_A 0x41
	*  "D",/*Qt::Key_D 0x44
	*  "S",/*Qt::Key_S 0x53
	*/

	static int goLevel=0;
	static int offsetLevel=0;
	if(e->key()==Qt::Key_W)     //前进
	{
		goLevel++;
		////emit order(Communication::goLevelOrder(goLevel));
		ui->sldGo->setValue(goLevel);
	}
	else if(e->key()==Qt::Key_S)  //后退
	{
		goLevel--;
		////emit order(Communication::goLevelOrder(goLevel));
		ui->sldGo->setValue(goLevel);
	}
	else if(e->key()==Qt::Key_D)   //右移
	{
		offsetLevel++;
		////emit order(Communication::offsetLevelOrder(offsetLevel));
		ui->sldOffet->setValue(offsetLevel);
	}
	else if(e->key()==Qt::Key_A)   //左移
	{
		offsetLevel--;
		////emit order(Communication::offsetLevelOrder(offsetLevel));
		ui->sldOffet->setValue(offsetLevel);
	}

}
