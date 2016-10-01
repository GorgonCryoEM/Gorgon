/*
 * Shapes.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */


#ifndef SRC_TOOLKIT_VISUALIZATION_SHAPES_H_
#define SRC_TOOLKIT_VISUALIZATION_SHAPES_H_

#include "MathTools/Dim3D.h"
#include "GorgonGL.h"

namespace Visualization {

    template <class T>
    void drawSphere(T center, float radius) {
        glPushMatrix();
        glTranslatef(center.X(), center.Y(), center.Z());
        GLUquadric * quadricSphere = gluNewQuadric();
        gluSphere(quadricSphere, radius, 10, 10);
        gluDeleteQuadric(quadricSphere);
        glPopMatrix();
    }

    void drawCylinder(Vec3F pt1, Vec3F pt2,
                             float radius, int slices = 10, int stacks = 10);
    void drawLine(Vec3F pt1, Vec3F pt2);


}  // namespace Visualization

#endif /* SRC_TOOLKIT_VISUALIZATION_SHAPES_H_ */
