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

            for(int i = 0; i < (int)helices.size(); i++) {
                glPushAttrib(GL_LIGHTING_BIT);
//                if(helices[i]->isObjectSpecificColoring) {
                    float colorR, colorG, colorB, colorA;
                    helices[i]->getColor(colorR, colorG, colorB, colorA);
                    OpenGLUtils::SetColor(colorR, colorG, colorB, colorA);
//                }

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
    }

    void SSERenderer::loadHelixFile(string fileName) {
        for(unsigned int i = 0; i < helices.size(); i++) {
            delete helices[i];
        }
//        #ifdef GORGON_DEBUG
              cout<<"\033[32mDEBUG: File:   SSERenderer.cpp"<<endl;
              cout<<"DEBUG: Method: SSERenderer::loadHelixFile(string)\033[0m"<<endl;
              cout<<"YAY"<<endl;
//        #endif

        helices.clear();

        int pos = fileName.rfind(".") + 1;
        string extension = fileName.substr(pos, fileName.length()-pos);
        extension = StringUtils::StringToUpper(extension);
        if(strcmp(extension.c_str(), "WRL") == 0) {
            loadHelixFileVRML(fileName);
        } else if(strcmp(extension.c_str(), "VRML") == 0) {
            loadHelixFileVRML(fileName);
        } else if(strcmp(extension.c_str(), "SSE") == 0) {
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
                Polygon pp = sheets[i]->polygons[j];
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
//            if(helices[i]->GetSelected()) {
                rotated = true;
                Vec3D move = centerOfMass - helices[i]->getCenter();
                Matrix4 rotMatrix = Matrix4::rotation(rotationAxis, angle);
                Vec3D newMove = rotMatrix * move;
                helices[i]->setCenter(centerOfMass - newMove);

                helices[i]->rotate(rotationAxis, angle);
//            }
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

    void SSERenderer::clearOtherHighlights(){
        selectedPDBHelices.clear();
    }

    void SSERenderer::saveHelixFileVRML(FILE* fout) {
        Shape::writeToFile(this->helices, fout);
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
