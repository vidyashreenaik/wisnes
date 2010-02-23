#ifndef __pattern_directional_antenna_h__
#define __pattern_directional_antenna_h__

#include <list>
#include <map>
#include <string>

#include "directional_antenna.h"

class PatternDirectionalAntenna: public DirectionalAntenna {

public:
	PatternDirectionalAntenna();
	PatternDirectionalAntenna(const PatternDirectionalAntenna& antenna);
	virtual ~PatternDirectionalAntenna();

	virtual double getTxGain(double dX, double dY, double dZ, double lambda);
	virtual double getRxGain(double dX, double dY, double dZ, double lambda);

	virtual Antenna * copy();
	virtual void release();

	virtual int command(int, const char * const *);

private:
	static list<DirectionalAntenna*> copies_;

	int loadPatternFile();
	int getSmallerAngleIndex(double angle);
	double interpolate(double x, double x1, double x2, double y1, double y2);

	int gainsCount_;
	double* anglesIndex_;
	map<double, double> anglesToGains_;
	string patternFile_;
};

#endif /* __pattern_directional_antenna_h__ */
