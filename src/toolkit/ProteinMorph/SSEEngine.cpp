/*
 * SSEEngine.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "SSEEngine.h"

namespace Visualization {

     void SSEEngine::drawAllPaths(int sceneIndex, bool showPaths, bool showHelixCorners, bool showSheetCorners, bool showSheetColors) {
        std::cout << "SSECorrespondenceEngine::DrawAllPaths called" << std::endl;

        float R = 5.0;
        glColor3f(.5,.2,.7);
        drawSphere(Vec3F(), R);

        // draw paths
        if (true) {
            glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_HINT_BIT);
            //glDisable(GL_LIGHTING);
            glLineWidth(5);
            glEnable(GL_LINE_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

            int nodeCount = skeleton.getNodeCount();
            for(int i = 0; i < nodeCount; i++) {
                for(int j = i+1; j < nodeCount; j++) {
                    //cout << "adding path from " << i << " to " << j << endl;
                    int n1 = i;
                    int n2 = j;
                    if((n1 >= 0)  && (n2 >= 0)) {
                        vector<Vec3I> path = skeleton.paths[n1][n2];
                        if(path.size() == 0) {
                            path = skeleton.paths[n2][n1];
                            swap(n1, n2);
                        }
                        //cout << "adding path from " << n1 << " to " << n2 << ", which has length " << path.size() << endl;

                        // get colors of beginning and ending SSEs
                        int numHelices = skeleton.getHelixCount();

                        // start SSE color
                        int startSSENumber;
                        if(skeleton.adjacencyMatrix[n2][n2][0] == GRAPHNODE_SHEET){
                            startSSENumber = n2 - numHelices;
                        } else {
                            startSSENumber = n2/2;
                        }

                        float startColorR, startColorG, startColorB, startColorA;
                        skeleton.skeletonHelixes[startSSENumber]->getColor(startColorR, startColorG, startColorB, startColorA);

                        // end SSE color
                        int endSSENumber;
                        if(skeleton.adjacencyMatrix[n1][n1][0] == GRAPHNODE_SHEET){
                            endSSENumber = n1 - numHelices;
                        } else {
                            endSSENumber = n1/2;
                        }

                        float endColorR, endColorG, endColorB, endColorA;
                        skeleton.skeletonHelixes[endSSENumber]->getColor(endColorR, endColorG, endColorB, endColorA);

                        glBegin(GL_LINE_STRIP);
                        int pathSize = path.size(); // for color
                        float stepColorR = (endColorR - startColorR) / (pathSize-1);
                        float stepColorG = (endColorG - startColorG) / (pathSize-1);
                        float stepColorB = (endColorB - startColorB) / (pathSize-1);

                        Shape * vv = skeleton.skeletonHelixes[0];
                        Vec3D org = vv->getOrigin();

                        for(int k = 0; k < pathSize; k++) {
                            //cout << "adding path from " << n1 << " to " << n2 << ", point " << path[j].X() << "," << path[j].Y() << "," << path[j].Z() << endl;
                            glColor3f(startColorR + stepColorR * k, startColorG + stepColorG * k, startColorB + stepColorB * k);

                            Vec3D loc(path[k].X(), path[k].Y(), path[k].Z());
                            loc += org;

                            glVertex3d(loc[0], loc[1], loc[2]);
                        }
                        glEnd();
                    }
                }
            }
            glPopAttrib();
        }

        // draw corner nodes (helices)
        if (true) {
            glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_HINT_BIT);
            for(int i = 0; i < (int)skeleton.skeletonHelixes.size(); i++) {
                Shape * vv = skeleton.skeletonHelixes[i];
                if (skeleton.skeletonHelixes[i]->type == GRAPHEDGE_HELIX) {
                    for(int j = 0; j < (int)skeleton.skeletonHelixes[i]->cornerCells.size(); j++) {
                        // Color first helix corner white, second corner gray
                        const Point3Pair &cornerCells = vv->cornerCells[j];
                        GLfloat col = 1.0 - 0.6 * (cornerCells.node - 1);
                        glColor3f(col, col, col);
                        double sphereRadius = 2.;
                        cout << Vec3F(cornerCells.x, cornerCells.y, cornerCells.z) << "\t";

                        cout<<vv->getCenter()<<"\ti: "<<i<<"\tj: "<<j<<endl;

                        Vec3D org = vv->getOrigin();
                        Vec3D loc(cornerCells.x, cornerCells.y, cornerCells.z);
                        loc += org;
                        drawSphere(Vec3F(loc[0], loc[1], loc[2]), sphereRadius);


                        // Label the points with their graph node numbers
                        glColor3f(1.0, 1.0, 1.0);

                        glRasterPos3d(loc[0], loc[1], loc[2]);
                        int cornerNum = cornerCells.node; // 0 or 1
                        int labelInt = 2 * i + cornerNum;
                        std::ostringstream tmpStream;
                        tmpStream << labelInt;
                        string labelStr = tmpStream.str();
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
                        for (unsigned int i = 0; i < labelStr.length(); i++) {
                            //glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, labelStr[i]);
                            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, labelStr[i]);
                        }
                    }
                }
            }
            glPopAttrib();
        }

        // draw corner nodes (sheets)
        if (true) {
            glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_HINT_BIT);
            int lastHelix = 0;
            for(int i = 0; i < (int)skeleton.skeletonHelixes.size(); i++) {
                if (true) {
                    lastHelix=i;
                }
                if (true) {
                    for(int j = 0; j < (int)skeleton.skeletonHelixes[i]->cornerCells.size(); j++) {
                        glColor3f(1.0, 1.0, 1.0);

                        Shape * vv = skeleton.skeletonHelixes[j];
                        Vec3D org = vv->getOrigin();
                        Vec3D loc(skeleton.skeletonHelixes[i]->cornerCells[j].x, skeleton.skeletonHelixes[i]->cornerCells[j].y, skeleton.skeletonHelixes[i]->cornerCells[j].z);
                        loc += org;
                        drawSphere(Vec3F(loc[0], loc[1], loc[2]), 2);

                        // Label the points with their graph node numbers
                        glRasterPos3d(loc[0], loc[1], loc[2]);
                        int labelInt = i + lastHelix + 2;
                        std::ostringstream tmpStream;
                        tmpStream << labelInt;
                        string labelStr = tmpStream.str();
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
                        for (unsigned int i = 0; i < labelStr.length(); i++) {
                            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, labelStr[i]);
                            //glutBitmapCharacter(GLUT_BITMAP_9_BY_15, labelStr[i]);
                            //glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, labelStr[i]);
                        }

                    }
                }
            }
            glPopAttrib();
        }
    }
}


