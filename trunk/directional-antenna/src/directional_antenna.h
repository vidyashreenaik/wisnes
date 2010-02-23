#ifndef __directional_antenna_h__
#define __directional_antenna_h__

#include <antenna.h>

const double PI = 3.14159265358979323846;

class DirectionalAntenna: public Antenna {

public:
	DirectionalAntenna();
	DirectionalAntenna(const DirectionalAntenna &antenna);
	virtual ~DirectionalAntenna();

	virtual double getTxGain(double dX, double dY, double dZ, double lambda) = 0;
	virtual double getRxGain(double dX, double dY, double dZ, double lambda) = 0;

	virtual Antenna * copy() = 0;
	virtual void release() = 0;

    double getBeamwidth() const;
	double getBoresight() const;
	int getId() const;

protected:
    double getAzimuthAngle(double dX, double dY, double dZ);
    double getElevationAngle(double dX, double dY, double dZ);
    double getAzimuthAngleRelativeToBoresight(double dX, double dY, double dZ);
    double boresight_; /* pointing direction (deg) */
    double beamwidth_; /* width of the main beam (deg) */
    int id_; /* numeric ID to keep track of antennas for debug */
    int debug_;
};

#endif /* __directional_antenna_h__ */
