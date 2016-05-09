/*
 * IsoSurface.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_ISOSURFACE_H_
#define SRC_TOOLKIT_VISUALIZATION_ISOSURFACE_H_

#include "DisplayType.h"

namespace Visualization {

    /*
     *
     */
    class IsoSurface : public DisplayType {
        public:
            IsoSurface(const Volume & vol);
            virtual ~IsoSurface();

            bool calculateDisplay();

            void draw(int subSceneIndex, bool selectEnabled);

            void setDisplayRadius(const int radius);
            void setDisplayRadiusOrigin(float radiusOriginX,
                                        float radiusOriginY,
                                        float radiusOriginZ);
            void useDisplayRadius(bool useRadius);

        private:
            int displayRadius;
            Vec3F radiusOrigin;
            bool _useDisplayRadius;

            Mesh surfaceMesh;
    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_VISUALIZATION_ISOSURFACE_H_ */
