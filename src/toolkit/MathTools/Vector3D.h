#ifndef TOOLKIT_MATHTOOLS_VECTOR3DTEMPLATE_H
#define TOOLKIT_MATHTOOLS_VECTOR3DTEMPLATE_H

//#include <cstdio>
//#include <cmath>
#include "BasicDefines.h"
#include "Matrix.h"

namespace MathTools {
    template <class T> class
    Vector3D {
    public:
        Vector3D();
        Vector3D(T x, T y, T z);
        Vector3D(const vector<T>& vec);
        Vector3D(const Vector3D<T>& rhs);
        ~Vector3D();

        T X() const;
        T Y() const;
        T Z() const;
        T& operator[](int subscript);
        T operator[](int subscript) const;
        double Length();
        T operator*(const Vector3D<T> &d ); // Dot Product
        int XInt();
        int YInt();
        int ZInt();

        bool operator!=(Vector3D<T> &d);
        bool operator==(Vector3D<T> &d);
        bool operator>(Vector3D<T> &d);
        bool operator<(Vector3D<T> &d);
        bool operator>=(Vector3D<T> &d);
        bool operator<=(Vector3D<T> &d);

        Vector3D<T> operator+(const Vector3D<T> &d);
        Vector3D<T> operator-();
        Vector3D<T> operator-(const Vector3D<T> &d );
        Vector3D<T> operator^(const Vector3D<T> &d );		 // Cross Product
        Vector3D<T> operator*(double s);
        Vector3D<T>& operator=(const Vector3D<T>& d);
        Vector3D<T>& operator+=(const Vector3D<T>& d);
        Vector3D<T>& operator-=(const Vector3D<T>& d);
        Vector3D<T> GetOrthogonal();
        Vector3D<T> Rotate(Vector3D<T> axis, double angle);
        Vector3D<T> Transform(Matrix<T> transformation);
        T * begin();
        T * end();

        void Normalize();
        bool IsBadNormal();
        void Print();

        static Vector3D<T> Normalize(Vector3D<T> d);
        static Vector3D<T> Project3Dto2D(Vector3D<T> point, Vector3D<T> planePt, Vector3D<T> planeVec1, Vector3D<T> planeVec2);

        T values[3];

        friend ostream & operator<<(ostream & out, const Vector3D<T> & obj){
            return out
                    <<"{"
                    <<obj.values[0]<<", "
                    <<obj.values[1]<<", "
                    <<obj.values[2]
                    <<"}";
        }

        friend bool operator<(const Vector3D<T> & l, const Vector3D<T> & r){
            return (l.values[0]<r.values[0]) && (l.values[1]<r.values[1]) && (l.values[2]<r.values[2]);
        }
    };

    typedef Vector3D<int>    Vector3DInt;
    typedef Vector3D<float>  Vector3DFloat;
    typedef Vector3D<double> Vector3DDouble;

    template <class T>
    Vector3D<T>::Vector3D() {
        values[0] = (T)0.0;
        values[1] = (T)0.0;
        values[2] = (T)0.0;
    }

    template <class T>
    Vector3D<T>::Vector3D(T x, T y, T z) {
        values[0] = x;
        values[1] = y;
        values[2] = z;
    }

    template <class T>
    Vector3D<T>::Vector3D(const vector<T>& vec) {
        values[0] = vec[0];
        values[1] = vec[1];
        values[2] = vec[2];
    }

    template <class T>
    Vector3D<T>::Vector3D(const Vector3D<T>& rhs) {
        values[0] = rhs.values[0];
        values[1] = rhs.values[1];
        values[2] = rhs.values[2];
    }

    template <class T>
    Vector3D<T>::~Vector3D() {
    }


    template <class T>
    bool Vector3D<T>::operator!=(Vector3D<T> &d) {
        return (X() != d.X()) || (Y() != d.Y()) || (Z() != d.Z());
    }

    template <class T>
    bool Vector3D<T>::operator==(Vector3D<T> &d) {
        return (X() == d.X()) && (Y() == d.Y()) && (Z() == d.Z());
    }

