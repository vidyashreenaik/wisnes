#ifndef __smart_directional_antenna_h__
#define __smart_directional_antenna_h__

#include <map>
#include "flattop_directional_antenna.h"

class SmartDirectionalAntenna: public FlatTopDirectionalAntenna {

public:

	SmartDirectionalAntenna();
	SmartDirectionalAntenna(const SmartDirectionalAntenna& antenna);
	virtual ~SmartDirectionalAntenna();

	virtual double getTxGain(double dX, double dY, double dZ, double lambda);
	virtual double getRxGain(double dX, double dY, double dZ, double lambda);

	virtual Antenna* copy();
	virtual void release();

	virtual int command(int, const char * const *);

	double getBeamwidth() const;

protected:
    static Antenna* copy(const SmartDirectionalAntenna& original) {
        SmartDirectionalAntenna* copy = new SmartDirectionalAntenna(original);
        
        if (SmartDirectionalAntenna::copies_.count(copy->index_) > 0) {
            SmartDirectionalAntenna::copies_.erase(copy->index_);
        }
        SmartDirectionalAntenna::copies_.insert(make_pair(copy->index_, copy));
        
        return copy;
    }

    static Antenna* release(int index) {
        SmartDirectionalAntenna::copies_.erase(index);
    }

	bool isAngleWithin(double angle, double center, double width);

	static map<int, SmartDirectionalAntenna*> copies_;
    static int indexes;

	double nullLocation_;  /* position of the null (deg) */
	double nullWidth_;     /* width of the null (deg) */
	double nullGain_;      /* gain of the null */
	
	int index_;
};

#endif /* __smart_directional_antenna_h__ */
