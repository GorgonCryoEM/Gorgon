/*
 * Volume.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "Volume.h"

namespace Core {

    Volume::Volume()
            : VolumeData(), Operation(volume)
    {
        // TODO Auto-generated constructor stub


    }

    Volume::Volume(int sizeX, int sizeY, int sizeZ, double val)
            : VolumeData(sizeX, sizeY, sizeZ, val), Operation(volume)
    {
    }

    Volume::~Volume() {
        // TODO Auto-generated destructor stub
    }

} /* namespace Core */
