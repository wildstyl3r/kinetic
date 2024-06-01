#ifndef CHART_H
#define CHART_H

#include <QtCharts/QChartView>
#include <QObject>


QT_CHARTS_BEGIN_NAMESPACE
class QScatterSeries;
class QLineSeries;
class QValueAxis;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class Chart : public QChartView
{
    Q_OBJECT
public:
    explicit Chart(QString xLabel, QString yLabel, QVector<QColor> colors, QString title, bool negative = false, bool logx = false, bool logy = false, int plots = 1, QVector<QString> snames = {}, QWidget *parent = nullptr, Qt::WindowFlags wFlags = {});
    void clear();
    void append(double x, double y, int s = 0);
    QGraphicsScene* scene();
    double max();

private:
    double max_value = 0;
    bool has_negative;
    QChart *chart;
    QVector<QScatterSeries*> series;
    QAbstractAxis *ox;
    QAbstractAxis *oy;
};

#endif // CHART_H
