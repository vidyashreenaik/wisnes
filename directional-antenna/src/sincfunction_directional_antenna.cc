#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "sincfunction_directional_antenna.h"

list<DirectionalAntenna*> SincFunctionDirectionalAntenna::copies_;

//static class FlatTopDirectionalAntennaClass: public TclClass {
//public:
//	FlatTopDirectionalAntennaClass() :
//		TclClass("Antenna/DirectionalAntenna/SincFunction") {
//	}
//
//	TclObject* create(int, const char* const *) {
//		return (new SincFunctionDirectionalAntenna);
//	}
//} class_FlatTopDirectionalAntenna;

SincFunctionDirectionalAntenna::SincFunctionDirectionalAntenna() :
	mainGain_(0.0), nullWidth_(0.0), nullLocation_(0.0) {
	bind("Gm_", &mainGain_);
	bind("NullWidth_", &nullWidth_);
	bind("NullLocation_", &nullLocation_);
}

SincFunctionDirectionalAntenna::SincFunctionDirectionalAntenna(
		const SincFunctionDirectionalAntenna& antenna) :
	DirectionalAntenna(antenna), mainGain_(antenna.mainGain_) {
	bind("Gm_", &mainGain_);
	bind("NullWidth_", &nullWidth_);
	bind("NullLocation_", &nullLocation_);
}

SincFunctionDirectionalAntenna::~SincFunctionDirectionalAntenna() {
}

Antenna * SincFunctionDirectionalAntenna::copy() {
	DirectionalAntenna* copy = new SincFunctionDirectionalAntenna(*this);
	copies_.push_back(copy);

	return copy;
}

void SincFunctionDirectionalAntenna::release() {
	if (!copies_.empty()) {
		DirectionalAntenna* antenna = copies_.front();
		copies_.pop_front();

		delete antenna;
	}
}

int SincFunctionDirectionalAntenna::command(int argc, const char* const * argv) {
	return DirectionalAntenna::command(argc, argv);
}

double SincFunctionDirectionalAntenna::getTxGain(double dX, double dY, double dZ,
		double lambda) {
	double azimuthAngle = getAngleRelativeToBoresight(getAzimuthAngle(dX, dY, dZ));
	double gain = sin(mainGain_ * PI * (cos(azimuthAngle - 1) / 4)) /
			sin(PI * (cos(azimuthAngle - 1) / 4));

	// Convert back to linear units as expected by propagation.cc
	gain = pow(10, gain / 10.0);

	return gain;
}

double SincFunctionDirectionalAntenna::getRxGain(double dX, double dY, double dZ,
		double lambda) {
	double nullAngle = getAngleRelativeToBoresight(nullLocation_);
	// If there is a null, return 0
	if (isAngleWithin(nullAngle, nullLocation_, nullWidth_)) {
		return 0.0;
	}

	// Return the same power as the one that would've been transmitted
	return getTxGain(dX, dY, dZ, lambda);
}

bool SincFunctionDirectionalAntenna::isAngleWithin(double angle, double center, double width) {
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
		if ((angle >= lowerBound && angle < 360) ||
				(angle >= 0 && angle <= upperBound)) {
			return true;
		}
	}

	return false;
}
