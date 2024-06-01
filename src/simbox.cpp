#include "simbox.h"
#include <iostream>
#include <QPainter>
#include <QFormLayout>
#include <QFileDialog>
#include <QtCharts/QChartView>

SimBox::SimBox(QWidget *parent)
    : QWidget(parent)
    //, ui(new Ui::SimBox)
{
    //ui->setupUi(this);
    main_layout = new QHBoxLayout(this);

    Townsend_edit = new QLineEdit;

    start_button = new QPushButton("Start");
    connect(start_button, &QPushButton::clicked,
            this,         &SimBox::startComputation);
    results_button = new QPushButton("Results");
    results_button->setEnabled(false);
    connect(results_button, &QPushButton::clicked,
            this,         &SimBox::showResults);

    indicator = new ProbeIndicator;
    visualizer = new Particles2D;
    main_layout->addWidget(visualizer);

    Plasma* plasma = new Plasma;
    plasma->moveToThread(&worker_thread);


    QFormLayout* form = new QFormLayout;
    load_from_file_button = new QPushButton("Load from file");
    form->addRow(load_from_file_button);

    form->addRow("do not use bounds", nobounds_checkbox = new QCheckBox(this));

    form->addRow(boundary_file = new QLabel("[boundary file]", this));
    boundary_file->setAlignment(Qt::AlignCenter);
    boundary_file->setMaximumWidth(400);

    form->addRow(boundary_button = new QPushButton("set boundary", this));
    connect(boundary_button, &QPushButton::clicked,
            this, [&](){
                QString filename = QFileDialog::getOpenFileName(this);
                if (!filename.isEmpty()){
                    boundary_file->setText(filename);
                    boundary_file->setToolTip(filename);
                }
            });

    connect(nobounds_checkbox, &QCheckBox::stateChanged,
            this, [&](int checked){
                if(checked){
                    boundary_button->setEnabled(false);
                } else {
                    boundary_button->setEnabled(true);
                }
            });

    form->addRow(cross_sections_file = new QLabel("[cross-sections file]", this));
    cross_sections_file->setAlignment(Qt::AlignCenter);
    cross_sections_file->setMaximumWidth(400);
    QPushButton* cross_sections = new QPushButton("set cross-sections", this);
    form->addRow(cross_sections);
    connect(cross_sections, &QPushButton::clicked,
            this, [&](){
                QString filename = QFileDialog::getOpenFileName(this);
                if (!filename.isEmpty()){
                    cross_sections_file->setText(filename);
                    cross_sections_file->setToolTip(filename);
                }
            });




    form->addRow("Pressure, Pa", p_h_edit = new QLineEdit(this));
    form->addRow("neutral atomic mass", neutral_atomic_mass = new QLineEdit(this));
    form->addRow("T_0, K", T_h_edit = new QLineEdit(this));
    form->addRow("T_e, K", T_e_edit = new QLineEdit(this));
    form->addRow("#electrons", electrons_edit = new QLineEdit(this));
    form->addRow("#electrons to show", electrons_to_show_edit = new QLineEdit(this));
    form->addRow("total steps", total_steps_edit = new QLineEdit(this));
    form->addRow("skip steps", skip_steps_edit = new QLineEdit(this));
    form->addRow("Townsend coefficient for E/N, Td", Townsend_edit = new QLineEdit(this));
    form->addRow("random seed", seed_edit = new QLineEdit(this));
    form->addRow("ionization energy share W", W_edit = new QLineEdit(this));
    form->addRow("show metrics' charts", show_indicator_checkbox = new QCheckBox(this));


    QVBoxLayout* control_panel = new QVBoxLayout;
    control_panel->addLayout(form);
    control_panel->addWidget(start_button);
    control_panel->addWidget(results_button);
    //control_panel->addWidget(new QChartView(indicator));
    main_layout->addLayout(control_panel);

    eedf = new Chart("Energy [eV]", "f_0(eV^(-3/2))", {Qt::blue}, "EEDF", false, false, true);


    connect(plasma, &Plasma::stateUpdate,
            indicator, &ProbeIndicator::updateIndicator
            );
    connect(plasma, &Plasma::sendResults,
            this, &SimBox::getResults
            );

//    connect(plasma, &Plasma::stateUpdate,
//            this, &SimBox::updateSample
//            );

    connect(plasma, &Plasma::stateUpdate,
            visualizer, &Particles2D::updateSample
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


    connect(load_from_file_button, &QPushButton::clicked,
            this, [&](){
        QString filename = QFileDialog::getOpenFileName(this, "Load simulation parameters", "", "*.toml");
        if(filename != "") {
            const auto data = toml::parse(filename.toStdString());
            parameters = toml::get<EngineParameters>(data);
            p_h_edit->setText(QString::number(parameters.heavy_pressure));
            neutral_atomic_mass->setText(QString::number(parameters.neutral_atomic_mass));
            T_h_edit->setText(QString::number(parameters.heavy_temperature));
            T_e_edit->setText(QString::number(parameters.electron_temperature));
            electrons_edit->setText(QString::number(parameters.electrons_number));
            electrons_edit->setText(QString::number(parameters.electrons_number));
            electrons_to_show_edit->setText(QString::number(parameters.sample_size));
            total_steps_edit->setText(QString::number(parameters.total_steps));
            skip_steps_edit->setText(QString::number(parameters.skip_steps));
            double n0 = parameters.heavy_pressure/(physics::k * parameters.heavy_temperature);
            Townsend_edit->setText(QString::number(parameters.e_field_strength / (physics::Townsend_to_V_m2 * n0)));
            seed_edit->setText(QString::number(parameters.seed));
            W_edit->setText(QString::number(parameters.W));
            skip_steps_edit->setText(QString::number(parameters.skip_steps));
            skip_steps_edit->setText(QString::number(parameters.skip_steps));
            skip_steps_edit->setText(QString::number(parameters.skip_steps));
            nobounds_checkbox->setChecked(parameters.no_bounds);
            cross_sections_file->setText(QString::fromStdString(toml::find_or<std::string>(data, "cross-sections", "[cross-sections file]")));
            boundary_file->setText(QString::fromStdString(toml::find_or<std::string>(data, "boundary", "[boundary file]")));
        }
    });


    worker_thread.start();

}

SimBox::~SimBox()
{
}



void SimBox::resizeEvent(QResizeEvent * /* event */)
{
    //change graphicsView position

}

void SimBox::paintEvent(QPaintEvent* /* event*/)
{
//    QPainter canvas(this);
//    canvas.drawRect(0,0, box_width, box_height);
//    for(auto &parameters : sample.particles) {
//        canvas.drawPoint(parameters.coordinate[0], parameters.coordinate[1]);
//        //canvas.drawEllipse(QPoint(parameters.coordinate[0], parameters.coordinate[1]), 3, 3);
//    }
}

void SimBox::updateSample(const Sample& s){
    sample = s;
    repaint();
    //update();
}

void SimBox::showResults(){
    eedf->clear();
    for (int i = 0; i < results.eedf.size(); ++i) {
        eedf->append(results.eedf_granulation * (i+1), results.eedf[i]);
    }
    eedf->show();
}

void SimBox::getResults(const Results& r){
    results = r;
    results_button->setEnabled(true);
}

void SimBox::startComputation()
{
    emit abortPlasmaComputation();
    results_button->setEnabled(false);
    indicator->clear();
    sample_size = electrons_to_show_edit->text().toInt();


    parameters.no_bounds = nobounds_checkbox->isChecked();
    parameters.boundary = Boundary(boundary_file->text());

    box_width = parameters.boundary.bbox().width()+1;//width_edit->text().toDouble();
    box_height = parameters.boundary.bbox().height()+1;
    if (box_width == 0) box_width = 1000;
    if (box_height == 0) box_height = 700;

    parameters.seed = seed_edit->text().toInt();

    parameters.bounding_box[0] = box_width;
    parameters.bounding_box[1] = box_height;

    parameters.sample_size = sample_size;

    parameters.total_steps = total_steps_edit->text().toInt();

    parameters.neutral_atomic_mass = neutral_atomic_mass->text().toDouble();

    parameters.skip_steps = skip_steps_edit->text().toInt();
    if (parameters.skip_steps == 0) parameters.skip_steps = 1;

    parameters.electrons_number = electrons_edit->text().toInt();
    bool n_ok;
    parameters.heavy_pressure= p_h_edit->text().toDouble(&n_ok);
    if (!n_ok){
        parameters.heavy_pressure = 101325./760.;
    }


    parameters.heavy_temperature = T_h_edit->text().toDouble();
    if (parameters.heavy_temperature < 0.001) {
        parameters.heavy_temperature = 0.001;
    }
    double n0 = parameters.heavy_pressure/(physics::k * parameters.heavy_temperature);
    parameters.collisions = Collisions(cross_sections_file->text(), n0);
    parameters.electron_temperature = T_e_edit->text().toDouble();
    bool w_ok;
    parameters.W = W_edit->text().toDouble(&w_ok);
    if (!w_ok) {
        parameters.W = 0.5;
    }


    visualizer->resize(box_width, box_height);
    visualizer->setMinimumSize(box_width, box_height);
    visualizer->setMaximumSize(box_width, box_height);
    visualizer->setBoundary(parameters.boundary);

    parameters.e_field_strength = Townsend_edit->text().toDouble() * physics::Townsend_to_V_m2 * n0;

    if (show_indicator_checkbox->isChecked()) {
        indicator->showCharts();
    }

    emit setupPlasma(parameters);
}
