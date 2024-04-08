#include "simbox.h"
#include "./ui_simbox.h"
#include <iostream>
#include <QPainter>

#include "testclock.h"

SimBox::SimBox(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SimBox)
{
    ui->setupUi(this);

    connect(ui->startButton, &QPushButton::clicked,
            this, &SimBox::startComputation
            );


    MeanVelocity* mv = new MeanVelocity(this);
    ui->verticalLayout->addWidget(mv);

    Plasma* plasma = new Plasma;
    plasma->moveToThread(&worker_thread);

    AnalogClock* clock = new AnalogClock(this);
    //ui->horizontalLayout->addWidget(clock);
    clock->show();
    setLayout(ui->horizontalLayout);




    connect(plasma, &Plasma::stateUpdate,
            mv, &MeanVelocity::updateMeanVelocity
            );

    connect(plasma, &Plasma::stateUpdate,
            this, &SimBox::updateSample
            );
    connect(this, &SimBox::abortPlasmaComputation,
            plasma, &Plasma::abortComputation);
    connect(this, &SimBox::setupPlasma,
            plasma, &Plasma::compute
            );
    connect(this, &SimBox::finish,
            &worker_thread, &QThread::quit);
    connect(&worker_thread, &QThread::finished, &worker_thread, &QThread::deleteLater);
    connect(qApp, &QApplication::aboutToQuit,   &worker_thread, [=]{
        worker_thread.quit();
        worker_thread.wait();
    });
    worker_thread.start();

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
    QPainter canvas(this);
    canvas.drawRect(0,0, box_width, box_height);
    //canvas.drawText(QRectF(10, 10, width(), height()), Qt::AlignLeft, QString::number(counter++));
    for(auto &p : sample.particles) {
        canvas.drawPoint(p.coordinate[0], p.coordinate[1]);
        //canvas.drawEllipse(QPoint(p.coordinate[0], p.coordinate[1]), 3, 3);
    }
}

void SimBox::updateSample(const Sample& s){
    sample = s;
    repaint();
    //update();
}

void SimBox::startComputation()
{
    emit abortPlasmaComputation();
    box_width = ui->width_lineEdit->text().toDouble();
    box_height = ui->height_lineEdit->text().toDouble();
    electrons_to_show = ui->electrons_to_show_lineEdit->text().toInt();
    EngineParameters p;
    p.bounding_box[0] = box_width;
    p.bounding_box[1] = box_height;
    p.sample_size = electrons_to_show;
    p.total_steps = ui->total_steps_lineEdit->text().toInt();
    p.skip_steps = ui->skip_steps_lineEdit->text().toInt();
    p.electrons_number = ui->electrons_lineEdit->text().toInt();
    p.heavy_number_density = ui->gas_density_lineEdit->text().toDouble();
    p.heavy_temperature = ui->gas_temperature_lineEdit->text().toDouble();

    emit setupPlasma(p);
}
