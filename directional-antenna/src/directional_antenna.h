#ifndef __directional_antenna_h__
#define __directional_antenna_h__

#include <cstddef>
#include <cmath>
#include <cstdio>

#include <antenna.h>

class DirectionalAntenna: public Antenna {

public:
    DirectionalAntenna();
    DirectionalAntenna(int id, int debug = 0, int boresight = 0.0, int mode = OMNI_DIRECTIONAL);
    DirectionalAntenna(const DirectionalAntenna &antenna);
    virtual ~DirectionalAntenna();

    virtual double getTxGain(double dX, double dY, double dZ, double lambda);
    virtual double getRxGain(double dX, double dY, double dZ, double lambda);

    virtual Antenna * copy();
    virtual void release();

    virtual double getX() const { return X_; }
    virtual double getY() const { return Y_; }
    virtual double getZ() const { return Y_; }

    virtual void setBoresight(int boresight);
    virtual double getBoresight() const { return boresight_; }

    virtual double getBeamwidth() const { return beamwidth_; }

    virtual void setMode(int mode);
    virtual int getMode() const { return mode_; }

    int getId() const { return id_; }

    static const int OMNI_DIRECTIONAL;
    static const int UNI_DIRECTIONAL;

protected:
    static const double PI_;

    double getAzimuthAngle(double dX, double dY, double dZ);
    double getElevationAngle(double dX, double dY, double dZ);
    double getAngleRelativeToBoresight(double angle);

    double boresight_; /* pointing direction (deg) */
    double beamwidth_; /* width of the main beam (deg) */
    int mode_; /* mode the antenna is currently running */
    int id_; /* numeric ID to keep track of antennas for debug */
    int debug_;
};

#endif /* __directional_antenna_h__ */
