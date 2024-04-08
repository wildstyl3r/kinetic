#include "plasma.h"
#include <iostream>
#include <math.h>

Plasma::Plasma(QObject *parent)
    : QObject(parent)
{

}

Plasma::~Plasma()
{
//    mutex.lock();
//    abort = true;
//    condition.wakeOne();
//    mutex.unlock();

//    wait();
}

void Plasma::abortComputation()
{
    mutex.lock();
    abort = true;
    mutex.unlock();
}


void Plasma::compute(const EngineParameters& params)
{
    //QMutexLocker locker(&mutex);
    mutex.lock();
    abort = false;
    mutex.unlock();

    this->box_dimensions[0] = params.bounding_box[0];
    this->box_dimensions[1] = params.bounding_box[1];

    this->sample_size = params.sample_size;
    this->total_steps = params.total_steps;
    this->skip_steps = params.skip_steps;

    _particles.resize(params.electrons_number);
    this->heavy_number_density = params.heavy_number_density;
    this->heavy_temperature = params.heavy_temperature;

    this->Lambda = std::cbrt(1/heavy_number_density);

    this->generator = std::default_random_engine(14523);
    std::uniform_real_distribution<double> x_distribution(0,box_dimensions[0]);
    std::uniform_real_distribution<double> y_distribution(0,box_dimensions[1]);
    for(size_t i = 0; i < _particles.size(); ++i) {
        std::cout<< (_particles[i].coordinate[0] = x_distribution(generator)) << '\n';
        _particles[i].coordinate[1] = y_distribution(generator);
    }

    std::uniform_real_distribution<double> v_x_distribution(-3,3);
    std::uniform_real_distribution<double> v_y_distribution(-3,3);
    for (size_t i = 0; i < _particles.size(); ++i) {
        _particles[i].velocity[0] = 2;//v_x_distribution(generator);
        _particles[i].velocity[1] = 2;//v_y_distribution(generator);
    }
        std::cout << "Lambda: " << Lambda << '\n';
        std::uniform_real_distribution<double> collision_check(0, M_PI*pow(r0+Lambda, 2));
        std::uniform_real_distribution<double> deflection_angle(0, 2*M_PI);
        double collision_threshold = M_PI * pow(r0, 2);

        for (int t = 0; t < total_steps; ++t) {
            if (abort)
                return;
            //particles_mutex.lock();
            for (size_t i = 0; i < _particles.size(); ++i){
                if(collision_check(generator) < collision_threshold) {
                    double theta = deflection_angle(generator);
                    double cosTh = cos(theta);
                    double sinTh = sin(theta);
                    double v_x = _particles[i].velocity[0] * cosTh - _particles[i].velocity[1] * sinTh;
                    double v_y = _particles[i].velocity[0] * sinTh + _particles[i].velocity[1] * cosTh;

                    _particles[i].velocity[0] = v_x;
                    _particles[i].velocity[1] = v_y;

                }
                for (int d = 0; d < dimensions; ++d){
                    _particles[i].coordinate[d] += _particles[i].velocity[d];
                    //std::cout<<"cupd\n";
                }

                for (int d = 0; d < dimensions; ++d){
                    if (_particles[i].coordinate[d] < 0 || _particles[i].coordinate[d] > box_dimensions[d]){
                        _particles[i].velocity[d] = -_particles[i].velocity[d];
                        //std::cout<<"bound\n";
                        _particles[i].coordinate[d] += _particles[i].velocity[d];
                    }
                }
            }
            //particles_mutex.unlock();



            if (t % skip_steps == 0) {
                Particle mean = {{.0,.0}, {.0,.0}};
                for (size_t i = 0; i < _particles.size(); ++i){
                    for (size_t d = 0; d < dimensions; ++d){
                        mean.coordinate[d] += _particles[i].coordinate[d];
                        mean.velocity[d] += _particles[i].velocity[d];
                    }
                }
                for (size_t d = 0; d < dimensions; ++d){
                    mean.coordinate[d] /= _particles.size();
                    mean.velocity[d] /= _particles.size();
                }

                emit stateUpdate(Sample{
                                        vector<Particle>(_particles.begin(), _particles.begin()+std::min(sample_size, _particles.size())),
                                        mean
                                        });
                qApp->processEvents();
            }
        }
}
