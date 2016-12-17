/*
 * SSERenderer.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "SSERenderer.h"
#include "Shapes.h"
#include "Foundation/StringUtils.h"
#include "MathTools/LinearSolver.h"
#include "Foundation/OpenGLUtils.h"

namespace Visualization {

    SSERenderer::SSERenderer() {
        sheetCount = 0;
        graphSheetCount = 0;
    }

    SSERenderer::~SSERenderer() {
        for(unsigned int i = 0; i < helices.size(); i++)
            delete helices[i];

        for(unsigned int i = 0; i < sheets.size(); i++)
            delete sheets[i];
    }

    vector<int> SSERenderer::getSelectedHelixIndices(){
        return selectedHelices;
    }

    vector<Shape*> * SSERenderer::getHelices(){
        return &helices;
    }

    void SSERenderer::addHelix(Vec3F p1, Vec3F p2) {
        Shape * newHelix = Shape::createHelix(p1, p2, 2.5);
        
        setMinMax(p1);
        setMinMax(p2);
        
        helices.push_back(newHelix);
    }

    void SSERenderer::finalizeHelix() {
        Vec3F p1, p2;
        LinearSolver::FindBestFitLine(p1, p2, tempSSEPoints);
        addHelix(p1, p2);
    }

    void SSERenderer::startNewSSE() {
        tempSSEPoints.clear();
    }

    void SSERenderer::addSSEPoint(Vec3F p) {
        tempSSEPoints.push_back(p);
        cout<<"   ."<<tempSSEPoints.size()
                <<"\t"<<p
                <<endl;
    }

    void SSERenderer::draw(int subSceneIndex, bool selectEnabled) {
        GLfloat emissionColor[4] = {1.0, 1.0, 1.0, 1.0};
        GLfloat frontColor[4]    = {1.0, 0.0, 0.0, 1.0};
        GLfloat backColor[4]     = {0.0, 0.0, 1.0, 1.0};

        glPushName(subSceneIndex);
        if(subSceneIndex == 0) {
            if(selectEnabled) {
                glPushName(0);
            }

            cout<<"selectedPDBHelices.size(): "<<selectedPDBHelices.size()<<endl;
            cout<<"helices.size(): "<<helices.size()<<endl;
            for(unsigned int kk=0; kk<helices.size(); ++kk)
                cout<<helices[kk]->getCenter()<<endl;

            vector<int> SSEIndices;

            for(unsigned int i = 0; i < corrs.size(); ++i){
                int SSEIndex = corrs[i].first;
                for(unsigned int k = 0; k < selectedPDBHelices.size(); ++k){
                    if(selectedPDBHelices[k] == SSEIndex){
                        SSEIndices.push_back(corrs[i].second);
                    }
                }
            }

            for(int i = 0; i < (int)helices.size(); i++) {
                glPushAttrib(GL_LIGHTING_BIT);
                if(helices[i]->isObjectSpecificColoring) {
                    float colorR, colorG, colorB, colorA;
                    helices[i]->getColor(colorR, colorG, colorB, colorA);
                    OpenGLUtils::SetColor(colorR, colorG, colorB, colorA);
                }

                if(helices[i]->isSelected()) {
                    glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
                    glMaterialfv( GL_BACK, GL_EMISSION, emissionColor);
                }

                glPushMatrix();
                glMultMatrixd(helices[i]->getWorldToObjectMatrix().mat);
                glRotated(90, 1, 0, 0);
                glTranslated(0.0, 0.0, -0.5);
                if(selectEnabled) {
                    glLoadName(i);
                }

                GLUquadric * quadricCylinder = gluNewQuadric();
                gluCylinder(quadricCylinder, 0.5, 0.5, 1.0, 10, 10);
                gluDeleteQuadric(quadricCylinder);
                glPopMatrix();
                glPopAttrib();

                if(helices[i]->isSelected()) {

                    Vec3F corner1 = getHelixCorner(i, 0);
                    Vec3F corner2 = getHelixCorner(i, 1);
                    cout << "Drawing selected cylinder. Size of helix flips is " << helixFlips.size() << endl;
                    if(helixFlips.size()  > 0){
                        if(!helixFlips[i]){
                            OpenGLUtils::SetColor(1.0, 0.0, 0.0, 1.0);
                            drawSphere(corner2, 1.0);
                            OpenGLUtils::SetColor(0.0, 0.0, 1.0, 1.0);
                            drawSphere(corner1, 1.0);
                            fflush(stdout);
                        }else{
                            OpenGLUtils::SetColor(1.0, 0.0, 0.0, 1.0);
                            drawSphere(corner1, 1.0);
                            OpenGLUtils::SetColor(0.0, 0.0, 1.0, 1.0);
                            drawSphere(corner2, 1.0);
                            fflush(stdout);
                        }
                    }
                }


                for(unsigned int j = 0; j < SSEIndices.size(); ++j){
                    if(SSEIndices[j] == i){
                        Vec3F corner1 = getHelixCorner(i, 0);
                        Vec3F corner2 = getHelixCorner(i, 1);
                        if(!helixFlips[i]){
                            OpenGLUtils::SetColor(1.0, 0.0, 0.0, 1.0);
                            drawSphere(corner2, 1.0);
                            OpenGLUtils::SetColor(0.0, 0.0, 1.0, 1.0);
                            drawSphere(corner1, 1.0);
                            fflush(stdout);
                        }else{
                            OpenGLUtils::SetColor(1.0, 0.0, 0.0, 1.0);
                            drawSphere(corner1, 1.0);
                            OpenGLUtils::SetColor(0.0, 0.0, 1.0, 1.0);
                            drawSphere(corner2, 1.0);
                            fflush(stdout);
                        }
                    }
                }
            }

            if(selectEnabled) {
                glPopName();
            }
        }

        glPopName();
    }

    void SSERenderer::loadHelixFileSSE(string fileName) {

        FILE* fin = fopen((char*)fileName.c_str(), "rt");

        char line[1000];
        string lineStr;
        float x1, x2, y1, y2, z1, z2;
        while(!feof(fin)) {
            fscanf(fin, "%s", line);
            lineStr = string(line);
            if(lineStr.compare("ALPHA") == 0) {
                fscanf(fin, "%s", line);
                fscanf(fin, "%s", line);
                fscanf(fin, "%s", line);
                fscanf(fin, "%s", line);
                fscanf(fin, "%f", &x1);
                fscanf(fin, "%f", &y1);
                fscanf(fin, "%f", &z1);
                fscanf(fin, "%f", &x2);
                fscanf(fin, "%f", &y2);
                fscanf(fin, "%f", &z2);
                addHelix(Vec3F(x1, y1, z1), Vec3F(x2, y2, z2));
            }
        }

        fclose(fin);
        cout<<"       SSERenderer::loadHelixFileSSE"<<endl;
        cout<<"helices.size(): "<<helices.size()<<endl;
    }

    void SSERenderer::loadHelixFileVRML(string fileName) {
        readHelixFile(fileName, "", helices);
        
        for(unsigned int i = 0; i < helices.size(); i++) {
            setMinMax(helices[i]->getCenter());
        }

    }

    void SSERenderer::loadHelixFile(string fileName) {
        for(unsigned int i = 0; i < helices.size(); i++) {
            delete helices[i];
        }
        #ifdef GORGON_DEBUG
              cout<<"\033[32mDEBUG: File:   SSERenderer.cpp"<<endl;
              cout<<"DEBUG: Method: SSERenderer::loadHelixFile(string)\033[0m"<<endl;
              cout<<"YAY"<<endl;
        #endif

        helices.clear();

        int pos = fileName.rfind(".") + 1;
        string extension = fileName.substr(pos, fileName.length()-pos);
        extension = StringUtils::StringToUpper(extension);
        if(extension == "WRL" || extension == "VRML") {
            loadHelixFileVRML(fileName);
        } else if(extension == "SSE") {
            loadHelixFileSSE(fileName);
        }
    }

    void SSERenderer::loadSheetFile(string fileName) {
        if(helices.size() == 0) {
//            RendererBase::load(fileName);
        }
        //vector<Shape *> sheets;
        sheets.clear();
        readSheetFile((char *)fileName.c_str(), sheets);

        sheetListToMesh(sheets);
    }

    void SSERenderer::sheetListToMesh(vector<Shape*> & sheets) {
        sheetMesh = SkeletonMesh();

        sheetCount = sheets.size();
        for(int i = 0; i <= sheetCount; i++) {
            selectedSheets[i] = false;
        }


        for(unsigned int i = 0; i < sheets.size(); i++) {
            vector<int> indices;

            for(unsigned int j = 0; j < sheets[i]->polygonPoints.size(); j++) {
                Vec3D pt = sheets[i]->polygonPoints[j];
                indices.push_back(sheetMesh.addVertex(pt));
            }

            for(unsigned int j = 0; j < sheets[i]->polygons.size(); j++) {
                GraphMatch::Polygon pp = sheets[i]->polygons[j];
                Vec3U ppp;
                ppp[0] = indices[pp.pointIndex1];
                ppp[1] = indices[pp.pointIndex2];
                ppp[2] = indices[pp.pointIndex3];
                sheetMesh.addFace(ppp);
            }
        }
    }

    void SSERenderer::unload() {
        for(unsigned int i = 0; i < helices.size(); i++) {
            delete helices[i];
        }
        helices.clear();
        for(unsigned int i = 0; i < sheets.size(); i++) {
            delete sheets[i];
        }
        sheets.clear();
    }

    void SSERenderer::unloadGraphSSEs() {
        graphSheetCount = 0;
    }

    void SSERenderer::setHelixColor(int i, float r, float g, float b, float a) {
        helices[i]->setColor(r, g, b, a);
    }

    void SSERenderer::setSheetColor(int i, float r, float g, float b, float a) {
        sheets[i]->setColor(r, g, b, a);
    }

    // set the color of an SSE. assumes that SSEs are indexed with helices first and sheets second.
    void SSERenderer::setSSEColor(int i, float r, float g, float b, float a) {
        int numHelices = helices.size();
        if (i < numHelices) {
            helices[i]->setColor(r, g, b, a);
        } else {
            sheets[i - numHelices]->setColor(r, g, b, a);
        }
    }

    bool SSERenderer::selectionRotate(Vec3F centerOfMass, Vec3F rotationAxis, float angle) {
        bool rotated = false;

        for(unsigned int i = 0; i < helices.size(); i++) {
            if(helices[i]->isSelected()) {
                rotated = true;
                Vec3D move = centerOfMass - helices[i]->getCenter();
                Matrix4 rotMatrix = Matrix4::rotation(rotationAxis, angle);
                Vec3D newMove = rotMatrix * move;
                helices[i]->setCenter(centerOfMass - newMove);

                helices[i]->rotate(rotationAxis, angle);
            }
        }

        // I think the sheet center of mass is wrong!  Need to fix this
        /*for(unsigned int i=0; i < sheetMesh->vertices.size(); i++) {
            sheetMesh->vertices[i].tag = false;
        }

        unsigned int vertexIx;
        for(unsigned int i = 0; i < sheetMesh->faces.size(); i++) {
            if(sheetMesh->faces[i].tag.selected) {
                rotated = true;
                for(unsigned int j = 0; j < sheetMesh->faces[i].vertexIds.size(); j++) {
                    vertexIx = sheetMesh->GetVertexIndex(sheetMesh->faces[i].vertexIds[j]);
                    NonManifoldMeshVertex<bool> * v = &(sheetMesh->vertices[vertexIx]);
                    if(!v->tag) {
                        v->tag = true;
                        v->position = v->position - centerOfMass;
                        v->position = v->position.Rotate(rotationAxis, angle);
                        v->position = v->position + centerOfMass;
                    }
                }
            }
        }       */

        return rotated;
    }

    int SSERenderer::selectionObjectCount() {
        int count = 0;
        for(unsigned int i = 0; i < helices.size(); i++) {
            if(helices[i]->isSelected()) {
                count++;
            }
        }

        return count;
    }

    Vec3F SSERenderer::selectionCenterOfMass(){
        Vec3F helixCenterOfMass(0,0,0);
        Vec3F helixCenter;
        int helixCount = 0;

        for(unsigned int i = 0; i < helices.size(); i++) {
            if(helices[i]->isSelected()) {
                helixCount++;
                helixCenter = helices[i]->getCenter();
                helixCenterOfMass = helixCenterOfMass + helixCenter;
            }
        }

        int totalCount = selectionObjectCount();
        int sheetCount = totalCount - helixCount;

        Vec3F sheetsCenterOfMass(0,0,0);
        Vec3F currentFaceCenterOfMass;
        Vec3F currentSheetCenterOfMass;

        Vec3F centerOfMass;
        if ((helixCount > 0) && (sheetCount > 0)) {
            centerOfMass = (helixCenterOfMass + sheetsCenterOfMass) * (1.0f/(float)totalCount);
        } else if (helixCount > 0) {
            centerOfMass = helixCenterOfMass * (1.0f/(float)helixCount);
        } else {
            centerOfMass = sheetsCenterOfMass * (1.0f/(float)sheetCount);
        }
        return centerOfMass;
    }

    bool SSERenderer::selectionMove(Vec3F moveDirection) {
        bool moved = false;
        for(unsigned int i = 0; i < helices.size(); i++) {
            if(helices[i]->isSelected()) {
                helices[i]->setCenter(helices[i]->getCenter() + moveDirection);
                moved = true;
            }
        }
        return moved;
    }

    void SSERenderer::selectionClear() {
        for(unsigned int i = 0; i < helices.size(); i++) {
            helices[i]->setSelected(false);
        }

        selectedHelices.clear();
        selectedPDBHelices.clear();
    }

    void SSERenderer::clearOtherHighlights(){
        selectedPDBHelices.clear();
    }

    void SSERenderer::saveHelixFileVRML(FILE* fout) {
        Shape::writeToFile(this->helices, fout);
    }

    void SSERenderer::selectionToggle(int subsceneIndex, bool forceTrue, int ix0, int ix1, int ix2, int ix3, int ix4) {
        if((subsceneIndex == 0) && (ix0 >= 0) && (ix0 <= (int)helices.size())) {
            if(forceTrue || !helices[ix0]->isSelected())
                selectedHelices.push_back(ix0);
            helices[ix0]->setSelected(forceTrue || !helices[ix0]->isSelected());
        }
    }

    void SSERenderer::saveHelixFileSSE(FILE* fout) {
        Vec3F start, end;
        float helixLength;
        int intLength;

        for(unsigned int i = 0; i < helices.size(); i++) {
            end = helices[i]->getCornerCell3(1);
            start = helices[i]->getCornerCell3(2);
            helixLength = (start-end).length();
            intLength = (int)ceil(helixLength / HELIX_LENGTH_TO_RESIDUE_RATIO);

            fprintf(fout, "ALPHA 'A%d' '%d' '%d' %d %f %f %f %f %f %f\n", i, i*100,i*100+(intLength-1), intLength, start.X(), start.Y(), start.Z(), end.X(), end.Y(), end.Z());
        }
    }

    void SSERenderer::saveHelixFile(string fileName) {
        FILE* fout = fopen((char*)fileName.c_str(), "wt");
        int pos = fileName.rfind(".") + 1;
        string extension = fileName.substr(pos, fileName.length()-pos);

        extension = StringUtils::StringToUpper(extension);

        if(strcmp(extension.c_str(), "WRL") == 0) {
            saveHelixFileVRML(fout);
        } else if(strcmp(extension.c_str(), "VRML") == 0) {
            saveHelixFileVRML(fout);
        } else if(strcmp(extension.c_str(), "SSE") == 0) {
            saveHelixFileSSE(fout);
        }
        fclose(fout);
    }

    string SSERenderer::getSupportedHelixLoadFileFormats() {
        return "All Supported Formats(*.vrml *.wrl *.sse);; VRML models (*.vrml *.wrl);; SSEHunter annotations (*.sse)";
    }

    string SSERenderer::getSupportedSheetLoadFileFormats() {
        return "VRML models (*.vrml *.wrl)";
    }

    string SSERenderer::getSupportedHelixSaveFileFormats() {
        return "All Supported Formats(*.vrml *.wrl *.sse);; VRML models (*.vrml *.wrl);; SSEHunter annotations (*.sse)";
    }

    string SSERenderer::getSupportedSheetSaveFileFormats() {
        return "All Supported Formats(*.vrml *.wrl);; VRML models (*.vrml *.wrl)";
    }

    void SSERenderer::removeHelices() {
        helices.clear();
    }

    void SSERenderer::removeSheets() {
        sheets.clear();
    }

    void SSERenderer::removeSelectedSSEs() {
        vector<Shape*> newHelices;
        for(unsigned int i = 0; i < helices.size(); i++) {
            if(helices[i]->isSelected()) {
                delete helices[i];
            } else {
                newHelices.push_back(helices[i]);
            }
        }
        helices = newHelices;
    }

    int SSERenderer::getHelixCount() {
        return helices.size();
    }

    Vec3F SSERenderer::getHelixCorner(int helixIx, int cornerIx) {
        return helices[helixIx]->getCornerCell3(cornerIx);
    }

    void SSERenderer::setSSEOrientationFlips(vector<bool> in){
        helixFlips = in;
    }

    void SSERenderer::setHelixCorrs(  vector < int > flatCorrespondences){
        if(flatCorrespondences.size() %2 != 0)
            return;
        else
            corrs.clear();
        for(int i=0; i < flatCorrespondences.size(); i = i+2){
            corrs.push_back(make_pair(flatCorrespondences[i], flatCorrespondences[i+1]));
        }
    }

    void SSERenderer::setSelectedPDBHelices(vector<int> indices){
        selectedPDBHelices.clear();
        selectedPDBHelices = indices;
    }

} /* namespace Visualization */
