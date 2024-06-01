#include "plasma.h"
#include <iostream>
#include <math.h>

Plasma::Plasma(QObject *parent)
    : QObject(parent)
{

}

Plasma::~Plasma()
{}

void Plasma::abortComputation()
{
    mutex.lock();
    abort = true;
    mutex.unlock();
}

double Plasma::mean_energy(){
    double mean_e = 0;
    for(Particle& p : _particles) {
        mean_e += p.v.lengthSquared() * 0.5 * physics::m_e;
    }
    mean_e /= _particles.size();
    return mean_e;
};

double Plasma::mobility(double nu_total) {
    double avg_loss_ratio = 0;
    for(Particle& p : _particles) {
        double v = p.v.length();
        double u = p.prev_v.length();
        avg_loss_ratio += (v-u)/v;
    }
    avg_loss_ratio /= _particles.size();
    return avg_loss_ratio * nu_total;
}

void Plasma::compute(const EngineParameters& params)
{
    mutex.lock();
    abort = false;
    mutex.unlock();

    _particles.resize(params.electrons_number);

    //double velocity_squared_to_energy_factor = physics::m_e * 0.5 / _particles.size();
    double averaging_weight = 1./(double)_particles.size();

    this->generator = std::default_random_engine(params.seed);
    std::uniform_real_distribution<double> x_distribution(0,params.bounding_box[0]);
    std::uniform_real_distribution<double> y_distribution(0,params.bounding_box[1]);
    Vector3DD center;
    if (params.no_bounds)
        center = {0,0,0};
    else
        center = {params.bounding_box[0]/2.,params.bounding_box[1]/2.,params.bounding_box[2]/2.};
    std::uniform_real_distribution<double> uniform_zero_to_one(0,1);

    std::normal_distribution<double> v_distribution(0,sqrt(physics::k*params.electron_temperature/physics::m_e));

    size_t loopout = 0;
    size_t p = 0;
    while (loopout < 100*_particles.size() && p < _particles.size()) {
        double x = x_distribution(generator);
        double y = y_distribution(generator);

        if(params.boundary.containsPoint(QPointF(x,y), Qt::OddEvenFill)){
            if(params.no_bounds)
                _particles[p] = Particle({0,0, 0}, {v_distribution(generator), v_distribution(generator), v_distribution(generator)});
            else
                _particles[p] = Particle({x,y, 0}, {v_distribution(generator), v_distribution(generator), v_distribution(generator)});
            p++;
        }
        loopout++;
    }
    if (p < _particles.size()) {
        std::cerr << "invalid bounding box or shape area is too small";
        //exit(0);
    }
    double nu_trial = params.collisions.total_collision_frequency();

    double total_accum_velocity_squared = 0;
    Vector3DD flux_drift_velocity(0,0,0);
    static const double eedf_granulation = 0.01;
    static const double eedf_max_energy = 60;
    QVector<double> eedf_ev (eedf_max_energy/eedf_granulation + 1, 0.);
    double current_time = 0;
    double steady_time = 0;
    double skip_time = 0;
    const double mass_share = physics::m_e / (physics::m_e + params.neutral_atomic_mass * physics::dalton_to_kg);
    bool steady_state = false;
    int steady_steps = 0;
    QVector<double> energies_at_time(params.total_steps);
    QVector<double> rv(_particles.size(),0);
    QVector<Vector3DD> r(_particles.size(),{0.,0.,0.});
    QVector<Vector3DD> v(_particles.size(),{0.,0.,0.});
    long long ionizations_minus_attachments = 0;

    for (int t = 0; t < params.total_steps; ++t) {
        double velocity = 0;
        double velocity_delta = 0;

        for (size_t i = 0; i < _particles.size(); ++i) {
            int r = params.collisions.choose_reaction(_particles[i].energy(), uniform_zero_to_one(generator));
            if (r == -1)
                continue;

            Vector3DD v_cm, v_dash;
            double e = 0, th = 0, residual_vt;
            int replace;
            Particle ejected;
            switch(params.collisions.reactions[r].type) {
            case ReactionType::Elastic:
                v_cm = mass_share * _particles[i].v;
                v_dash = _particles[i].v - v_cm;
                _particles[i].v = Vector3DD::isotropic_from_random(uniform_zero_to_one(generator), uniform_zero_to_one(generator)) * v_dash.length() + v_cm;
                break;
            case ReactionType::Inelastic:
                e = _particles[i].energy();
                th = params.collisions.reactions[r].threshold;
                if (e >= th) {
                    velocity = _particles[i].v.length();
                    velocity_delta = sqrt(2 * params.collisions.reactions[r].threshold / physics::m_e);
                    v_cm = mass_share * _particles[i].v;
                    v_dash = _particles[i].v - v_cm;
                    _particles[i].v = Vector3DD::isotropic_from_random(uniform_zero_to_one(generator), uniform_zero_to_one(generator)) * v_dash.length() + v_cm;
                    _particles[i].v *= (velocity - velocity_delta) / velocity;
                }
                break;
            case ReactionType::Attachment:
                replace = (size_t) ((double)(_particles.size()-2) * uniform_zero_to_one(generator));
                if (replace >= i) replace++;
                _particles[i] = _particles[replace];
                ionizations_minus_attachments--;
                break;
            case ReactionType::Ionization:
                replace = (int) ((double)(_particles.size()-1) * uniform_zero_to_one(generator));
                velocity = _particles[i].v.length();
                velocity_delta = sqrt(2 * params.collisions.reactions[r].threshold / physics::m_e);
                residual_vt = _particles[i].v.length() * ((velocity - velocity_delta) / velocity);
                ejected.prev_x = ejected.x = _particles[i].x;
                ejected.v = Vector3DD::isotropic_from_random(uniform_zero_to_one(generator), uniform_zero_to_one(generator)) * params.W * residual_vt;
                _particles[i].v = Vector3DD::isotropic_from_random(uniform_zero_to_one(generator), uniform_zero_to_one(generator)) * (1.-params.W) * residual_vt;
                _particles[replace] = ejected;
                ionizations_minus_attachments++;

                break;
            case ReactionType::Superelastic:
                break;
            }
        }


        double p = uniform_zero_to_one(generator);
        while(p == 0.) {
            p = uniform_zero_to_one(generator);
        }
        time_delta = - log(p) / nu_trial;
        current_time += time_delta;
        skip_time += time_delta;


        double instant_mean_energy = 0;
        double d_xv = 0;
        Vector3DD d_mean_x(0,0,0), d_flux_drift_velocity(0,0,0);
        for (size_t i = 0; i < _particles.size(); ++i){
            if (abort)
                return;

            if (params.e_field_strength != 0.) {
                _particles[i].a[0] = physics::q_e * params.e_field_strength / physics::m_e; // a * dt
            } else {
                _particles[i].a[0] = 0;
            }
            _particles[i].advance(time_delta);
            instant_mean_energy += _particles[i].v.lengthSquared();
            if(!params.no_bounds){
                params.boundary.process(_particles[i]);
            }

            rv[i] += Vector3DD::dotProduct(_particles[i].x - center, _particles[i].v) * time_delta;
            r[i] += (_particles[i].x - center) * time_delta;
            v[i] += _particles[i].v * time_delta;
            d_xv += Vector3DD::dotProduct(_particles[i].x - center, _particles[i].v);
            d_mean_x += (_particles[i].x - center);
            d_flux_drift_velocity += _particles[i].v;
        }
        d_flux_drift_velocity *= averaging_weight;
        d_mean_x *= averaging_weight;

        Vector3DD width(0,0,0);
        double flux_drift_diffusion = 0;
        for (size_t i = 0; i < _particles.size(); ++i){
            if (abort)
                return;
            width += (_particles[i].x - center - d_mean_x) * (_particles[i].x - center - d_mean_x);
            flux_drift_diffusion += rv[i]/current_time - Vector3DD::dotProduct(r[i], v[i])/(current_time * current_time);
        }

        flux_drift_velocity += d_flux_drift_velocity * time_delta;

        instant_mean_energy *= physics::m_e * 0.5 * averaging_weight;
        energies_at_time[t] = instant_mean_energy;
        if(!steady_state && t > 16) {
            double mean_2q = 0;
            for(int i = t/4; i < t/2; ++i) {
                mean_2q += energies_at_time[i];
            }
            mean_2q /= (t/2 - t/4);

            double mean_4q = 0;
            for(int i = 2*t/3; i < t; ++i) {
                mean_4q += energies_at_time[i];
            }
            mean_4q /= (t - 3*t/4);

            double disp_4q = 0;
            for(int i = 3*t/4; i < t; ++i) {
                double en = energies_at_time[i];
                double dev = en - mean_4q;
                disp_4q += dev * dev;
            }
            disp_4q /= (t - 3*t/4);
            if (abs(mean_2q - mean_4q) < sqrt(disp_4q)) {
                steady_state = true;
                steady_time = current_time;
            }
        }

        if(steady_state) {
            steady_steps++;

            for (size_t i = 0; i < _particles.size(); ++i){
                if (abort)
                    return;
                total_accum_velocity_squared += _particles[i].v.lengthSquared() * time_delta;
                double e = _particles[i].energy() / physics::eV_to_J;
                if(e < eedf_max_energy) {
                    eedf_ev[e / eedf_granulation] += 1.;
                }
            }
        }




        if (t > 0 && t % params.skip_steps == 0) {

            Sample s;
            s.time = current_time;

            s.particles = vector<Particle>(_particles.begin(), _particles.begin()+std::min(params.sample_size, _particles.size()));
            s.flux_drift_velocity = flux_drift_velocity / current_time;
            //flux_drift_velocity = {0.,0.,0.};

            s.flux_drift_diffusion = flux_drift_diffusion * averaging_weight;//(xv - Vector3DD::dotProduct(mean_x, flux_drift_velocity)) * averaging_weight / current_time;

            s.mean_energy = instant_mean_energy;

            if(steady_state)
                s.mobility = abs(s.flux_drift_velocity[0] / params.e_field_strength) * 1.e4;//mobility(nu_trial);
            else
                s.mobility = 0;

            s.steady_state = steady_state;

            s.ionizations_minus_attachments = ionizations_minus_attachments;
            s.mean = d_mean_x;
            s.width = width * averaging_weight;

            emit stateUpdate(s);
            qApp->processEvents();
            t++;
            skip_time = 0;
        }
    }

    for(int i = 0; i < eedf_ev.size(); ++i){
        eedf_ev[i] /= ((double) (steady_steps * _particles.size()) * sqrt((double(i)+0.5) * eedf_granulation) * eedf_granulation);
    }

    emit sendResults(Results{
                         eedf_ev,
                         eedf_granulation,
                         total_accum_velocity_squared / (current_time - steady_time) * physics::m_e * 0.5 * averaging_weight //time mean energy
                     });

    return;
}