    template <class T>
    bool Vector3D<T>::operator>(Vector3D<T> &d) {
        return (X() > d.X()) ||
            ((X() == d.X()) && (Y() > d.Y())) ||
            ((X() == d.X()) && (Y() == d.Y()) && (Z() > d.Y()));
    }

    template <class T>
    bool Vector3D<T>::operator<(Vector3D<T> &d) {
        return d > *this;
    }

    template <class T>
    bool Vector3D<T>::operator>=(Vector3D<T> &d) {
        return (*this > d) || (*this == d);
    }

    template <class T>
    bool Vector3D<T>::operator<=(Vector3D<T> &d) {
        return (*this < d) || (*this == d);
    }

    template <class T>
    T Vector3D<T>::X() const {
        return values[0];
    }

    template <class T>
    T Vector3D<T>::Y() const {
        return values[1];
    }

    template <class T>
    T Vector3D<T>::Z() const {
        return values[2];
    }
    template <class T>
    T& Vector3D<T>::operator[](int subscript) {
        if (subscript < 0 || subscript > 2)
            throw "Vector3D [] operator BoundsError!\n";
        else
            return values[subscript];
    }
    template <class T>
    T Vector3D<T>::operator[](int subscript) const {
        if (subscript < 0 || subscript > 2)
            throw "Vector3D [] operator BoundsError!\n";
        else
            return values[subscript];
    }

    template <class T>
    double Vector3D<T>::Length() {
        return (double)sqrt((double)(values[0] * values[0] + values[1] * values[1] + values[2] * values[2]));
    }

    template <class T>
    int Vector3D<T>::XInt() {
        return (int)round(values[0]);
    }

    template <class T>
    int Vector3D<T>::YInt() {
        return (int)round(values[1]);
    }

    template <class T>
    int Vector3D<T>::ZInt() {
        return (int)round(values[2]);
    }

    template <class T>
    T Vector3D<T>::operator*(const Vector3D<T> &d ) {		// Dot Product
        return X() * d.X() + Y() * d.Y() + Z() * d.Z();
    }

    template <class T>
    Vector3D<T> Vector3D<T>::operator+(const Vector3D<T> &d) {
        return Vector3D<T>(X() + d.X(), Y() + d.Y(), Z() + d.Z());
    }

    template <class T>
    Vector3D<T> Vector3D<T>::operator-() {
        return Vector3D<T>(-X(), -Y(), -Z());
    }

    template <class T>
    Vector3D<T> Vector3D<T>::operator-(const Vector3D<T> &d ) {
        return Vector3D<T>(X() - d.X(), Y() - d.Y(), Z() - d.Z());
    }

    template <class T>
    Vector3D<T> Vector3D<T>::operator^(const Vector3D<T> &d ) { // Cross Product
        return Vector3D<T>(
            values[1] * d.values[2] - values[2] * d.values[1],
            values[2] * d.values[0] - values[0] * d.values[2],
            values[0] * d.values[1] - values[1] * d.values[0]);
    }

    template <class T>
    Vector3D<T> Vector3D<T>::operator*(double s) {
        return Vector3D<T>((T)(X()*s), (T)(Y()*s), (T)(Z()*s));
    }

    template <class T>
    Vector3D<T>& Vector3D<T>::operator+=(const Vector3D<T>& d) {
        for(int i = 0; i < 3; i++) {
            values[i] += d.values[i];
        }
        return *this;
    }

    template <class T>
    Vector3D<T>& Vector3D<T>::operator-=(const Vector3D<T>& d) {
        for(int i = 0; i < 3; i++) {
            values[i] -= d.values[i];
        }
        return *this;
    }
    template <class T>
    Vector3D<T>& Vector3D<T>::operator=(const Vector3D<T>& d) {
        for(int i = 0; i < 3; i++) {
            values[i] = d.values[i];
        }
        return *this;
    }

