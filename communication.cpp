#include "communication.h"
#include "defines.h"
#include <QMessageBox>
#include <QThread>
#include <QTime>

Communication::Communication(QObject *parent) : QObject(parent)
{

}

Communication::~Communication()
{
	if (propellerPort->isOpen())
		propellerPort->deleteLater();
	if (switchArduinoPort->isOpen())
		switchArduinoPort->deleteLater();
	if (LEDArduinoPort->isOpen())
		LEDArduinoPort->deleteLater();
	if (depthSensorPort->isOpen())
		depthSensorPort->deleteLater();
	if (postureSensorPort->isOpen())
		postureSensorPort->deleteLater();

	if(depthTimer)
		depthTimer->deleteLater();
}

void Communication::create()
{
	propellerPort = new QSerialPort;
	switchArduinoPort = new QSerialPort;
	LEDArduinoPort = new QSerialPort;
	depthSensorPort = new QSerialPort;
	postureSensorPort = new QSerialPort;

	openPropellerPort();
	openSwitchArduinoPort();
	openLEDArduinoPort();
	openDepthSensorPort();
	openPostureSensorPort();

	depthTimer = new QTimer;
	/*$：起始符 00RP0：传感器地址 32："00RP0"每个字符取异或的结果 \r：结束符*/
	connect(depthTimer, &QTimer::timeout, this, [this]{sendToDepthSensor(QSL("$00RP032\r"));});
	depthTimer->start(100);
}

void Communication::setZeroDepth()
{

}

void Communication::sendToPropeller(QString s)
{
	QByteArray ba = s.toLatin1();
	if(propellerPort && propellerPort->isOpen())
	{
		propellerPort->write(ba);
	}
	else
	{
		qDebug()<<"propeller port did not open";
	}
}

void Communication::sendToSwitchArduino(QString s)
{
	QByteArray ba = s.toLatin1();
	if(switchArduinoPort && switchArduinoPort->isOpen())
	{
		switchArduinoPort->write(ba);
	}
	else
	{
		qDebug()<<"switch arduino port did not open";
	}
}

void Communication::sendToLEDArduino(QString s)
{
	QByteArray ba = s.toLatin1();
	if(LEDArduinoPort && LEDArduinoPort->isOpen())
	{
		LEDArduinoPort->write(ba);
	}
	else
	{
		qDebug()<<"LED arduino port did not open";
	}
}

void Communication::sendToDepthSensor(QString s)
{
	QByteArray ba = s.toLatin1();
	if(depthSensorPort && depthSensorPort->isOpen())
	{
		depthSensorPort->write(ba);
	}
	else
	{
		qDebug()<<"depth sensor port did not open";
	}
}

bool Communication::openPropellerPort()
{
	propellerPort->close();
	propellerPort->setPortName(PROPELLER_PORT);
	if (propellerPort->open(QIODevice::ReadWrite))
	{
		propellerPort->setBaudRate(QSerialPort::Baud9600);  //波特率
		propellerPort->setDataBits(QSerialPort::Data8);     //数据位
		propellerPort->setParity(QSerialPort::NoParity);    //无奇偶校验
		propellerPort->setStopBits(QSerialPort::OneStop);   //无停止位
		propellerPort->setFlowControl(QSerialPort::NoFlowControl);  //无控制
		emit operateLog(QSL("通信：推进器串口打开为 %1").arg(propellerPort->portName()), LogPane::info);
		connect(propellerPort, SIGNAL(readyRead()), this, SLOT(readPropellerPort()));
		return true;
	}
	else
	{
		emit operateLog(QSL("通信：推进器串口打开失败"), LogPane::err);
		QMessageBox::critical(NULL, QStringLiteral("串口错误"), \
		                      QSL("推进器串口(\"%1\")打开失败，请检查它的状态。\n程序因此无法正常运行。")\
		                      .arg(propellerPort->portName()));
		return false;
	}
}

bool Communication::openSwitchArduinoPort()
{
	switchArduinoPort->close();
	switchArduinoPort->setPortName(SWITCH_ARDUINO_PORT);
	if (switchArduinoPort->open(QIODevice::ReadWrite))
	{
		switchArduinoPort->setBaudRate(QSerialPort::Baud9600);  //波特率
		switchArduinoPort->setDataBits(QSerialPort::Data8);     //数据位
		switchArduinoPort->setParity(QSerialPort::NoParity);    //无奇偶校验
		switchArduinoPort->setStopBits(QSerialPort::OneStop);   //无停止位
		switchArduinoPort->setFlowControl(QSerialPort::NoFlowControl);  //无控制
		emit operateLog(QSL("通信：开关控制板串口打开为 %1").arg(switchArduinoPort->portName()), LogPane::info);
		connect(switchArduinoPort, SIGNAL(readyRead()), this, SLOT(readSwitchArduinoPort()));
		return true;
	}
	else
	{
		emit operateLog(QSL("通信：开关控制板串口打开失败"), LogPane::err);
		//		QApplication::beep();
		QMessageBox::critical(NULL, QStringLiteral("串口错误"), \
		                      QSL("开关控制板串口(\"%1\")打开失败，请检查它的状态。\n程序因此无法正常运行。")\
		                      .arg(switchArduinoPort->portName()));
		return false;
	}
}

