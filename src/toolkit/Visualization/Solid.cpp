/*
 * Solid.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "Solid.h"

namespace Visualization {

    Solid::Solid()
            : cuttingVolume(Volume(2, 2, 2))
    {
        cuttingMesh = new NonManifoldMesh();
    }

    Solid::~Solid() {
        delete cuttingMesh;
    }


    void Solid::load3DTexture() {
        if(textureLoaded) {
            glDeleteTextures(1, &textureName);
            textureLoaded = false;
        }

        if(volData != NULL) {
            textureSize[0] = smallest2ndPower(volData->getSizeX());
            textureSize[1] = smallest2ndPower(volData->getSizeY());
            textureSize[2] = smallest2ndPower(volData->getSizeZ());
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

    void Solid::draw(int subSceneIndex, bool selectEnabled) {
        if(subSceneIndex == 0) {
            glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);
            glDisable(GL_LIGHTING);
            glDisable(GL_CULL_FACE);
            Vec3F vertex;
            // The outside box


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
                        vertex = cuttingMesh->vertices[cuttingMesh->getVertexIndex(cuttingMesh->faces[i].vertexIds[j])].position;
                        glTexCoord3d(vertex.X() * xRatio, vertex.Y()* yRatio, vertex.Z() * zRatio);
                        glVertex3f(vertex.X() * (float)getSizeX(), vertex.Y() * (float)getSizeY(), vertex.Z() * (float)getSizeZ());
                    }
                    glEnd();
                }
            //}

            glPopAttrib();
        }
    }

} /* namespace Visualization */
