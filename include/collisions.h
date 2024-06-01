#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "reaction.h"
#include "constants.h"
#include <random>

struct Collisions
{
    double n0;
    double nu_tot = 0;
    QList<Reaction> reactions;
    QList<QList<double>> frequency_grid;
    double grid_factor = 33./32.;
    double grid_min = 0.01 * physics::eV_to_J; //eV
    double grid_max = 1100 * physics::eV_to_J; //eV

public:
    Collisions(QString filename = "", double n0 = 1.e22);//(double n, QVector<std::pair<double, double>> cross_section);
    int choose_reaction(double energy, double uniform_0_1) const;
    double total_collision_frequency() const;
    //double total_cross_section(double energy);
    //void process(Particle& p, std::default_random_engine& generator, double total_collision_frequency) const;
};

#endif // COLLISIONS_H
