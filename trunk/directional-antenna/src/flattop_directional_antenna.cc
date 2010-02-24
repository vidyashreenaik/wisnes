#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "flattop_directional_antenna.h"

list<DirectionalAntenna*> FlatTopDirectionalAntenna::copies_;

static class FlatTopDirectionalAntennaClass: public TclClass {
public:
	FlatTopDirectionalAntennaClass() :
		TclClass("Antenna/DirectionalAntenna/FlatTop") {
	}

	TclObject* create(int, const char* const *) {
		return (new FlatTopDirectionalAntenna);
	}
} class_FlatTopDirectionalAntenna;

FlatTopDirectionalAntenna::FlatTopDirectionalAntenna() :
	mainGain_(0.0), sideGain_(0.0), nullWidth_(0.0), nullAngle_(0.0) {
	bind("Gm_", &mainGain_);
	bind("Gs_", &sideGain_);
	bind("NullWidth_", &nullWidth_);
	bind("NullAngle_", &nullAngle_);
}

FlatTopDirectionalAntenna::FlatTopDirectionalAntenna(
		const FlatTopDirectionalAntenna& antenna) :
	DirectionalAntenna(antenna), mainGain_(antenna.mainGain_), sideGain_(
			antenna.sideGain_) {
	bind("Gm_", &mainGain_);
	bind("Gs_", &sideGain_);
}

FlatTopDirectionalAntenna::~FlatTopDirectionalAntenna() {
}

Antenna * FlatTopDirectionalAntenna::copy() {
	DirectionalAntenna* copy = new FlatTopDirectionalAntenna(*this);
	copies_.push_back(copy);

	return copy;
}

void FlatTopDirectionalAntenna::release() {
	if (!copies_.empty()) {
		DirectionalAntenna* antenna = copies_.front();
		copies_.pop_front();

		delete antenna;
	}
}

int FlatTopDirectionalAntenna::command(int argc, const char* const * argv) {
	return DirectionalAntenna::command(argc, argv);
}

double FlatTopDirectionalAntenna::getTxGain(double dX, double dY, double dZ,
		double lambda) {
	double gain = sideGain_;
	double azimuthAngle = getAngleRelativeToBoresight(getAzimuthAngle(dX, dY, dZ));

	if (isAngleWithin(azimuthAngle, boresight_, beamwidth_)) {
		gain = mainGain_;
	}

	// Convert back to linear units as expected by propagation.cc
	gain = pow(10, gain / 10.0);

	return gain;
}

double FlatTopDirectionalAntenna::getRxGain(double dX, double dY, double dZ,
		double lambda) {
	double nullAngle = getAngleRelativeToBoresight(nullAngle_);
	// If there is a null, return 0
	if (isAngleWithin(nullAngle, nullAngle_, nullWidth_)) {
		return 0.0;
	}

	// Return the same power as the one that would've been transmitted
	return getTxGain(dX, dY, dZ, lambda);
}

bool FlatTopDirectionalAntenna::isAngleWithin(double angle, double center, double width) {
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
