#ifndef PARTICLES2D_H
#define PARTICLES2D_H

#include <QWidget>
#include "plasma.h"

class Particles2D : public QWidget
{
    Q_OBJECT
public:
    explicit Particles2D(QWidget *parent = nullptr);
    void setBoundary(Boundary b);

protected:
    void paintEvent(QPaintEvent *event) override;

signals:

public slots:
    void updateSample(const Sample& s);
private:
    Sample sample;
    Boundary boundary;
};

#endif // PARTICLES2D_H
