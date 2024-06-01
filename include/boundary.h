#ifndef BOUNDARY_H
#define BOUNDARY_H

#include <QPolygonF>
#include <QLineF>
#include <QSet>
#include "particle.h"

using std::string, std::pair;

class Boundary : public QPolygonF
{
    //Q_OBJECT
public:
    Boundary();
    Boundary(QString filename);
    bool containsParticle(Particle& p) const;
    void process(Particle& p) const;
    pair<int, QPointF> find_intersection_segment(QPointF next, QPointF velocity) const;
    QLineF bisect_angle(QLineF wall_a, QLineF wall_b);
    QSizeF bbox();
    bool cycled(int i);
private:
    QSizeF bounding_box;
    QSet<int> h_cycles, v_cycles;
};

#endif // BOUNDARY_H
