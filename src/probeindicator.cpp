#include "probeindicator.h"
#include "constants.h"
#include <iostream>
#include <QGridLayout>

ProbeIndicator::ProbeIndicator(QWidget *parent)
    : QWidget(parent)
{
    mean_energy = new Chart("Time [ns]", "Energy [eV]", {Qt::magenta}, "Mean energy");
    ionizations_minus_attachments = new Chart("Time[ns]", "# of ionizations - # of attachments", {Qt::blue}, "#ionization-#attachment");
    mobility = new Chart("Time [ns]", "Mobility [cm^2/(V*s)]", {Qt::red}, "Mobility");
    flux_drift_diffusion = new Chart("Time[ns]", "Flux drift diffusion [m^2/s]", {Qt::darkCyan}, "Flux drift diffusion");
    mean_x = new Chart("Time [ns]", "position [m]", {Qt::blue, Qt::magenta, Qt::red}, "Swarm center of mass", true, false, false, 3, {"x", "y", "z"});
    swarm_width = new Chart("Time [ns]", "width as coordinate dispersion [m^2]", {Qt::blue, Qt::magenta, Qt::red}, "Swarm width", true, false, false, 3, {"x", "y", "z"});
//    QGridLayout *layout = new QGridLayout;
//    layout->addWidget(mean_energy);
//    layout->addWidget(ionizations_minus_attachments);
//    setLayout(layout);
}


ProbeIndicator::~ProbeIndicator()
{

}

void ProbeIndicator::showCharts(){
    mean_energy->show();
    ionizations_minus_attachments->show();
    mobility->show();
    flux_drift_diffusion->show();
    mean_x->show();
    swarm_width->show();
}

void ProbeIndicator::updateIndicator(const Sample& s){
    double ns = 1000000000.;
    double time = s.time * ns;
    if(s.steady_state != steady) {
        steady = true;
        mean_energy->scene()->addRect(time, -mean_energy->max(), 3, 2*mean_energy->max());
    }

    if (s.mean_energy != INFINITY){
        mean_energy->append(time, s.mean_energy / physics::eV_to_J);
        ionizations_minus_attachments->append(time, s.ionizations_minus_attachments);
        mobility->append(time, s.mobility);
        flux_drift_diffusion->append(time, s.flux_drift_diffusion);
        for(int d =0; d < 3; ++d){
            mean_x->append(time, s.mean[d], d);
            swarm_width->append(time, s.width[d], d);
        }
    }
}

void ProbeIndicator::clear() {
    steady = false;
    mean_energy->clear();
    ionizations_minus_attachments->clear();
    flux_drift_diffusion->clear();
    mobility->clear();
    mean_x->clear();
    swarm_width->clear();
}
