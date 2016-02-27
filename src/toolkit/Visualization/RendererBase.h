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
#include <MathTools/MathLib.h>
#include "MathTools/Dim3D.h"

//using namespace std;
//using namespace wustl_mm::MathTools;
using namespace Foundation;

namespace Visualization {
    class RendererBase {
        public:
            RendererBase();
            virtual ~RendererBase();
            virtual void drawBoundingBox();
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
            virtual string getSupportedLoadFileFormats();
            virtual string getSupportedSaveFileFormats();
            virtual Vec3F set3DCoordinates(int subsceneIndex, int ix0,
                                           int ix1 = -1, int ix2 = -1,
                                           int ix3 = -1, int ix4 = -1);
            virtual bool setCuttingPlane(float position, float vecX, float vecY, float vecZ);
            void static drawSphere(Vec3F center, float radius);
            void static drawCylinder(Vec3F pt1, Vec3F pt2,
                                     float radius, int slices = 10, int stacks = 10);
            void static drawLine(Vec3F pt1, Vec3F pt2);
            virtual void setDisplayStyle(int style);
            virtual void setObjectSpecificColoring(bool objectSpecific);
            virtual void updateBoundingBox();

            float getMin(int dimension);
            float getMax(int dimension);

        protected:
            Vec3F minPts;
            Vec3F maxPts;
            bool selected;
            Vec3F cuttingPlaneCenter;
            Vec3F cuttingPlaneDirection;
            int displayStyle;
            bool isObjectSpecificColoring;
    };

    RendererBase::RendererBase() {
        selected = false;
        isObjectSpecificColoring = false;
    }

    RendererBase::~RendererBase() {
    }

    float RendererBase::getMin(int dimension) {
        return minPts[dimension];
    }

    float RendererBase::getMax(int dimension) {
        return maxPts[dimension];
    }

    void RendererBase::setObjectSpecificColoring(bool objectSpecific) {
        isObjectSpecificColoring = objectSpecific;
    }

    void RendererBase::drawBoundingBox() {
        glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);
        glDisable (GL_LIGHTING);
        glPushMatrix();
        glTranslatef(minPts[0] + (maxPts[0] - minPts[0]) / 2.0,
                     minPts[1] + (maxPts[1] - minPts[1]) / 2.0,
                     minPts[2] + (maxPts[2] - minPts[2]) / 2.0);
        glScalef(maxPts[0] - minPts[0], maxPts[1] - minPts[1],
                 maxPts[2] - minPts[2]);
        glutWireCube(1.0);
        glPopMatrix();
        glPopAttrib();
    }

    void RendererBase::updateBoundingBox() {
        minPts = -0.5;
        maxPts = 0.5;
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

    bool RendererBase::selectionClear() {
        if(selected) {
            selected = false;
            return true;
        } else {
            return false;
        }

    }

    void RendererBase::selectionToggle(int subsceneIndex, bool forceTrue,
                                   int ix0, int ix1, int ix2, int ix3, int ix4)
    {
        selected = true;
    }

    Vec3F RendererBase::set3DCoordinates(int subsceneIndex,
                                     int ix0, int ix1, int ix2, int ix3, int ix4)
    {
        return Vec3F(0, 0, 0);
    }

    void RendererBase::unload() {
        selected = false;
    }

    string RendererBase::getSupportedLoadFileFormats() {
        return "All Files (*.*)";
    }

    string RendererBase::getSupportedSaveFileFormats() {
        return "All Files (*.*)";
    }

    bool RendererBase::setCuttingPlane(float position,
                                   float vecX, float vecY, float vecZ)
    {
        Vec3F center = (minPts + maxPts) / 2.0;
        float distance = (minPts - center).length();
        cuttingPlaneDirection = Vec3F(vecX, vecY, vecZ);
        cuttingPlaneDirection.normalize();
        cuttingPlaneCenter = center + cuttingPlaneDirection * position * distance;
        //printf("%lf %lf %lf - %lf %lf\n", cuttingPlaneCenter.values[0], cuttingPlaneCenter.values[1], cuttingPlaneCenter.values[2], position, distance); flushall();
        return false;
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

    void RendererBase::setDisplayStyle(int style) {
        this->displayStyle = style;
    }
}

#endif /* SRC_TOOLKIT_VISUALIZATION_RENDERERBASE_H_ */
