// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   The base renderer.  This has to be extended by all renderer classes.

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$
//   Revision 1.28  2008/11/20 18:33:00  ssa1
//   Using the origin of the MRC volume
//
//   Revision 1.27  2008/11/13 20:54:40  ssa1
//   Using the correct scale when loading volumes
//
//   Revision 1.26  2008/11/06 20:34:23  ssa1
//   Proper lighting for bounding boxes
//
//   Revision 1.25  2008/10/28 22:18:05  ssa1
//   Changing visualization of meshes, and sketches
//
//   Revision 1.24  2008/10/16 02:39:57  ssa1
//   Modifying the sketch behavior to supplement line drawing instead of replace it.
//
//   Revision 1.23  2008/09/29 20:36:35  ssa1
//   Drawing skeletal curves as cylinders and spheres
//
//   Revision 1.22  2008/09/29 16:01:17  ssa1
//   Adding in CVS meta information
//

#ifndef GORGON_RENDERER_H
#define GORGON_RENDERER_H

#include <string>
#include <glut.h>
#include <Foundation/OpenGLUtils.h>
#include <MathTools/Vector3D.h>
#include <MathTools/MathLib.h>

using namespace std;
using namespace wustl_mm::MathTools;
using namespace wustl_mm::Foundation;

namespace wustl_mm {
	namespace Visualization {	
		class Renderer {
		public:
			Renderer();
			virtual ~Renderer();
			virtual void Draw(int subSceneIndex, bool selectEnabled);
			virtual void DrawBoundingBox();
			virtual void LoadFile(string fileName);
			virtual void SaveFile(string fileName);
			virtual void Unload();
			virtual bool SelectionRotate(Vector3DFloat centerOfMass, Vector3DFloat rotationAxis, float angle);
			virtual int SelectionObjectCount();
			virtual Vector3DFloat SelectionCenterOfMass();
			virtual bool SelectionMove(Vector3DFloat moveDirection);
			virtual bool SelectionClear();
			virtual void SelectionToggle(int subsceneIndex, bool forceTrue, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
			virtual string GetSupportedLoadFileFormats();
			virtual string GetSupportedSaveFileFormats();
			virtual Vector3DFloat Get3DCoordinates(int subsceneIndex, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
			bool SetCuttingPlane(float position, float vecX, float vecY, float vecZ);
			void static DrawSphere(Vector3DFloat center, float radius);
			void static DrawCylinder(Vector3DFloat pt1, Vector3DFloat pt2, float radius);
			virtual void SetSpacing(float spX, float spY, float spZ);
			virtual float GetSpacingX();
			virtual float GetSpacingY();
			virtual float GetSpacingZ();
			virtual void SetOrigin(float orgX, float orgY, float orgZ);
			virtual float GetOriginX();
			virtual float GetOriginY();
			virtual float GetOriginZ();

			float GetMin(int dimension);
			float GetMax(int dimension);

		protected:
			virtual void UpdateBoundingBox();			
			float minPts[3];
			float maxPts[3];
			float spacing[3];
			float origin[3];
			bool selected;
			Vector3DFloat cuttingPlaneCenter;
			Vector3DFloat cuttingPlaneDirection;
			GLUquadric * quadricSphere;
			GLUquadric * quadricCylinder;
		};

		Renderer::Renderer() {
			SetSpacing(1.0f, 1.0f, 1.0f);
			SetOrigin(0.0f, 0.0f, 0.0f);
			selected = false;
			quadricSphere = gluNewQuadric();
			quadricCylinder = gluNewQuadric();

		}

		Renderer::~Renderer() {
			gluDeleteQuadric(quadricSphere);
			gluDeleteQuadric(quadricCylinder);
		}

		float Renderer::GetMin(int dimension) {
			return minPts[dimension];
		}

		float Renderer::GetMax(int dimension) {
			return maxPts[dimension];
		}

		void Renderer::Draw(int subSceneIndex, bool selectEnabled) { 
		}

		void Renderer::DrawBoundingBox() {
			glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);
			glDisable(GL_LIGHTING);
			glPushMatrix();
			glTranslatef(minPts[0]+(maxPts[0]-minPts[0])/2.0, minPts[1]+(maxPts[1]-minPts[1])/2.0, minPts[2]+(maxPts[2]-minPts[2])/2.0);
			glScalef(maxPts[0]-minPts[0], maxPts[1]-minPts[1], maxPts[2]-minPts[2]);
			glutWireCube(1.0);
			glPopMatrix();
			glPopAttrib();
		}

