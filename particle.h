#ifndef PARTICLE_H
#define PARTICLE_H
#include "vector3dd.h"

struct Particle {
    Vector3DD x;
    Vector3DD prev_x;
    Vector3DD v;
    Vector3DD prev_v;
    Vector3DD a;

    Particle(Vector3DD x = {.0, .0, 0.}, Vector3DD v = {.0, .0, .0}, Vector3DD a = {.0, .0, .0});
    void advance(double time_delta = 0);
    double energy();
};

#endif // PARTICLE_H
