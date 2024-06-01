#include "reaction.h"

Reaction::Reaction(QList<std::pair<qreal,qreal>> datapoints, ReactionType type, double threshold, double mass_ratio):
    datapoints(datapoints), type(type), threshold(threshold), mass_ratio(mass_ratio)
{}

double Reaction::cross_section(double energy) const {
    if (energy < threshold || energy < datapoints.at(0).first) {
        return 0;
    }
    int l = 0, r = datapoints.size(), c;
    while (l + 1 < r) {
        c = (l + r) / 2;
        if (datapoints.at(c).first > energy) {
            r = c;
        } else {
            l = c;
        }
    }
    if(l < datapoints.size()-1) {
        double w = (energy - datapoints.at(l).first) / (datapoints.at(l + 1).first - datapoints.at(l).first);
        return datapoints.at(l).second + w * (datapoints.at(l+1).second - datapoints.at(l).second);
    } else {
        return datapoints.at(l).second;
    }
}
