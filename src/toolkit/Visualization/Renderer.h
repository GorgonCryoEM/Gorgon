/*
 * Renderer.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */


#ifndef SRC_TOOLKIT_VISUALIZATION_RENDERER_H_
#define SRC_TOOLKIT_VISUALIZATION_RENDERER_H_

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
    class Renderer {
        public:
            Renderer();
            virtual ~Renderer();
            virtual void DrawBoundingBox();
            virtual void Unload();
            virtual bool SelectionRotate(Vec3F centerOfMass,
                                         Vec3F rotationAxis,
                                         float angle);
            virtual int SelectionObjectCount();
            virtual Vec3F SelectionCenterOfMass();
            virtual bool SelectionMove(Vec3F moveDirection);
            virtual bool SelectionClear();
            virtual void SelectionToggle(int subsceneIndex, bool forceTrue,
                                         int ix0, int ix1 = -1, int ix2 = -1,
                                         int ix3 = -1, int ix4 = -1);
            virtual string GetSupportedLoadFileFormats();
            virtual string GetSupportedSaveFileFormats();
            virtual Vec3F Get3DCoordinates(int subsceneIndex, int ix0,
                                                  int ix1 = -1, int ix2 = -1,
                                                  int ix3 = -1, int ix4 = -1);
            bool SetCuttingPlane(float position, float vecX, float vecY, float vecZ);
            void static DrawSphere(Vec3F center, float radius);
            void static DrawCylinder(Vec3F pt1, Vec3F pt2,
                                     float radius, int slices = 10, int stacks = 10);
            void static DrawLine(Vec3F pt1, Vec3F pt2);
            virtual void SetDisplayStyle(int style);
            virtual void SetObjectSpecificColoring(bool objectSpecific);
            virtual void UpdateBoundingBox();

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

    Renderer::Renderer() {
        selected = false;
        isObjectSpecificColoring = false;
    }

    Renderer::~Renderer() {
    }

    float Renderer::getMin(int dimension) {
        return minPts[dimension];
    }

    float Renderer::getMax(int dimension) {
        return maxPts[dimension];
    }

    void Renderer::SetObjectSpecificColoring(bool objectSpecific) {
        isObjectSpecificColoring = objectSpecific;
    }

    void Renderer::DrawBoundingBox() {
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

    void Renderer::UpdateBoundingBox() {
        minPts = -0.5;
        maxPts = 0.5;
    }

    bool Renderer::SelectionRotate(Vec3F centerOfMass, Vec3F rotationAxis, float angle) {
        return false;
    }

    int Renderer::SelectionObjectCount() {
        return 0;
    }

    Vec3F Renderer::SelectionCenterOfMass() {
        return (maxPts - minPts) / 2.0;
    }

    bool Renderer::SelectionMove(Vec3F moveDirection) {
        //printf("Moving by %f %f %f\n", moveDirection.X(), moveDirection.Y(), moveDirection.Z());
        return false;
    }

    bool Renderer::SelectionClear() {
        if(selected) {
            selected = false;
            return true;
        } else {
            return false;
        }

    }

    void Renderer::SelectionToggle(int subsceneIndex, bool forceTrue,
                                   int ix0, int ix1, int ix2, int ix3, int ix4)
    {
        selected = true;
    }

    Vec3F Renderer::Get3DCoordinates(int subsceneIndex,
                                     int ix0, int ix1, int ix2, int ix3, int ix4)
    {
        return Vec3F(0, 0, 0);
    }

    void Renderer::Unload() {
        selected = false;
    }

    string Renderer::GetSupportedLoadFileFormats() {
        return "All Files (*.*)";
    }

    string Renderer::GetSupportedSaveFileFormats() {
        return "All Files (*.*)";
    }

    bool Renderer::SetCuttingPlane(float position,
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

    void Renderer::DrawSphere(Vec3F center, float radius) {
        glPushMatrix();
        glTranslatef(center.X(), center.Y(), center.Z());
        GLUquadric * quadricSphere = gluNewQuadric();
        gluSphere(quadricSphere, radius, 10, 10);
        gluDeleteQuadric(quadricSphere);
        glPopMatrix();
    }

    void Renderer::DrawCylinder(Vec3F pt1, Vec3F pt2,
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

    void Renderer::DrawLine(Vec3F pt1, Vec3F pt2) {
        glBegin(GL_LINES);
        glVertex3f(pt1.X(), pt1.Y(), pt1.Z());
        glVertex3f(pt2.X(), pt2.Y(), pt2.Z());
        glEnd();
    }

    void Renderer::SetDisplayStyle(int style) {
        this->displayStyle = style;
    }
}

#endif /* SRC_TOOLKIT_VISUALIZATION_RENDERER_H_ */
