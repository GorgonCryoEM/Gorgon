/*
 * Volume.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_CORE_VOLUME_H_
#define SRC_CORE_VOLUME_H_

#include "Operations.h"
#include "VolumeData.h"

namespace Core {

    /*
     *
     */
    class Volume : public VolumeData, public Operation {
        public:
            Volume();
            Volume(int sizeX, int sizeY, int sizeZ, double val=0.0);
            virtual ~Volume();
    };

} /* namespace Core */

#endif /* SRC_CORE_VOLUME_H_ */
