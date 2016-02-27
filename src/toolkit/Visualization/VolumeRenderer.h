/*
 * VolumeRenderer.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */


#ifndef SRC_TOOLKIT_VISUALIZATION_VOLUMERENDERER_H_
#define SRC_TOOLKIT_VISUALIZATION_VOLUMERENDERER_H_

#define GL_GLEXT_PROTOTYPES
//#define USE_OCTREE_OPTIMIZATION

//#include <iostream>
//#include <GorgonGL.h>
//#include <string>
//#include <GraphMatch/VectorMath.h>
#include "Renderer.h"
//#include "GlobalConstants.h"
//#include "MeshRenderer.h"
#include <Core/volume.h>
//#include <GraySkeletonCPP/GlobalDefinitions.h>
//#include <GraySkeletonCPP/VolumeSkeletonizer.h>
//#include <GraySkeletonCPP/VolumeFormatConverter.h>
//#include <ProteinMorph/NonManifoldMesh.h>
#include "TriangleMesh.h"
#include "MathTools/Vector3.h"
#include "MathTools/Dim3D.h"
#include "Octree.h"
//#include <queue>
#include <Readers/reader.h>
#include <algorithm>
#include "Mesh.h"

using namespace std;

//using namespace wustl_mm::GraySkeletonCPP;
//using namespace wustl_mm::Protein_Morph;
using namespace MathTools;
using namespace GraphMatch;
using namespace SkeletonMaker;
using namespace Core;

namespace Visualization {
    const int VIEWING_TYPE_ISO_SURFACE = 0;
    const int VIEWING_TYPE_CROSS_SECTION = 1;
    const int VIEWING_TYPE_SOLID = 2;
    #ifndef _WIN32
        typedef int PFNGLTEXIMAGE3DPROC;
    #endif

    typedef Octree<Range> VolumeRendererOctreeType;
    typedef OctreeNode<Range> VolumeRendererOctreeNodeType;
    typedef TriangleMesh VolumeSurfaceMeshType;

    class VolumeRenderer : public Volume, public Renderer {
    public:
        VolumeRenderer();
        ~VolumeRenderer();

            float getMaxDensity();
            float getMinDensity();
            float getSurfaceValue() const;
            int getSampleInterval() const;
            string getSupportedLoadFileFormats();
            string getSupportedSaveFileFormats();
            void enableDraw(bool enable);
            void draw(int subSceneIndex, bool selectEnabled);
            void load(string fileName);
            void save(string fileName);
            void setDisplayRadius(const int radius);
            void setDisplayRadiusOrigin(float radiusOriginX,
                                        float radiusOriginY,
                                        float radiusOriginZ);
            void useDisplayRadius(bool useRadius);
            void setViewingType(const int type);
            void setSampleInterval(const int size);
            void setSurfaceValue(const float value);
            void setMaxSurfaceValue(const float value);
            bool setCuttingPlane(float position, float vecX, float vecY, float vecZ);
            void updateBoundingBox();
            void unload();

    private:
            bool calculateSurface();
            bool calculateCuttingSurface();
            bool calculateSolidRendering();
            bool calculateDisplay();
            void load3DTextureSolidRendering();
            void load3DTextureCrossSection();
            void initializeOctree();
            void initializeOctreeTag(VolumeRendererOctreeNodeType * node);
            void calculateOctreeNode(VolumeRendererOctreeNodeType * node);
            void MarchingCube(Volume * vol, Mesh * mesh,
                              const float iso_level, int iX, int iY, int iZ,
                              int iScale);
            int smallest2ndPower(int value);
    private:
        int marchingCubeCallCount;
        bool drawEnabled;
        bool textureLoaded;
        Dim3D<int> textureSize;
        GLuint textureName;
        float surfaceValue;
        float maxSurfaceValue;
        int sampleInterval;
        int displayRadius;
        int viewingType;
        Volume cuttingVolume;
        Vec3F radiusOrigin;
        bool _useDisplayRadius;

