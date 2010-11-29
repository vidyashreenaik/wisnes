#include <cmath>

#include "poisson_random_variable.h"

static class PoissonRandomVariableClass: public TclClass {
public:
    PoissonRandomVariableClass() :
        TclClass("RandomVariable/Poisson") {
    }

    TclObject* create(int, const char* const *) {
        return (new PoissonRandomVariable());
    }
} class_exponentialranvar;

PoissonRandomVariable::PoissonRandomVariable() {
    bind("avg_", &avg_);
}

PoissonRandomVariable::PoissonRandomVariable(double avg) {
    avg_ = avg;
}

double PoissonRandomVariable::avg() {
    return avg_;
}

double PoissonRandomVariable::value() {
    return generatePoissionBasedUponMultiplicationOfUniformVariates();
}

double PoissonRandomVariable::generatePoissionBasedUponExponentialInterArrivalTime() {
    double k = 0;
    double sum = 0;

    do {
        k = k + 1;
        sum = sum + rng_->exponential();
    } while (sum < avg_);

    return k - 1;
}

double PoissonRandomVariable::generatePoissionBasedUponMultiplicationOfUniformVariates() {
    double k = 0;
    double p = 1;
    double L = exp(-avg_);

    do {
        k = k + 1;
        p = p * rng_->uniform(0, 1);
    } while (p > L);

    return k - 1;
}
