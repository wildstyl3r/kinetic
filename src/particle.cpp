#include "particle.h"
#include "constants.h"

Particle::Particle(Vector3DD x, Vector3DD v, Vector3DD a) : x(x), prev_x(x), v(v), prev_v(v), a(a) {}

void Particle::advance(double time_delta) {
    prev_x = x;
    x += v * time_delta;
    prev_v = v;
    v += a * time_delta;
}

double Particle::energy() {
    return v.lengthSquared() * 0.5 * physics::m_e;
}
