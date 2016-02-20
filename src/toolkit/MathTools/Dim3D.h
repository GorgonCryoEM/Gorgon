/*
 * Dim3D.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_DIM3D_H_
#define SRC_TOOLKIT_DIM3D_H_

#include <iostream>

using namespace std;

namespace Toolkit {

  template<class T>
  class Dim3D{
    public:
      Dim3D();
      Dim3D(T val);
      Dim3D(T xx, T yy, T zz);
      virtual ~Dim3D();
      T X() const;
      T Y() const;
      T Z() const;
      T& operator[](int i);
      const T& operator[](int i) const;

    private:
      T x;
      T y;
      T z;

      friend ostream& operator<<(ostream& out, const Dim3D<T> &obj){
          return out<<"\t"<<obj.x
                    <<"\t"<<obj.y
                    <<"\t"<<obj.z
                    <<endl;
      }
  };

#include "Dim3D.tpl"

} /* namespace Toolkit */

#endif /* SRC_TOOLKIT_DIM3D_H_ */
