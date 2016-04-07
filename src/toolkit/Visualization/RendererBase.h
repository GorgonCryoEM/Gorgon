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
            virtual void unload();
            virtual bool selectionRotate(Vec3F centerOfMass,
                                         Vec3F rotationAxis,
                                         float angle);
            virtual int selectionObjectCount();
            virtual Vec3F selectionCenterOfMass();
            virtual bool selectionMove(Vec3F moveDirection);
            virtual bool selectionClear();
            virtual void selectionToggle(int subsceneIndex, bool forceTrue,
                                         int ix0, int ix1 = -1, int ix2 = -1,
                                         int ix3 = -1, int ix4 = -1);
            virtual Vec3F set3DCoordinates(int subsceneIndex, int ix0,
                                           int ix1 = -1, int ix2 = -1,
                                           int ix3 = -1, int ix4 = -1);
            void drawSphere(Vec3F center, float radius);
            void static drawCylinder(Vec3F pt1, Vec3F pt2,
                                     float radius, int slices = 10, int stacks = 10);
            void static drawLine(Vec3F pt1, Vec3F pt2);

            float getMin(int dimension);
            float getMax(int dimension);

        protected:
            Vec3F minPts;
            Vec3F maxPts;
            bool selected;
    };
}

#endif /* SRC_TOOLKIT_VISUALIZATION_RENDERERBASE_H_ */