        VolumeSurfaceMeshType * surfaceMesh;
        NonManifoldMesh * cuttingMesh;
        VolumeRendererOctreeType * octree;
        #ifdef _WIN32
            PFNGLTEXIMAGE3DPROC glTexImage3D;
        #endif
    };

    VolumeRenderer::VolumeRenderer()
        : Renderer(),
        cuttingVolume(Volume(2, 2, 2))
    {
        textureLoaded = false;
        _useDisplayRadius = false;
        viewingType = VIEWING_TYPE_ISO_SURFACE;
        surfaceMesh = new VolumeSurfaceMeshType();
        octree = NULL;
        surfaceValue = 1.5;
        displayRadius = 1;
        sampleInterval = 1;
        cuttingMesh = new NonManifoldMesh();
        drawEnabled = false;
    }

    VolumeRenderer::~VolumeRenderer() {
        if(textureLoaded) {
            glDeleteTextures(1, &textureName);
            textureLoaded = false;
        }
        delete surfaceMesh;
        if(octree != NULL) {
            delete octree;
        }
        delete cuttingMesh;
    }

    float VolumeRenderer::getMaxDensity(){
        return Volume::getMax();
    }

    float VolumeRenderer::getMinDensity() {
        return Volume::getMin();
    }

    float VolumeRenderer::getSurfaceValue() const {
        return surfaceValue;
    }

    int VolumeRenderer::getSampleInterval() const  {
        return sampleInterval;
    }

    int VolumeRenderer::smallest2ndPower(int value) {
        int power = 1;
        while (power < value) {
            power = power * 2;
        }
        return power;
    }

    string VolumeRenderer::getSupportedLoadFileFormats() {
        return "All Files (*.mrc *.ccp4 *.map *.raw *.pts);; Volumes (*.mrc *.ccp4 *.map *.raw);;Point Cloud (*.pts)";
    }

    string VolumeRenderer::getSupportedSaveFileFormats() {
        return "Volumes (*.mrc *.ccp4 *.map *.raw);;Mathematica List (*.nb);;Bitmap Image set (*.bmp);;Structure Tensor Field (*.tns);;Surface Mesh(*.off)";
    }

    void VolumeRenderer::enableDraw(bool enable) {
        if(drawEnabled != enable) {
            drawEnabled = enable;
            if(drawEnabled) {
                calculateDisplay();
            }
        }
    }

    void VolumeRenderer::setViewingType(const int type) {
        viewingType = type;
        if(viewingType == VIEWING_TYPE_SOLID) {
            load3DTextureSolidRendering();
        } else if  (viewingType == VIEWING_TYPE_CROSS_SECTION) {
            load3DTextureCrossSection();
        }
        calculateDisplay();
    }

    bool VolumeRenderer::setCuttingPlane(float position, float vecX, float vecY, float vecZ) {
        Renderer::setCuttingPlane(position, vecX, vecY, vecZ);
        bool redraw = false;
        if((viewingType == VIEWING_TYPE_CROSS_SECTION) || (viewingType == VIEWING_TYPE_SOLID)) {
            redraw = calculateDisplay();
        }
        return redraw;
    }

    void VolumeRenderer::initializeOctree() {
        #ifdef USE_OCTREE_OPTIMIZATION
            if(octree != NULL) {
                delete octree;
            }
            unsigned int sizeX = dataVolume->getSizeX();
            unsigned int sizeY = dataVolume->getSizeY();
            unsigned int sizeZ = dataVolume->getSizeZ();
            octree  = new VolumeRendererOctreeType(sizeX, sizeY, sizeZ);
            for(unsigned int x = 0; x < sizeX-1; x++) {
                for(unsigned int y = 0; y < sizeY-1; y++) {
                    for(unsigned int z = 0; z < sizeZ-1; z++) {
                        octree->AddNewLeaf(x, y, z, 1);
                    }
                }
            }
            initializeOctreeTag(octree->GetRoot());
            float minVal, maxVal, val;
            VolumeRendererOctreeNodeType * node;

            for(unsigned int x = 0; x < sizeX-1; x++) {
                for(unsigned int y = 0; y < sizeY-1; y++) {
                    for(unsigned int z = 0; z < sizeZ-1; z++) {
                        node = octree->GetLeaf(x, y, z);
                        minVal = MAX_FLOAT;
                        maxVal = MIN_FLOAT;
                        for(unsigned int xx = 0; xx < 2; xx++) {
                            for(unsigned int yy = 0; yy < 2; yy++) {
                                for(unsigned int zz = 0; zz < 2; zz++) {
                                    val = dataVolume->(*this)(x+xx, y+yy, z+zz);
                                    minVal = min(minVal, val);
                                    maxVal = max(maxVal, val);
                                }
                            }
                        }

                        while(node != NULL) {
                            node->tag.maxVal = max(node->tag.maxVal, maxVal);
                            node->tag.minVal = min(node->tag.minVal, minVal);
                            node = node->parent;
                        }
                    }
                }
            }
        #endif
    }