    /** Add this function to make it iterable in Python,
     * so we can call list() or tuple() to convert Vec3f in python to a list or tuple.
     *
     * @return the iterator (here is the pointer) of the first element
     * */
    template <class T>
    T * Vector3D<T>::begin() {
        return &values[0];
    }

    /** Add this function to make it iterable in Python,
     * so we can call list() or tuple() to convert Vec3f in python to a list or tuple.
     *
     * @return the iterator (here is the pointer) of the one beyond the last element.
     * */
    template <class T>
    T * Vector3D<T>::end() {
        return &values[2];
    }

    template <class T>
    void Vector3D<T>::Normalize() {
        double base = Length();
        if(base == 0) {
            values[0] = 0;
            values[1] = 0;
            values[2] = 0;
        } else {
            values[0] = (T)((double)values[0]/base);
            values[1] = (T)((double)values[1]/base);
            values[2] = (T)((double)values[2]/base);
        }
    }

    template <class T>
    Vector3D<T> Vector3D<T>::Normalize(Vector3D<T> d) {
        Vector3D<T> ret = d;
        ret.Normalize();
        return ret;
    }

    template <class T>
    Vector3D<T> Vector3D<T>::GetOrthogonal() {
        Vector3D<T> orthVec = Vector3D<T>(1, 1, 1);
        orthVec = Vector3D<T>(this->X(), this->Y(), this->Z()) ^ orthVec;
        if(isZero(orthVec.Length())) {
            orthVec = Vector3D<T>(1, -1, -1);
            orthVec = Vector3D<T>(this->X(), this->Y(), this->Z()) ^ orthVec;
        }
        return orthVec;
    }

    template <class T>
    Vector3D<T> Vector3D<T>::Rotate(Vector3D<T> axis, double angle) {
        double r = angle;
        T a = axis.values[0];
        T b = axis.values[1];
        T c = axis.values[2];

        T q0 = (T)cos(r/2.0);
        T q1 = (T)sin(r/2.0)* a;
        T q2 = (T)sin(r/2.0)* b;
        T q3 = (T)sin(r/2.0)* c;


        T R[3][3] = {{q0*q0 + q1*q1 - q2*q2 - q3*q3,		2*(q1*q2 - q0*q3),					2*(q1*q3 + q0*q2)},
                          {2*(q2*q1 + q0*q3),					q0*q0 - q1*q1 + q2*q2 - q3*q3,		2*(q2*q3 - q0*q1)},
                          {2*(q3*q1 - q0*q2),					2*(q3*q2 + q0*q1),					q0*q0 - q1*q1 - q2*q2 + q3*q3}};


        Vector3D<T> v = Vector3D(0.0, 0.0, 0.0);
        for(int x = 0; x < 3; x++) {
            for(int y = 0; y < 3; y++) {
                v.values[x] = v.values[x] + R[y][x] * values[y];
            }
        }
        return v;
    }

    template <class T>
    bool Vector3D<T>::IsBadNormal() {
        return !isZero(Length() - 1.0, 0.00001);
    }

    template <class T>
    Vector3D<T> Vector3D<T>::Project3Dto2D(Vector3D<T> point, Vector3D<T> planePt, Vector3D<T> planeVec1, Vector3D<T> planeVec2) {
        Vector3D<T> ray = point - planePt;
        return Vector3D<T>(ray * planeVec1, ray * planeVec2, 0);
        //return Vector3DTemplate<T>(point * planeVec1, point * planeVec2, 0);

    }

    template <class T>
    void Vector3D<T>::Print() {
        printf("{%f, %f, %f} \n", X(), Y(), Z());
    }

    template <class T>
    Vector3D<T> Vector3D<T>::Transform(Matrix<T> transformation) {
        Matrix<T> p = Matrix<T>(4, 1);
        p.SetValue(values[0], 0, 0);
        p.SetValue(values[1], 1, 0);
        p.SetValue(values[2], 2, 0);
        p.SetValue((T)1, 3, 0);

        p = transformation * p;
        return Vector3D<T>(p.GetValue(0,0), p.GetValue(1,0), p.GetValue(2,0));
    }
}

#endif

