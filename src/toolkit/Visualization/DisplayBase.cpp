/*
 * DisplayBase.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "DisplayBase.h"
#include "Foundation/StringUtils.h"

namespace Visualization {

    DisplayBase::DisplayBase()
        : RendererBase(),
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

    DisplayBase::~DisplayBase() {
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

    float DisplayBase::getSurfaceValue() const {
        return surfaceValue;
    }

    int DisplayBase::getSampleInterval() const  {
        return sampleInterval;
    }

    string DisplayBase::getSupportedLoadFileFormats() {
        return "All Files (*.mrc *.ccp4 *.map *.raw *.pts);; Volumes (*.mrc *.ccp4 *.map *.raw);;Point Cloud (*.pts)";
    }

    string DisplayBase::getSupportedSaveFileFormats() {
        return "Volumes (*.mrc *.ccp4 *.map *.raw);;Mathematica List (*.nb);;Bitmap Image set (*.bmp);;Structure Tensor Field (*.tns);;Surface Mesh(*.off)";
    }

    void DisplayBase::enableDraw(bool enable) {
        if(drawEnabled != enable) {
            drawEnabled = enable;
            if(drawEnabled) {
                calculateDisplay();
            }
        }
    }

    void DisplayBase::setViewingType(const int type) {
        viewingType = type;

        load3DTexture();
        calculateDisplay();
    }

    bool DisplayBase::setCuttingPlane(float position, float vecX, float vecY, float vecZ) {
        return false;
    }

    void DisplayBase::initializeOctree() {
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

    void DisplayBase::initializeOctreeTag(VolumeRendererOctreeNodeType * node) {
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

    void DisplayBase::draw(int subSceneIndex, bool selectEnabled) {
    }

    bool DisplayBase::calculateDisplay() {
        return false;
    }

    void DisplayBase::load3DTexture() {
    }

    void DisplayBase::load(string fileName) {
//        #ifdef GORGON_DEBUG
              cout<<"\033[32mDEBUG: File:   DisplayBase.cpp"<<endl;
              cout<<"DEBUG: Method: DisplayBase::load(string)\033[0m"<<endl;
              cout<<(Volume)(*this)<<endl;
//        #endif

        Volume::load(fileName);

//        #ifdef GORGON_DEBUG
              cout<<"\033[32mDEBUG: File:   DisplayBase.cpp"<<endl;
              cout<<"DEBUG: After load()\033[0m"<<endl;
              cout<<(Volume)(*this)<<endl;
//        #endif

        initializeOctree();
        updateBoundingBox();

        #ifdef _WIN32
            glTexImage3D = (PFNGLTEXIMAGE3DPROC) wglGetProcAddress("glTexImage3D");
        #endif

        setDisplayRadiusOrigin(getSizeX()/2, getSizeY()/2, getSizeZ()/2);
    }

    void DisplayBase::save(string fileName) {
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

    void DisplayBase::MarchingCube(Volume * vol, Mesh * mesh, const float iso_level, int iX, int iY, int iZ, int iScale){
//        extern int aiCubeEdgeFlags[256];
//        extern int a2iTriangleConnectionTable[256][16];

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

    void DisplayBase::setSampleInterval(const int size) {
        sampleInterval = size;

        calculateDisplay();
    }

    void DisplayBase::setSurfaceValue(const float value) {
        surfaceValue = value;

        calculateDisplay();
        load3DTexture();
    }

    void DisplayBase::setMaxSurfaceValue(const float value) {
        maxSurfaceValue = value;

        load3DTexture();
    }

    void DisplayBase::setDisplayRadius(const int radius) {
        displayRadius = radius;
    }

    void DisplayBase::setDisplayRadiusOrigin(float radiusOriginX, float radiusOriginY, float radiusOriginZ) {
        radiusOrigin = Vec3F(radiusOriginX, radiusOriginY, radiusOriginZ);
    }

    void DisplayBase::useDisplayRadius(bool useRadius) {
        _useDisplayRadius = useRadius;
    }

    void DisplayBase::unload() {
        RendererBase::unload();
        if(octree != NULL) {
            delete octree;
        }
        octree = NULL;
        if(textureLoaded) {
            glDeleteTextures(1, &textureName);
            textureLoaded = false;
        }
        calculateDisplay();
        updateBoundingBox();
    }

    void DisplayBase::updateBoundingBox() {
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

    int smallest2ndPower(int value) {
        int power = 1;
        while (power < value) {
            power = power * 2;
        }
        return power;
    }

}
