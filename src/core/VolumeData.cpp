/*
 * VolumeData.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */


#include <cmath>
#include "VolumeData.h"

using namespace Core;

VolumeData::VolumeData()
      : size(0, 0, 0)
{}

VolumeData::VolumeData(int sizeX, int sizeY, int sizeZ, double val)
      : size(sizeX, sizeY, sizeZ)
{
    setSize(sizeX, sizeY, sizeZ, val);
}

VolumeData::iterator VolumeData::begin(){
    return data.begin();
}

VolumeData::iterator VolumeData::end(){
    return data.end();
}

VolumeData::const_iterator VolumeData::begin() const {
    return data.begin();
}

VolumeData::const_iterator VolumeData::end() const {
    return data.end();
}

void VolumeData::print() const{
    for(const_iterator it=data.begin(); it!=data.end(); ++it){
        cout<<*it<<endl;
    }
}

bool VolumeData::cmp(const VolumeData& obj) const {
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

void VolumeData::setItem(int i, int j, int k, double val){
    (*this)(i, j, k) = val;
}

double & VolumeData::operator()(int i, int j, int k){
    return const_cast<double &>(static_cast<const VolumeData&>(*this)(i, j, k));
}

const double & VolumeData::operator()(int i, int j, int k) const {
    return data[getIndex(i, j, k)];
}

int VolumeData::getIndex(int x, int y, int z) const {
    return (x * getSizeY() * getSizeZ() + y * getSizeZ() + z);
}

void VolumeData::setSize(int sizeX, int sizeY, int sizeZ, double val) {
    size = Dim3D<int>(sizeX, sizeY, sizeZ);
    data.resize(size.X() * size.Y() * size.Z(), val);
}
