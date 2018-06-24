#include "logpane.h"
#include "defines.h"
#include <QDir>
#include <QMessageBox>
#include <QDateTime>
#include <QTextStream>
#include <QApplication>

LogPane::LogPane(QWidget *parent):
	QListWidget(parent)
{
	QDateTime t = QDateTime::currentDateTime();
	QDir dir(RECORD_DIR);
	dir.exists() ? 0 : dir.mkpath(RECORD_DIR);
	LogFile.setFileName(RECORD_DIR + t.toString("MM-dd-hh_mm_ss_") + "logFile.csv");
	if (!LogFile.open(QFile::WriteOnly | QIODevice::Truncate))//打开文件，只写并覆盖
	{
		QMessageBox::warning(this, "record file warning", \
		                     QStringLiteral("logFile 创建文件失败，磁盘已满或写保护？\n程序因此无法记录操作日志"));
		addString(QStringLiteral("logFile 创建文件失败"), err);
	}
}

LogPane::~LogPane()
{
	if (LogFile.isOpen())
	{
		LogFile.close();
	}
}

void LogPane::addString(QString str, LogPane::WarnLevel level)
{
	//添加项
	QDateTime time = QDateTime::currentDateTime();
	QString timeStr = time.toString("hh:mm:ss:zzz");
	QListWidgetItem * item = new QListWidgetItem;
	switch(level)
	{
	case warn:
		item->setBackgroundColor(QColor(255, 255, 192));
		break;
	case err:
		item->setBackgroundColor(QColor(255, 192, 192));
		break;
	case info:
	default :break;
	}

	//末尾是\n的话，就添加一行无色的新行
	if (str.endsWith('\n'))
	{
		str.remove(str.length() - 1, 1);
		item->setText(timeStr + " - " + str);
		addItem(item);
		addItem("");
	}
	else
	{
		item->setText(timeStr + " - " + str);
		addItem(item);
	}

	//保持数据规模
	if (maxRcodNum > 0)
	{
		while (count() > maxRcodNum)
			takeItem(0);
	}
	setCurrentRow(count() - 1);

	//文件记录
	if (LogFile.isOpen())
	{
		QTextStream logTxt(&LogFile);
		logTxt << timeStr << ',' << str << "\r\n";
	}
}
