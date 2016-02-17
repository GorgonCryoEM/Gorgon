/*
 * Vector3.h
 *
 *  Created on: Feb 16, 2016
 *      Author: shadow_walker
 */

#ifndef SRC_TOOLKIT_MATHTOOLS_VECTOR3_H_
#define SRC_TOOLKIT_MATHTOOLS_VECTOR3_H_

namespace GraphMatch {

    template <class T>
    class Vector3 {
        public:
            Vector3();
            Vector3(T _x, T _y, T _z);
            Vector3(const Vector3& v);
//            Vector3D(const vector<T>& vec);

//            T X() const;
//            T Y() const;
//            T Z() const;

            const T &operator[](int n) const;
            T &operator[](int n);
            T length() const;
            T lengthSquared() const;

            T operator*(const Vector3 &v) const; // Dot
            Vector3 operator^(const Vector3 &v) const; // Cross product
//            int XInt();
//            int YInt();
//            int ZInt();

            Vector3& operator=(const Vector3& a);
            Vector3& operator+=(const Vector3& a);
            Vector3& operator-=(const Vector3& a);
            Vector3& operator*=(T s);
//            Vector3& operator/=(T s);

            Vector3 operator-() const;
            Vector3 operator+() const;
            Vector3 operator+(const Vector3 &v) const;
            Vector3 operator-(const Vector3 &v) const;
            Vector3 operator/(const T s) const;
            Vector3 operator*(const T s) const;
//            Vector3D<T> operator+(const Vector3D<T> &d);
//            Vector3D<T> operator-();
//            Vector3D<T> operator-(const Vector3D<T> &d );
//            Vector3D<T> operator^(const Vector3D<T> &d );        // Cross Product
//            Vector3D<T> operator*(T s);


            bool operator==(const Vector3 &v) const;
            bool operator!=(const Vector3 &v) const;
            bool approxEqual(const Vector3 &v, T eps = 1e-12) const;
//            bool operator!=(Vector3D<T> &d);
//            bool operator==(Vector3D<T> &d);
//            bool operator>(Vector3D<T> &d);
//            bool operator<(Vector3D<T> &d);
//            bool operator>=(Vector3D<T> &d);
//            bool operator<=(Vector3D<T> &d);


            void normalize();
            void print() const;

//            Vector3D<T> GetOrthogonal();
//            Vector3D<T> Rotate(Vector3D<T> axis, T angle);
//            Vector3D<T> Transform(Matrix<T> transformation);
//            T * begin();
//            T * end();
//
//            void Normalize();
//            bool IsBadNormal();
//            void Print();
//
//            static Vector3D<T> Normalize(Vector3D<T> d);
//            static Vector3D<T> Project3Dto2D(Vector3D<T> point, Vector3D<T> planePt, Vector3D<T> planeVec1, Vector3D<T> planeVec2);


        private:
            T x, y, z;
//            T values[3];

//            friend ostream & operator<<(ostream & out, const Vector3D<T> & obj){
//                return out
//                        <<"{"
//                        <<obj.values[0]<<", "
//                        <<obj.values[1]<<", "
//                        <<obj.values[2]
//                        <<"}";
//            }
    };

//    typedef Vector3D<int>    Vector3DInt;
//    typedef Vector3D<float>  Vector3DFloat;
//    typedef Vector3D<double> Vector3DDouble;

    template <class T>
    Vector3<T>::Vector3()
            : x(0), y(0), z(0)
    {}

    template <class T>
    Vector3<T>::Vector3(const Vector3& v)
            : x(v[0]), y(v[1]), z(v[2])
    {}

    template <class T>
    Vector3<T>::Vector3(T _x, T _y, T _z)
            : x(_x), y(_y), z(_z)
    {}

    template <class T>
    Vector3& Vector3<T>::operator=(const Vector3& a) {
        x = a[0];
        y = a[1];
        z = a[2];
        return *this;
    }

    template <class T>
    const T& Vector3<T>::operator[](int n) const {
        return (&x)[n];
    }

    template <class T>
    T& Vector3<T>::operator[](int n) {
        return (&x)[n];
    }

    template <class T>
    template <class T>
    Vector3& Vector3<T>::operator+=(const Vector3& a) {
        x += a[0];
        y += a[1];
        z += a[2];
        return *this;
    }

    template <class T>
    Vector3& Vector3<T>::operator-=(const Vector3& a) {
        x -= a[0];
        y -= a[1];
        z -= a[2];
        return *this;
    }

    template <class T>
    Vector3& Vector3<T>::operator*=(T s) {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    template <class T>
    Vector3 Vector3<T>::operator-() const {
        return Vector3(-x, -y, -z);
    }

    template <class T>
    Vector3 Vector3<T>::operator+() const {
        return *this;
    }

    template <class T>
    Vector3 Vector3<T>::operator+(const Vector3 &v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    template <class T>
    Vector3 Vector3<T>::operator-(const Vector3 &v) const {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    template <class T>
    Vector3 Vector3<T>::operator/(const T s) const {
        assert(s > 0.0);
        return Vector3(x / s, y / s, z / s);
    }

    template <class T>
    Vector3 Vector3<T>::operator*(const T s) const {
        return Vector3(x * s, y * s, z * s);
    }

    // Dot
    template <class T>
    T Vector3<T>::operator*(const Vector3 &v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    // Cross product
    template <class T>
    Vector3 Vector3<T>::operator^(const Vector3 &v) const {
        return Vector3(y * v.z - z * v.y, z * v.x - x * v.z,
                x * v.y - y * v.x);
    }

    template <class T>
    T Vector3<T>::length() const {
        return (T)sqrt(x * x + y * y + z * z);
    }

    template <class T>
    T Vector3<T>::lengthSquared() const {
        return x * x + y * y + z * z;
    }

    template <class T>
    void Vector3<T>::normalize() {
        T s = 1.0 / (T)sqrt(x * x + y * y + z * z);
        x *= s;
        y *= s;
        z *= s;
    }

    template <class T>
    bool Vector3<T>::operator==(const Vector3 &v) const {
        return x == v.x && y == v.y && z == v.z;
    }

    template <class T>
    bool Vector3<T>::operator!=(const Vector3 &v) const {
        return x != v.x || y != v.y || z != v.z;
    }

    template <class T>
    bool Vector3<T>::approxEqual(const Vector3 &v, T eps) const {
        return isZero(x - v.x, eps) && isZero(y - v.y, eps)
               && isZero(z - v.z, eps);
    }

    template <class T>
    void Vector3<T>::print() const {
        std::cout << x << " " << y << " " << z << "\n";
    }


    template <class T>
    inline Vector3 operator*(const T s, const Vector3 &v) {
        return Vector3(v[0] * s, v[1] * s, v[2] * s);
    }

    template <class T>
    inline T dot(const Vector3 &w, const Vector3 &v) {
        return w * v;
    }

    template <class T>
    inline Vector3 cross(const Vector3 &w, const Vector3 &v) {
        return w ^ v;
    }

    template <class T>
    inline T length(const Vector3 &v) {
        return v.length();
    }

    template <class T>
    inline Vector3 unit(const Vector3 &v) {
        const T len = v.length();
        return v / len;
    }

    template <class T>
    inline std::ostream& operator<<(std::ostream& os, const Vector3& v) {
        os << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
        return os;
    }

}


#endif /* SRC_TOOLKIT_MATHTOOLS_VECTOR3_H_ */
