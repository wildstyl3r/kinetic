#include "boundary.h"
#include <iostream>
#include <random>
#include <QFile>
#include <QTextStream>
#include <QLineF>

Boundary::Boundary()
{
}

Boundary::Boundary(QString filename)
{
    using std::cerr;

    if(filename == "") {
        *this <<  QPointF(0.0, 0.0);
        *this <<  QPointF(1000.0, 0.0);
        *this <<  QPointF(1000.0, 500.0);
        *this <<  QPointF(0.0, 500.0);
        v_cycles.insert(0);
        h_cycles.insert(1);
        v_cycles.insert(2);
        h_cycles.insert(3);
        bounding_box.setWidth(1000);
        bounding_box.setHeight(500);
        return;
    }

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        *this <<  QPointF(0.0, 0.0);
        *this <<  QPointF(1000.0, 0.0);
        *this <<  QPointF(1000.0, 500.0);
        *this <<  QPointF(0.0, 500.0);
        v_cycles.insert(0);
        h_cycles.insert(1);
        v_cycles.insert(2);
        h_cycles.insert(3);
        bounding_box.setWidth(1000);
        bounding_box.setHeight(500);
        return;
    }

    QTextStream in(&file);
    bounding_box = {0,0};
    int i = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList coords = line.split(u' ');
        if (coords.length() > 2) {
            if(coords[2] == "hc") {
                h_cycles.insert(i);
            }
            if(coords[2] == "vc") {
                v_cycles.insert(i);
            }
        }
        bounding_box.setWidth(std::max(coords[0].toDouble(), bounding_box.width()));
        bounding_box.setHeight(std::max(coords[1].toDouble(), bounding_box.height()));
        *this <<  QPointF(coords[0].toDouble(), coords[1].toDouble());
        i++;
    }
    double min_x = bounding_box.width(), min_y = bounding_box.height();
    for (int i = 0; i < this->size(); ++i){
        if (min_x > this->at(i).x()) {
            min_x = this->at(i).x();
        }
        if (min_y > this->at(i).y()) {
            min_y = this->at(i).y();
        }
    }
    for (int i = 0; i < this->size(); ++i) {
        this->operator[](i).setX(this->operator[](i).x() - min_x);
        this->operator[](i).setY(this->operator[](i).y() - min_y);
    }
    for (int i = 0; i < this->size(); ++i) {
        if (h_cycles.contains(i) && this->at(i).x() != this->at((i+1) % this->size()).x()) {
            h_cycles.remove(i);
        }
        if (v_cycles.contains(i) && this->at(i).y() != this->at((i+1) % this->size()).y()) {
            v_cycles.remove(i);
        }
    }
}

void Boundary::process(Particle& p) const {
    if (this->containsParticle(p)){
        return;
    }
    QPointF next(p.x[0], p.x[1]);
    QPointF prev(p.prev_x[0], p.prev_x[1]);


    auto [segment_index, intersection] = this->find_intersection_segment(next, prev);
    if (h_cycles.contains(segment_index)) {
        if (p.x[0] < 0.) {
            p.x[0] += bounding_box.width();
        } else {
            p.x[0] -= bounding_box.width();
        }
        return;
    }
    if (v_cycles.contains(segment_index)) {
        if (p.x[1] < 0.) {
            p.x[1] += bounding_box.height();
        } else {
            p.x[1] -= bounding_box.height();
        }
        return;
    }
    QLineF segment(this->at(segment_index), this->at((segment_index+1) % this->size()));
//    std::cout<<"pt: (" << p.x[0] << " " << p.x[1] << ")\n";
//    std::cout<<"seg: (" << segment.p1().x() << " " << segment.p1().y() << "," << segment.p2().x() << " " << segment.p2().y() << ")\n";
    double abs_v = QLineF({0,0}, {p.v[0], p.v[1]}).length();
    //std::cout << "abs_vel: " << abs_velocity << '\n';
    double rx = prev.x() - intersection.x();
    double ry = prev.y() - intersection.y();
    double mx = segment.p2().x() - segment.p1().x();
    double my = segment.p2().y() - segment.p1().y();

    //std::cout << "change: " << rx * (my * my - mx * mx) - 2 * ry * mx * my << " " << ry * (mx * mx - my * my) - 2 * rx * mx * my << '\n';

    QLineF alt_dir(intersection, QPointF(rx * (my * my - mx * mx) - 2 * ry * mx * my, ry * (mx * mx - my * my) - 2 * rx * mx * my));
    alt_dir.setLength(abs_v - QLineF(QPointF(0,0), intersection-prev).length());
    QPointF alt = alt_dir.p1();
    //std::cout << "alt: " << alt.x() << " " << alt.y() << '\n';
    alt_dir.setLength(abs_v);
    QPointF vel = alt_dir.p2() - alt_dir.p1();

    p.x[0] = alt.x()+vel.x();
    p.x[1] = alt.y()+vel.y();
    p.v[0] = vel.x();
    p.v[1] = vel.y();
    return;
}

pair<int, QPointF> Boundary::find_intersection_segment(QPointF next, QPointF prev) const{
    if(this->size() < 2) {
        return {0, QPointF()};
    }
    QLineF path(prev, next);
    int closest_segment = this->size()-1;
    QPointF closest_intersection;
    QLineF res(this->at(this->size()-1), this->at(0));
    res.intersects(path, &closest_intersection);
    for(int i = 1; i < this->size(); ++i) {
        QPointF intersection;
        if (QLineF(this->at(i-1), this->at(i)).intersects(path, &intersection) == QLineF::IntersectType::BoundedIntersection &&
            QPointF::dotProduct(closest_intersection-prev, closest_intersection-prev) > QPointF::dotProduct(intersection-prev, intersection-prev)){
            closest_intersection = intersection;
            closest_segment = i-1;
            //res = QLineF(this->at(i-1), this->at(i));
        }
    }
    return {closest_segment, closest_intersection};
}


bool Boundary::containsParticle(Particle& p) const{
    return containsPoint(QPointF(p.x[0], p.x[1]), Qt::OddEvenFill);
}

QSizeF Boundary::bbox() {
    return bounding_box;
}

bool Boundary::cycled(int i) {
    return h_cycles.contains(i) || v_cycles.contains(i);
}
