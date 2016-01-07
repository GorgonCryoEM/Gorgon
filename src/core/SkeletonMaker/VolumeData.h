#ifndef CORE_SKELETON_MAKER_VOLUME_DATA_H
#define CORE_SKELETON_MAKER_VOLUME_DATA_H

#include <cstdlib>
using std::malloc;

#include "Dim3D.h"

using namespace Core;

namespace SkeletonMaker {
    class VolumeData {
    public:
        VolumeData(int sizeX, int sizeY, int sizeZ);
        VolumeData(int sizeX, int sizeY, int sizeZ, float val);
        VolumeData(int sizeX, int sizeY, int sizeZ, int offsetX, int offsetY, int offsetZ, VolumeData * data);
        VolumeData(VolumeData& obj);
        ~VolumeData();

        int GetSizeX();
        int GetSizeY();
        int GetSizeZ();
        float GetSpacingX();
        float GetSpacingY();
        float GetSpacingZ();
        float GetOriginX();
        float GetOriginY();
        float GetOriginZ();
        float GetDataAt(int x, int y, int z);
        float GetDataAt(int index);
        int GetIndex(int x, int y, int z);
        int GetMaxIndex();
        float* GetArrayCopy(int padX=0, int padY=0, int padZ=0, float padValue=0); //uses malloc as required by FFT libraries

        void SetSpacing(float spacingX, float spacingY, float spacingZ);
        void SetOrigin(float originX, float originY, float originZ);
        void SetDataAt(int x, int y, int z, float value);
        void SetDataAt(int index, float value);
        void Pad(int padBy, double padValue);
    private:
        void InitializeVolumeData(int sizeX, int sizeY, int sizeZ, float spacingX, float spacingY, float spacingZ, float originX, float originY, float originZ, bool initializeData, float val);
        void SetSize(int sizeX, int sizeY, int sizeZ);
    private:
        Dim3D<int> size;
        Dim3D<float> spacing;
        Dim3D<float> origin;
        float * data;
    };

    VolumeData::VolumeData(VolumeData& obj) {
        size = obj.size;
        spacing = obj.spacing;
        origin = obj.origin;

        int N = size.X()*size.Z()*size.Z();
        data = new float[N];
        for (int i = 0; i < N; i++) {
            data[i] = obj.data[i];
        }
    }

    VolumeData::VolumeData(int sizeX, int sizeY, int sizeZ) {
        InitializeVolumeData(sizeX, sizeY, sizeZ, 1, 1, 1, 0, 0, 0, true, 0);
    }

    VolumeData::VolumeData(int sizeX, int sizeY, int sizeZ, float val) {
        InitializeVolumeData(sizeX, sizeY, sizeZ, 1, 1, 1, 0, 0, 0, true, val);
    }

    VolumeData::VolumeData(int sizeX, int sizeY, int sizeZ, int offsetX, int offsetY, int offsetZ, VolumeData * data) {
        InitializeVolumeData(sizeX, sizeY, sizeZ, data->GetSpacingX(), data->GetSpacingY(), data->GetSpacingZ(), data->GetOriginX(), data->GetOriginY(), data->GetOriginZ(), false, 0);
        int ct = 0 ;
        for (int i = offsetX; i < sizeX + offsetX; i++) {
            for (int j = offsetY; j < sizeY + offsetY; j++ ) {
                for ( int k = offsetZ; k < sizeZ + offsetZ; k++) {
                    this->data[ct] = data->GetDataAt(i, j, k);
                    ct++;
                }
            }
        }
    }

    VolumeData::~VolumeData() {
        delete [] data;
    }

    void VolumeData::InitializeVolumeData(int sizeX, int sizeY, int sizeZ, float spacingX, float spacingY, float spacingZ, float originX, float originY, float originZ, bool initializeData, float val) {
        SetSize(sizeX, sizeY, sizeZ);
        SetSpacing(spacingX, spacingY, spacingZ);
        SetOrigin(originX, originY, originZ);
        int maxIndex = GetMaxIndex();
        data = new float [maxIndex];
        if(initializeData) {
            for (int i=0; i < maxIndex; i++) {
                data[i] = val;
            }
        }
    }

