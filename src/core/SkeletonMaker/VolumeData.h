#ifndef CORE_SKELETON_MAKER_VOLUME_DATA_H
#define CORE_SKELETON_MAKER_VOLUME_DATA_H

#include <vector>
#include "Dim3D.h"

using namespace std;
using namespace Core;

namespace SkeletonMaker {
    class VolumeData {
    public:
        VolumeData();
        VolumeData(int sizeX, int sizeY, int sizeZ);
        VolumeData(int sizeX, int sizeY, int sizeZ, float val);
        VolumeData(int sizeX, int sizeY, int sizeZ, int offsetX, int offsetY, int offsetZ, VolumeData * data);
        ~VolumeData();

        int getSizeX();
        int getSizeY();
        int getSizeZ();
        int getSize();
        Dim3D<int> getSizeObj();
        float getSpacingX();
        float getSpacingY();
        float getSpacingZ();
        float getOriginX();
        float getOriginY();
        float getOriginZ();
        float getDataAt(int x, int y, int z);
        float getDataAt(int index);
        int getIndex(int x, int y, int z);
        int getMaxIndex();
        //uses malloc as required by FFT libraries
        // :WARNING: Update: no malloc anymore, data is a vector
        // malloc allocation will be done in a wrapper, if absolutely necessary
        vector<float> GetArrayCopy(int padX=0, int padY=0, int padZ=0, float padValue=0);

        void setSpacing(float spacingX, float spacingY, float spacingZ);
        void setOrigin(float originX, float originY, float originZ);
        void setDataAt(int x, int y, int z, float value);
        void setDataAt(int index, float value);
        void Pad(int padBy, double padValue);
    private:
        void InitializeVolumeData(int sizeX, int sizeY, int sizeZ, float spacingX, float spacingY, float spacingZ, float originX, float originY, float originZ, bool initializeData, float val);
        void setSize(int sizeX, int sizeY, int sizeZ);
    protected:
        Dim3D<int> size;
        Dim3D<float> spacing;
        Dim3D<float> origin;
        vector<float> data;
    };

    VolumeData::VolumeData() {
      InitializeVolumeData(0, 0, 0, 1, 1, 1, 0, 0, 0, true, 0);
    }

    VolumeData::VolumeData(int sizeX, int sizeY, int sizeZ) {
        InitializeVolumeData(sizeX, sizeY, sizeZ, 1, 1, 1, 0, 0, 0, true, 0);
    }

    VolumeData::VolumeData(int sizeX, int sizeY, int sizeZ, float val) {
        InitializeVolumeData(sizeX, sizeY, sizeZ, 1, 1, 1, 0, 0, 0, true, val);
    }

    VolumeData::VolumeData(int sizeX, int sizeY, int sizeZ, int offsetX, int offsetY, int offsetZ, VolumeData * data) {
        InitializeVolumeData(sizeX, sizeY, sizeZ, data->getSpacingX(), data->getSpacingY(), data->getSpacingZ(), data->getOriginX(), data->getOriginY(), data->getOriginZ(), false, 0);
        int ct = 0 ;
        for (int i = offsetX; i < sizeX + offsetX; i++) {
            for (int j = offsetY; j < sizeY + offsetY; j++ ) {
                for ( int k = offsetZ; k < sizeZ + offsetZ; k++) {
                    this->data[ct] = data->getDataAt(i, j, k);
                    ct++;
                }
            }
        }
    }

    VolumeData::~VolumeData() {}

    void VolumeData::InitializeVolumeData(int sizeX, int sizeY, int sizeZ, float spacingX, float spacingY, float spacingZ, float originX, float originY, float originZ, bool initializeData, float val) {
        setSize(sizeX, sizeY, sizeZ);
        setSpacing(spacingX, spacingY, spacingZ);
        setOrigin(originX, originY, originZ);
        int maxIndex = getMaxIndex();
        data.resize(maxIndex);
        if(initializeData) {
            for (int i=0; i < maxIndex; i++) {
                data[i] = val;
            }
        }
    }

