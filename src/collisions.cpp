#include "collisions.h"
#include <QFile>
#include <QTextStream>
#include <QHash>
#include <QRegularExpression>
#include <iostream>


Collisions::Collisions(QString filename, double n0):
    n0(n0)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    QTextStream in(&file);
    QHash<QString, ReactionType> reaction_types{
        {"ELASTIC", ReactionType::Elastic},
        {"IONIZATION", ReactionType::Ionization},
        {"ATTACHMENT", ReactionType::Attachment},
        {"EXCITATION", ReactionType::Inelastic}
    };
    bool in_reaction = false;
    ReactionType current_reaction;
    QString species;
    double threshold = 0, mass_ratio = 0;
    static const QRegularExpression QRspaces("\\s+");
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (in_reaction) {
            QList<std::pair<qreal, qreal>> plot;//{J, m^2}
            while(!line.startsWith("-----")) {
                QStringList sl = line.split(QRspaces, Qt::SkipEmptyParts);
                double l = sl[0].toDouble() * physics::eV_to_J, r = sl[1].toDouble();
                plot.append({l, r});
                if (in.atEnd()) break;
                line = in.readLine();
            }
            reactions.append(Reaction(plot, current_reaction, threshold, mass_ratio));
            in_reaction = false;
        } else {
            if (!reaction_types.contains(line))
                continue;
            in_reaction = true;
            current_reaction = reaction_types.value(line);
            species = in.readLine();
            QString line = in.readLine();
            if(current_reaction == ReactionType::Inelastic || current_reaction == ReactionType::Ionization) {
                threshold = line.split(" ", Qt::SkipEmptyParts).at(0).toDouble() * physics::eV_to_J;
            }
            if(current_reaction == ReactionType::Elastic) {
                mass_ratio = line.split(" ", Qt::SkipEmptyParts).at(0).toDouble();
            }
            while(!line.startsWith("-----")) {
                line = in.readLine();
            }
        }
    }

    double e = grid_min, f = grid_factor;
    static const double e_to_v_electron_factor = 1. / sqrt(physics::m_e/2);

    nu_tot = 0;
    while(e < grid_max){
        QList<double> e_slice;
        double nu_accum = 0;
        for(Reaction& r : reactions) {
            double cs = r.cross_section(e);
            double v = sqrt(e) * e_to_v_electron_factor;
            double nu_cur = cs * n0 * v;
            nu_accum += nu_cur;
            if (e_slice.size() > 0) {
                e_slice.append(nu_cur + e_slice.last());
            } else  {
                e_slice.append(nu_cur);
            }
        }
        if (nu_tot < nu_accum) {
            nu_tot = nu_accum;
        }
        frequency_grid.append(e_slice);
        e = grid_min * f;
        f *= grid_factor;
    }
    for(int i = 0; i < frequency_grid.size(); ++i) {
        for(int r = 0; r < frequency_grid[i].size(); ++r) {
            frequency_grid[i][r] /= nu_tot;
        }
    }
}

int Collisions::choose_reaction(double energy, double uniform_0_1) const {
    double e = grid_min, f = grid_factor;
    int i = 0;
    while(!(e < energy && energy < e*grid_factor) && i < frequency_grid.size()-2) {
        e = grid_min * f;
        f *= grid_factor;
        i++;
    }
    if (uniform_0_1 < frequency_grid[i][0]) {
        return 0;
    }
    for(int r = 1; r < frequency_grid[i].size(); ++r) {
        if (frequency_grid[i][r-1] < uniform_0_1 && uniform_0_1 < frequency_grid[i][r]) {
            return r;
        }
    }
    return -1;
}
double Collisions::total_collision_frequency() const {
    return nu_tot;
}
