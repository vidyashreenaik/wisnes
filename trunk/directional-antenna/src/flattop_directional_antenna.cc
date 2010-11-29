#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "flattop_directional_antenna.h"

static int antennaId = 0;

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
    DirectionalAntenna(++antennaId), beamwidth_(0.0), mainGain_(0.0), sideGain_(0.0),
            omniGain_(0.0) {
    bind("Beamwidth_", &beamwidth_);
    bind("Gm_", &mainGain_);
    bind("Gs_", &sideGain_);
    bind("Go_", &omniGain_);
}

FlatTopDirectionalAntenna::FlatTopDirectionalAntenna(const FlatTopDirectionalAntenna& antenna) :
    DirectionalAntenna(antenna), beamwidth_(antenna.beamwidth_), mainGain_(antenna.mainGain_),
            sideGain_(antenna.sideGain_), omniGain_(antenna.omniGain_) {
    bind("Beamwidth_", &beamwidth_);
    bind("Gm_", &mainGain_);
    bind("Gs_", &sideGain_);
    bind("Go_", &omniGain_);
}

FlatTopDirectionalAntenna::~FlatTopDirectionalAntenna() {
}

double FlatTopDirectionalAntenna::getBeamwidth() const {
    return beamwidth_;
}

Antenna * FlatTopDirectionalAntenna::copy() {
    return this;
}

void FlatTopDirectionalAntenna::release() {
    /*do nothing*/
}

int FlatTopDirectionalAntenna::command(int argc, const char* const * argv) {
    return DirectionalAntenna::command(argc, argv);
}

double FlatTopDirectionalAntenna::getTxGain(double dX, double dY, double dZ, double lambda) {
    if (debug_) {
        cout << "[" << id_ << "]: Tx ";
    }
    // Return the same power as the one that would've been transmitted
    return getGain(dX, dY, dZ, lambda);
}

double FlatTopDirectionalAntenna::getRxGain(double dX, double dY, double dZ, double lambda) {
    if (debug_) {
        cout << "[" << id_ << "]: Rx ";
    }
    // Return the same power as the one that would've been transmitted
    return getGain(dX, dY, dZ, lambda);
}

double FlatTopDirectionalAntenna::getGain(double dX, double dY, double dZ, double lambda) {
    double gain = sideGain_;
    double direction = getAzimuthAngle(dX, dY, dZ);

    if (mode_ == UNI_DIRECTIONAL) {
        if (isAngleWithin(direction, boresight_, beamwidth_)) {
            gain = mainGain_;
        }
    } else {
        gain = omniGain_;
    }

    if (debug_) {
        cout << "direction: " << direction << " gain: " << gain << " dB" << endl;
    }

    // Convert back to linear units as expected by propagation.cc
    gain = pow(10, gain / 10.0);

    return gain;
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
        if ((angle >= lowerBound && angle < 360) || (angle >= 0 && angle <= upperBound)) {
            return true;
        }
    }

    return false;
}
