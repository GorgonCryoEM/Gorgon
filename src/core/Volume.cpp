/*
 * VolumeData.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */


#include "Volume.h"
#include <cmath>

using namespace Core;

Volume::Volume()
      : size(0, 0, 0)
{}

Volume::Volume(int sizeX, int sizeY, int sizeZ, double val)
      : size(sizeX, sizeY, sizeZ)
{
    setSize(sizeX, sizeY, sizeZ, val);
}

bool Volume::cmp(const Volume& obj) const {
  if(data.size() != obj.data.size())
    return false;
  else {
    double tolerance = 0.0001;
    int N = data.size();
    for(int i=0; i<N; ++i){
      double ratio = data[i]/obj.data[i];
      if(abs(ratio - 1.0) > tolerance)
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

int Volume::getIndex(int x, int y, int z) const {
    return (x * getSizeY() * getSizeZ() + y * getSizeZ() + z);
}

void Volume::setSize(int sizeX, int sizeY, int sizeZ, double val) {
    size = Dim3D<int>(sizeX, sizeY, sizeZ);
    data.resize(size.X() * size.Y() * size.Z(), val);
}