bool Communication::openLEDArduinoPort()
{
	LEDArduinoPort->close();
	LEDArduinoPort->setPortName(LED_ARDUINO_PORT);
	if (LEDArduinoPort->open(QIODevice::ReadWrite))
	{
		LEDArduinoPort->setBaudRate(QSerialPort::Baud9600);  //波特率
		LEDArduinoPort->setDataBits(QSerialPort::Data8);     //数据位
		LEDArduinoPort->setParity(QSerialPort::NoParity);    //无奇偶校验
		LEDArduinoPort->setStopBits(QSerialPort::OneStop);   //无停止位
		LEDArduinoPort->setFlowControl(QSerialPort::NoFlowControl);  //无控制
		emit operateLog(QSL("通信：LED控制板串口打开为 %1").arg(LEDArduinoPort->portName()), LogPane::info);
		connect(LEDArduinoPort, SIGNAL(readyRead()), this, SLOT(readLEDArduinoPort()));
		return true;
	}
	else
	{
		emit operateLog(QSL("通信：LED控制板串口打开失败"), LogPane::err);
		//		QApplication::beep();
		QMessageBox::critical(NULL, QStringLiteral("串口错误"), \
		                      QSL("LED控制板串口(\"%1\")打开失败，请检查它的状态。\n程序因此无法正常运行。")\
		                      .arg(LEDArduinoPort->portName()));
		return false;
	}
}

bool Communication::openDepthSensorPort()
{
	depthSensorPort->close();
	depthSensorPort->setPortName(DEPTH_SENSOR_PORT);
	if (depthSensorPort->open(QIODevice::ReadWrite))
	{
		depthSensorPort->setBaudRate(QSerialPort::Baud9600);  //波特率
		depthSensorPort->setDataBits(QSerialPort::Data8);     //数据位
		depthSensorPort->setParity(QSerialPort::NoParity);    //无奇偶校验
		depthSensorPort->setStopBits(QSerialPort::OneStop);   //无停止位
		depthSensorPort->setFlowControl(QSerialPort::NoFlowControl);  //无控制
		emit operateLog(QSL("通信：深度传感器串口打开为 %1").arg(depthSensorPort->portName()), LogPane::info);
		connect(depthSensorPort, &QSerialPort::readyRead, this, &Communication::readDepthSensorPort);
		return true;
	}
	else
	{
		emit operateLog(QSL("通信：深度传感器机串口打开失败"), LogPane::err);
		//		QApplication::beep();
		QMessageBox::critical(NULL, QStringLiteral("串口错误"), \
							  QSL("深度传感器串口(\"%1\")打开失败，请检查它的状态。\n程序因此无法正常运行。")\
		                      .arg(depthSensorPort->portName()));
		return false;
	}
}

bool Communication::openPostureSensorPort()
{
	postureSensorPort->close();
	postureSensorPort->setPortName(POSTURE_SENSOR_PORT);
	if (postureSensorPort->open(QIODevice::ReadWrite))
	{
		postureSensorPort->setBaudRate(QSerialPort::Baud9600);  //波特率
		postureSensorPort->setDataBits(QSerialPort::Data8);     //数据位
		postureSensorPort->setParity(QSerialPort::NoParity);    //无奇偶校验
		postureSensorPort->setStopBits(QSerialPort::OneStop);   //无停止位
		postureSensorPort->setFlowControl(QSerialPort::NoFlowControl);  //无控制
		emit operateLog(QSL("通信：姿态传感器串口打开为 %1").arg(postureSensorPort->portName()), LogPane::info);
		connect(postureSensorPort, &QSerialPort::readyRead, this, &Communication::readPostureSensorPort);
		return true;
	}
	else
	{
		emit operateLog(QSL("通信：姿态传感器串口打开失败"), LogPane::err);
		//		QApplication::beep();
		QMessageBox::critical(NULL, QStringLiteral("串口错误"), \
							  QSL("姿态传感器串口(\"%1\")打开失败，请检查它的状态。\n程序因此无法正常运行。")\
		                      .arg(postureSensorPort->portName()));
		return false;
	}
}

void Communication::propellerDataProc(QString &data)
{
	Q_UNUSED(data);
}

void Communication::switchArduinoDataProc(QString &data)
{

}

