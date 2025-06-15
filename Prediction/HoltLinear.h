#ifndef HOLTLINEAR_H
#define HOLTLINEAR_H

#include <vector>

class HoltLinear {
public:
    HoltLinear(double alpha = 0.2, double beta = 0.2);

    // Fit the model to the data
    void fit(const std::vector<double>& data);

    // Predict the value 'steps' into the future
    double predict(int steps = 1) const;

    // Getters for internal state
    double getLevel() const;
    double getTrend() const;

private:
    double alpha = 0.2;
    double beta = 0.2;
    double level = 0;
    double trend = 0;
    bool trained = false;
};

#endif // HOLTLINEAR_H
