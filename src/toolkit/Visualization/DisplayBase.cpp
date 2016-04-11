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
    {
        textureLoaded = false;
        viewingType = VIEWING_TYPE_ISO_SURFACE;
        surfaceValue = 1.5;
        sampleInterval = 1;
    }

    DisplayBase::~DisplayBase() {
        if(textureLoaded) {
            glDeleteTextures(1, &textureName);
            textureLoaded = false;
        }
    }

    float DisplayBase::getSurfaceValue() const {
        return surfaceValue;
    }

    int DisplayBase::getSampleInterval() const  {
        return sampleInterval;
    }

    void DisplayBase::setViewingType(const int type) {
        viewingType = type;

        load3DTexture();
        calculateDisplay();
    }

    void DisplayBase::draw(int subSceneIndex, bool selectEnabled) {
    }

    bool DisplayBase::calculateDisplay() {
        return false;
    }

    float DisplayBase::getMinPos(int i) const {
        return 0.0;
    }

    float DisplayBase::getMaxPos(int i) const {
        float result;
        switch(i) {
            case 0:
                result = float(getSizeX()-1);
                break;
            case 1:
                result = float(getSizeY()-1);
                break;
            case 2:
                result = float(getSizeZ()-1);
                break;
        }

        return result;
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

        #ifdef _WIN32
            glTexImage3D = (PFNGLTEXIMAGE3DPROC) wglGetProcAddress("glTexImage3D");
        #endif

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

    void DisplayBase::unload() {
        if(textureLoaded) {
            glDeleteTextures(1, &textureName);
            textureLoaded = false;
        }
        calculateDisplay();
    }

    int smallest2ndPower(int value) {
        int power = 1;
        while (power < value) {
            power = power * 2;
        }
        return power;
    }

    void MarchingCube(const Volume & vol, Mesh & mesh, const float iso_level, int iX, int iY, int iZ, int iScale){
        //        extern int aiCubeEdgeFlags[256];
        //        extern int a2iTriangleConnectionTable[256][16];

        int iVertex, iFlagIndex, iEdgeFlags;
        float fOffset;
        float afCubeValue[8];
        Vec3D asEdgeVertex[12];
        int vertexIds[12];

        //Make a local copy of the values at the cube's corners
        for(int iVertex = 0; iVertex < 8; iVertex++) {
            afCubeValue[iVertex] = vol.getVoxelData(iX + a2iVertexOffset[iVertex][0]*iScale,
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
                fOffset = vol.getOffset(afCubeValue[ a2iEdgeConnection[iEdge][0] ], afCubeValue[ a2iEdgeConnection[iEdge][1] ], iso_level);

                asEdgeVertex[iEdge][0] = (float)iX + ((float)a2iVertexOffset[ a2iEdgeConnection[iEdge][0] ][0] +  fOffset * (float)a2iEdgeDirection[iEdge][0]) * (float)iScale;
                asEdgeVertex[iEdge][1] = (float)iY + ((float)a2iVertexOffset[ a2iEdgeConnection[iEdge][0] ][1] +  fOffset * (float)a2iEdgeDirection[iEdge][1]) * (float)iScale;
                asEdgeVertex[iEdge][2] = (float)iZ + ((float)a2iVertexOffset[ a2iEdgeConnection[iEdge][0] ][2] +  fOffset * (float)a2iEdgeDirection[iEdge][2]) * (float)iScale;

                vertexIds[iEdge] = mesh.addMarchingVertex(Vec3F(asEdgeVertex[iEdge][0], asEdgeVertex[iEdge][1], asEdgeVertex[iEdge][2]), vol.getHashKey(iX, iY, iZ, iEdge, iScale));
            }
        }


        //Draw the triangles that were found.  There can be up to five per cube
        for(int iTriangle = 0; iTriangle < 5; iTriangle++)
        {
            if(a2iTriangleConnectionTable[iFlagIndex][3*iTriangle] < 0)
                break;
            TriangleMeshFace triangleVertices;
            for(int iCorner = 0; iCorner < 3; iCorner++)
            {
                iVertex = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+iCorner];
                triangleVertices[iCorner] = vertexIds[iVertex];
            }

            mesh.addMarchingFace(triangleVertices);
        }
    }

}