void Communication::LEDArduinoDataProc(QString &data)
{
	bool isGoodColor = true;

	if(data.contains("water"))//漏液检测
		emit leakWater();

	while(data.count() >= 2)
	{		\
		switch (data.at(0).toLatin1()) {
		case 'R':
			if(data.at(1) == '1')
				LEDColor.setRed(255);
			else if(data.at(1) == '0')
				LEDColor.setRed(0);
			data.remove(0,2);
			break;
		case 'G':
			if(data.at(1) == '1')
				LEDColor.setGreen(255);
			else if(data.at(1) == '0')
				LEDColor.setGreen(0);
			data.remove(0,2);
			break;
		case 'B':
			if(data.at(1) == '1')
				LEDColor.setBlue(255);
			else if(data.at(1) == '0')
				LEDColor.setBlue(0);
			data.remove(0,2);
			break;
		default:
			isGoodColor = false;
			data.remove(0,1);
			break;
		}
		if(isGoodColor)
		{
			emit LED(LEDColor);
		}
	}//while(data.count() >= 2)
}

void Communication::depthSensorDataProc(QString &data)
{
	QTime t;
	t.start();
	//1. 按照\r对数据进行分割
//	qDebug()<<"data:"<< data;
	QStringList list = data.split('\r', QString::SkipEmptyParts);//注意分割得到的字符串没有'\r'了
//	qDebug()<<"list"<< list;
	if(!list.isEmpty())//列表中有元素
	{
		//2. 获取具有正确报尾的 最新收到的字符串
		QString latestStr;
		int uselessDataCnt = 0;
		if(data.endsWith("\r"))			//数据最后一个字符是正确报尾？是：说明最后一项具有正确报尾，拿到它并检查报头；否：看倒数第二个
		{
			latestStr = *(list.end()-1);
			uselessDataCnt = data.length();//整条数据都无用了
		}
		else if(list.size() >= 2)		//列表有多于两个的项？是：说明倒数第二个具有正确报尾；否：只收到一个数据且数据不完全，直接返回
		{
			latestStr = *(list.end()-2);
			uselessDataCnt = data.length() - (*(list.end()-1)).length();//保留最后一个数据
		}
		else
			return;
//		qDebug()<<"uselessDataCnt:"<<uselessDataCnt;

		//3. 检查上一步得到的（具有正确报尾的）字符串的报头
		QString checkStr;
		if(latestStr.startsWith("*"))	//正确报头？是：进行下一步异或检查；否：字符串异常，删除之并返回
			checkStr = latestStr.mid(1, latestStr.length()-3);
		else
		{
			data.remove(0, uselessDataCnt);
			return;
		}
//		qDebug()<<"checkStr:"<< checkStr;

		//4. 异或检查
		char ch = 0;
		foreach (QChar c, checkStr) {
			ch ^= c.toLatin1();
		}

		QString valueStr;
		if(latestStr.mid(latestStr.length()-2, 2).toInt(nullptr, 16) == ch)//异或结果与收到相符？是：获取值字符串；否：字符串异常，删除并返回
			valueStr = latestStr.mid(3, latestStr.length()-5);
		else
		{
			data.remove(0, uselessDataCnt);
			return;
		}
//		qDebug()<<"valueStr"<< valueStr;

		//5. 发送信号
		double dep = valueStr.toDouble();
		if(isRecali)//校准零深度
		{
			operateLog(QSL("通信：设定深度零点为 %1 mH2O").arg(dep), LogPane::info);
			zeroDepth = dep;
			isRecali = false;
		}
		emit depth((dep - zeroDepth) * 100);

		//6. 清除旧字符串
		data.remove(0, uselessDataCnt);
//		qDebug()<<"after proc:"<<data;
//		qDebug()<<"proc time"<<t.elapsed();
//		qDebug()<<"";
	}
}

void Communication::postureSensorDataProc(QString &data)
{

}

void Communication::readPropellerPort()
{
	qDebug() <<"readPropellerPort"<< QThread::currentThreadId();
	QByteArray temp = propellerPort->readAll();
	propellerRecvStr.append(temp);
	propellerDataProc(propellerRecvStr);
//	qDebug()<< "readPropellerPort:" <<temp;
}

void Communication::readSwitchArduinoPort()
{
//	qDebug() << "switch arduino port:" << switchArduinoPort->readAll();
	QByteArray temp = switchArduinoPort->readAll();
	switchArduinoRecvStr.append(temp);
	switchArduinoDataProc(switchArduinoRecvStr);
}

void Communication::readLEDArduinoPort()
{
//	qDebug() << "readLEDArduinoPort:" << LEDArduinoPort->readAll();
	qDebug()<<QThread::currentThreadId();
	QByteArray temp = LEDArduinoPort->readAll();
	LEDArduinoRecvStr.append(temp);
	LEDArduinoDataProc(LEDArduinoRecvStr);
}

void Communication::readDepthSensorPort()
{
//	qDebug() << "readDepthSensorPort:" << depthSensorPort->readAll();
	QByteArray temp = depthSensorPort->readAll();
	depthSensorRecvStr.append(temp);
	depthSensorDataProc(depthSensorRecvStr);
}

void Communication::readPostureSensorPort()
{
//	qDebug() << "readPostureSensorPore:" << postureSensorPort->readAll();
	QByteArray temp = postureSensorPort->readAll();
	postureSensorRecvStr.append(temp);
	postureSensorDataProc(postureSensorRecvStr);
}