    int VolumeData::GetSizeX() {
        return size.X();
    }

    int VolumeData::GetSizeY() {
        return size.Y();
    }

    int VolumeData::GetSizeZ() {
        return size.Z();
    }

    float VolumeData::GetSpacingX() {
        return spacing.X();
    }

    float VolumeData::GetSpacingY() {
        return spacing.Y();
    }

    float VolumeData::GetSpacingZ() {
        return spacing.Z();
    }

    float VolumeData::GetOriginX() {
        return origin.X();
    }

    float VolumeData::GetOriginY() {
        return origin.Y();
    }

    float VolumeData::GetOriginZ() {
        return origin.Z();
    }


    float VolumeData::GetDataAt(int x, int y, int z) {
        return GetDataAt(GetIndex(x, y, z));
    }

    float VolumeData::GetDataAt(int index) {
        return data[index];
    }

    int VolumeData::GetIndex(int x, int y, int z) {
        return (x * GetSizeY() * GetSizeZ() + y * GetSizeZ() + z);
    }

    int VolumeData::GetMaxIndex() {
        return size.X() * size.Y() * size.Z();
    }

    void VolumeData::SetSpacing(float spacingX, float spacingY, float spacingZ) {
        spacing = Dim3D<float>(spacingX, spacingY, spacingZ);
    }

    void VolumeData::SetOrigin(float originX, float originY, float originZ) {
        origin = Dim3D<float>(originX, originY, originZ);
    }


    void VolumeData::SetSize(int sizeX, int sizeY, int sizeZ) {
        size = Dim3D<int>(sizeX, sizeY, sizeZ);
    }

    void VolumeData::SetDataAt(int x, int y, int z, float value) {
        SetDataAt(GetIndex(x, y, z), value);
    }

    void VolumeData::SetDataAt(int index, float value) {
        data[index] = value;
    }
    void VolumeData::Pad(int padBy, double padValue) {
        int sizex = GetSizeX();
        int sizey = GetSizeY();
        int sizez = GetSizeZ();
        int newSizeX = sizex + 2*padBy;
        int newSizeY = sizey + 2*padBy;
        int newSizeZ = sizez + 2*padBy;

        float * newData = new float[newSizeX * newSizeY * newSizeZ];
        double value;


        for(int x = 0; x < newSizeX; x++) {
            for(int y = 0; y < newSizeY; y++) {
                for(int z = 0; z < newSizeZ; z++) {
                    if ((x < padBy) || (y < padBy) || (z < padBy) || (x >= padBy + sizex) || (y >= padBy + sizey) || (z >= padBy + sizez)) {
                        value = padValue;
                    } else {
                        value = GetDataAt(x-padBy, y-padBy, z-padBy);
                    }

                    newData[x * newSizeY * newSizeZ + y * newSizeZ + z] = (float)value;
                }
            }
        }
        delete [] data;
        data = newData;
        SetSize(newSizeX, newSizeY, newSizeZ);

    }


    float* VolumeData::GetArrayCopy(int padX, int padY, int padZ, float padValue) {
        int xSize = GetSizeX()+padX;
        int ySize = GetSizeY()+padY;
        int zSize = GetSizeZ()+padZ;
        float* copy = (float*) malloc(sizeof(float)*xSize*ySize*zSize);

        for (int i=0; i < xSize; i++)
            for (int j=0; j < ySize; j++)
                for (int k=0; k < zSize; k++) {
                    if ( i<GetSizeX() && j<GetSizeY() && k<GetSizeZ() ) {
                        copy[k+(j+i*ySize)*zSize] = GetDataAt(i, j, k);
                    } else {
                        copy[k+(j+i*ySize)*zSize] = padValue;
                    }

                }

        return copy;
    }
}


#endif
