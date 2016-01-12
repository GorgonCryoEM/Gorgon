#ifndef SRC_CORE_VOLUMERENDERER_H
#define SRC_CORE_VOLUMERENDERER_H


//#include <iostream>
#include <string>
//#include <GorgonGL.h>
//#include <GraphMatch/VectorMath.h>
#include "Renderer.h"
#include "GlobalConstants.h"
#include <SkeletonMaker/volume.h>
#include <GraySkeletonCPP/GlobalDefinitions.h>
#include <GraySkeletonCPP/VolumeSkeletonizer.h>
#include <GraySkeletonCPP/VolumeFormatConverter.h>
//#include "MeshRenderer.h"
//#include <ProteinMorph/NonManifoldMesh.h>
//#include <ProteinMorph/TriangleMesh.h>
//#include <MathTools/Vector3D.h>
#include <MathTools/MathLib.h>
//#include <Foundation/Octree.h>
//#include <queue>

using namespace std;

using namespace GraySkeletonCPP;
//using namespace Protein_Morph;
//using namespace MathTools;
//using namespace GraphMatch;
using namespace SkeletonMaker;

namespace Visualization {
    class VolumeRenderer : public Renderer {
    public:
        VolumeRenderer();
        ~VolumeRenderer();

        float GetMaxDensity();
        float GetMinDensity();
        float GetSurfaceValue() const ;
        string GetSupportedLoadFileFormats();
        string GetSupportedSaveFileFormats();
        void LoadFile(string fileName);
        void LoadFileRAW(string fileName, int bitsPerCell, int sizeX, int sizeY, int sizeZ);
        void SaveFile(string fileName);
        void Unload();
        void NormalizeVolume();
        void DownsampleVolume();
        void PerformSmoothLaplacian(double convergenceRate, int iterations);
        Volume * GetVolume();
        void setVolume(Volume *vol);
        Volume * PerformBinarySkeletonizationJu2007(double threshold, int minCurveSize, int minSurfaceSize);
        Volume * PerformGrayscaleSkeletonizationAbeysinghe2008(double startDensity, int stepCount, int minCurveSize, int minSurfaceSize, int curveRadius, int surfaceRadius, int skeletonSmoothenRadius);
        void SetSpacing(float spX, float spY, float spZ);
        float GetSpacingX();
        float GetSpacingY();
        float GetSpacingZ();
        void SetOrigin(float orgX, float orgY, float orgZ);
        float GetOriginX();
        float GetOriginY();
        float GetOriginZ();

    private:
        int GetHashKey(int x, int y, int z, int edge, int iScale);
        float GetVoxelData(Volume * vol, int x, int y, int z);
        float GetVoxelData(Volume * vol, float x, float y, float z);
        float GetOffset(float fValue1, float fValue2, float fValueDesired);
        int Smallest2ndPower(int value);
    private:
        float surfaceValue;
        float maxSurfaceValue;
        Volume * dataVolume;
    };

    VolumeRenderer::VolumeRenderer() {
        dataVolume = NULL;
        surfaceValue = 1.5;
    }

    VolumeRenderer::~VolumeRenderer() {
        if(dataVolume != NULL) {
            delete dataVolume;
        }
    }

    float VolumeRenderer::GetMaxDensity(){
        return dataVolume->getMax();
    }

    float VolumeRenderer::GetMinDensity() {
        return dataVolume->getMin();
    }

    float VolumeRenderer::GetOffset(float fValue1, float fValue2, float fValueDesired) {
        double fDelta = fValue2 - fValue1;
        if(fDelta == 0.0) {
                return 0.5;
        }
        return (fValueDesired - fValue1)/fDelta;
    }



    float VolumeRenderer::GetSurfaceValue() const {
        return surfaceValue;
    }

    float VolumeRenderer::GetVoxelData(Volume * vol, int x, int y, int z) {
        if((x < 0) || (x > vol->getSizeX()-1) || (y < 0) || (y > vol->getSizeY()-1) || (z < 0) || (z > vol->getSizeZ()-1)) {
            return 0.0f;
        } else {
            return vol->getDataAt(x, y, z);
        }
    }

