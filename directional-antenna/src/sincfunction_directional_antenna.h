#ifndef __sincfunction_directional_antenna_h__
#define __sincfunction_directional_antenna_h__

#include <list>
#include "directional_antenna.h"

class SincFunctionDirectionalAntenna: public DirectionalAntenna {

public:
	SincFunctionDirectionalAntenna();
	SincFunctionDirectionalAntenna(const SincFunctionDirectionalAntenna& antenna);
	virtual ~SincFunctionDirectionalAntenna();

	virtual double getTxGain(double dX, double dY, double dZ, double lambda);
	virtual double getRxGain(double dX, double dY, double dZ, double lambda);

	virtual Antenna * copy();
	virtual void release();

	virtual int command(int, const char * const *);

private:
    bool isAngleWithin(double angle, double center, double width);

	static list<DirectionalAntenna*> copies_;

	double mainGain_; /* Main lobe gain */

	double nullLocation_; /* position of the null (deg) */
	double nullWidth_; /* width of the null (deg) */
};

#endif /* __sincfunction_directional_antenna_h__ */