		void Renderer::UpdateBoundingBox() {
			for(int i = 0; i < 3; i++) {
				minPts[i] = -0.5;
				maxPts[i] = 0.5;
			}
		}

		void Renderer::LoadFile(string fileName) {
			SetSpacing(1.0f, 1.0f, 1.0f);
		}

		void Renderer::SaveFile(string fileName) {
		}

		bool Renderer::SelectionRotate(Vector3DFloat centerOfMass, Vector3DFloat rotationAxis, float angle) {
			return false;
		}

		int	Renderer::SelectionObjectCount() {
			return 0;
		}

		Vector3DFloat Renderer::SelectionCenterOfMass() {
			return Vector3DFloat((maxPts[0] - minPts[0]) / 2.0, (maxPts[1] - minPts[1]) / 2.0, (maxPts[2] - minPts[2]) / 2.0);
		}

		bool Renderer::SelectionMove(Vector3DFloat moveDirection) {
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

		void Renderer::SelectionToggle(int subsceneIndex, bool forceTrue, int ix0, int ix1, int ix2, int ix3, int ix4) {
			selected = true;
		}

		Vector3DFloat Renderer::Get3DCoordinates(int subsceneIndex, int ix0, int ix1, int ix2, int ix3, int ix4) {
			return Vector3DFloat(0,0,0);
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

		bool Renderer::SetCuttingPlane(float position, float vecX, float vecY, float vecZ) {
			Vector3DFloat center = Vector3DFloat( (minPts[0] + maxPts[0])/2.0, (minPts[1] + maxPts[1])/2.0, (minPts[2] + maxPts[2])/2.0);
			float distance = (Vector3DFloat(minPts[0], minPts[1], minPts[2]) - center).Length();
			cuttingPlaneDirection = Vector3DFloat(vecX, vecY, vecZ);	
			cuttingPlaneDirection.Normalize();
			cuttingPlaneCenter = center +  cuttingPlaneDirection * position * distance;
			//printf("%lf %lf %lf - %lf %lf\n", cuttingPlaneCenter.values[0], cuttingPlaneCenter.values[1], cuttingPlaneCenter.values[2], position, distance); flushall();
			return false;
		}

		void Renderer::DrawSphere(Vector3DFloat center, float radius) {
			glPushMatrix();
			glTranslatef(center.X(), center.Y(), center.Z());
			GLUquadric * quadricSphere = gluNewQuadric();
			gluSphere(quadricSphere, radius, 10, 10);
			gluDeleteQuadric(quadricSphere);
			glPopMatrix();
		}

		void Renderer::DrawCylinder(Vector3DFloat pt1, Vector3DFloat pt2, float radius) {
			Vector3DFloat qmp = pt1-pt2;
			float length = qmp.Length();
			qmp.Normalize();			
			Vector3DFloat z = Vector3DFloat(0,0,1);
			Vector3DFloat axis = z ^ qmp;
			float angle = acos(qmp * z)* 180.0 / PI;

			glPushMatrix();
			glTranslatef(pt2.X(), pt2.Y(), pt2.Z());
			glRotatef(angle, axis.X(), axis.Y(), axis.Z());

			GLUquadric * quadricCylinder = gluNewQuadric();
			gluCylinder(quadricCylinder, radius, radius, length, 10, 10);
			gluDeleteQuadric(quadricCylinder);

			glPopMatrix();
		}

		void Renderer::SetSpacing(float spX, float spY, float spZ) {	
			spacing[0] = spX;
			spacing[1] = spY;
			spacing[2] = spZ;
		}

		float Renderer::GetSpacingX() {
			return spacing[0];
		}

		float Renderer::GetSpacingY() {
			return spacing[1];
		}

		float Renderer::GetSpacingZ() {
			return spacing[2];
		}

		void Renderer::SetOrigin(float orgX, float orgY, float orgZ) {	
			origin[0] = orgX;
			origin[1] = orgY;
			origin[2] = orgZ;
		}


		float Renderer::GetOriginX() {
			return origin[0];
		}

		float Renderer::GetOriginY() {
			return origin[1];
		}

		float Renderer::GetOriginZ() {
			return origin[2];
		}


	}
}


#endif
