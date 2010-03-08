#include <cmath>
#include <cstdio>

#include "directional_antenna.h"

static int antennaId = 0;

DirectionalAntenna::DirectionalAntenna() :
	id_(antennaId++), debug_(0), boresight_(0.0), beamwidth_(0.0) {
	bind("Boresight_", &boresight_);
	bind("Beamwidth_", &beamwidth_);

	bind("Id_", &id_);
	bind("Debug_", &debug_);
}

DirectionalAntenna::DirectionalAntenna(const DirectionalAntenna &antenna) :
	id_(antenna.id_), debug_(antenna.debug_), boresight_(antenna.boresight_),
			beamwidth_(antenna.beamwidth_) {
	bind("Boresight_", &boresight_);
	bind("Beamwidth_", &beamwidth_);

	bind("Id_", &id_);
	bind("Debug_", &debug_);
}

DirectionalAntenna::~DirectionalAntenna() {
}

double DirectionalAntenna::getBeamwidth() const {
	return beamwidth_;
}

double DirectionalAntenna::getBoresight() const {
	return boresight_;
}

int DirectionalAntenna::getId() const {
	return id_;
}

double DirectionalAntenna::getAzimuthAngle(double dX, double dY, double dZ) {
	double azimuthAngle = atan2(dY, dX);
	// Convert to degrees
	azimuthAngle = azimuthAngle * 180.0 / PI;

	return azimuthAngle;
}

double DirectionalAntenna::getElevationAngle(double dX, double dY, double dZ) {
	// Currently propagation models in NS-2 do not handle elevation;
	return 0.0;
}

double DirectionalAntenna::getAngleRelativeToBoresight(double angle) {
	double relativeAngle = angle - boresight_;
	// make sure the angle is between -180 and 180
	for (/*do nothing*/; relativeAngle < -180.0; relativeAngle += 360.0)
		;

	return relativeAngle;
}
