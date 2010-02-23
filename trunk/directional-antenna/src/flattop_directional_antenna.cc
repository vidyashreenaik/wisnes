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
	mainGain_(0.0), sideGain_(0.0) {
	bind("Gm_", &mainGain_);
	bind("Gs_", &sideGain_);
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

	double azimuthAngle = getAzimuthAngleRelativeToBoresight(dX, dY, dZ);

	double halfBeamwidth = beamwidth_ / 2;

	double lowerBound = boresight_ - halfBeamwidth;
	if (lowerBound < 0)
		lowerBound = lowerBound + 360;

	double upperBound = boresight_ + halfBeamwidth;
	if (upperBound >= 360)
		upperBound = upperBound - 360;

	if (upperBound >= lowerBound) {
		if (azimuthAngle >= lowerBound && azimuthAngle <= upperBound) {
			// e.g. between 0 and 60 degrees and angle is 10
			gain = mainGain_;
		}
	} else {
		if ((azimuthAngle >= lowerBound && azimuthAngle < 360) || (azimuthAngle
				>= 0 && azimuthAngle <= upperBound)) {
			// e.g. between 330 and 30 degrees and angle is 340
			gain = mainGain_;
		}
	}
	// Convert back to linear units as expected by propagation.cc
	gain = pow(10, gain / 10.0);

	return gain;
}

double FlatTopDirectionalAntenna::getRxGain(double dX, double dY, double dZ,
		double lambda) {
	return getTxGain(dX, dY, dZ, lambda);
}

