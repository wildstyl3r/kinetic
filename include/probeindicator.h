#ifndef MEANVELOCITY_H
#define MEANVELOCITY_H

#include <QWidget>
#include "plasma.h"
#include "chart.h"

class ProbeIndicator : public QWidget
{
    Q_OBJECT
public:
    explicit ProbeIndicator(QWidget *parent = nullptr);
    void clear();
    virtual ~ProbeIndicator();
    void showCharts();

public slots:
    void updateIndicator(const Sample& s);
signals:

private:
    bool steady = false;

    Chart *mean_energy;
    Chart *mobility;
    Chart *flux_drift_diffusion;
    Chart *ionizations_minus_attachments;
    Chart *mean_x;
    Chart *swarm_width;
};

#endif // MEANVELOCITY_H
