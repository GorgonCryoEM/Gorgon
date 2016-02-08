/*
 * Volume.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "Volume.h"

#include <algorithm>
#include <numeric>

namespace Core {

    Volume::Volume()
            : VolumeData(),
              volume(dynamic_cast<VolumeData &>(*this))
    {
        // TODO Auto-generated constructor stub


    }

    Volume::Volume(int sizeX, int sizeY, int sizeZ, double val)
            : VolumeData(sizeX, sizeY, sizeZ, val),
              volume(dynamic_cast<VolumeData &>(*this))
    {
    }

    Volume::~Volume() {
        // TODO Auto-generated destructor stub
    }

    void Volume::fill(double val)
    {
        for(VolumeData::iterator it=volume.begin(); it!=volume.end(); ++it)
                    *it = val;
    }

    void Volume::applyMask(double maskValue, bool keepMaskValue) {
        for(VolumeData::iterator it=volume.begin(); it!=volume.end(); ++it)
            if(((*it== maskValue) && !keepMaskValue) ||
                        ((*it != maskValue) && keepMaskValue))
                        *it = 0;
    }

    double Volume::getMin() const {
        return *min_element(volume.begin(), volume.end());
    }

    double Volume::getMax() const {
        return *max_element(volume.begin(), volume.end());
    }

    double Volume::getMean() const {
        return accumulate(volume.begin(), volume.end(), 0.0) / data.size();
    }

    double Volume::getEdgeMean() const {
    }

    double Volume::getStdDev() const {
    }

//    Vector3DFloat Volume::getCenterOfMass() const {
//    }

} /* namespace Core */
