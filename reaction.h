#ifndef REACTION_H
#define REACTION_H

#include <QString>
#include <QList>
#include "particle.h"

enum class ReactionType{
    Elastic,
    Ionization,
    Attachment,
    Inelastic,
    Superelastic
};

struct Reaction
{
    QList<std::pair<qreal, qreal>> datapoints;
    ReactionType type;
    double threshold = 0;
    double mass_ratio = 0;

public:
    Reaction(QList<std::pair<qreal,qreal>> datapoints, ReactionType type, double threshold = 0, double mass_ratio = 1);
    double cross_section(double energy) const;
};

#endif // REACTION_H
