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
			virtual void Draw();
			virtual void DrawBoundingBox();
			virtual void LoadFile(string fileName);
			virtual void Unload();
			virtual string GetSupportedFileFormats();

			float GetMin(int dimension);
			float GetMax(int dimension);

		protected:
			virtual void UpdateBoundingBox();
			bool drawBoundingBox;
			float minPts[3];
			float maxPts[3];
		};


		float Renderer::GetMin(int dimension) {
			return minPts[dimension];
		}

		float Renderer::GetMax(int dimension) {
			return maxPts[dimension];
		}

		void Renderer::Draw() { 
		}

		void Renderer::DrawBoundingBox() {
			if(drawBoundingBox) {
				glPushMatrix();
				glTranslatef(minPts[0]+(maxPts[0]-minPts[0])/2.0, minPts[1]+(maxPts[1]-minPts[1])/2.0, minPts[2]+(maxPts[2]-minPts[2])/2.0);
				glScalef(maxPts[0]-minPts[0], maxPts[1]-minPts[1], maxPts[2]-minPts[2]);
				glutWireCube(1.0);
				glPopMatrix();
			}
		}

		void Renderer::UpdateBoundingBox() {
			for(int i = 0; i < 3; i++) {
				minPts[i] = -0.5;
				maxPts[i] = 0.5;
			}
		}

		void Renderer::LoadFile(string fileName) {
		}

		void Renderer::Unload() {
		}
		string Renderer::GetSupportedFileFormats() {
			return "All Files (*.*)";
		}

	}
}


#endif