#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "smart_directional_antenna.h"

int SmartDirectionalAntenna::indexes = 0;

map<int, SmartDirectionalAntenna* > SmartDirectionalAntenna::copies_;

static class SmartDirectionalAntennaClass: public TclClass {
public:
	SmartDirectionalAntennaClass() :
		TclClass("Antenna/DirectionalAntenna/Smart") {
	}

	TclObject* create(int, const char* const *) {
		return (new SmartDirectionalAntenna);
	}
} class_SmartDirectionalAntenna;

SmartDirectionalAntenna::SmartDirectionalAntenna() :
	FlatTopDirectionalAntenna(), nullGain_(0.0), nullWidth_(0.0),
			nullLocation_(0.0) {
	bind("NullWidth_", &nullWidth_);
	bind("NullLocation_", &nullLocation_);
	
	index_ = SmartDirectionalAntenna::indexes++;
}

SmartDirectionalAntenna::SmartDirectionalAntenna(
		const SmartDirectionalAntenna& antenna) :
	FlatTopDirectionalAntenna(antenna), nullGain_(antenna.nullGain_),
	nullLocation_(antenna.nullLocation_), nullWidth_(antenna.nullWidth_) {
	bind("NullWidth_", &nullWidth_);
	bind("NullLocation_", &nullLocation_);
	
	index_ = SmartDirectionalAntenna::indexes++;
}

SmartDirectionalAntenna::~SmartDirectionalAntenna() {
}

double SmartDirectionalAntenna::getBeamwidth() const {
	return beamwidth_;
}

Antenna * SmartDirectionalAntenna::copy() {
    return SmartDirectionalAntenna::copy(*this);
}

void SmartDirectionalAntenna::release() {
    SmartDirectionalAntenna::release(index_);
}

int SmartDirectionalAntenna::command(int argc, const char* const * argv) {
	return DirectionalAntenna::command(argc, argv);
}

double SmartDirectionalAntenna::getTxGain(double dX, double dY, double dZ,
		double lambda) {
    double gain = sideGain_;
	double azimuthAngle = getAzimuthAngle(dX, dY, dZ);
    double relativeAngle = getAngleRelativeToBoresight(azimuthAngle);
    
    if (debug_) {
        cout << id_ << " absolute azimuth angle " << azimuthAngle << endl;
        cout << id_ << " relative azimuth angle " << relativeAngle << endl;
    }
        
	if (isAngleWithin(azimuthAngle, boresight_, beamwidth_)) {
		gain = mainGain_;
	}

    if (debug_)
	        cout << id_ << " gain " << gain << " dB" << endl;
	// Convert back to linear units as expected by propagation.cc
	gain = pow(10, gain / 10.0);

	return gain;
}

double SmartDirectionalAntenna::getRxGain(double dX, double dY, double dZ,
		double lambda) {
	double azimuthAngle = getAzimuthAngle(dX, dY, dZ);
	if (isAngleWithin(azimuthAngle, nullLocation_, nullWidth_)) {
		return nullGain_;
	}

	// Return the same power as the one that would've been transmitted
	return getTxGain(dX, dY, dZ, lambda);
}

bool SmartDirectionalAntenna::isAngleWithin(double angle, double center,
		double width) {
	double halfWidth = beamwidth_ / 2;

	double lowerBound = center - halfWidth;
	if (lowerBound < 0)
		lowerBound = lowerBound + 360;

	double upperBound = center + halfWidth;
	if (upperBound >= 360)
		upperBound = upperBound - 360;

	if (upperBound >= lowerBound) {
		if (angle >= lowerBound && angle <= upperBound) {
			return true;
		}
	} else {
		if ((angle >= lowerBound && angle < 360) || (angle >= 0 && angle
				<= upperBound)) {
			return true;
		}
	}

	return false;
}