    void VolumeRenderer::initializeOctreeTag(VolumeRendererOctreeNodeType * node) {
        if(node != NULL) {
            Range tag;
            node->tag = tag;
            if (!node->isLeaf) {
                for(int i = 0; i < 8; i++) {
                    initializeOctreeTag(node->children[i]);
                }
            }
        }
    }

    void VolumeRenderer::draw(int subSceneIndex, bool selectEnabled) {
        if(subSceneIndex == 0) {
            if((viewingType == VIEWING_TYPE_ISO_SURFACE) && (surfaceMesh != NULL)) {
                surfaceMesh->draw(true, selectEnabled, _useDisplayRadius, displayRadius, radiusOrigin);
            } else if((viewingType == VIEWING_TYPE_CROSS_SECTION) || (viewingType == VIEWING_TYPE_SOLID)) {
                glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);
                glDisable(GL_LIGHTING);
                glDisable(GL_CULL_FACE);
                Vec3F vertex;
                // The outside box

                if(viewingType == VIEWING_TYPE_CROSS_SECTION) {
                    glBegin(GL_LINES);
                    for(unsigned int i = 0; i < cuttingMesh->edges.size(); i++) {
                        if(cuttingMesh->edges[i].faceIds.size() == 1) {

                            for(unsigned int j = 0; j < 2; j++) {
                                vertex = cuttingMesh->vertices[cuttingMesh->GetVertexIndex(cuttingMesh->edges[i].vertexIds[j])].position;
                                glVertex3f(vertex.X() * (float)getSizeX(), vertex.Y() * (float)getSizeY(), vertex.Z() * (float)getSizeZ());
                            }
                        }
                    }
                    glEnd();
                }


                // The cutting surface
                glEnable(GL_TEXTURE_3D);
                //GLboolean resident;
                //glAreTexturesResident(1, &textureName, &resident);
                //if(resident) {
                    glBindTexture(GL_TEXTURE_3D, textureName);

                    double xRatio = (double)getSizeX() / (double)textureSize.X();
                    double yRatio = (double)getSizeY() / (double)textureSize.Y();
                    double zRatio = (double)getSizeZ() / (double)textureSize.Z();

                    for(unsigned int i = 0; i < cuttingMesh->faces.size(); i++) {
                        glBegin(GL_POLYGON);
                        for(unsigned int j = 0; j < cuttingMesh->faces[i].vertexIds.size(); j++) {
                            vertex = cuttingMesh->vertices[cuttingMesh->GetVertexIndex(cuttingMesh->faces[i].vertexIds[j])].position;
                            glTexCoord3d(vertex.X() * xRatio, vertex.Y()* yRatio, vertex.Z() * zRatio);
                            glVertex3f(vertex.X() * (float)getSizeX(), vertex.Y() * (float)getSizeY(), vertex.Z() * (float)getSizeZ());
                        }
                        glEnd();
                    }
                //}

                glPopAttrib();
            }
        }
    }


    void VolumeRenderer::calculateOctreeNode(VolumeRendererOctreeNodeType * node) {
        queue<VolumeRendererOctreeNodeType *> q;
        q.push(node);

        while(!q.empty()) {
            node = q.front();
            q.pop();
            if((node->tag.min <= surfaceValue) && (node->tag.max >= surfaceValue)) {
                if((int)node->cellSize <= sampleInterval + sampleInterval) {
                    for(int i = 0; i < 8; i++) {
                        if(node->children[i] != NULL) {
                            MarchingCube(this, surfaceMesh, surfaceValue, node->children[i]->pos[0], node->children[i]->pos[1], node->children[i]->pos[2], sampleInterval);
                        }
                    }
                } else {
                    for(int i = 0; i < 8; i++) {
                        if(node->children[i] != NULL) {
                            q.push(node->children[i]);
                        }
                    }
                }
            }
        }
    }

    bool VolumeRenderer::calculateSurface() {
        bool redraw = false;
        #ifndef USE_OCTREE_OPTIMIZATION
            //appTimeManager.PushCurrentTime();
            //appTimeManager.PushCurrentTime();
            surfaceMesh->Clear();
            //appTimeManager.PopAndDisplayTime("Marching Cubes)  Clearing : %f seconds |");
            redraw = false;
            marchingCubeCallCount = 0;
            if(drawEnabled && volData != NULL) {
                redraw = true;
                int maxX = getSizeX();
                int maxY = getSizeY();
                int maxZ = getSizeZ();
                for(int i = 0; i < maxX; i+=sampleInterval) {
                    for(int j = 0; j < maxY; j+=sampleInterval) {
                        for(int k = 0; k < maxZ; k+=sampleInterval) {
                            MarchingCube(this, surfaceMesh, surfaceValue, i, j, k, sampleInterval);
                        }
                    }
                }
                /*for(iX = max(radiusOrigin.X() - displayRadius, 0); iX < min(maxX, radiusOrigin.X() + displayRadius); iX+=sampleInterval) {
                    for(iY = max(radiusOrigin.Y() - displayRadius, 0); iY < min(maxY, radiusOrigin.Y() + displayRadius); iY+=sampleInterval) {
                        for(iZ = max(radiusOrigin.Z() - displayRadius, 0); iZ < min(maxZ, radiusOrigin.Z() + displayRadius); iZ+=sampleInterval) {
                            MarchingCube(dataVolume, surfaceMesh, surfaceValue, iX, iY, iZ, sampleInterval);
                        }
                    }
                }*/
            }
            //appTimeManager.PopAndDisplayTime("Meshing: %f seconds |");
        #else
            appTimeManager.PushCurrentTime();
            appTimeManager.PushCurrentTime();
            surfaceMesh->Clear();
            appTimeManager.PopAndDisplayTime("Octree)          Clearing : %f seconds |");
            marchingCubeCallCount = 0;
            redraw = false;
            if(drawEnabled && dataVolume != NULL && octree != NULL) {
                redraw = true;
                calculateOctreeNode(octree->GetRoot());
            }

            appTimeManager.PopAndDisplayTime("Meshing: %f seconds |");
        #endif
        //printf("Marching Cubes called %d times\n", marchingCubeCallCount);
        return redraw;

    }

    bool VolumeRenderer::calculateCuttingSurface() {
        cuttingMesh->Clear();

        bool redraw = false;

        if(volData != NULL) {
            redraw = true;

            if((cuttingPlaneCenter.X() >= minPts[0]) && (cuttingPlaneCenter.X() <= maxPts[0]) &&
                (cuttingPlaneCenter.Y() >= minPts[1]) && (cuttingPlaneCenter.Y() <= maxPts[1]) &&
                (cuttingPlaneCenter.Z() >= minPts[2]) && (cuttingPlaneCenter.Z() <= maxPts[2])) {


                int iX, iY, iZ;

                for(iX = 0; iX < 2; iX++) {
                    for(iY = 0; iY < 2; iY++) {
                        for(iZ = 0; iZ < 2; iZ++) {
                            cuttingVolume(iX, iY, iZ) = (cuttingPlaneCenter - Vec3F(iX * getSizeX(), iY * getSizeY(), iZ * getSizeZ()))* cuttingPlaneDirection;
                        }
                    }
                }
                MarchingCube(&cuttingVolume, cuttingMesh, 0.0f, 0, 0, 0, 1);
            }
        }
        return redraw;
    }


    bool VolumeRenderer::calculateSolidRendering() {
        cuttingMesh->Clear();
        bool redraw = false;
        if(volData != NULL) {
            redraw = true;
            NonManifoldMesh tempMesh;


            Vec3F modelCenter = (minPts + maxPts)/2.0;
            float distance = (minPts - modelCenter).length();

            for(float position = 1.0; position >= -1.0; position -= 0.01) {
                Vec3F center = modelCenter + cuttingPlaneDirection * position * distance;

                if((center.X() >= minPts[0]) && (center.X() <= maxPts[0]) &&
                    (center.Y() >= minPts[1]) && (center.Y() <= maxPts[1]) &&
                    (center.Z() >= minPts[2]) && (center.Z() <= maxPts[2])) {


                    for(int i = 0; i < 2; i++) {
                        for(int j = 0; j < 2; j++) {
                            for(int k = 0; k < 2; k++) {
                                cuttingVolume(i, j, k) = (center - Vec3F(i * getSizeX(), j * getSizeY(), k * getSizeZ()))* cuttingPlaneDirection;
                            }
                        }
                    }
                    tempMesh.Clear();
                    MarchingCube(&cuttingVolume, &tempMesh, 0.0f, 0, 0, 0, 1);
                    cuttingMesh->MergeMesh(&tempMesh);
                }
            }
        }
        return redraw;
    }
    bool VolumeRenderer::calculateDisplay() {
        bool redraw = false;
        switch (viewingType) {
            case VIEWING_TYPE_ISO_SURFACE:
                redraw = calculateSurface();
                break;
            case VIEWING_TYPE_CROSS_SECTION:
                redraw = calculateCuttingSurface();
                break;
            case VIEWING_TYPE_SOLID:
                redraw = calculateSolidRendering();
                break;
        }
        return redraw;
    }

    void VolumeRenderer::load(string fileName) {
        Volume::load(fileName);
        initializeOctree();
        updateBoundingBox();

        #ifdef _WIN32
            glTexImage3D = (PFNGLTEXIMAGE3DPROC) wglGetProcAddress("glTexImage3D");
        #endif

        setDisplayRadiusOrigin(getSizeX()/2, getSizeY()/2, getSizeZ()/2);
    }

    void VolumeRenderer::load3DTextureSolidRendering() {
        if(textureLoaded) {
            glDeleteTextures(1, &textureName);
            textureLoaded = false;
        }

        if(volData != NULL) {
            textureSize[0] = smallest2ndPower(getSizeX());
            textureSize[1] = smallest2ndPower(getSizeY());
            textureSize[2] = smallest2ndPower(getSizeZ());
            double maxVal = maxSurfaceValue;
            double minVal = surfaceValue;
            unsigned char val;

            unsigned char * texels = new unsigned char[textureSize.X() * textureSize.Y() * textureSize.Z()];
            unsigned int pos = 0;
            for(int z = 0; z < textureSize.Z(); z++) {
                for(int y = 0; y < textureSize.Y(); y++) {
                    for(int x = 0; x < textureSize.X(); x++) {
                        if((x < getSizeX()) && (y < getSizeY()) && (z < getSizeZ())) {
                            val = (unsigned char)round((min(max((double)(*this)(x, y, z), minVal), maxVal) - minVal) * 255.0 / (maxVal - minVal));
                        } else {
                            val = 0;
                        }
                        texels[pos] = val;
                        pos++;
                    }
                }
            }
            glGenTextures(1, &textureName);
            glBindTexture(GL_TEXTURE_3D, textureName);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
            try {
                glTexImage3D(GL_TEXTURE_3D, 0, GL_ALPHA, textureSize.X(), textureSize.Y(), textureSize.Z(), 0, GL_ALPHA, GL_UNSIGNED_BYTE, texels);
                textureLoaded = true;
            }   catch (int) {
                textureLoaded = false;
            }
            delete [] texels;

        }

    }

    void VolumeRenderer::load3DTextureCrossSection() {
        if(textureLoaded) {
            glDeleteTextures(1, &textureName);
            textureLoaded = false;
        }

        if(volData != NULL) {
            textureSize[0] = smallest2ndPower(getSizeX());
            textureSize[1] = smallest2ndPower(getSizeY());
            textureSize[2] = smallest2ndPower(getSizeZ());
            double maxVal = maxSurfaceValue;
            double minVal = surfaceValue;
            unsigned char val;

            // Approximations to avoid division by zero
            if(isZero(minVal - maxVal, 0.000000000001)) {
                maxVal = minVal + (getMaxDensity() - getMinDensity()) / 1000.0;
            }
            if(isZero(minVal - maxVal, 0.000000000001)) {
                maxVal = minVal + 0.0001;
            }

            unsigned char * texels = new unsigned char[textureSize.X() * textureSize.Y() * textureSize.Z()];
            unsigned int pos = 0;
            for(int z = 0; z < textureSize.Z(); z++) {
                for(int y = 0; y < textureSize.Y(); y++) {
                    for(int x = 0; x < textureSize.X(); x++) {
                        if((x < getSizeX()) && (y < getSizeY()) && (z < getSizeZ())) {
                            val = (unsigned char)round((min(max((double)(*this)(x, y, z), minVal), maxVal) - minVal) * 255.0 / (maxVal - minVal));
                        } else {
                            val = 0;
                        }
                        texels[pos] = val;
                        pos++;
                    }
                }
            }
            glGenTextures(1, &textureName);
            glBindTexture(GL_TEXTURE_3D, textureName);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
            try {
                glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, textureSize.X(), textureSize.Y(), textureSize.Z(), 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texels);
                textureLoaded = true;
            }   catch (int) {
                textureLoaded = false;
            }
            delete [] texels;

        }

    }

    void VolumeRenderer::save(string fileName) {
        if(volData != NULL) {
            int pos = fileName.rfind(".") + 1;
            string extension = fileName.substr(pos, fileName.length()-pos);

            extension = StringUtils::StringToUpper(extension);

            if(strcmp(extension.c_str(), "MRC") == 0) {
                toMRCFile((char *)fileName.c_str());
            } else {
                printf("Input format %s not supported!\n", extension.c_str());
            }


        }
    }

    void VolumeRenderer::MarchingCube(Volume * vol, Mesh * mesh, const float iso_level, int iX, int iY, int iZ, int iScale){
        marchingCubeCallCount++;
        extern int aiCubeEdgeFlags[256];
        extern int a2iTriangleConnectionTable[256][16];

        int iVertex, iFlagIndex, iEdgeFlags;
        float fOffset;
        float afCubeValue[8];
        Vec3D asEdgeVertex[12];
        int vertexIds[12];

        //Make a local copy of the values at the cube's corners
        for(int iVertex = 0; iVertex < 8; iVertex++) {
            afCubeValue[iVertex] = getVoxelData(iX + a2iVertexOffset[iVertex][0]*iScale,
                                                iY + a2iVertexOffset[iVertex][1]*iScale,
                                                iZ + a2iVertexOffset[iVertex][2]*iScale);
        }

        //Find which vertices are inside of the surface and which are outside
        iFlagIndex = 0;
        for(int iVertexTest = 0; iVertexTest < 8; iVertexTest++)
        {
                if(afCubeValue[iVertexTest] <= iso_level)
                        iFlagIndex |= 1<<iVertexTest;
        }

        //Find which edges are intersected by the surface
        iEdgeFlags = aiCubeEdgeFlags[iFlagIndex];

        //If the cube is entirely inside or outside of the surface, then there will be no intersections
        if(iEdgeFlags == 0)
        {
                return;
        }

        //Find the point of intersection of the surface with each edge
        //Then find the normal to the surface at those points
        for(int iEdge = 0; iEdge < 12; iEdge++)
        {
                //if there is an intersection on this edge
                if(iEdgeFlags & (1<<iEdge))
                {
                        fOffset = getOffset(afCubeValue[ a2iEdgeConnection[iEdge][0] ], afCubeValue[ a2iEdgeConnection[iEdge][1] ], iso_level);

                        asEdgeVertex[iEdge][0] = (float)iX + ((float)a2iVertexOffset[ a2iEdgeConnection[iEdge][0] ][0] +  fOffset * (float)a2iEdgeDirection[iEdge][0]) * (float)iScale;
                        asEdgeVertex[iEdge][1] = (float)iY + ((float)a2iVertexOffset[ a2iEdgeConnection[iEdge][0] ][1] +  fOffset * (float)a2iEdgeDirection[iEdge][1]) * (float)iScale;
                        asEdgeVertex[iEdge][2] = (float)iZ + ((float)a2iVertexOffset[ a2iEdgeConnection[iEdge][0] ][2] +  fOffset * (float)a2iEdgeDirection[iEdge][2]) * (float)iScale;

                        vertexIds[iEdge] = mesh->AddMarchingVertex(Vec3F(asEdgeVertex[iEdge][0], asEdgeVertex[iEdge][1], asEdgeVertex[iEdge][2]), getHashKey(iX, iY, iZ, iEdge, iScale));
                }
        }


        //Draw the triangles that were found.  There can be up to five per cube
        for(int iTriangle = 0; iTriangle < 5; iTriangle++)
        {
                if(a2iTriangleConnectionTable[iFlagIndex][3*iTriangle] < 0)
                        break;
                int triangleVertices[3];
                for(int iCorner = 0; iCorner < 3; iCorner++)
                {
                    iVertex = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+iCorner];
                    triangleVertices[iCorner] = vertexIds[iVertex];
                }

                mesh->AddMarchingFace(triangleVertices[0], triangleVertices[1], triangleVertices[2]);
        }
    }

    void VolumeRenderer::setSampleInterval(const int size) {
        sampleInterval = size;
        if(viewingType == VIEWING_TYPE_ISO_SURFACE) {
            calculateSurface();
        } else if (viewingType == VIEWING_TYPE_CROSS_SECTION) {
            calculateCuttingSurface();
        }
    }

    void VolumeRenderer::setSurfaceValue(const float value) {
        surfaceValue = value;
        switch(viewingType) {
            case VIEWING_TYPE_ISO_SURFACE:
                calculateSurface();
                break;
            case VIEWING_TYPE_CROSS_SECTION:
                load3DTextureCrossSection();
                break;
            case VIEWING_TYPE_SOLID:
                load3DTextureSolidRendering();
                break;
        }
    }

    void VolumeRenderer::setMaxSurfaceValue(const float value) {
        maxSurfaceValue = value;
        switch(viewingType) {
            case VIEWING_TYPE_ISO_SURFACE:
                break;
            case VIEWING_TYPE_CROSS_SECTION:
                load3DTextureCrossSection();
                break;
            case VIEWING_TYPE_SOLID:
                load3DTextureSolidRendering();
                break;
        }
    }

    void VolumeRenderer::setDisplayRadius(const int radius) {
        displayRadius = radius;
    }

    void VolumeRenderer::setDisplayRadiusOrigin(float radiusOriginX, float radiusOriginY, float radiusOriginZ) {
        radiusOrigin = Vec3F(radiusOriginX, radiusOriginY, radiusOriginZ);
    }

    void VolumeRenderer::useDisplayRadius(bool useRadius) {
        _useDisplayRadius = useRadius;
    }

    void VolumeRenderer::unload() {
        Renderer::unload();
        if(octree != NULL) {
            delete octree;
        }
        octree = NULL;
        if(textureLoaded) {
            glDeleteTextures(1, &textureName);
            textureLoaded = false;
        }
        calculateSurface();
        updateBoundingBox();
    }

    void VolumeRenderer::updateBoundingBox() {
        if(volData == NULL) {
            minPts = 0.0;
            maxPts = 1.0;
        } else {
            minPts = 0.0;

            maxPts[0] = getSizeX()-1;
            maxPts[1] = getSizeY()-1;
            maxPts[2] = getSizeZ()-1;
        }
    }

}




#endif /* SRC_TOOLKIT_VISUALIZATION_VOLUMERENDERER_H_ */
