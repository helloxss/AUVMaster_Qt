#ifndef LOGPANE_H
#define LOGPANE_H

#include <QListWidget>
#include <QFile>

#define RECORD_DIR (qApp->applicationDirPath() + "/record/")

class LogPane : public QListWidget
{
	Q_OBJECT
public:
	explicit LogPane(QWidget *parent = 0);
	~LogPane();
	int maxRcodNum = 100;
	enum WarnLevel
	{
		info,
		warn,
		err,
	};


signals:

public slots:
	void addString(QString str, WarnLevel level = info);

private:
	QFile LogFile;

};

#endif // LOGPANE_H
