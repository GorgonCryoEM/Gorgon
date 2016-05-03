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
        helices.clear();
        sheetMesh = NULL;
        graphSheetMesh = NULL;
        sheetCount = 0;
        graphSheetCount = 0;
        selectedHelices.clear();
    }

    SSERenderer::~SSERenderer() {
        for(unsigned int i = 0; i < helices.size(); i++) {
            delete helices[i];
        }
        for(unsigned int i = 0; i < sheets.size(); i++) {
            delete sheets[i];
        }
        if(sheetMesh != NULL) {
            delete sheetMesh;
        }
    }

    vector<int> SSERenderer::GetSelectedHelixIndices(){
        return selectedHelices;
    }

    vector<Shape*> * SSERenderer::GetHelices(){
        return &helices;
    }

    void SSERenderer::AddHelix(Vec3F p1, Vec3F p2) {

        Shape * newHelix = Shape::CreateHelix(p1, p2, 2.5);

        helices.push_back(newHelix);
    }

    void SSERenderer::FinalizeHelix() {
        Vec3F p1, p2;
        LinearSolver::FindBestFitLine(p1, p2, tempSSEPoints);
        AddHelix(p1, p2);
    }

    void SSERenderer::StartNewSSE() {
        tempSSEPoints.clear();
    }

    void SSERenderer::AddSSEPoint(Vec3F p) {
        tempSSEPoints.push_back(p);
    }

    void SSERenderer::Draw(int subSceneIndex, bool selectEnabled) {
        GLfloat emissionColor[4] = {1.0, 1.0, 1.0, 1.0};
        GLfloat frontColor[4] = {1.0, 0.0, 0.0, 1.0};
        GLfloat backColor[4] = {0.0, 0.0, 1.0, 1.0};

        glPushName(subSceneIndex);
        float colorR, colorG, colorB, colorA;
        if(subSceneIndex == 0) {
            if(selectEnabled) {
                glPushName(0);
            }

            vector<int> SSEIndices;

            for(unsigned int i = 0; i < corrs.size(); ++i){
                int SSEIndex = get<0> (corrs[i]);
                for(unsigned int k = 0; k < selectedPDBHelices.size(); ++k){
                    if(selectedPDBHelices[k] == SSEIndex){
                        SSEIndices.push_back( get<1>( corrs[i]) );
                    }
                }
            }

            Point<double> pt;
            for(int i = 0; i < (int)helices.size(); i++) {
                glPushAttrib(GL_LIGHTING_BIT);
                if(isObjectSpecificColoring) {
                    helices[i]->GetColor(colorR, colorG, colorB, colorA);
                    OpenGLUtils::SetColor(colorR, colorG, colorB, colorA);

                }

                if(helices[i]->GetSelected()) {

                    glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
                    glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);

                }
                glPushMatrix();
                glMultMatrixd(helices[i]->GetWorldToObjectMatrix().mat);
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

                if(helices[i]->GetSelected()) {

                    Vec3F corner1 = GetHelixCorner(i, 0);
                    Vec3F corner2 = GetHelixCorner(i, 1);
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
                        Vec3F corner1 = GetHelixCorner(i, 0);
                        Vec3F corner2 = GetHelixCorner(i, 1);
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
        else if((subSceneIndex == 1) && (sheetMesh != NULL)) {

            int k;
            if(selectEnabled) {
                glPushName(0);
            }
            // for color code
            int prevSheet = -1;
            int thisSheet;
            float colorR, colorG, colorB, colorA;
            GLfloat diffuseMaterial[4];
            GLfloat ambientMaterial[4];
            GLfloat specularMaterial[4];
            // end color code
//                }
//                }
            if(selectEnabled) {
                glPopName();
            }
        }
        else if((subSceneIndex == 2) && (graphSheetMesh != NULL)) {
            int k;
            if(selectEnabled) {
                glPushName(0);
            }
            // for color code
            int prevSheet = -1;
            int thisSheet;
            float colorR, colorG, colorB, colorA;
            // end color code
//                }
//                }

            glPushAttrib(GL_LIGHTING_BIT);
            glDisable(GL_LIGHTING);
            OpenGLUtils::SetColor(0.0, 0.0, 0.0, 1.0);
            glPopAttrib();
            // end graph-type sheet rendering code

            if(selectEnabled) {
                glPopName();
            }
        }

        glPopName();
    }


    void SSERenderer::LoadHelixFileSSE(string fileName) {

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
                AddHelix(Vec3F(x1, y1, z1), Vec3F(x2, y2, z2));
            }
        }

        fclose(fin);
    }

    void SSERenderer::LoadHelixFileVRML(string fileName) {
        SkeletonReader::ReadHelixFile((char *)fileName.c_str(), NULL, helices);
    }

    void SSERenderer::LoadHelixFile(string fileName) {

        if(sheetMesh == NULL) {
            Display::load(fileName);
        }
        for(unsigned int i = 0; i < helices.size(); i++) {
            delete helices[i];
        }
        helices.clear();

        int pos = fileName.rfind(".") + 1;
        string extension = fileName.substr(pos, fileName.length()-pos);
        extension = StringUtils::StringToUpper(extension);
        if(strcmp(extension.c_str(), "WRL") == 0) {
            LoadHelixFileVRML(fileName);
        } else if(strcmp(extension.c_str(), "VRML") == 0) {
            LoadHelixFileVRML(fileName);
        } else if(strcmp(extension.c_str(), "SSE") == 0) {
            LoadHelixFileSSE(fileName);
        }
    }

    void SSERenderer::LoadSheetFile(string fileName) {
        if(helices.size() == 0) {
            Display::load(fileName);
        }
        //vector<Shape *> sheets;
        sheets.clear();
        if(sheetMesh != NULL) {
            delete sheetMesh;
        }
        SkeletonReader::ReadSheetFile((char *)fileName.c_str(), sheets);

        SheetListToMesh(sheets);
    }

    void SSERenderer::SheetListToMesh(vector<Shape*> & sheets) {
        if(sheetMesh != NULL) {
            delete sheetMesh;
        }
        sheetMesh = new NonManifoldMesh();

        Vec3D pt;
        vector<int> indices;

        sheetCount = sheets.size();
        for(int i = 0; i <= sheetCount; i++) {
            selectedSheets[i] = false;
        }

        for(unsigned int i = 0; i < sheets.size(); i++) {
            indices.clear();
            for(unsigned int j = 0; j < sheets[i]->polygonPoints.size(); j++) {
                pt = sheets[i]->polygonPoints[j];
                indices.push_back(sheetMesh->addVertex(Vec3F(pt[0], pt[1], pt[2])));
            }

            for(unsigned int j = 0; j < sheets[i]->polygons.size(); j++) {
                Polygon pp = sheets[i]->polygons[j];
                Vec3U ppp;
                ppp[0] = indices[pp.pointIndex1];
                ppp[1] = indices[pp.pointIndex2];
                ppp[2] = indices[pp.pointIndex3];
                sheetMesh->addFace(ppp);
            }
        }
        indices.clear();
    }

    void SSERenderer::Unload() {
        for(unsigned int i = 0; i < helices.size(); i++) {
            delete helices[i];
        }
        helices.clear();
        for(unsigned int i = 0; i < sheets.size(); i++) {
            delete sheets[i];
        }
        sheets.clear();
        if(sheetMesh != NULL) {
            delete sheetMesh;
        }
        sheetMesh = NULL;
        if(graphSheetMesh != NULL) {
            delete graphSheetMesh;
        }
        graphSheetMesh = NULL;
    }

    void SSERenderer::UnloadGraphSSEs() {
        graphSheetCount = 0;
        if(graphSheetMesh != NULL) {
            delete graphSheetMesh;
        }
        graphSheetMesh = NULL;

    }

    bool SSERenderer::SelectionRotate(Vec3F centerOfMass, Vec3F rotationAxis, float angle) {
        bool rotated = false;
        Vec3D centerOfMassP3(centerOfMass.X(), centerOfMass.Y(), centerOfMass.Z());
        Vec3D rotationV3(rotationAxis.X(), rotationAxis.Y(), rotationAxis.Z());

        for(unsigned int i = 0; i < helices.size(); i++) {
//            if(helices[i]->GetSelected()) {
                rotated = true;
                Vec3D move = centerOfMassP3 - helices[i]->GetCenter();
                Matrix4 rotMatrix = Matrix4::rotation(rotationV3, angle);
                Vec3D newMove = rotMatrix * move;
                helices[i]->SetCenter(centerOfMassP3 - newMove);

                helices[i]->Rotate(rotationV3, angle);
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

    void SSERenderer::ClearOtherHighlights(){
        selectedPDBHelices.clear();
    }

    void SSERenderer::SaveHelixFileVRML(FILE* fout) {
        Shape::WriteToFile(this->helices, fout);
    }

    void SSERenderer::SaveHelixFileSSE(FILE* fout) {
        Vec3F start, end;
        float helixLength;
        int intLength;

        for(unsigned int i = 0; i < helices.size(); i++) {
            end = helices[i]->GetCornerCell3(1);
            start = helices[i]->GetCornerCell3(2);
            helixLength = (start-end).length();
            intLength = (int)ceil(helixLength / HELIX_LENGTH_TO_RESIDUE_RATIO);

            fprintf(fout, "ALPHA 'A%d' '%d' '%d' %d %f %f %f %f %f %f\n", i, i*100,i*100+(intLength-1), intLength, start.X(), start.Y(), start.Z(), end.X(), end.Y(), end.Z());
        }
    }

    void SSERenderer::SaveHelixFile(string fileName) {
        FILE* fout = fopen((char*)fileName.c_str(), "wt");
        int pos = fileName.rfind(".") + 1;
        string extension = fileName.substr(pos, fileName.length()-pos);

        extension = StringUtils::StringToUpper(extension);

        if(strcmp(extension.c_str(), "WRL") == 0) {
            SaveHelixFileVRML(fout);
        } else if(strcmp(extension.c_str(), "VRML") == 0) {
            SaveHelixFileVRML(fout);
        } else if(strcmp(extension.c_str(), "SSE") == 0) {
            SaveHelixFileSSE(fout);
        }
        fclose(fout);
    }

    string SSERenderer::GetSupportedHelixLoadFileFormats() {
        return "All Supported Formats(*.vrml *.wrl *.sse);; VRML models (*.vrml *.wrl);; SSEHunter annotations (*.sse)";
    }

    string SSERenderer::GetSupportedSheetLoadFileFormats() {
        return "VRML models (*.vrml *.wrl)";
    }

    string SSERenderer::GetSupportedHelixSaveFileFormats() {
        return "All Supported Formats(*.vrml *.wrl *.sse);; VRML models (*.vrml *.wrl);; SSEHunter annotations (*.sse)";
    }

    string SSERenderer::GetSupportedSheetSaveFileFormats() {
        return "All Supported Formats(*.vrml *.wrl);; VRML models (*.vrml *.wrl)";
    }

    void SSERenderer::RemoveHelices() {
        helices.clear();
    }

    void SSERenderer::RemoveSheets() {
        sheets.clear();
    }

    int SSERenderer::GetHelixCount() {
        return helices.size();
    }

    Vec3F SSERenderer::GetHelixCorner(int helixIx, int cornerIx) {
        return helices[helixIx]->GetCornerCell3(cornerIx);
    }

    void SSERenderer::SetSSEOrientationFlips(vector<bool> in){
        helixFlips = in;
    }


    void SSERenderer::SetHelixCorrs(  vector < int > flatCorrespondences){
        if(flatCorrespondences.size() %2 != 0)
            return;
        else
            corrs.clear();
        for(int i=0; i < flatCorrespondences.size(); i = i+2){
            corrs.push_back(boost::tuple<int, int>(flatCorrespondences[i], flatCorrespondences[i+1]));
        }
    }
    void SSERenderer::SetSelectedPDBHelices(vector<int> indices){
        selectedPDBHelices.clear();
        selectedPDBHelices = indices;
    }

} /* namespace Visualization */
