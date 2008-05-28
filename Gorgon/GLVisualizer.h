#ifndef GL_VISUALIZER_H_
#define GL_VISUALIZER_H_

#ifdef _WIN32
	#include <GL/glut.h>
#else
	#include <glut.h>
#endif
#include "GraphMatch/StandardGraph.h"

using namespace std;
using namespace wustl_mm::GraphMatch;

namespace wustl_mm {
	namespace Visualization {

		class GLVisualizer {
		public:
			GLVisualizer() { _quad = gluNewQuadric(); }
			// METHODS
			void DrawSkeleton(StandardGraph* skeleton);
			void DrawSequence(StandardGraph* sequence);
			void DrawHelix();
		private:
			GLUquadric * _quad;
		};

		void GLVisualizer::DrawSkeleton(StandardGraph* skeleton) {
			float size_x = skeleton->skeletonVolume->getSizeX();
			float size_y = skeleton->skeletonVolume->getSizeY();
			float size_z = skeleton->skeletonVolume->getSizeZ();
			
			Matrix4 resize = Matrix4::scaling(1/size_x, 1/size_y, 1/size_z);
			//cout << size_x << ", " << size_y << ", " << size_z << endl;
			glMultMatrixd((double*)&resize);


			for(unsigned int i=0; i<skeleton->skeletonHelixes.size(); i++) {
				Matrix4 matrix = skeleton->skeletonHelixes[i]->GetWorldToObjectMatrix();
				glPushMatrix();
					glMultMatrixd((double*)&matrix);
					DrawHelix();
				glPopMatrix();
			}
		}
		void GLVisualizer::DrawSequence(StandardGraph* sequence) {
			
			float total = 1.0f * sequence->pdbStructures[sequence->pdbStructures.size()-1]->endPosition;

			for(unsigned int i=0; i<sequence->pdbStructures.size(); i++) {
				float start = (float)sequence->pdbStructures[i]->startPosition;
				float end = (float)sequence->pdbStructures[i]->endPosition;
				glRectd(start/total * 100, 0.0, end/total * 100 , 10.0);
			}
		}

		void GLVisualizer::DrawHelix() {
			// Fix the caps!
			int resolution = 8;
			glPushMatrix();
				glTranslated(0.0, 0.5, 0.0);
				glScalef(1.0, 0.2, 1.0);
				gluSphere(_quad, 0.5, resolution, resolution);
			glPopMatrix();
			glPushMatrix();
				glTranslated(0.0, -0.5, 0.0);
				glScalef(1.0, 0.2, 1.0);
				gluSphere(_quad, 0.5, resolution, resolution);
			glPopMatrix();
			glPushMatrix();
				glRotatef(90.0, 1.0, 0.0, 0.0);
				glTranslatef(0.0, 0.0, -0.5);
				gluCylinder(_quad, 0.5, 0.5, 1.0, resolution, resolution);
			glPopMatrix();
		}
	}
}
#endif
