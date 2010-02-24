#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "pattern_directional_antenna.h"

list<DirectionalAntenna*> PatternDirectionalAntenna::copies_;

static class PatternDirectionalAntennaClass: public TclClass {
public:
	PatternDirectionalAntennaClass() :
		TclClass("Antenna/DirectionalAntenna/Pattern") {
	}

	TclObject* create(int, const char* const *) {
		return (new PatternDirectionalAntenna);
	}
} class_PatternDirectionalAntenna;

PatternDirectionalAntenna::PatternDirectionalAntenna() :
	gainsCount_(0), patternFile_("antenna.pattern") {

	loadPatternFile();
}

PatternDirectionalAntenna::PatternDirectionalAntenna(
		const PatternDirectionalAntenna& antenna) :
	DirectionalAntenna(antenna), gainsCount_(antenna.gainsCount_),
			patternFile_(antenna.patternFile_) {
	loadPatternFile();
}

PatternDirectionalAntenna::~PatternDirectionalAntenna() {
	list<DirectionalAntenna*>::iterator iter(copies_.begin());
	while (iter != copies_.end()) {
		delete *iter;
		++iter;
	}
	copies_.clear();
}

Antenna * PatternDirectionalAntenna::copy() {
	DirectionalAntenna* copy = new PatternDirectionalAntenna(*this);
	copies_.push_back(copy);

	return copy;
}

void PatternDirectionalAntenna::release() {
	if (!copies_.empty()) {
		DirectionalAntenna* antenna = copies_.front();
		copies_.pop_front();

		delete antenna;
	}
}

int PatternDirectionalAntenna::command(int argc, const char* const * argv) {
	Tcl& tcl = Tcl::instance();

	if (argc == 3) {
		if (strcmp(argv[1], "file") == 0) {
			patternFile_ = argv[2];
			if (patternFile_ == "")
				return TCL_ERROR;

			return loadPatternFile();
		}
	}
	return DirectionalAntenna::command(argc, argv);
}

double PatternDirectionalAntenna::getTxGain(double dX, double dY, double dZ,
		double lambda) {
	double gain = 0.0;

	double angle = getAngleRelativeToBoresight(getAzimuthAngle(dX, dY, dZ));

	map<double, double>::iterator iter = anglesToGains_.find(angle);
	if (iter == anglesToGains_.end()) {
		int i = getSmallerAngleIndex(angle);
		int j = i + 1;
		if (i == gainsCount_ - 1) {
			j = 0;
		}

		double x1 = *(anglesIndex_ + i);
		iter = anglesToGains_.find(x1);
		double y1 = (*iter).second;
		double x2 = *(anglesIndex_ + j);
		iter = anglesToGains_.find(x2);
		double y2 = (*iter).second;

		gain = interpolate(angle, x1, x2, y1, y2);
	} else {
		gain = (*iter).second;
	}
	// Convert back to linear units as expected by propagation.cc
	gain = pow(10, gain / 10.0);

	return gain;
}

double PatternDirectionalAntenna::getRxGain(double dX, double dY, double dZ,
		double lambda) {
	return getTxGain(dX, dY, dZ, lambda);
}

int PatternDirectionalAntenna::loadPatternFile() {
	anglesToGains_.clear();

	ifstream patternFile(patternFile_.c_str(), ios::in);
	if (patternFile) {
		double angle;
		double gain;
		patternFile >> gainsCount_;

		if (gainsCount_ < 0 || gainsCount_ > 360) {
			if (debug_) {
				cerr
						<< "Error loading pattern: invalid number of gain values ("
						<< gainsCount_ << ")" << endl;
			}
			patternFile.close();
			return TCL_ERROR;
		}

		if (anglesIndex_) {
			delete[] anglesIndex_;
		}
		anglesIndex_ = new double[gainsCount_];
		for (int i = 0; i < gainsCount_; i++) {
			patternFile >> angle >> gain;
			if (angle >= 0 && angle < 360) {
				*(anglesIndex_ + i) = angle;
				anglesToGains_[angle] = gain;
			} else {
				if (debug_) {
					cerr << "Error loading pattern: invalid angle (" << angle
							<< ")" << endl;
				}
				patternFile.close();
				return TCL_ERROR;
			}
		}
		patternFile.close();
		return TCL_OK;
	} else {
		if (debug_) {
			cerr << "Error loading pattern: could not open antenna.pattern"
					<< endl;
		}
		return TCL_ERROR;
	}
}

int PatternDirectionalAntenna::getSmallerAngleIndex(double angle) {
	for (int i = 0; i < gainsCount_; i++) {
		if (*(anglesIndex_ + i) > angle) {
			if (i == 0) {
				return gainsCount_ - 1;
			} else {
				return i - 1;
			}
		}
	}

	return -1;
}

double PatternDirectionalAntenna::interpolate(double x, double x1, double x2,
		double y1, double y2) {
	double y = y1 + (y2 - y1) / (x2 - x1) * (x - x1);

	return y;
}
