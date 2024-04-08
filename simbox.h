#ifndef SIMBOX_H
#define SIMBOX_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include "meanvelocity.h"
#include "plasma.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SimBox; }
QT_END_NAMESPACE

class   SimBox : public QWidget
{
    Q_OBJECT

public:
    SimBox(QWidget *parent = nullptr);
    ~SimBox();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

public slots:
    void updateSample(const Sample& s);//(const vector<Particle>& particles);
    void startComputation();

signals:
    void setupPlasma(const EngineParameters& params);//double width, double height, int electrons_number, double heavy_number_density, double heavy_temperature, int skip_steps, int total_steps);
    void abortPlasmaComputation();
    void finish();

private:
    QThread worker_thread;
    Sample sample;
    int counter = 0;
    int electrons_to_show;
    Ui::SimBox *ui;
    vector<QGraphicsEllipseItem*> particles;
    double centerX;
    double centerY;
    double box_width;
    double box_height;
};
#endif // SIMBOX_H
