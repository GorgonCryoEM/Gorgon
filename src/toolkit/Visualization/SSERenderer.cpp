/*
 * SSERenderer.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "SSERenderer.h"
#include "Shapes.h"
#include "Foundation/StringUtils.h"

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
//                if(isObjectSpecificColoring) {
//                    helices[i]->GetColor(colorR, colorG, colorB, colorA);
//                    OpenGLUtils::SetColor(colorR, colorG, colorB, colorA);

//                }

//                if(helices[i]->GetSelected()) {

                    glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
                    glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);

//                }
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

//                if(helices[i]->GetSelected()) {

                    Vec3F corner1 = GetHelixCorner(i, 0);
                    Vec3F corner2 = GetHelixCorner(i, 1);
                    cout << "Drawing selected cylinder. Size of helix flips is " << helixFlips.size() << endl;
                    if(helixFlips.size()  > 0){
                        if(!helixFlips[i]){

//                            OpenGLUtils::SetColor(1.0, 0.0, 0.0, 1.0);
                            drawSphere(corner2, 1.0);
//                            OpenGLUtils::SetColor(0.0, 0.0, 1.0, 1.0);
                            drawSphere(corner1, 1.0);
                            fflush(stdout);
                        }else{
//                            OpenGLUtils::SetColor(1.0, 0.0, 0.0, 1.0);
                            drawSphere(corner1, 1.0);
//                            OpenGLUtils::SetColor(0.0, 0.0, 1.0, 1.0);
                            drawSphere(corner2, 1.0);
                            fflush(stdout);
                        }
                    }
//                }


                for(unsigned int j = 0; j < SSEIndices.size(); ++j){
                    if(SSEIndices[j] == i){
                        Vec3F corner1 = GetHelixCorner(i, 0);
                        Vec3F corner2 = GetHelixCorner(i, 1);
                        if(!helixFlips[i]){
//                            OpenGLUtils::SetColor(1.0, 0.0, 0.0, 1.0);
                            drawSphere(corner2, 1.0);
//                            OpenGLUtils::SetColor(0.0, 0.0, 1.0, 1.0);
                            drawSphere(corner1, 1.0);
                            fflush(stdout);
                        }else{
//                            OpenGLUtils::SetColor(1.0, 0.0, 0.0, 1.0);
                            drawSphere(corner1, 1.0);
//                            OpenGLUtils::SetColor(0.0, 0.0, 1.0, 1.0);
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
            for(unsigned int i = 0; i < sheetMesh->faces.size(); i++) {

                glPushAttrib(GL_LIGHTING_BIT);
//                if(sheetMesh->faces[i].tag.selected) {
                    glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
                    glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
//                }
                if(selectEnabled) {
                    glLoadName(i);
                }

                // color code
//                if(sheetMesh->faces[i].tag.id != prevSheet) {
//                    thisSheet = (int) (sheetMesh->faces[i].tag.id);
//                    sheets[thisSheet-1]->GetColor(colorR, colorG, colorB, colorA);
//                    prevSheet = thisSheet;
//                }
//                if(isObjectSpecificColoring) {
//                    OpenGLUtils::SetColor(colorR, colorG, colorB, colorA);
//                }

                glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
                // end color code
                glBegin(GL_POLYGON);
                Vec3F normal;
                for(unsigned int j = 0; j < sheetMesh->faces[i].vertexIds.size(); j++) {
                    normal = sheetMesh->getVertexNormal(sheetMesh->faces[i].vertex(j));
                    k = sheetMesh->GetVertexIndex(sheetMesh->faces[i].vertexIds[j]);
                    glNormal3f(normal.X(), normal.Y(), normal.Z());
                    glVertex3fv(sheetMesh->vertices[k].position.values);
                }
                glEnd();
                glPopAttrib();
                glPopAttrib(); // for color code
            }
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
            for(unsigned int i = 0; i < graphSheetMesh->faces.size(); i++) {
                glPushAttrib(GL_LIGHTING_BIT);
                glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
                //glDisable(GL_LIGHTING);
//                if(graphSheetMesh->faces[i].tag.selected) {
                    glMaterialfv(GL_FRONT, GL_EMISSION, emissionColor);
                    glMaterialfv(GL_BACK, GL_EMISSION, emissionColor);
//                }
                if(selectEnabled) {
                    glLoadName(i);
                }

                // color code
//                if(graphSheetMesh->faces[i].tag.id != prevSheet) {
//                    //cout << "picking graph sheet color. i=" << i << ", id=" << (int) (graphSheetMesh->faces[i].tag.id) << endl;
//                    thisSheet = (int) (graphSheetMesh->faces[i].tag.id);
//                    sheets[thisSheet-1]->GetColor(colorR, colorG, colorB, colorA); // probably gets the wrong color.
//                    prevSheet = thisSheet;
//                }

//                if(isObjectSpecificColoring) {
//                    OpenGLUtils::SetColor(colorR, colorG, colorB, colorA);
//                }

                glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
                // end color code
                glBegin(GL_POLYGON);
                Vec3F normal;
                for(unsigned int j = 0; j < graphSheetMesh->faces[i].vertexIds.size(); j++) {
                    normal = graphSheetMesh->getFaceNormal(i);
                    k = graphSheetMesh->GetVertexIndex(graphSheetMesh->faces[i].vertexIds[j]);
                    glNormal3f(normal.X(), normal.Y(), normal.Z());
                    glVertex3fv(graphSheetMesh->vertices[k].position.values);
                }
                glEnd();
                glPopAttrib();
                glPopAttrib(); // for color code
            }

            glPushAttrib(GL_LIGHTING_BIT);
            glDisable(GL_LIGHTING);
//            OpenGLUtils::SetColor(0.0, 0.0, 0.0, 1.0);
            for(unsigned int i = 0; i < graphSheetMesh->faces.size(); i++) {
                glBegin(GL_LINE_LOOP);
                for(unsigned int j = 0; j < graphSheetMesh->faces[i].vertexIds.size(); j++) {
                    k = graphSheetMesh->GetVertexIndex(graphSheetMesh->faces[i].vertexIds[j]);
                    glVertex3fv(graphSheetMesh->vertices[k].position.values);
                }
                glEnd();
            }
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

        Point3 pt;
        vector<int> indices;
        SheetIdsAndSelect faceTag;
        faceTag.selected = false;

        sheetCount = sheets.size();
        for(int i = 0; i <= sheetCount; i++) {
            selectedSheets[i] = false;
        }

        for(unsigned int i = 0; i < sheets.size(); i++) {
            indices.clear();
            for(unsigned int j = 0; j < sheets[i]->polygonPoints.size(); j++) {
                pt = sheets[i]->polygonPoints[j];
                indices.push_back(sheetMesh->AddVertex(Vec3F((float)pt[0], (float)pt[1], (float)pt[2])));
            }

            for(unsigned int j = 0; j < sheets[i]->polygons.size(); j++) {
                faceTag.id = i+1;
                sheetMesh->AddTriangle(indices[sheets[i]->polygons[j].pointIndex1], indices[sheets[i]->polygons[j].pointIndex2], indices[sheets[i]->polygons[j].pointIndex3], NULL, faceTag);
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

    void SSERenderer::LoadGraphSSE(int index, Shape* sse, float offsetx, float offsety, float offsetz, float scalex, float scaley, float scalez) {
        // make a volume from the internal cells
        int xmin=MAXINT, xmax=-MAXINT, ymin=MAXINT, ymax=-MAXINT, zmin=MAXINT, zmax=-MAXINT;
        for (unsigned int i = 0; i < sse->internalCells.size(); i++) {
            if (sse->internalCells[i].x < xmin) {xmin = sse->internalCells[i].x;}
            if (sse->internalCells[i].x > xmax) {xmax = sse->internalCells[i].x;}
            if (sse->internalCells[i].y < ymin) {ymin = sse->internalCells[i].y;}
            if (sse->internalCells[i].y > ymax) {ymax = sse->internalCells[i].y;}
            if (sse->internalCells[i].z < zmin) {zmin = sse->internalCells[i].z;}
            if (sse->internalCells[i].z > zmax) {zmax = sse->internalCells[i].z;}
        }
        int dimx = xmax-xmin+1;
        int dimy = ymax-ymin+1;
        int dimz = zmax-zmin+1;
        Volume* vol = new Volume( dimx, dimy, dimz) ;
        vol->setOrigin(xmin, ymin, zmin);
        for (unsigned int i = 0; i < sse->internalCells.size(); i++) {
            vol->setDataAt( sse->internalCells[i].x-xmin, sse->internalCells[i].y-ymin, sse->internalCells[i].z-zmin, 1.0 ) ;
        }

        // make a mesh from the volume
        NonManifoldMesh_Annotated * thisSheetMesh = new NonManifoldMesh_Annotated(vol);
        delete vol;
        // add offset to all points in new mesh
        for (unsigned int i = 0; i < thisSheetMesh->vertices.size(); i++) {
            Vec3F newpos = thisSheetMesh->vertices[i].position;
            newpos.values[0] *= scalex;
            newpos.values[1] *= scaley;
            newpos.values[2] *= scalez;
            thisSheetMesh->vertices[i].position = newpos + Vec3F(xmin * scalex, ymin * scaley, zmin * scalez) + Vec3F(offsetx, offsety, offsetz);
        }

        // merge this mesh with the mesh containing other sheets
        if(graphSheetMesh == NULL) {
            graphSheetMesh = new NonManifoldMesh();
            graphSheetCount = 0;
        }
        if (thisSheetMesh->faceCount > 0) {
            // mesh merging code. not using code NonManifoldMesh::mergeMesh because it doesn't allow setting of tags
            graphSheetCount++;
            SheetIdsAndSelect faceTag = SheetIdsAndSelect();
            faceTag.id = graphSheetCount;
            selectedGraphSheets[index - helices.size() + 1] = false;
            faceTag.selected = false;
            vector<int> indices;
            indices.clear();
            for(unsigned int i = 0; i < thisSheetMesh->vertices.size(); i++) {
                indices.push_back(graphSheetMesh->AddVertex(thisSheetMesh->vertices[i]));
            }
            for(unsigned int i = 0; i < thisSheetMesh->edges.size(); i++) {
                graphSheetMesh->AddEdge(indices[thisSheetMesh->edges[i].vertexIds[0]], indices[thisSheetMesh->edges[i].vertexIds[1]], thisSheetMesh->edges[i].tag);
            }
            for(unsigned int i = 0; i < thisSheetMesh->faces.size(); i++) {
                if(thisSheetMesh->faces[i].vertexIds.size() == 3) {
                    graphSheetMesh->AddTriangle(indices[thisSheetMesh->faces[i].vertexIds[0]], indices[thisSheetMesh->faces[i].vertexIds[1]], indices[thisSheetMesh->faces[i].vertexIds[2]], NULL, faceTag);
                }
            }
            // end mesh merging
        }

        delete thisSheetMesh;
        return;
    }

    void SSERenderer::UnloadGraphSSEs() {
        graphSheetCount = 0;
        if(graphSheetMesh != NULL) {
            delete graphSheetMesh;
        }
        graphSheetMesh = NULL;

    }

    void SSERenderer::SetHelixColor(int index, float r, float g, float b, float a) {
        helices[index]->SetColor(r, g, b, a);
    }

    void SSERenderer::SetSheetColor(int index, float r, float g, float b, float a) {
        sheets[index]->SetColor(r, g, b, a);
    }

    // set the color of an SSE. assumes that SSEs are indexed with helices first and sheets second.
    void SSERenderer::SetSSEColor(int index, float r, float g, float b, float a) {
        int numHelices = helices.size();
        if (index < numHelices) {
            helices[index]->SetColor(r, g, b, a);
        } else {
            sheets[index - numHelices]->SetColor(r, g, b, a);
        }
    }

    bool SSERenderer::SelectionRotate(Vec3F centerOfMass, Vec3F rotationAxis, float angle) {
        bool rotated = false;
        Point3 centerOfMassP3 = Point3(centerOfMass.X(), centerOfMass.Y(), centerOfMass.Z());
        Vector3 rotationV3 = Vector3(rotationAxis.X(), rotationAxis.Y(), rotationAxis.Z());

        for(unsigned int i = 0; i < helices.size(); i++) {
            if(helices[i]->GetSelected()) {
                rotated = true;
                Vector3 move = centerOfMassP3 - helices[i]->GetCenter();
                Matrix4 rotMatrix = Matrix4::rotation(rotationV3, angle);
                Vector3 newMove = rotMatrix * move;
                helices[i]->SetCenter(centerOfMassP3 - newMove);

                helices[i]->Rotate(Vector3(rotationAxis.X(), rotationAxis.Y(), rotationAxis.Z()), angle);
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

    int SSERenderer::SelectionObjectCount() {
        int count = 0;
        for(unsigned int i = 0; i < helices.size(); i++) {
            if(helices[i]->GetSelected()) {
                count++;
            }
        }

        if(sheetMesh != NULL) {
            for(int i = 0; i <= sheetCount; i++) {
                if(selectedSheets[i]) {
                    count++;
                }
            }
        }

        if(graphSheetMesh != NULL) {
            for(int i = 0; i <= graphSheetCount; i++) {
                if(selectedGraphSheets[i]) {
                    count++;
                }
            }
        }

        return count;
    }

    Vec3F SSERenderer::SelectionCenterOfMass(){
        Vec3F helixCenterOfMass = Vec3F(0,0,0);
        Point3 helixCenter;
        int helixCount = 0;

        for(unsigned int i = 0; i < helices.size(); i++) {
            if(helices[i]->GetSelected()) {
                helixCount++;
                helixCenter = helices[i]->GetCenter();
                helixCenterOfMass = helixCenterOfMass + Vec3F(helixCenter[0], helixCenter[1], helixCenter[2]);
            }
        }

        int totalCount = SelectionObjectCount();
        int sheetCount = totalCount - helixCount;

        Vec3F sheetsCenterOfMass = Vec3F(0,0,0);
        Vec3F currentFaceCenterOfMass;
        Vec3F currentSheetCenterOfMass;

        if((sheetCount > 0) && (sheetMesh != NULL)) {


            int currentSheetFaceCount;

            for(int j = 0; j <= this->sheetCount; j++) {
                if(selectedSheets[j]) {
                    currentSheetCenterOfMass = Vec3F(0,0,0);
                    currentSheetFaceCount = 0;
                    for(unsigned int i = 0; i < sheetMesh->faces.size(); i++) {
                        if(sheetMesh->faces[i].tag.id == j) {
                            currentSheetFaceCount++;
                            currentFaceCenterOfMass = Vec3F(0,0,0);
                            for(unsigned int k = 0; k < sheetMesh->faces[i].vertexIds.size(); k++) {
                                currentFaceCenterOfMass = currentFaceCenterOfMass + sheetMesh->vertices[sheetMesh->faces[i].vertexIds[k]].position;
                            }
                            currentFaceCenterOfMass = currentFaceCenterOfMass * (1.0f / (float)sheetMesh->faces[i].vertexIds.size());
                            currentSheetCenterOfMass = currentSheetCenterOfMass + currentFaceCenterOfMass;
                        }
                    }

                    currentSheetCenterOfMass = currentSheetCenterOfMass * (1.0f / (float)currentSheetFaceCount);
                }
                sheetsCenterOfMass = sheetsCenterOfMass + currentSheetCenterOfMass;
            }
        }

        Vec3F centerOfMass;
        if(totalCount == 0) {
            centerOfMass = Renderer::SelectionCenterOfMass();
        } else if ((helixCount > 0) && (sheetCount > 0)) {
            centerOfMass = (helixCenterOfMass + sheetsCenterOfMass) * (1.0f/(float)totalCount);
        } else if (helixCount > 0) {
            centerOfMass = helixCenterOfMass * (1.0f/(float)helixCount);
        } else {
            centerOfMass = sheetsCenterOfMass * (1.0f/(float)sheetCount);
        }
        return centerOfMass;
    }

    bool SSERenderer::SelectionMove(Vec3F moveDirection) {
        bool moved = false;
        for(unsigned int i = 0; i < helices.size(); i++) {
            if(helices[i]->GetSelected()) {
                helices[i]->SetCenter(helices[i]->GetCenter() + Vector3(moveDirection.X(), moveDirection.Y(), moveDirection.Z()));
                moved = true;
            }
        }
        if(sheetMesh != NULL) {
            for(unsigned int i=0; i < sheetMesh->vertices.size(); i++) {
                sheetMesh->vertices[i].tag = false;
            }


            for(unsigned int i = 0; i < sheetMesh->faces.size(); i++) {
                if(sheetMesh->faces[i].tag.selected) {
                    for(unsigned int j = 0; j < sheetMesh->faces[i].vertexIds.size(); j++) {
                        NonManifoldMeshVertex<bool> * v = &(sheetMesh->vertices[sheetMesh->faces[i].vertexIds[j]]);
                        if(!v->tag) {
                            v->position = v->position + moveDirection;
                            moved = true;
                            v->tag = true;
                        }
                    }
                }
            }
        }
        UpdateBoundingBox();
        return moved;
    }

    bool SSERenderer::SelectionClear() {
        if(Renderer::SelectionClear()) {

            for(unsigned int i = 0; i < helices.size(); i++) {
                helices[i]->SetSelected(false);
            }

            if(sheetMesh != NULL) {
                for(unsigned int i = 0; i < sheetMesh->faces.size(); i++) {
                    sheetMesh->faces[i].tag.selected = false;
                }
                for(int i = 0; i <= sheetCount; i++) {
                    selectedSheets[i] = false;
                }
            }

            if(graphSheetMesh != NULL) {
                for(unsigned int i = 0; i < graphSheetMesh->faces.size(); i++) {
                    graphSheetMesh->faces[i].tag.selected = false;
                }
                for(int i = 0; i <= graphSheetCount; i++) {
                    selectedGraphSheets[i] = false;
                }
            }
            selectedHelices.clear();
            selectedPDBHelices.clear();
            return true;
        }
        return false;
    }

    void SSERenderer::ClearOtherHighlights(){
        selectedPDBHelices.clear();
    }

    void SSERenderer::SelectionToggle(int subsceneIndex, bool forceTrue, int ix0, int ix1, int ix2, int ix3, int ix4) {
        Renderer::SelectionToggle(subsceneIndex, forceTrue, ix0, ix1, ix2, ix3, ix4);
        if((subsceneIndex == 0) && (ix0 >= 0) && (ix0 <= (int)helices.size())) {
            if(forceTrue || !helices[ix0]->GetSelected())
                selectedHelices.push_back(ix0);
            helices[ix0]->SetSelected(forceTrue || !helices[ix0]->GetSelected());
        }

        if(subsceneIndex == 1) {
            for(unsigned int i = 0; i < sheetMesh->faces.size(); i++) {
                if(sheetMesh->faces[i].tag.id == ix0) {
                    sheetMesh->faces[i].tag.selected = forceTrue || !sheetMesh->faces[i].tag.selected;
                }
            }

            selectedSheets[ix0] = forceTrue || !selectedSheets[ix0];
            sheets[ix0-1]->SetSelected(selectedSheets[ix0]);
        }
        if(subsceneIndex == 2) {
            for(unsigned int i = 0; i < graphSheetMesh->faces.size(); i++) {
                if(graphSheetMesh->faces[i].tag.id == ix0) {
                    graphSheetMesh->faces[i].tag.selected = forceTrue || !graphSheetMesh->faces[i].tag.selected;
                }
            }

            selectedGraphSheets[ix0] = forceTrue || !selectedGraphSheets[ix0];
        }
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
            helixLength = (start-end).Length();
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

    void SSERenderer::SaveSheetFileVRML(FILE* fout) {

        map<int, int> vertexIxs;
        vector<Vec3F> vertices;
        bool addVertex;

        NonManifoldMeshVertex<bool> tempVertex;
        NonManifoldMeshEdge<bool> tempEdge;
        NonManifoldMeshFace<SheetIdsAndSelect> tempFace;

        fprintf(fout, "#VRML V2.0 utf8\n");

        for(int i = 0; i < sheetCount; i++) {
            vertexIxs.clear();
            vertices.clear();

            for(unsigned int j = 0; j < sheetMesh->vertices.size(); j++) {
                tempVertex = sheetMesh->vertices[j];
                addVertex = false;

                for(unsigned int k = 0; k < tempVertex.edgeIds.size(); k++) {
                    tempEdge = sheetMesh->edges[sheetMesh->GetEdgeIndex(tempVertex.edgeIds[k])];

                    for(unsigned int l = 0; l < tempEdge.faceIds.size(); l++) {
                        tempFace = sheetMesh->faces[sheetMesh->GetFaceIndex(tempEdge.faceIds[l])];
                        if(tempFace.tag.id == i+1) {
                            addVertex = true;
                        }
                    }

                }

                if(addVertex) {
                    vertexIxs[j] = vertices.size();
                    vertices.push_back(tempVertex.position);
                }
            }

            fprintf(fout, "Shape {\n");
            fprintf(fout, "    appearance Appearance {\n");
            fprintf(fout, "        material Material {\n");
            fprintf(fout, "            emissiveColor 0.0 0.0 0.5\n");
            fprintf(fout, "        }\n");
            fprintf(fout, "    }\n");
            fprintf(fout, "    geometry IndexedFaceSet {\n");
            fprintf(fout, "       coord Coordinate {\n");
            fprintf(fout, "            point [\n");

            for(unsigned int j = 0; j < vertices.size(); j++) {
                fprintf(fout, "                 %f %f %f,\n", vertices[j].X(), vertices[j].Y(), vertices[j].Z());
            }

            fprintf(fout, "            ]\n");
            fprintf(fout, "        }\n");
            fprintf(fout, "        coordIndex [\n");

            int vertexIx, startVertexIx;;
            for(unsigned int j = 0; j < sheetMesh->faces.size(); j++) {
                tempFace = sheetMesh->faces[j];
                if(tempFace.tag.id == i+1) {
                    fprintf(fout, "            ");

                    for(unsigned int k = 0; k < tempFace.vertexIds.size(); k++) {
                        vertexIx = sheetMesh->GetVertexIndex(tempFace.vertexIds[k]);
                        if(k == 0) {
                            startVertexIx = vertexIx;
                        }
                        fprintf(fout, "%d,", vertexIxs[vertexIx]);
                    }

                    if(tempFace.vertexIds.size() > 0) {
                        fprintf(fout, "%d\n", vertexIxs[startVertexIx]);
                    } else {
                        fprintf(fout, "\n");
                    }
                }

            }

            fprintf(fout, "        ]\n");
            fprintf(fout, "    }\n");
            fprintf(fout, "}\n");
        }

    }

    void SSERenderer::SaveSheetFile(string fileName) {
        FILE* fout = fopen((char*)fileName.c_str(), "wt");
        int pos = fileName.rfind(".") + 1;
        string extension = fileName.substr(pos, fileName.length()-pos);

        extension = StringUtils::StringToUpper(extension);

        if(strcmp(extension.c_str(), "WRL") == 0) {
            SaveSheetFileVRML(fout);
        } else if(strcmp(extension.c_str(), "VRML") == 0) {
            SaveSheetFileVRML(fout);
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

    Vec3F SSERenderer::Get3DCoordinates(int subsceneIndex, int ix0, int ix1, int ix2, int ix3, int ix4) {
        Vec3F position;
        switch(subsceneIndex) {
            case(0):
                if((ix0 >= 0) && (ix0 <= (int)helices.size())) {
                    Point3 pt = helices[ix0]->GetCenter();
                    position = Vec3F(pt[0], pt[1], pt[2]);
                }
                break;
            case(1):
                position = Vec3F(0,0,0);
                for(unsigned int i = 0; i < sheetMesh->faces.size(); i++) {
                    if(sheetMesh->faces[i].tag.id == ix0) {
                        for(unsigned int j = 0; j < sheetMesh->faces[i].vertexIds.size(); j++) {
                            position += sheetMesh->vertices[sheetMesh->faces[i].vertexIds[j]].position;
                        }
                    }
                }
                position = position * (1.0/(sheetMesh->faces.size() * 3.0));
                break;
            default:
                position = Vec3F(0,0,0);
                break;
        }
        return position;
    }

    void SSERenderer::FitSelectedSSEs(Volume * vol) {
        SSEFlexibleFitter * fitter = new SSEFlexibleFitter(vol);

        const double discretizationStep = 0.01;


        for(unsigned int i = 0; i < helices.size(); i++) {
            if(helices[i]->GetSelected()) {
                fitter->FitHelix(helices[i], 0.005, 1.0/360.0, discretizationStep, 200);
            }
        }

        /*if(sheetMesh != NULL) {
            for(unsigned int i = 0; i <= sheetCount; i++) {
                if(selectedSheets[i]) {
                    fitter->FitSheet(i, sheetMesh, 0.005, 1.0/360.0, 0.1, 200);
                }
            }
        } */

        delete fitter;
        UpdateBoundingBox();
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
