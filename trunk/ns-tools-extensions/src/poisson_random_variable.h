#ifndef __poisson_random_variable_h__
#define __poisson_random_variable_h__

#include <ranvar.h>

class PoissonRandomVariable: public RandomVariable {
public:
    PoissonRandomVariable();
    PoissonRandomVariable(double);

    virtual double value();
    virtual double avg();

private:

    double generatePoissionBasedUponExponentialInterArrivalTime();
    double generatePoissionBasedUponMultiplicationOfUniformVariates();
    double avg_;
};

#endif /* __poisson_random_variable_h__ */

