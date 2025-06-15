#include "HoltLinear.h"

HoltLinear::HoltLinear(double alpha, double beta)
    : alpha(alpha),
      beta(beta)
{
}

void HoltLinear::fit(const std::vector<double>& data) {
    if (data.size() < 2) return;

    level = data[0];
    trend = data[1] - data[0];

    for (unsigned long i = 1; i < data.size(); ++i) {
        double prev_level = level;
        level = alpha * data[i] + (1.0 - alpha) * (level + trend);
        trend = beta * (level - prev_level) + (1.0 - beta) * trend;
    }

    trained = true;
}

double HoltLinear::predict(int steps) const {
    if (!trained) return 0.0;
    return level + steps * trend;
}

double HoltLinear::getLevel() const {
    return level;
}

double HoltLinear::getTrend() const {
    return trend;
}
