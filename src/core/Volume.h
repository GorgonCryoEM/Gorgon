/*
 * Volume.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_CORE_VOLUME_H_
#define SRC_CORE_VOLUME_H_

#include "VolumeData.h"

namespace Core {

    /*
     *
     */
    class Volume : public VolumeData {
        public:
            Volume();
            Volume(int sizeX, int sizeY, int sizeZ, double val=0.0);
            virtual ~Volume();

            void fill(double val);
            void applyMask(double maskValue, bool keepMaskValue);
            double getMin() const;
            double getMax() const;

        private:
            VolumeData & volume;
    };

} /* namespace Core */

#endif /* SRC_CORE_VOLUME_H_ */