    float VolumeRenderer::GetVoxelData(Volume * vol, float x, float y, float z) {
        int f[3] = {(int)x, (int)y, (int)z};
        int c[3] = {f[0]+1, f[1]+1, f[2]+1};
        float d[3] = {x - f[0], y - f[1], z - f[2]};

        float i1 = GetVoxelData(vol, f[0], f[1], f[2]) * (1.0 - d[2]) + GetVoxelData(vol, f[0], f[1], c[2]) * d[2];
        float i2 = GetVoxelData(vol, f[0], c[1], f[2]) * (1.0 - d[2]) + GetVoxelData(vol, f[0], c[1], c[2]) * d[2];
        float j1 = GetVoxelData(vol, c[0], f[1], f[2]) * (1.0 - d[2]) + GetVoxelData(vol, c[0], f[1], c[2]) * d[2];
        float j2 = GetVoxelData(vol, c[0], c[1], f[2]) * (1.0 - d[2]) + GetVoxelData(vol, c[0], c[1], c[2]) * d[2];

        float w1 = i1 * (1.0 - d[1]) + i2 * d[1];
        float w2 = j1 * (1.0 - d[1]) + j2 * d[1];

        return w1 * (1.0 - d[0]) + w2 * d[0];
    }

    int VolumeRenderer::GetHashKey(int x, int y, int z, int edge, int iScale) {

        x += a2iEdgeHash[edge][1]*iScale;
        y += a2iEdgeHash[edge][2]*iScale;
        z += a2iEdgeHash[edge][3]*iScale;

        edge = a2iEdgeHash[edge][0];
        return x * dataVolume->getSizeY() * dataVolume->getSizeZ() * 3 + y * dataVolume->getSizeZ() * 3 + z * 3 + edge;
    }

    int VolumeRenderer::Smallest2ndPower(int value) {
        int power = 1;
        while (power < value) {
            power = power * 2;
        }
        return power;
    }
    string VolumeRenderer::GetSupportedLoadFileFormats() {
        return "All Files (*.mrc *.ccp4 *.map *.raw *.pts);; Volumes (*.mrc *.ccp4 *.map *.raw);;Point Cloud (*.pts)";
    }

    string VolumeRenderer::GetSupportedSaveFileFormats() {
        return "Volumes (*.mrc *.ccp4 *.map *.raw);;Mathematica List (*.nb);;Bitmap Image set (*.bmp);;Structure Tensor Field (*.tns);;Surface Mesh(*.off)";
    }

    void VolumeRenderer::DownsampleVolume() {
        Volume * sourceVol = dataVolume;
        Volume * destVol = new Volume(sourceVol->getSizeX()/2, sourceVol->getSizeY()/2, sourceVol->getSizeZ()/2);
        double val;

        int radius = 1;
        MathLib * math = new MathLib();

        ProbabilityDistribution3D gaussianFilter;
        gaussianFilter.radius = radius;
        math->GetBinomialDistribution(gaussianFilter);

        for(int x = radius; x < destVol->getSizeX()-radius; x++) {
            for(int y = radius; y < destVol->getSizeY()-radius; y++) {
                for(int z = radius; z < destVol->getSizeZ()-radius; z++) {
                    val = 0;
                    for(int xx = -radius; xx <= radius; xx++) {
                        for(int yy = -radius; yy <= radius; yy++) {
                            for(int zz = -radius; zz <= radius; zz++) {
                                val += sourceVol->getDataAt(2*x+xx, 2*y+yy, 2*z+zz) * gaussianFilter.values[xx+radius][yy+radius][zz+radius] ;
                            }
                        }
                    }
                    destVol->setDataAt(x, y, z, val);
                }
            }
        }

        delete math;
        delete sourceVol;
        dataVolume = destVol;
    }


    void VolumeRenderer::LoadFile(string fileName) {
        Renderer::LoadFile(fileName);
        if(dataVolume != NULL) {
            delete dataVolume;
        }
        dataVolume = VolumeFormatConverter::LoadVolume(fileName);
    }

    void VolumeRenderer::LoadFileRAW(string fileName, int bitsPerCell, int sizeX, int sizeY, int sizeZ) {
        Renderer::LoadFile(fileName);
        if(dataVolume != NULL) {
            delete dataVolume;
        }
        dataVolume = VolumeFormatConverter::LoadVolume(fileName, bitsPerCell, sizeX, sizeY, sizeZ);
    }


