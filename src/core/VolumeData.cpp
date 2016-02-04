/*
 * VolumeData.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */


#include "VolumeData.h"

VolumeData::VolumeData()
      : size(0, 0, 0),
        spacing(1, 1, 1),
        origin(0, 0, 0)
{}

VolumeData::VolumeData(int sizeX, int sizeY, int sizeZ, float val)
      : size(sizeX, sizeY, sizeZ),
        spacing(1, 1, 1),
        origin(0, 0, 0)
{
    setSize(sizeX, sizeY, sizeZ);
    data.assign(data.size(), val);
}

bool VolumeData::cmp(const VolumeData& obj) const {
  if(data.size() != obj.data.size())
    return false;
  else {
    double tolerance = 0.0001;
    int N = data.size();
    for(int i=0; i<N; ++i){
      double ratio = data[i]/obj.data[i];
      if(ratio - 1.0 > tolerance)
        return false;
    }
  }
    return true;
}

int VolumeData::getSizeX() const {
    return size.X();
}

int VolumeData::getSizeY() const {
    return size.Y();
}

int VolumeData::getSizeZ() const {
    return size.Z();
}

int VolumeData::getSize() const {
    return data.size();
}

Dim3D<int> VolumeData::getSizeObj() const {
    return size;
}

float VolumeData::getSpacingX() const {
    return spacing.X();
}

float VolumeData::getSpacingY() const {
    return spacing.Y();
}

float VolumeData::getSpacingZ() const {
    return spacing.Z();
}

float VolumeData::getOriginX() const {
    return origin.X();
}

float VolumeData::getOriginY() const {
    return origin.Y();
}

float VolumeData::getOriginZ() const {
    return origin.Z();
}

float VolumeData::getDataAt(int x, int y, int z) const {
    return getDataAt(getIndex(x, y, z));
}

float VolumeData::getDataAt(int index) const {
    return data[index];
}

int VolumeData::getIndex(int x, int y, int z) const {
    return (x * getSizeY() * getSizeZ() + y * getSizeZ() + z);
}

int VolumeData::getMaxIndex() const {
    return size.X() * size.Y() * size.Z();
}

void VolumeData::setSpacing(float spacingX, float spacingY, float spacingZ) {
    spacing = Dim3D<float>(spacingX, spacingY, spacingZ);
}

void VolumeData::setOrigin(float originX, float originY, float originZ) {
    origin = Dim3D<float>(originX, originY, originZ);
}

void VolumeData::setSpacing(Dim3D<float> val) {
    spacing = val;
}

void VolumeData::setOrigin(Dim3D<float> val) {
    origin = val;
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
void VolumeData::pad(int padBy, double padValue) {
  #ifdef GORGON_DEBUG
        cout<<"\033[36mDEBUG: File:   VolumeData.h"<<endl;
        cout<<"DEBUG: Method: VolumeData::pad\033[0m"<<endl;
        cout<<"DEBUG: Args: int, double\033[0m"<<endl;
        cout<<getSize()<<endl;

        cout<<*this;
  #endif

    int sizex = getSizeX();
    int sizey = getSizeY();
    int sizez = getSizeZ();
    int newSizeX = sizex + 2*padBy;
    int newSizeY = sizey + 2*padBy;
    int newSizeZ = sizez + 2*padBy;

#ifdef GORGON_DEBUG
    cout<<"\033[31m";
    cout<<" "<<sizex
        <<" "<<sizey
        <<" "<<sizez
        <<" "<<endl;
    cout<<"\033[0m";

    cout<<"\033[32m";
    cout<<" "<<newSizeX
        <<" "<<newSizeY
        <<" "<<newSizeZ
        <<" "<<endl;
    cout<<"\033[0m";

    cout<<"\033[33m";
    cout<<" "<<padBy
        <<" "<<endl;
    cout<<"\033[0m";
#endif


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
    size = Dim3D<int>(newSizeX, newSizeY, newSizeZ);
#ifdef GORGON_DEBUG
    cout<<*this;
#endif
}

