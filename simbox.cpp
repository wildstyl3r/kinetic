#include "simbox.h"
#include "./ui_simbox.h"
#include <iostream>
#include <QPainter>

SimBox::SimBox(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SimBox)
{
    ui->setupUi(this);
    connect(ui->startButton, &QPushButton::clicked,
            this, &SimBox::startComputation
            );
    connect(&plasma_thread, &Plasma::stateUpdate,
            this, &SimBox::updateParticlePositions
            );
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
//    ui->graphicsView->setScene(scene);
//    ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
}

SimBox::~SimBox()
{
    delete ui;
}



void SimBox::resizeEvent(QResizeEvent * /* event */)
{
    //change graphicsView position

}

void SimBox::paintEvent(QPaintEvent* /* event*/)
{
    vector<Particle> draw_probe = plasma_thread.first_n(electrons_to_show);
    QPainter canvas(this);
    canvas.drawRect(0,0, box_width, box_height);
    //canvas.drawText(QRectF(10, 10, width(), height()), Qt::AlignLeft, QString::number(counter++));
    for(auto &p : draw_probe) {
        canvas.drawPoint(p.coordinate[0], p.coordinate[1]);
        //canvas.drawEllipse(QPoint(p.coordinate[0], p.coordinate[1]), 3, 3);
    }
}

void SimBox::updateParticlePositions(){//(const vector<Particle>& particles){
//    plasma_thread.particles_mutex.lock();
////    if(this->particles.size() != plasma_thread.particles.size()) {
////        for(auto &p : std::as_const(plasma_thread.particles)){
////            particles.push_back(ui->graphicsView->scene()->addEllipse(p.coordinate[0], p.coordinate[1], 5, 5));
////        }
////    } else {
////        size_t i = 0;
////        for(auto p : particles){
////            //std::cout << plasma_thread.particles[i].coordinate[0] <<'\n';
////            //std::cout << plasma_thread.particles[i].velocity[0] <<'\n';
////            p->setPos(plasma_thread.particles[i].coordinate[0], plasma_thread.particles[i].coordinate[1]);
////            i++;
////        }
////        //std::cout <<"p\n";
////    }
//    QPainter canvas(ui->canvas);
//    int current_height = height();
//    int current_width = width();
//    canvas.drawText(QRectF(10, 10, width(), height()), Qt::AlignLeft, QString::number(counter));
//    counter++;
//    for(auto &p : std::as_const(plasma_thread.particles)) {
//        canvas.drawPoint(p.coordinate[0], p.coordinate[1]);
//        canvas.drawEllipse(QPoint(p.coordinate[0], p.coordinate[1]), 3, 3);
//    }
//    plasma_thread.particles_mutex.unlock();
    update();
}

void SimBox::startComputation()
{
    plasma_thread.terminate();
    box_width = ui->width_lineEdit->text().toDouble();
    box_height = ui->height_lineEdit->text().toDouble();
    electrons_to_show = ui->electrons_to_show_lineEdit->text().toInt();
    this->particles.clear();
//    ui->graphicsView->scene()->clear();
//    ui->graphicsView->scene()->setSceneRect(0,0, box_width, box_height);
//    ui->graphicsView->scene()->addRect(0,0, box_width, box_height);
    plasma_thread.setup( box_width,
                         box_height,
                        ui->electrons_lineEdit->text().toInt(),
                        ui->gas_density_lineEdit->text().toDouble(),
                        ui->gas_temperature_lineEdit->text().toDouble(),
                        ui->skip_steps_lineEdit->text().toInt(),
                        ui->total_steps_lineEdit->text().toInt());
}
