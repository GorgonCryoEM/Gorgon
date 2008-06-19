#ifndef GORGON_RENDERER_H
#define GORGON_RENDERER_H

#include <string>
#include <glut.h>
#include <MathTools/Vector3D.h>
#include <MathTools/MathLib.h>

using namespace std;
using namespace wustl_mm::MathTools;

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
			virtual void Select(int subsceneIndex, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
			virtual string GetSupportedLoadFileFormats();
			virtual string GetSupportedSaveFileFormats();
			virtual Vector3DFloat Get3DCoordinates(int subsceneIndex, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
			bool SetCuttingPlane(float position, float vecX, float vecY, float vecZ);
			void DrawCylinder(Vector3DFloat pt1, Vector3DFloat pt2, float radius);

			float GetMin(int dimension);
			float GetMax(int dimension);

		protected:
			virtual void UpdateBoundingBox();			
			float minPts[3];
			float maxPts[3];
			int selectedSubSceneIndex;
			int selectedIx[5];
			Vector3DFloat cuttingPlaneCenter;
			Vector3DFloat cuttingPlaneDirection;
			GLUquadric * quadricSphere;
			GLUquadric * quadricCylinder;


		};

		Renderer::Renderer() {
			selectedSubSceneIndex = -1;
			for(int i = 0; i < 5; i++) {
				selectedIx[i] = -1;
			}
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
			glPushMatrix();
			glTranslatef(minPts[0]+(maxPts[0]-minPts[0])/2.0, minPts[1]+(maxPts[1]-minPts[1])/2.0, minPts[2]+(maxPts[2]-minPts[2])/2.0);
			glScalef(maxPts[0]-minPts[0], maxPts[1]-minPts[1], maxPts[2]-minPts[2]);
			glutWireCube(1.0);
			glPopMatrix();
		}

		void Renderer::UpdateBoundingBox() {
			for(int i = 0; i < 3; i++) {
				minPts[i] = -0.5;
				maxPts[i] = 0.5;
			}
		}

		void Renderer::LoadFile(string fileName) {
		}

		void Renderer::SaveFile(string fileName) {
		}

		void Renderer::Select(int subsceneIndex, int ix0, int ix1, int ix2, int ix3, int ix4) {
			selectedSubSceneIndex = subsceneIndex;
			selectedIx[0] = ix0;
			selectedIx[1] = ix1;
			selectedIx[2] = ix2;
			selectedIx[3] = ix3;
			selectedIx[4] = ix4;
		}

		Vector3DFloat Renderer::Get3DCoordinates(int subsceneIndex, int ix0, int ix1, int ix2, int ix3, int ix4) {
			return Vector3DFloat(0,0,0);
		}

		void Renderer::Unload() {
			selectedSubSceneIndex = -1;
			for(int i = 0; i < 5; i++) {
				selectedIx[i] = -1;
			}
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
	}
}


#endif