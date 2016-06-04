/*
 * SSEEngine.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "SSEEngine.h"

namespace Visualization {
    void SSEEngine::draw(int sceneIndex) {
        cout << "SSEEngine::draw called" << endl;

        int n1, n2, sse1, sse2;
        vector<Vec3I> path;
        if(curInd >= 0 && solutions.size() > 0) {
            SSEResult result = solutions[curInd];
//            if(result.getCost()<=0.0)
//                return;

            n1 = -1;
            n2 = -1;
            sse1 = -1;
            sse2 = -1;

            int seqNumber = 0;
            int seqIndex = 0;
            int strandsPassed = 0;

            // the following code iterates over the matcher->solutions, finding a valid edge at each iteration.
            // start at node 0 of this result, continue until i is at last node
            int numNodes = result.getNodeCount();
            for(int i = 0; i < result.getNodeCount()-1; ) {

                // set n1 to be the ith result. if the result is -1, increment i and repeat.
                for(n1 = -1; n1 < 0; ) {
                    n1 = result.getSkeletonNode(i);
                    sse1 = result.nodeToHelix(n1);
                    i++;
                    // update the seqIndex
                    if (sequence.adjacencyMatrix[i][i][0] == GRAPHNODE_SHEET) {
                        strandsPassed ++;
                    }
                }
                // set n2 to be the ith result. if the result is -1, increment i and repeat
                //for(n2 = -1; n2 < 0l; ) {
                for(n2 = -1; n2 < 0; ) {
                    n2 = result.getSkeletonNode(i);
                    sse2 = result.nodeToHelix(n2);
                    i++;
                    // update the seqIndex
                    if (sequence.adjacencyMatrix[i][i][0] == GRAPHNODE_SHEET) {
                        strandsPassed ++;
                    }
                    if (i >= result.getNodeCount()) {
                        //cout << "found skip edge at end of matcher->solutions. breaking out of loop." << endl;
                        break;
                    }
                }
                if (sequence.adjacencyMatrix[i][i][0] == GRAPHNODE_SHEET) {
                    strandsPassed --;
                }
                i--;
                seqIndex = (i + strandsPassed + 1)/2 + 1;

                path = skeleton.paths[n1][n2];

                if(path.size() == 0) {
                    path = skeleton.paths[n2][n1];
                    int n1old = n1;
                    n1 = n2;
                    n2 = n1old;
                }

                // color code

                // get colors of beginning and ending SSEs
                int numHelices = skeleton.getHelixCount();

                // start SSE color
                int startSSENumber;
                if(skeleton.adjacencyMatrix[n1][n1][0] == GRAPHNODE_SHEET){
                    startSSENumber = n1 - numHelices;
                } else {
                    startSSENumber = n1/2;
                }

                // end SSE color
                int endSSENumber;
                if(skeleton.adjacencyMatrix[n2][n2][0] == GRAPHNODE_SHEET){
                    endSSENumber = n2 - numHelices;
                } else {
                    endSSENumber = n2/2;
                }

                if(startSSENumber == endSSENumber && startSSENumber < numHelices){
                    seqNumber += 0; // internal helix loop
                } else {
                    seqNumber += 1;
                }
            }
        }
    }

     void SSEEngine::drawAllPaths(int sceneIndex, bool showPaths, bool showHelixCorners, bool showSheetCorners, bool showSheetColors) {
        std::cout << "SSECorrespondenceEngine::DrawAllPaths called" << std::endl;

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
                            int n1old = n1;
                            n1 = n2;
                            n2 = n1old;
                        }
                        //cout << "adding path from " << n1 << " to " << n2 << ", which has length " << path.size() << endl;

                        // get colors of beginning and ending SSEs
                        int numHelices = skeleton.getHelixCount();

                        // start SSE color
                        int startSSENumber;
                        float startColorR, startColorG, startColorB, startColorA;
                        if(skeleton.adjacencyMatrix[n2][n2][0] == GRAPHNODE_SHEET){
                            startSSENumber = n2 - numHelices;
                        } else {
                            startSSENumber = n2/2;
                        }
                        skeleton.skeletonHelixes[startSSENumber]->getColor(startColorR, startColorG, startColorB, startColorA);

                        // end SSE color
                        int endSSENumber;
                        float endColorR, endColorG, endColorB, endColorA;
                        if(skeleton.adjacencyMatrix[n1][n1][0] == GRAPHNODE_SHEET){
                            endSSENumber = n1 - numHelices;
                        } else {
                            endSSENumber = n1/2;
                        }
                        skeleton.skeletonHelixes[endSSENumber]->getColor(endColorR, endColorG, endColorB, endColorA);

                        glBegin(GL_LINE_STRIP);
                        int pathSize = path.size(); // for color
                        float stepColorR = (endColorR - startColorR) / (pathSize-1);
                        float stepColorG = (endColorG - startColorG) / (pathSize-1);
                        float stepColorB = (endColorB - startColorB) / (pathSize-1);
                        for(int k = 0; k < pathSize; k++) {
                            //cout << "adding path from " << n1 << " to " << n2 << ", point " << path[j].X() << "," << path[j].Y() << "," << path[j].Z() << endl;
                            glColor3f(startColorR + stepColorR * k, startColorG + stepColorG * k, startColorB + stepColorB * k);
                            glVertex3d(path[k].X(), path[k].Y(), path[k].Z());
                        }
                        glEnd();
                    }
                }
            }
            glPopAttrib();
        }

        // draw start and end of paths (subset of corner nodes)
        /*
        for(int i = 0; i < skeleton.GetNodeCount(); i++) {
            for(int j = i; j < skeleton.GetNodeCount(); j++) {
                n1 = i;
                n2 = j;
                if((n1 >= 0)  && (n2 >= 0)) {
                    path = skeleton.paths[n1][n2];
                    if(path.size() > 0) {
                        Renderer::DrawSphere(Vector3DFloat(path[0].X(), path[0].Y(), path[0].Z()), 1.0);
                        Renderer::DrawSphere(Vector3DFloat(path[path.size()-1].X(), path[path.size()-1].Y(), path[path.size()-1].Z()), 1.0);
                    }
                }
            }
        }
        */

        // draw corner nodes (helices)
        if (true) {
            glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_HINT_BIT);
            for(int i = 0; i < (int)skeleton.skeletonHelixes.size(); i++) {
                if (skeleton.skeletonHelixes[i]->type == GRAPHEDGE_HELIX) {
                    for(int j = 0; j < (int)skeleton.skeletonHelixes[i]->cornerCells.size(); j++) {
                        // Color first helix corner white, second corner gray
                        GLfloat col = 1.0 - 0.6 * (skeleton.skeletonHelixes[i]->cornerCells[j].node - 1);
                        glColor3f(col, col, col);
                        double sphereRadius = 2.;
                        cout<<Vec3F(skeleton.skeletonHelixes[i]->cornerCells[j].x, skeleton.skeletonHelixes[i]->cornerCells[j].y, skeleton.skeletonHelixes[i]->cornerCells[j].z)<<endl;
                        drawSphere(Vec3F(skeleton.skeletonHelixes[i]->cornerCells[j].x, skeleton.skeletonHelixes[i]->cornerCells[j].y, skeleton.skeletonHelixes[i]->cornerCells[j].z), sphereRadius);


                        // Label the points with their graph node numbers
                        glColor3f(1.0, 1.0, 1.0);

                        glRasterPos3d(skeleton.skeletonHelixes[i]->cornerCells[j].x, skeleton.skeletonHelixes[i]->cornerCells[j].y, skeleton.skeletonHelixes[i]->cornerCells[j].z);
                        int cornerNum = skeleton.skeletonHelixes[i]->cornerCells[j].node; // 0 or 1
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
                        drawSphere(Vec3F(skeleton.skeletonHelixes[i]->cornerCells[j].x, skeleton.skeletonHelixes[i]->cornerCells[j].y, skeleton.skeletonHelixes[i]->cornerCells[j].z), 2);

                        // Label the points with their graph node numbers
                        glRasterPos3d(skeleton.skeletonHelixes[i]->cornerCells[j].x, skeleton.skeletonHelixes[i]->cornerCells[j].y, skeleton.skeletonHelixes[i]->cornerCells[j].z);
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


