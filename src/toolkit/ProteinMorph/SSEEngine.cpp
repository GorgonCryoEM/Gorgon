/*
 * SSEEngine.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "SSEEngine.h"

namespace Visualization {

    void SSEEngine::draw(int sceneIndex) {
        std::cout << "SSECorrespondenceEngine::Draw called" << std::endl;
        int n1, n2, sse1, sse2;
        vector<Vec3I> path;
        if(curInd >= 0) {
            SSEResult result = getResult(curInd + 1);

            glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_HINT_BIT);
            glDisable(GL_LIGHTING);
            glLineWidth(5);
            glEnable(GL_LINE_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
            glBegin(GL_LINE_STRIP); // test!!! 

            n1 = -1;
            n2 = -1;
            sse1 = -1;
            sse2 = -1;

            int seqNumber = 0;
            int seqIndex = 0;
            int strandsPassed = 0;


            double pathx, pathy, pathz;
            double prevx, prevy, prevz;
            bool pastFirstStructure = false;

            // the following code iterates over the correspondence, finding a valid edge at each iteration.
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
                        //cout << "found skip edge at end of correspondence. breaking out of loop." << endl;
                        break;
                    }
                }
                if (sequence.adjacencyMatrix[i][i][0] == GRAPHNODE_SHEET) {
                    strandsPassed --;
                }
                i--;
                seqIndex = (i + strandsPassed + 1)/2 + 1;

                path = skeleton.paths[n1][n2];
                //cout << "path sizes. fwd:" << skeleton.paths[n1][n2].size() << ", rev:" << skeleton.paths[n2][n1].size() << endl;
                if(path.size() == 0) {
                    path = skeleton.paths[n2][n1];
                    int n1old = n1;
                    n1 = n2;
                    n2 = n1old;
                }

                // color code

                // get colors of beginning and ending SSEs
                int numHelices = skeleton.GetHelixCount();

                // start SSE color
                int startSSENumber;
                float startColorR, startColorG, startColorB, startColorA;
                if(skeleton.adjacencyMatrix[n1][n1][0] == GRAPHNODE_SHEET){
                    startSSENumber = n1 - numHelices;
                } else {
                    startSSENumber = n1/2;
                }
                skeleton.skeletonHelixes[startSSENumber]->GetColor(startColorR, startColorG, startColorB, startColorA);

                // end SSE color
                int endSSENumber;
                float endColorR, endColorG, endColorB, endColorA;
                if(skeleton.adjacencyMatrix[n2][n2][0] == GRAPHNODE_SHEET){
                    endSSENumber = n2 - numHelices;
                } else {
                    endSSENumber = n2/2;
                }
                skeleton.skeletonHelixes[endSSENumber]->GetColor(endColorR, endColorG, endColorB, endColorA);

                if(startSSENumber == endSSENumber && startSSENumber < numHelices){
                    seqNumber += 0; // internal helix loop
                } else {
                    seqNumber += 1;
                }


                if (path.size() != 0) {
                    glEnd(); // test!!! 
                    // draw labeled sphere at beginning of path
                    //GLfloat col = 1.0;
                    glColor3f(startColorR, startColorG, startColorB);
                    double sphereRadius = 0.5;
                    drawSphere(Vec3F(path[0].X(), path[0].Y(), path[0].Z()), sphereRadius);
                    /*
                    // Label the points with their graph node numbers
                    glColor3f(1.0, 1.0, 1.0);

                    glRasterPos3d(path[0].X(), path[0].Y(), path[0].Z());
                    //int labelInt = seqNumber;
                    int labelInt = seqIndex;
                    //int labelInt = sse1;
                    std::ostringstream tmpStream;
                    tmpStream << labelInt;
                    string labelStr = tmpStream.str();
                    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
                    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
                    for (int i = 0; i < labelStr.length(); i++) {
                        //glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, labelStr[i]);
                        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, labelStr[i]);
                    }
                    */
                    // labeled sphere code
                    glBegin(GL_LINE_STRIP);
                }

                int pathSize = path.size(); // for color
                float stepColorR = (endColorR - startColorR) / (pathSize-1);
                float stepColorG = (endColorG - startColorG) / (pathSize-1);
                float stepColorB = (endColorB - startColorB) / (pathSize-1);
                for(int j = 0; j < pathSize; j++) {
                    //cout << "adding path from " << n1 << " to " << n2 << ", point " << path[j].X() << "," << path[j].Y() << "," << path[j].Z() << endl;
                    glColor3f(startColorR + stepColorR * j, startColorG + stepColorG * j, startColorB + stepColorB * j);
                    /* UNCOMMENT THIS BLOCK TO DRAW STRAIGHT LINE PATHS ACROSS SSES
                    if (j==0 && pastFirstStructure) {
                        glVertex3d(prevx, prevy, prevz);
                    } */
                    double offset = 0.8*(-0.5 + (double)i / (double)numNodes );
                    pathx=path[j].X()+offset;
                    pathy=path[j].Y()+offset;
                    pathz=path[j].Z()+offset;
                    glVertex3d(pathx, pathy, pathz);
                    //glVertex3d(path[j].X()+offset, path[j].Y()+offset, path[j].Z()+offset);
                }

                // to draw paths across sheets
                pastFirstStructure=true;
                prevx=pathx;
                prevy=pathy;
                prevz=pathz;

                if (path.size() != 0) {
                    glEnd();
                    // draw labeled sphere at end of path
                    //GLfloat col = 1.0;
                    //glColor3f(col, col, col);
                    double sphereRadius = 0.5;
                    drawSphere(Vec3F(path[pathSize-1].X(), path[pathSize-1].Y(), path[pathSize-1].Z()), sphereRadius);

                    // Label the points with their graph node numbers
                    glColor3f(1.0, 1.0, 1.0);

                    glRasterPos3d(path[pathSize-1].X(), path[pathSize-1].Y(), path[pathSize-1].Z());
                    //int labelInt = seqNumber + 1;
                    int labelInt = seqIndex;
                    //int labelInt = sse2;
                    std::ostringstream tmpStream;
                    tmpStream << labelInt;
                    string labelStr = tmpStream.str();
                    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
                    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
                    for (unsigned int i = 0; i < labelStr.length(); i++) {
                        //glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, labelStr[i]);
                        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, labelStr[i]);
                    }
                    // labeled sphere code
                    glBegin(GL_LINE_STRIP);
                }
            }
            glEnd();
            glPopAttrib();
        }
    }

    void SSEEngine::drawAllPaths(int sceneIndex, bool showPaths, bool showHelixCorners, bool showSheetCorners, bool showSheetColors) {
         std::cout << "SSECorrespondenceEngine::DrawAllPaths called" << std::endl;


         const vector<Shape *> &helixes = skeleton.skeletonHelixes;
         // draw paths
         if (showPaths) {
            glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_HINT_BIT);
            glDisable(GL_LIGHTING);
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
                        helixes[startSSENumber]->getColor(startColorR, startColorG, startColorB, startColorA);

                        // end SSE color
                        int endSSENumber;
                        if(skeleton.adjacencyMatrix[n1][n1][0] == GRAPHNODE_SHEET){
                            endSSENumber = n1 - numHelices;
                        } else {
                            endSSENumber = n1/2;
                        }

                        float endColorR, endColorG, endColorB, endColorA;
                        helixes[endSSENumber]->getColor(endColorR, endColorG, endColorB, endColorA);

                        glBegin(GL_LINE_STRIP);
                        int pathSize = path.size(); // for color
                        float stepColorR = (endColorR - startColorR) / (pathSize-1);
                        float stepColorG = (endColorG - startColorG) / (pathSize-1);
                        float stepColorB = (endColorB - startColorB) / (pathSize-1);

                        Shape * vv = helixes[0];
                        Vec3D org = vv->getOrigin();
                        Vec3I size = vv->getSize();
                        Vec3D spacing = vv->getSpacing();

                        for(int k = 0; k < pathSize; k++) {
                            //cout << "adding path from " << n1 << " to " << n2 << ", point " << path[j].X() << "," << path[j].Y() << "," << path[j].Z() << endl;
                            glColor3f(startColorR + stepColorR * k, startColorG + stepColorG * k, startColorB + stepColorB * k);

                            Vec3D loc(path[k].X(), path[k].Y(), path[k].Z());
                            Vec3D apix(loc[0]*spacing[0], loc[1]*spacing[1], loc[2]*spacing[2]);
                            loc = org + apix;

                            glVertex3d(loc[0], loc[1], loc[2]);
                        }
                        glEnd();
                    }
                }
            }
            glPopAttrib();
        }

        // draw corner nodes (helices)
        if (showHelixCorners) {
            glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_HINT_BIT);
            for(int i = 0; i < (int) helixes.size(); i++) {
                Shape * vv = helixes[i];
                const Vec3D &center = vv->getCenter();
                const Vec3D &org = vv->getOrigin();
                const Vec3D &spacing = vv->getSpacing();

                if (helixes[i]->type == GRAPHEDGE_HELIX) {
                    for(int j = 0; j < (int) helixes[i]->cornerCells.size(); j++) {
                        const Point3Pair &cornerCells = vv->cornerCells[j];

                        cout << Vec3F(cornerCells.x, cornerCells.y, cornerCells.z) << "\t";
                        cout << center << "\ti: " << i << "\tj: " << j << endl;

                        // Color first helix corner white, second corner gray
                        GLfloat col = 1.0 - 0.6 * (cornerCells.node - 1);
                        glColor3f(col, col, col);

                        Vec3D loc(cornerCells.x, cornerCells.y, cornerCells.z);
                        Vec3D apix(loc[0]*spacing[0], loc[1]*spacing[1], loc[2]*spacing[2]);
//                        Vec3D apix((loc[0]-1)*spacing[0], (loc[1]-1)*spacing[1], (loc[2]-1)*spacing[2]);
                        loc = org + apix;

                        double sphereRadius = .25;
                        drawSphere(loc, sphereRadius);

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
        if (showSheetCorners) {
            glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_HINT_BIT);
            int lastHelix = 0;
            for(int i = 0; i < (int) helixes.size(); i++) {
                if (helixes[i]->type == GRAPHEDGE_HELIX) {
                    lastHelix=i;
                }
                if (helixes[i]->type == GRAPHEDGE_SHEET) {
                    for(int j = 0; j < (int) helixes[i]->cornerCells.size(); j++) {
                        glColor3f(1.0, 1.0, 1.0);

                        Shape * vv = helixes[j];
                        Vec3D org = vv->getOrigin();
                        Vec3D loc(helixes[i]->cornerCells[j].x, helixes[i]->cornerCells[j].y, helixes[i]->cornerCells[j].z);
                        loc += org;
                        drawSphere(loc, .25);

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


