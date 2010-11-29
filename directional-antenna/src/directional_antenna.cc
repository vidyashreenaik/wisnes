#include "directional_antenna.h"

static int antennaId = 0;

const int DirectionalAntenna::UNI_DIRECTIONAL = 0;
const int DirectionalAntenna::OMNI_DIRECTIONAL = 0;

const double DirectionalAntenna::PI_ = 3.14159265358979323846;

static class DirectionalAntennaClass: public TclClass {
public:
    DirectionalAntennaClass() :
        TclClass("Antenna/DirectionalAntenna") {
    }

    TclObject* create(int, const char* const *) {
        return (new DirectionalAntenna);
    }
} class_DirectionalAntenna;

DirectionalAntenna::DirectionalAntenna() :
    id_(antennaId++), debug_(0), boresight_(0.0), mode_(UNI_DIRECTIONAL) {
    bind("Boresight_", &boresight_);

    bind("Id_", &id_);
    bind("Debug_", &debug_);
}

DirectionalAntenna::DirectionalAntenna(int id, int debug, int boresight, int mode) :
    id_(id), debug_(debug), boresight_(boresight), mode_(mode) {
    bind("Boresight_", &boresight_);

    bind("Id_", &id_);
    bind("Debug_", &debug_);
}

DirectionalAntenna::DirectionalAntenna(const DirectionalAntenna &antenna) :
    id_(antenna.id_), debug_(antenna.debug_), boresight_(antenna.boresight_), mode_(antenna.mode_) {
    bind("Boresight_", &boresight_);

    bind("Id_", &id_);
    bind("Debug_", &debug_);
}

DirectionalAntenna::~DirectionalAntenna() {
}

double DirectionalAntenna::getTxGain(double dX, double dY, double dZ,
        double lambda) {
    return 0.0;
}

double DirectionalAntenna::getRxGain(double dX, double dY, double dZ,
        double lambda) {
    return 0.0;
}
Antenna * DirectionalAntenna::copy() {
    return this;
}

void DirectionalAntenna::release() {
    // Do nothing
}

void DirectionalAntenna::setBoresight(int boresight) {
    boresight_ = boresight;
}

void DirectionalAntenna::setMode(int mode) {
   mode_ = mode;
}

double DirectionalAntenna::getAzimuthAngle(double dX, double dY, double dZ) {
    double azimuthAngle = atan2(dY, dX);
    // Convert to degrees
    azimuthAngle = azimuthAngle * 180.0 / DirectionalAntenna::PI_;

    for (/*do nothing*/; azimuthAngle < 0; azimuthAngle += 360)
        ;
    for (/*do nothing*/; azimuthAngle >= 360; azimuthAngle -= 360)
        ;

    return azimuthAngle;
}

double DirectionalAntenna::getElevationAngle(double dX, double dY, double dZ) {
    // Currently propagation models in NS-2 do not handle elevation;
    return 0.0;
}

double DirectionalAntenna::getAngleRelativeToBoresight(double absoluteAnge) {
    double relativeAngle = absoluteAnge - boresight_;

    return relativeAngle;
}
