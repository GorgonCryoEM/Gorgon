/*
 * VolumeData.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */


#include "Volume.h"

using namespace Core;

Volume::Volume()
      : size(0, 0, 0),
        spacing(1, 1, 1),
        origin(0, 0, 0)
{}

Volume::Volume(int sizeX, int sizeY, int sizeZ, float val)
      : size(sizeX, sizeY, sizeZ),
        spacing(1, 1, 1),
        origin(0, 0, 0)
{
    setSize(sizeX, sizeY, sizeZ);
    data.assign(data.size(), val);
}

bool Volume::cmp(const Volume& obj) const {
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

int Volume::getSizeX() const {
    return size.X();
}

int Volume::getSizeY() const {
    return size.Y();
}

int Volume::getSizeZ() const {
    return size.Z();
}

int Volume::getSize() const {
    return data.size();
}

Dim3D<int> Volume::getSizeObj() const {
    return size;
}

float Volume::getSpacingX() const {
    return spacing.X();
}

float Volume::getSpacingY() const {
    return spacing.Y();
}

float Volume::getSpacingZ() const {
    return spacing.Z();
}

float Volume::getOriginX() const {
    return origin.X();
}

float Volume::getOriginY() const {
    return origin.Y();
}

float Volume::getOriginZ() const {
    return origin.Z();
}

float Volume::getDataAt(int x, int y, int z) const {
    return getDataAt(getIndex(x, y, z));
}

float Volume::getDataAt(int index) const {
    return data[index];
}

int Volume::getIndex(int x, int y, int z) const {
    return (x * getSizeY() * getSizeZ() + y * getSizeZ() + z);
}

int Volume::getMaxIndex() const {
    return size.X() * size.Y() * size.Z();
}

void Volume::setSpacing(float spacingX, float spacingY, float spacingZ) {
    spacing = Dim3D<float>(spacingX, spacingY, spacingZ);
}

void Volume::setOrigin(float originX, float originY, float originZ) {
    origin = Dim3D<float>(originX, originY, originZ);
}

void Volume::setSpacing(Dim3D<float> val) {
    spacing = val;
}

void Volume::setOrigin(Dim3D<float> val) {
    origin = val;
}


void Volume::setSize(int sizeX, int sizeY, int sizeZ) {
    size = Dim3D<int>(sizeX, sizeY, sizeZ);
    data.resize(getMaxIndex());
}

void Volume::setDataAt(int x, int y, int z, float value) {
    setDataAt(getIndex(x, y, z), value);
}

void Volume::setDataAt(int index, float value) {
    data[index] = value;
}
