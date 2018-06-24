#include "chartpane.h"
#include "defines.h"
#include <QtCore/QTime>

ChartPane::ChartPane(QWidget *parent):
    QChartView(parent)
{
	chart = new QChart;
	line = new QSplineSeries(chart);
	scatter = new QScatterSeries(chart);
	axis_X = new QValueAxis;
	axis_Y = new QValueAxis;

	QFont font;
	font.setPointSize(9);
	font.setFamily(QSL("微软雅黑"));

	chart->legend()->hide();
	chart->addSeries(line);
	chart->addSeries(scatter);
	chart->setAnimationOptions(QChart::NoAnimation);//动画选项
	chart->createDefaultAxes();
	//设置Y轴
	axis_Y->setTitleText(QSL("深度"));
	axis_Y->setTitleFont(font);
	axis_Y->setLabelsFont(font);
	axis_Y->setReverse(true);
	axis_Y->setRange(minY, maxY);
	chart->setAxisY(axis_Y, line);
	chart->setAxisY(axis_Y, scatter);
	//设置X轴
	axis_X->setTitleText(QSL("时间"));
	axis_X->setTitleFont(font);
	axis_X->setLabelsFont(font);
	axis_X->setTickCount(TickCountConst);
	axis_X->setRange(0, 10);
	chart->setAxisX(axis_X, line);
	chart->setAxisX(axis_X, scatter);
	//设置点系列
	scatter->setMarkerSize(2.0);
	scatter->setColor(Qt::red);
	scatter->setBorderColor(scatter->color());//在addseries后设置color才有效

	setChart(chart);
	setRenderHint(QPainter::Antialiasing);
}

ChartPane::~ChartPane()
{

}

void ChartPane::addData(double d)
{
	if(true == isFirst)
		isFirst = false;
	else
		latestPointX += 1 / ((double)(DispCount-1) / (double) TickCountConst);
	line->append(latestPointX, d);
	scatter->append(latestPointX, d);
	qDebug() << "the new add point = " << latestPointX << ',' << d;

	if(latestPointX > axis_X->max())//最新的数据超出最大显示范围：滚动chart
	{
		qreal scrollX = 0;
		scrollX = chart->plotArea().width() / (double)(DispCount-1);
		chart->scroll(scrollX, 0);
		qDebug()<<"scrollX = "<<scrollX;
		qDebug() << "display range = " << axis_X->min() << "->" << axis_X->max();
	}

	while(line->at(0).x() - axis_X->min() < -0.1)//循环删除显示范围之外的点，控制规模
	{
		qDebug()<<"removed line item "<<line->at(0);
		line->remove(0);//循环删除首元素
		scatter->remove(0);
	}

	//调整Y坐标轴范围
	if(d >= maxY)
	{
		maxY = d;
		chart->axisY()->setMax(maxY*1.15);
	}
	else if(d <= minY)
	{
		minY = d;
		chart->axisY()->setMin(minY*1.15);
	}

	//debug显示新加入的点、滚动像素、line容器中所有点
	QString str;
	for(int i = 0;i < line->count();i++) {
		str+=((QString)("(%1,%2) ")).arg(line->at(i).x()).arg(line->at(i).y());
	}
	qDebug()<<"line: "<<line->count()<<str;
	str.clear();
	for(int i = 0;i < scatter->count();i++) {
		str+=((QString)("(%1,%2) ")).arg(scatter->at(i).x()).arg(scatter->at(i).y());
	}
	qDebug()<<"line: "<<scatter->count()<<str;
	qDebug()<<endl;

	Q_ASSERT(line->count() == scatter->count());
}

void ChartPane::animation(bool isAnimation)
{
	if(isAnimation)
	{
		chart->setAnimationOptions(QChart::SeriesAnimations);
	}
	else
	{
		chart->setAnimationOptions(QChart::NoAnimation);
	}
}
