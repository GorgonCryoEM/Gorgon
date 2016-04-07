/*
 * Renderer.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_RENDERERBASE_H_
#define SRC_TOOLKIT_VISUALIZATION_RENDERERBASE_H_

//#include <string>
//#include <GorgonGL.h>
#include "OpenGLUtils.h"
//#include <MathTools/Vector3D.h>
#include "MathTools/Dim3D.h"

//using namespace std;
//using namespace wustl_mm::MathTools;
using namespace Foundation;

namespace Visualization {
    class RendererBase {
        public:
            RendererBase();
            virtual ~RendererBase();
            virtual bool selectionRotate(Vec3F centerOfMass,
                                         Vec3F rotationAxis,
                                         float angle);
            virtual Vec3F selectionCenterOfMass();
            virtual bool selectionMove(Vec3F moveDirection);
            void drawSphere(Vec3F center, float radius);
            void static drawCylinder(Vec3F pt1, Vec3F pt2,
                                     float radius, int slices = 10, int stacks = 10);
            void static drawLine(Vec3F pt1, Vec3F pt2);

            float getMin(int dimension);
            float getMax(int dimension);

        protected:
            Vec3F minPts;
            Vec3F maxPts;
    };
}

#endif /* SRC_TOOLKIT_VISUALIZATION_RENDERERBASE_H_ */
