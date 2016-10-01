/*
 * Shapes.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "Shapes.h"

namespace Visualization {

    void drawCylinder(Vec3F pt1, Vec3F pt2,
                                float radius, int slices, int stacks)
    {
        Vec3F qmp = pt1-pt2;
        float length = qmp.length();
        qmp.normalize();
        Vec3F z = Vec3F(0,0,1);
        Vec3F axis = z ^ qmp;
        float angle = acos(qmp * z)* 180.0 / PI;

        glPushMatrix();
        glTranslatef(pt2.X(), pt2.Y(), pt2.Z());
        glRotatef(angle, axis.X(), axis.Y(), axis.Z());

        GLUquadric * quadricCylinder = gluNewQuadric();
        gluCylinder(quadricCylinder, radius, radius, length, slices, stacks);
        gluDeleteQuadric(quadricCylinder);

        glPopMatrix();
    }

    void drawLine(Vec3F pt1, Vec3F pt2) {
        glBegin(GL_LINES);
        glVertex3f(pt1.X(), pt1.Y(), pt1.Z());
        glVertex3f(pt2.X(), pt2.Y(), pt2.Z());
        glEnd();
    }


}  // namespace Visualization
