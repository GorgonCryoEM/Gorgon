#ifndef GORGON_RENDERER_H
#define GORGON_RENDERER_H

#include <string>
#include <GL/glut.h>
#include <MathTools/Vector3D.h>

using namespace std;
using namespace wustl_mm::MathTools;

namespace wustl_mm {
	namespace Visualization {	
		class Renderer {
		public:
			Renderer();
			virtual void Draw(int subSceneIndex, bool selectEnabled);
			virtual void DrawBoundingBox();
			virtual void LoadFile(string fileName);
			virtual void Unload();
			virtual void Select(int subsceneIndex, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
			virtual string GetSupportedLoadFileFormats();
			virtual string GetSupportedSaveFileFormats();

			float GetMin(int dimension);
			float GetMax(int dimension);

		protected:
			virtual void UpdateBoundingBox();
			float minPts[3];
			float maxPts[3];
			int selectedSubSceneIndex;
			int selectedIx[5];
		};

		Renderer::Renderer() {
			selectedSubSceneIndex = -1;
			for(int i = 0; i < 5; i++) {
				selectedIx[i] = -1;
			}
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

		void Renderer::Select(int subsceneIndex, int ix0, int ix1, int ix2, int ix3, int ix4) {
			selectedSubSceneIndex = subsceneIndex;
			selectedIx[0] = ix0;
			selectedIx[1] = ix1;
			selectedIx[2] = ix2;
			selectedIx[3] = ix3;
			selectedIx[4] = ix4;
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

	}
}


#endif