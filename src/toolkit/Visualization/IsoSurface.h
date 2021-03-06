/*
 * IsoSurface.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_ISOSURFACE_H_
#define SRC_TOOLKIT_VISUALIZATION_ISOSURFACE_H_

#include "DisplayType.h"
#include "VolumeMesh.h"

namespace Visualization {

    class IsoSurface : public DisplayType {
        public:
            IsoSurface(const Volume & vol, VolumeMesh & mesh);
            virtual ~IsoSurface();

            bool calculateDisplay();

            void setDisplayRadius(const int radius);
            void setDisplayRadiusOrigin(float radiusOriginX,
                                        float radiusOriginY,
                                        float radiusOriginZ);
            void useDisplayRadius(bool useRadius);

        protected:
            int displayRadius;
            Vec3F radiusOrigin;
            bool _useDisplayRadius;

            VolumeMesh & surfaceMesh;
    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_VISUALIZATION_ISOSURFACE_H_ */
