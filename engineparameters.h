#ifndef ENGINEPARAMETERS_H
#define ENGINEPARAMETERS_H
#include "boundary.h"
#include "collisions.h"
#include <cstddef>
#include <toml.hpp>
#include <QApplication>

struct EngineParameters{
    double bounding_box[3];

    size_t sample_size;
    int total_steps;
    int skip_steps;

    int seed;


    int electrons_number;
    double heavy_pressure;
    double heavy_temperature;
    double neutral_atomic_mass;
    double electron_temperature;
    double e_field_strength;

    double W;

    bool no_bounds;
    Boundary boundary;
    Collisions collisions;

    void from_toml(const toml::value& v) {
        this->sample_size                 =                   toml::find_or<size_t     >(v, "sample_size", 10);
        this->total_steps                 =                   toml::find_or<int        >(v, "total_steps", 1000);
        this->skip_steps                  =                   toml::find_or<int        >(v, "skip_steps", 10);
        this->seed                        =                   toml::find_or<int        >(v, "seed", 0);
        this->electrons_number            =                   toml::find_or<int        >(v, "electrons_number", 1000);
        this->heavy_pressure              =                   toml::find_or<double     >(v, "heavy_pressure", 101325./760.);
        this->heavy_temperature           =                   toml::find_or<double     >(v, "heavy_temperature", 123);

        double n0 = heavy_pressure / (physics::k * heavy_temperature);

        this->neutral_atomic_mass         =                   toml::find_or<double     >(v, "neutral_atomic_mass", 2);
        this->electron_temperature        =                   toml::find_or<double     >(v, "electron_temperature",123);
        this->e_field_strength            =                   toml::find_or<double     >(v, "E_field_strength",1) * physics::Townsend_to_V_m2 * n0;
        this->W           =                                   toml::find_or<double     >(v, "W", 0.5);
        this->no_bounds   =                                   toml::find_or<bool       >(v, "nobounds_mode", false);
        this->boundary    = Boundary  (QString::fromStdString(toml::find_or<std::string>(v, "boundary", "")));
        this->collisions  = Collisions(QString::fromStdString(toml::find_or<std::string>(v, "cross-sections", "")), n0 );
        bounding_box[0] = boundary.bbox().width()+1;//width_edit->text().toDouble();
        bounding_box[1] = boundary.bbox().height()+1;
        if (bounding_box[0] == 0) bounding_box[0] = 1000;
        if (bounding_box[1] == 0) bounding_box[1] = 700;

        return;
    }
}; Q_DECLARE_METATYPE(EngineParameters)

#endif // ENGINEPARAMETERS_H
