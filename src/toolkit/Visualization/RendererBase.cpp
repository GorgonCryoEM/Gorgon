/*
 * RendererBase.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */


#include "RendererBase.h"

using namespace Visualization;

RendererBase::RendererBase() {
}

RendererBase::~RendererBase() {
}

float RendererBase::getMin(int dimension) {
    return minPts[dimension];
}

float RendererBase::getMax(int dimension) {
    return maxPts[dimension];
}

bool RendererBase::selectionRotate(Vec3F centerOfMass, Vec3F rotationAxis, float angle) {
    return false;
}

int RendererBase::selectionObjectCount() {
    return 0;
}

Vec3F RendererBase::selectionCenterOfMass() {
    return (maxPts - minPts) / 2.0;
}

bool RendererBase::selectionMove(Vec3F moveDirection) {
    //printf("Moving by %f %f %f\n", moveDirection.X(), moveDirection.Y(), moveDirection.Z());
    return false;
}

Vec3F RendererBase::set3DCoordinates(int subsceneIndex,
                                 int ix0, int ix1, int ix2, int ix3, int ix4)
{
    return Vec3F(0, 0, 0);
}

void RendererBase::unload() {
}

void RendererBase::drawSphere(Vec3F center, float radius) {
    glPushMatrix();
    glTranslatef(center.X(), center.Y(), center.Z());
    GLUquadric * quadricSphere = gluNewQuadric();
    gluSphere(quadricSphere, radius, 10, 10);
    gluDeleteQuadric(quadricSphere);
    glPopMatrix();
}

void RendererBase::drawCylinder(Vec3F pt1, Vec3F pt2,
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

void RendererBase::drawLine(Vec3F pt1, Vec3F pt2) {
    glBegin(GL_LINES);
    glVertex3f(pt1.X(), pt1.Y(), pt1.Z());
    glVertex3f(pt2.X(), pt2.Y(), pt2.Z());
    glEnd();
}
