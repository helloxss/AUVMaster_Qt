#ifndef CHARTPANE_H
#define CHARTPANE_H
#include <QWidget>
#include <QChart>
#include <QChartView>
#include <QSplineSeries>
#include <QScatterSeries>
#include <QValueAxis>
#include <QTimer>
#include <QDebug>

QT_CHARTS_USE_NAMESPACE

class ChartPane : public QChartView
{
	Q_OBJECT

public:
	explicit ChartPane(QWidget *parent = Q_NULLPTR);
	~ChartPane();

	QTimer *testTimer= nullptr;
	QChart *chart = nullptr;
	QSplineSeries *line = nullptr;
//	QLineSeries *line = nullptr;
	QLineSeries *tgtLine = nullptr;
	QScatterSeries *scatter = nullptr;

	void animation(bool isAnimation);
	void setDispCount(unsigned int c)
	{
//		qDebug()<<"set disp count as"<<c;
		if(c >= TickCountConst)
			DispCount = c;
	}

public slots:
	void addData(double d);
	void setTarget(double d){ tgt = d; }

private:
	void keyPressEvent(QKeyEvent * e);
	unsigned short easterEgg = 0;

	QValueAxis *axis_X = nullptr;
	QValueAxis *axis_Y = nullptr;

	const unsigned int TickCountConst = 11;//刻度数
	unsigned int DispCount = 11;//显示的点数，注意显示的线段数为点数-1，即点数-1为最后一个所处的刻度位置

	bool isFirst = true;
	double latestPointX = 0;
	double maxY = 200;
	double minY = 0;
	double tgt = 0;
};

#endif // CHARTPANE_H
