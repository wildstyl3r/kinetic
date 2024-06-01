#ifndef PLASMA_H
#define PLASMA_H

#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QApplication>
#include <vector>
#include <random>
#include <cmath>
#include "particle.h"
#include "engineparameters.h"

using std::vector;

//struct ParticleType{
//    double radius;
//    double mass;
//    int charge;
//};

struct Sample {
    double time;
    vector<Particle> particles;
    long long ionizations_minus_attachments;
    Vector3DD flux_drift_velocity;
    double flux_drift_diffusion;
    double mean_energy;
    double mobility;
    bool steady_state;
    Vector3DD mean;
    Vector3DD width;
}; Q_DECLARE_METATYPE(Sample)

struct Results {
    QVector<double> eedf;
    double eedf_granulation;
    double time_avg_energy;
    Vector3DD flux_drift_velocity;
    double flux_drift_diffusion;
}; Q_DECLARE_METATYPE(Results)

class Plasma : public QObject
{
    Q_OBJECT
public:
    const double r0 = 130e-12;
    Plasma(QObject *parent = nullptr);
    double mean_energy();
    double mobility(double nu_total);
    ~Plasma();
    QWaitCondition stopped;

public slots:
    void compute(const EngineParameters& params);
    void abortComputation();

signals:
    void stateUpdate(const Sample& s);
    void sendResults(const Results& s);

private:
    QMutex particles_mutex;
    vector<Particle> _particles;
    double time_delta = 1./1000000000000.; // s


    //vector<ParticleType> _particle_types;
    std::default_random_engine generator;


    QMutex mutex;
    bool abort = false;
};

#endif // PLASMA_H
