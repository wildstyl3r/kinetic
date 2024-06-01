#include "chart.h"
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLogValueAxis>
#include <cmath>

Chart::Chart(QString xLabel, QString yLabel, QVector<QColor> colors, QString title, bool has_negative, bool logx, bool logy, int plots, QVector<QString> snames, QWidget *parent, Qt::WindowFlags wFlags)
    : QChartView(parent),
      has_negative(has_negative)
{
    if(logx){
        ox = new QLogValueAxis();
    } else {
        ox = new QValueAxis();
    }
    if(logy){
        oy = new QLogValueAxis();
    } else {
        oy = new QValueAxis();
    }
    setWindowTitle(title);
    chart = new QChart();
    chart->addAxis(ox, Qt::AlignBottom);
    chart->addAxis(oy, Qt::AlignLeft);
    for(int i = 0; i < plots; ++i) {
        series.append(new QScatterSeries(this));
        QPen pen(colors[i]);
        pen.setWidth(1);
        QBrush brush(colors[i]);
        series[i]->setBrush(brush);
        series[i]->setMarkerSize(8);
        series[i]->setPen(pen);
        ox->setTitleText(xLabel);
        oy->setTitleText(yLabel);
        chart->addSeries(series[i]);
        series[i]->attachAxis(ox);
        series[i]->attachAxis(oy);
        if(snames.size() == plots)
            series[i]->setName(snames[i]);
    }
    if(!logy)
       dynamic_cast<QValueAxis*>(oy)->setTickCount(20);
    setChart(chart);
}

void Chart::append(double x, double y, int s) {
    if(y != INFINITY) {
        if(has_negative && max_value < abs(y)) {
            max_value = abs(y);
            oy->setRange(-1.25*max_value, 1.25*max_value);
        } else if (max_value < y) {
            max_value = y;
            oy->setRange(0, 1.25*max_value);
        }
        ox->setRange(0, 1.25*x);
        series[s]->append(x,y);
    }
}

QGraphicsScene* Chart::scene() {
    return chart->scene();
}
double Chart::max(){
    return max_value;
}

void Chart::clear() {
    max_value = 0;
    for(auto& s : series){
        s->clear();
    }
}