    int VolumeData::getSizeX() {
        return size.X();
    }

    int VolumeData::getSizeY() {
        return size.Y();
    }

    int VolumeData::getSizeZ() {
        return size.Z();
    }

    int VolumeData::getSize() {
        return data.size();
    }

    Dim3D<int> VolumeData::getSizeObj() {
        return size;
    }

    float VolumeData::getSpacingX() {
        return spacing.X();
    }

    float VolumeData::getSpacingY() {
        return spacing.Y();
    }

    float VolumeData::getSpacingZ() {
        return spacing.Z();
    }

    float VolumeData::getOriginX() {
        return origin.X();
    }

    float VolumeData::getOriginY() {
        return origin.Y();
    }

    float VolumeData::getOriginZ() {
        return origin.Z();
    }


    float VolumeData::getDataAt(int x, int y, int z) {
        return getDataAt(getIndex(x, y, z));
    }

    float VolumeData::getDataAt(int index) {
        return data[index];
    }

    int VolumeData::getIndex(int x, int y, int z) {
        return (x * getSizeY() * getSizeZ() + y * getSizeZ() + z);
    }

    int VolumeData::getMaxIndex() {
        return size.X() * size.Y() * size.Z();
    }

    void VolumeData::setSpacing(float spacingX, float spacingY, float spacingZ) {
        spacing = Dim3D<float>(spacingX, spacingY, spacingZ);
    }

    void VolumeData::setOrigin(float originX, float originY, float originZ) {
        origin = Dim3D<float>(originX, originY, originZ);
    }


    void VolumeData::setSize(int sizeX, int sizeY, int sizeZ) {
        size = Dim3D<int>(sizeX, sizeY, sizeZ);
        data.resize(getMaxIndex());
    }

    void VolumeData::setDataAt(int x, int y, int z, float value) {
        setDataAt(getIndex(x, y, z), value);
    }

    void VolumeData::setDataAt(int index, float value) {
        data[index] = value;
    }
    void VolumeData::Pad(int padBy, double padValue) {
        int sizex = getSizeX();
        int sizey = getSizeY();
        int sizez = getSizeZ();
        int newSizeX = sizex + 2*padBy;
        int newSizeY = sizey + 2*padBy;
        int newSizeZ = sizez + 2*padBy;

        vector<float> newData(newSizeX * newSizeY * newSizeZ);
        double value;


        for(int x = 0; x < newSizeX; x++) {
            for(int y = 0; y < newSizeY; y++) {
                for(int z = 0; z < newSizeZ; z++) {
                    if ((x < padBy) || (y < padBy) || (z < padBy) || (x >= padBy + sizex) || (y >= padBy + sizey) || (z >= padBy + sizez)) {
                        value = padValue;
                    } else {
                        value = getDataAt(x-padBy, y-padBy, z-padBy);
                    }

                    newData[x * newSizeY * newSizeZ + y * newSizeZ + z] = (float)value;
                }
            }
        }
        data = newData;
        setSize(newSizeX, newSizeY, newSizeZ);

    }


    vector<float> VolumeData::GetArrayCopy(int padX, int padY, int padZ, float padValue) {
        int xSize = getSizeX()+padX;
        int ySize = getSizeY()+padY;
        int zSize = getSizeZ()+padZ;
        vector<float> copy(xSize*ySize*zSize);

        for (int i=0; i < xSize; i++)
            for (int j=0; j < ySize; j++)
                for (int k=0; k < zSize; k++) {
                    if ( i<getSizeX() && j<getSizeY() && k<getSizeZ() ) {
                        copy[k+(j+i*ySize)*zSize] = getDataAt(i, j, k);
                    } else {
                        copy[k+(j+i*ySize)*zSize] = padValue;
                    }

                }

        return copy;
    }
}


#endif
