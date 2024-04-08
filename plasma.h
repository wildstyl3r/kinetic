#ifndef PLASMA_H
#define PLASMA_H

#include <QThread>
#include <QMutex>
#include <QApplication>
#include <vector>
#include <random>

using std::vector;

//struct ParticleType{
//    double radius;
//    double mass;
//    int charge;
//};

struct Particle {
    //int particle_type_index;
    double coordinate[2];
    double velocity[2];
    //double acceleration[];

};

struct EngineParameters{
    double bounding_box[3];

    int sample_size;
    int total_steps;
    int skip_steps;


    int electrons_number;
    double heavy_number_density;
    double heavy_temperature;
    double electron_temperature;
}; Q_DECLARE_METATYPE(EngineParameters)

struct Sample {
    vector<Particle> particles;
    Particle mean;
}; Q_DECLARE_METATYPE(Sample)

class Plasma : public QObject //QThread
{
    Q_OBJECT
public:
    const double r0 = 2.8179403267e-15;
    Plasma(QObject *parent = nullptr);
    ~Plasma();

public slots:
    void compute(const EngineParameters& params);//double width, double height, int electrons_number, double heavy_number_density, double heavy_temperature, int skip_steps, int total_steps);
    void abortComputation();

    //const vector<ParticleType>& particle_types();

signals:
    void stateUpdate(const Sample& s);//(const vector<Particle> &particles);

//protected:
//    void run() override;

private:
    QMutex particles_mutex;
    vector<Particle> _particles;
    //double width;
    //double height;
    double box_dimensions[3];
    int dimensions = 2;
    double heavy_number_density;
    double heavy_temperature;
    double particle_radius = 3;
    double Lambda = 0;

    int skip_steps;
    int total_steps;


    size_t sample_size;


    //vector<ParticleType> _particle_types;
    std::default_random_engine generator;


    QMutex mutex;
    bool abort = false;
};

#endif // PLASMA_H
