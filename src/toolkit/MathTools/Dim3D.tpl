/*
 * Dim3D.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

//template<class T>
//ostream& operator<<(ostream& out, const Dim3D<T> &obj){
//    return out<<"\t"<<obj.x
//              <<"\t"<<obj.y
//              <<"\t"<<obj.z
//              <<endl;
//}

template<class T>
inline Dim3D<T>::Dim3D()
{}

template<class T>
inline Dim3D<T>::~Dim3D(){}

template<class T>
inline Dim3D<T>::Dim3D(T val)
            : x(val), y(val), z(val)
{}

template<class T>
inline Dim3D<T>::Dim3D(T xx, T yy, T zz)
            : x(xx), y(yy), z(zz)
{}

template<class T>
inline T Dim3D<T>::X() const{
    return x;
}

template<class T>
inline T Dim3D<T>::Y() const{
    return y;
}

template<class T>
inline T Dim3D<T>::Z() const{
    return z;
}

template<class T>
T& Dim3D<T>::operator[](int i){
    return const_cast<T&>(static_cast<const Dim3D<T>& >(*this)[i]);
}

template<class T>
const T& Dim3D<T>::operator[](int i) const{
    switch(i) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        default:
            throw "Not valid Dim3D index";
    }
}
