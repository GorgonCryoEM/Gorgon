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
        int pos = fileName.rfind(".") + 1;
        string extension = fileName.substr(pos, fileName.length()-pos);

        extension = StringUtils::StringToUpper(extension);

        if(strcmp(extension.c_str(), "MRC") == 0) {
            toMRCFile((char *)fileName.c_str());
        } else {
            printf("Input format %s not supported!\n", extension.c_str());
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


        //Make a local copy of the values at the cube's corners
        float afCubeValue[8];

        for(int iVertex = 0; iVertex < 8; iVertex++) {
            afCubeValue[iVertex] = vol.getVoxelData(iX + a2iVertexOffset[iVertex][0]*iScale,
                                                    iY + a2iVertexOffset[iVertex][1]*iScale,
                                                    iZ + a2iVertexOffset[iVertex][2]*iScale);
        }

        //Find which vertices are inside of the surface and which are outside
        int iFlagIndex = 0;
        for(int i = 0; i < 8; i++)
        {
            if(afCubeValue[i] <= iso_level)
                iFlagIndex |= 1<<i;
        }

        //Find which edges are intersected by the surface
        int iEdgeFlags = aiCubeEdgeFlags[iFlagIndex];

        //If the cube is entirely inside or outside of the surface, then there will be no intersections
        if(iEdgeFlags == 0)
        {
            return;
        }

        //Find the point of intersection of the surface with each edge
        //Then find the normal to the surface at those points
        Vec3F asEdgeVertex[12];
        int vertexIds[12];

        for(int i = 0; i < 12; i++)
        {
            //if there is an intersection on this edge
            if(iEdgeFlags & (1<<i))
            {
                float fOffset = vol.getOffset(afCubeValue[ a2iEdgeConnection[i][0] ], afCubeValue[ a2iEdgeConnection[i][1] ], iso_level);

                asEdgeVertex[i][0] = (float)iX + ((float)a2iVertexOffset[ a2iEdgeConnection[i][0] ][0] +  fOffset * (float)a2iEdgeDirection[i][0]) * (float)iScale;
                asEdgeVertex[i][1] = (float)iY + ((float)a2iVertexOffset[ a2iEdgeConnection[i][0] ][1] +  fOffset * (float)a2iEdgeDirection[i][1]) * (float)iScale;
                asEdgeVertex[i][2] = (float)iZ + ((float)a2iVertexOffset[ a2iEdgeConnection[i][0] ][2] +  fOffset * (float)a2iEdgeDirection[i][2]) * (float)iScale;

                vertexIds[i] = mesh.addVertex(Vec3F(asEdgeVertex[i]), vol.getHashKey(iX, iY, iZ, i, iScale));
            }
        }


        //Draw the triangles that were found.  There can be up to five per cube
        for(int i = 0; i < 5; i++)
        {
            if(a2iTriangleConnectionTable[iFlagIndex][3*i] < 0)
                break;
            Vec3U triangleVertices;
            for(int j = 0; j < 3; j++)
            {
                int iVertex = a2iTriangleConnectionTable[iFlagIndex][3*i+j];
                triangleVertices[j] = vertexIds[iVertex];
            }

            mesh.addFace(triangleVertices);
        }
    }

}
