#ifndef SIMBOX_H
#define SIMBOX_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QtCharts/QChartView>
#include "plasma.h"
#include "probeindicator.h"
#include "particles2d.h"

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
    void showResults();
    void getResults(const Results& r);

signals:
    void setupPlasma(const EngineParameters& params);//double width, double height, int electrons_number, double heavy_number_density, double heavy_temperature, int skip_steps, int total_steps);
    void abortPlasmaComputation();
    void finish();

private:
    QThread worker_thread;

    QHBoxLayout* main_layout;

    Particles2D* visualizer;

    QLineEdit* width_edit;
    QLineEdit* height_edit;
    QLabel* boundary_file;
    QCheckBox* nobounds_checkbox;
    QLabel* cross_sections_file;

    QLineEdit* p_h_edit;
    QLineEdit* T_h_edit;
    QLineEdit* T_e_edit;

    QLineEdit* electrons_edit;
    QLineEdit* electrons_to_show_edit;

    QLineEdit* neutral_atomic_mass;

    QLineEdit* total_steps_edit;
    QLineEdit* skip_steps_edit;

    QLineEdit* Townsend_edit;
    QLineEdit* W_edit;
    QLineEdit* seed_edit;

    QPushButton* boundary_button;
    QPushButton* load_from_file_button;
    QPushButton* start_button;
    QPushButton* results_button;

    ProbeIndicator* indicator;
    QCheckBox* show_indicator_checkbox;

    Chart* eedf;

    EngineParameters parameters;
    Sample sample;
    Results results;
    int counter = 0;
    int sample_size;
    Ui::SimBox *ui;
    vector<QGraphicsEllipseItem*> particles;
    double centerX;
    double centerY;
    double box_width;
    double box_height;
};
#endif // SIMBOX_H
