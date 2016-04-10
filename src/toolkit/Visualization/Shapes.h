/*
 * Shapes.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */


#ifndef SRC_TOOLKIT_VISUALIZATION_SHAPES_H_
#define SRC_TOOLKIT_VISUALIZATION_SHAPES_H_

#include "MathTools/Dim3D.h"

namespace Visualization {

    void drawSphere(Vec3F center, float radius);
    void drawCylinder(Vec3F pt1, Vec3F pt2,
                             float radius, int slices = 10, int stacks = 10);
    void drawLine(Vec3F pt1, Vec3F pt2);


}  // namespace Visualization

#endif /* SRC_TOOLKIT_VISUALIZATION_SHAPES_H_ */
