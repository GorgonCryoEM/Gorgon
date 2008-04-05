#ifndef _MESH_H_
#define _MESH_H_

#include <iostream>
#include <tchar.h>
#include <GL/glut.h>
#include <GraphMatch/VectorMath.h>
#include <vector>

using std::vector;

// /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup

using namespace wustl_mm::GraphMatch;

namespace wustl_mm {
	namespace Visualization {
		class Mesh {
		public:
			Mesh() {
				points = *(new vector<Point3>());
				triangles = *(new vector<int>());
				normals = *(new vector<Vector3>());
			}

			~Mesh() {
				delete &points;
				delete &triangles;
				delete &normals;
			}

			void addTriangle(const Point3& p1, const Point3& p2, const Point3& p3, const Vector3& n1, const Vector3& n2, const Vector3& n3) {
				size_t index = points.size();
				points.push_back(p1);
				points.push_back(p2);
				points.push_back(p3);
				normals.push_back(n1);
				normals.push_back(n2);
				normals.push_back(n3);

				triangles.push_back(index);
				triangles.push_back(index+1);
				triangles.push_back(index+2);
			}

			void addTriangle(const Point3& p1, const Point3& p2, const Point3& p3, const Vector3& n1) {
				size_t index = points.size();
				points.push_back(p1);
				points.push_back(p2);
				points.push_back(p3);
				normals.push_back(n1);

				triangles.push_back(index);
				triangles.push_back(index+1);
				triangles.push_back(index+2);
			}

			void drawGL(float xScale, float yScale, float zScale) const {
				glPushMatrix();
				glScalef(xScale, yScale, zScale);
				glBegin(GL_TRIANGLES);
				int size = triangles.size()/3;
				for(int i=0; i<size; i++) {
					for(int j=0; j<3; j++) {
						const Point3& pt = points[triangles[3*i+j]];
						const Vector3& n = normals[3*i+j];
						glNormal3f(-n[0],-n[1],-n[2]);
						glVertex3f(pt[0],pt[1],pt[2]);
					}
				}
				glEnd();
				glPopMatrix();
			}

			void clear() {
				points.clear();
				triangles.clear();
				normals.clear();
			}

		private:
			vector<Point3> points;
			vector<int> triangles;
			vector<Vector3> normals;
};
	}
}
#endif