    void VolumeRenderer::SaveFile(string fileName) {
        if(dataVolume != NULL) {
            int pos = fileName.rfind(".") + 1;
            string extension = fileName.substr(pos, fileName.length()-pos);

            extension = StringUtils::StringToUpper(extension);

            if(strcmp(extension.c_str(), "MRC") == 0) {
                dataVolume->toMRCFile((char *)fileName.c_str());
            } else if(strcmp(extension.c_str(), "CCP4") == 0) {
                dataVolume->toMRCFile((char *)fileName.c_str());
            } else if(strcmp(extension.c_str(), "RAW") == 0) {
                VolumeReaderRAW::SaveVolume16bit(dataVolume, fileName);
            } else if(strcmp(extension.c_str(), "NB") == 0) {
                dataVolume->toMathematicaFile((char *)fileName.c_str());
            } else if(strcmp(extension.c_str(), "TNS") == 0) {
                VolumeReaderTNS::SaveVolume(dataVolume, fileName);
            } else if(strcmp(extension.c_str(), "BMP") == 0) {
                ImageReaderBMP::SaveVolumeAsImageSet(dataVolume, fileName);
            } else {
                printf("Input format %s not supported!\n", extension.c_str());
            }


        }
    }


    void VolumeRenderer::NormalizeVolume(){
        dataVolume->normalize(0, 1);
    }

    void VolumeRenderer::PerformSmoothLaplacian(double convergenceRate, int iterations) {
        if(dataVolume != NULL) {
            for(unsigned int i = 0; i < iterations; i++) {
                this->dataVolume->smooth(convergenceRate);
            }
        }
    }


    void VolumeRenderer::Unload() {
        Renderer::Unload();
        if(dataVolume != NULL) {
            delete dataVolume;
        }
        dataVolume = NULL;
    }


    Volume * VolumeRenderer::PerformBinarySkeletonizationJu2007(double threshold, int minCurveSize, int minSurfaceSize) {
        VolumeSkeletonizer * skeletonizer = new VolumeSkeletonizer(0,0,0,DEFAULT_SKELETON_DIRECTION_RADIUS);
        Volume * outputVol = skeletonizer->PerformPureJuSkeletonization(dataVolume, "", threshold, minCurveSize, minSurfaceSize);
        delete skeletonizer;
        return outputVol;
    }

    Volume * VolumeRenderer::PerformGrayscaleSkeletonizationAbeysinghe2008(double startDensity, int stepCount, int minCurveSize, int minSurfaceSize, int curveRadius, int surfaceRadius, int skeletonRadius) {
        double stepSize = (dataVolume->getMax() - startDensity) / stepCount;
        if(!isZero(stepSize)) {
            VolumeSkeletonizer * skeletonizer = new VolumeSkeletonizer(0, curveRadius, surfaceRadius, skeletonRadius);
            Volume * outputVol = skeletonizer->PerformImmersionSkeletonizationAndPruning(dataVolume, NULL, startDensity, dataVolume->getMax(), stepSize, 0, 0, minCurveSize, minSurfaceSize, 0, 0, "", true, 1.0, DEFAULT_PRUNE_THRESHOLD, DEFAULT_PRUNE_THRESHOLD);
            delete skeletonizer;
            return outputVol;
        } else {
            return NULL;
        }
    }

    Volume * VolumeRenderer::GetVolume() {
        return dataVolume;
    }

    void VolumeRenderer::setVolume(Volume *vol) {
        dataVolume = vol;
    }

    void VolumeRenderer::SetSpacing(float spX, float spY, float spZ) {
        if(dataVolume != NULL) {
            dataVolume->setSpacing(spX, spY, spZ);
        } else {
            Renderer::setSpacing(spX, spY, spZ);
        }
    }

    float VolumeRenderer::GetSpacingX() {
        if(dataVolume != NULL) {
            return dataVolume->getSpacingX();
        }
        return Renderer::getSpacingX();
    }

    float VolumeRenderer::GetSpacingY() {
        if(dataVolume != NULL) {
            return dataVolume->getSpacingY();
        }
        return Renderer::getSpacingY();
    }

    float VolumeRenderer::GetSpacingZ() {
        if(dataVolume != NULL) {
            return dataVolume->getSpacingZ();
        }
        return Renderer::getSpacingZ();
    }

    void VolumeRenderer::SetOrigin(float orgX, float orgY, float orgZ) {
        if(dataVolume != NULL) {
            dataVolume->setOrigin(orgX, orgY, orgZ);
        } else {
            Renderer::setOrigin(orgX, orgY, orgZ);
        }
    }

    float VolumeRenderer::GetOriginX() {
        if(dataVolume != NULL) {
            return dataVolume->getOriginX();
        }
        return Renderer::getOriginX();
    }

    float VolumeRenderer::GetOriginY() {
        if(dataVolume != NULL) {
            return dataVolume->getOriginY();
        }
        return Renderer::getOriginY();
    }

    float VolumeRenderer::GetOriginZ() {
        if(dataVolume != NULL) {
            return dataVolume->getOriginZ();
        }
        return Renderer::getOriginZ();
    }

}

#endif
