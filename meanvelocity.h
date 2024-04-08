#ifndef MEANVELOCITY_H
#define MEANVELOCITY_H

#include <QWidget>
#include <QPainter>
#include <QLabel>
#include "plasma.h"

class MeanVelocity : public QLabel
{
    Q_OBJECT
public:
    explicit MeanVelocity(QWidget *parent = nullptr);

//protected:
//    void paintEvent(QPaintEvent *event) override;

public slots:
    void updateMeanVelocity(const Sample& s);
signals:

private:
    double velocity[2];
};

#endif // MEANVELOCITY_H
