#ifndef SIMBOX_H
#define SIMBOX_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsItem>
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

private slots:
    void updateParticlePositions();//(const vector<Particle>& particles);
    void startComputation();

private:
    int counter = 0;
    int electrons_to_show;
    Ui::SimBox *ui;
    Plasma plasma_thread;
    vector<QGraphicsEllipseItem*> particles;
    double centerX;
    double centerY;
    double box_width;
    double box_height;
};
#endif // SIMBOX_H
