#ifndef __flattop_directional_antenna_h__
#define __flattop_directional_antenna_h__

#include <list>
#include "directional_antenna.h"

class FlatTopDirectionalAntenna: public DirectionalAntenna {

public:
	FlatTopDirectionalAntenna();
	FlatTopDirectionalAntenna(const FlatTopDirectionalAntenna& antenna);
	virtual ~FlatTopDirectionalAntenna();

	virtual double getTxGain(double dX, double dY, double dZ, double lambda);
	virtual double getRxGain(double dX, double dY, double dZ, double lambda);

	virtual Antenna * copy();
	virtual void release();

	virtual int command(int, const char * const *);

private:
	static list<DirectionalAntenna*> copies_;

	double mainGain_; /* Main lobe gain */
	double sideGain_; /* Side lobe gain */
};

#endif /* __flattop_directional_antenna_h__ */

