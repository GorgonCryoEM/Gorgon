/*
 * RendererBase.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_RENDERERBASE_H_
#define SRC_TOOLKIT_VISUALIZATION_RENDERERBASE_H_

#include "Core/volume.h"
#include "Visualization/MeshBase.h"
#include "Core/MinMax.h"
#include "MathTools/Dim3D.h"

using namespace Core;

namespace Visualization {

    class RendererBase {
        public:
            virtual void setDisplayStyle(int style);

            virtual float getMinPos(int i) const;
            virtual float getMaxPos(int i) const;
            virtual void setMinMax(Vec3F p);

            virtual float getSpacingX() const;
            virtual float getSpacingY() const;
            virtual float getSpacingZ() const;
            virtual float getOriginX() const;
            virtual float getOriginY() const;
            virtual float getOriginZ() const;
        protected:
            int displayStyle;

            Dim3D<MinMax> minmaxPts;
    };
}




#endif /* SRC_TOOLKIT_VISUALIZATION_RENDERERBASE_H_ */
