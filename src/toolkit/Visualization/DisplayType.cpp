/*
 * DisplayType.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "DisplayType.h"

#include "Foundation/StringUtils.h"

namespace Visualization {

    DisplayType::DisplayType(const Volume & v)
            : vol(v)
    {
        textureLoaded = false;
        viewingType = VIEWING_TYPE_ISO_SURFACE;
        surfaceValue = 1.5;
        sampleInterval = 1;
    }

    DisplayType::~DisplayType() {
        if(textureLoaded) {
            glDeleteTextures(1, &textureName);
            textureLoaded = false;
        }
    }

    float DisplayType::getSurfaceValue() const {
        return surfaceValue;
    }

    int DisplayType::getSampleInterval() const  {
        return sampleInterval;
    }

    void DisplayType::setViewingType(const int type) {
        viewingType = type;

        load3DTexture();
        calculateDisplay();
    }

    bool DisplayType::calculateDisplay() {
        return false;
    }

    void DisplayType::load3DTexture() {
    }

    void DisplayType::setSampleInterval(const int size) {
        sampleInterval = size;

        calculateDisplay();
    }

    void DisplayType::setSurfaceValue(const float value) {
        surfaceValue = value;

        calculateDisplay();
        load3DTexture();
    }

    void DisplayType::setMaxSurfaceValue(const float value) {
        maxSurfaceValue = value;

        load3DTexture();
    }

    void DisplayType::unload() {
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
