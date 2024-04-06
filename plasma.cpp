#include "plasma.h"
#include <iostream>
#include <math.h>

Plasma::Plasma(QObject *parent)
    : QThread(parent)
{

}

Plasma::~Plasma()
{
    mutex.lock();
    abort = true;
    condition.wakeOne();
    mutex.unlock();

    wait();
}


void Plasma::setup(double width, double height, int electrons_number, double heavy_number_density, double heavy_temperature, int skip_steps, int total_steps)
{
    QMutexLocker locker(&mutex);
    this->box_dimensions[0] = width;
    this->box_dimensions[1] = height;
    this->heavy_number_density = heavy_number_density;
    this->heavy_temperature = heavy_temperature;
    this->skip_steps = skip_steps;
    this->total_steps = total_steps;
    this->Lambda = std::cbrt(1/heavy_number_density);

    _particles.resize(electrons_number);
    this->generator = std::default_random_engine(14523);
    std::uniform_real_distribution<double> x_distribution(0,width);
    std::uniform_real_distribution<double> y_distribution(0,height);
    for(size_t i = 0; i < _particles.size(); ++i) {
        std::cout<< (_particles[i].coordinate[0] = x_distribution(generator)) << '\n';
        _particles[i].coordinate[1] = y_distribution(generator);
    }

    std::uniform_real_distribution<double> v_x_distribution(-3,3);
    std::uniform_real_distribution<double> v_y_distribution(-3,3);
    for (int i = 0; i < electrons_number; ++i) {
        _particles[i].velocity[0] = 2;//v_x_distribution(generator);
        _particles[i].velocity[1] = 2;//v_y_distribution(generator);
    }

    if (!isRunning()) {
        start(LowPriority);
    } else {
        restart = true;
        condition.wakeOne();
    }
}

void Plasma::run()
{
        mutex.lock();
        const double box_dimensions[3] = {this->box_dimensions[0],this->box_dimensions[1], this->box_dimensions[2]};
        const int dimensions = this->dimensions;
        mutex.unlock();
        std::cout << "Lambda: " << Lambda << '\n';
        std::uniform_real_distribution<double> collision_check(0, M_PI*pow(r0+Lambda, 2));
        std::uniform_real_distribution<double> deflection_angle(0, 2*M_PI);
        double collision_threshold = M_PI * pow(r0, 2);

        for (int t = 0; t < total_steps; ++t) {
            if (restart)
                break;
            if (abort)
                return;
            particles_mutex.lock();
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

                //simple box boundary
                for (int d = 0; d < dimensions; ++d){
                    if (_particles[i].coordinate[d] < 0 || _particles[i].coordinate[d] > box_dimensions[d]){
                        _particles[i].velocity[d] = -_particles[i].velocity[d];
                        //std::cout<<"bound\n";
                        _particles[i].coordinate[d] += _particles[i].velocity[d];
                    }
                }
            }
            particles_mutex.unlock();



            if (t % skip_steps == 0 && !restart) {
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
                                        vector<Particle>(_particles.begin(), _particles.begin()+sample_size),
                                        mean
                                        });//(particles);
                QThread::msleep(50);
                //std::cout << t<<"time\n";
            }
        }

        mutex.lock();
        if (!restart)
            condition.wait(&mutex);
        restart = false;
        mutex.unlock();
}
