#include "meanvelocity.h"

MeanVelocity::MeanVelocity(QWidget *parent)
    : QLabel{parent}
{
    resize(80,80);
    //setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    for(int i = 0; i < 2; ++i){
        velocity[i] = 0;
    }
}


//void MeanVelocity::paintEvent(QPaintEvent* /* event*/)
//{
//    setText("x: "+ QString::number(velocity[0]));
////    QPainter canvas(this);
////    canvas.drawText(20,20, "x: "+ QString::number(velocity[0]));
////    canvas.drawText(20,40, "y: "+ QString::number(velocity[1]));
//}

void MeanVelocity::updateMeanVelocity(const Sample& s){
    for(int i = 0; i < 2; ++i){
        velocity[i] = s.mean.velocity[i];
    }
    setText("x: " + QString::number(velocity[0]) + "\ny: " + QString::number(velocity[1]));
    repaint();
}
