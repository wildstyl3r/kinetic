#include "particles2d.h"
#include <QPainter>

Particles2D::Particles2D(QWidget *parent)
    : QWidget{parent}
{

}


void Particles2D::updateSample(const Sample& s){
    sample = s;
    repaint();
    //update();
}


void Particles2D::paintEvent(QPaintEvent* /* event*/)
{
    QPainter canvas(this);
    //canvas.drawRect(0,0, width()-1, height()-1);
    //canvas.drawPolygon(boundary);
    QPen faded = QPen(Qt::PenStyle::DotLine);
    faded.setColor(QColorConstants::White);
    for(int i = 1; i < boundary.size(); ++i) {
        bool fade = boundary.cycled(i-1);
        QPen p;
        if(fade) {
            p = canvas.pen();
            canvas.setPen(faded);
        }
        canvas.drawLine(boundary[i-1], boundary[i]);
        if(fade) {
            canvas.setPen(p);
        }
    }
    bool fade = boundary.cycled(boundary.size()-1);
    QPen p;
    if(fade) {
        p = canvas.pen();
        canvas.setPen(faded);
    }
    canvas.drawLine(boundary[boundary.size()-1], boundary[0]);
    if(fade) {
        canvas.setPen(p);
    }

    //canvas.drawText(QRectF(10, 10, width(), height()), Qt::AlignLeft, QString::number(counter++));
    for(auto &p : sample.particles) {
        canvas.setPen(Qt::white);
        canvas.drawPoint(p.x[0], p.x[1]);
//        canvas.setPen(Qt::yellow);
//        canvas.drawLine(p.coordinate[0]-p.velocity[0], p.coordinate[1]-p.velocity[1],p.coordinate[0], p.coordinate[1]);
        //canvas.drawEllipse(QPoint(p.coordinate[0], p.coordinate[1]), 3, 3);
    }
}

void Particles2D::setBoundary(Boundary b) {
    boundary = b;
}
