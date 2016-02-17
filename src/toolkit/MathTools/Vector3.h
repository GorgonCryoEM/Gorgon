/*
 * Vector3.h
 *
 *  Created on: Feb 16, 2016
 *      Author: shadow_walker
 */

#ifndef SRC_TOOLKIT_MATHTOOLS_VECTOR3_H_
#define SRC_TOOLKIT_MATHTOOLS_VECTOR3_H_

namespace GraphMatch {

    class Vector3 {
        public:
            Vector3();
            Vector3(double _x, double _y, double _z);
            Vector3(const Vector3& v);
//            Vector3D(const vector<T>& vec);

//            T X() const;
//            T Y() const;
//            T Z() const;

            const double &operator[](int n) const;
            double &operator[](int n);
            double length() const;
            double lengthSquared() const;

            double operator*(const Vector3 &v) const; // Dot
            Vector3 operator^(const Vector3 &v) const; // Cross product
//            int XInt();
//            int YInt();
//            int ZInt();

            Vector3& operator=(const Vector3& a);
            Vector3& operator+=(const Vector3& a);
            Vector3& operator-=(const Vector3& a);
            Vector3& operator*=(double s);
//            Vector3& operator/=(double s);

            Vector3 operator-() const;
            Vector3 operator+() const;
            Vector3 operator+(const Vector3 &v) const;
            Vector3 operator-(const Vector3 &v) const;
            Vector3 operator/(const double s) const;
            Vector3 operator*(const double s) const;
//            Vector3D<T> operator+(const Vector3D<T> &d);
//            Vector3D<T> operator-();
//            Vector3D<T> operator-(const Vector3D<T> &d );
//            Vector3D<T> operator^(const Vector3D<T> &d );        // Cross Product
//            Vector3D<T> operator*(double s);


            bool operator==(const Vector3 &v) const;
            bool operator!=(const Vector3 &v) const;
            bool approxEqual(const Vector3 &v, double eps = 1e-12) const;
//            bool operator!=(Vector3D<T> &d);
//            bool operator==(Vector3D<T> &d);
//            bool operator>(Vector3D<T> &d);
//            bool operator<(Vector3D<T> &d);
//            bool operator>=(Vector3D<T> &d);
//            bool operator<=(Vector3D<T> &d);


            void normalize();
            void print() const;

//            Vector3D<T> GetOrthogonal();
//            Vector3D<T> Rotate(Vector3D<T> axis, double angle);
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
            double x, y, z;
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

    Vector3::Vector3()
            : x(0), y(0), z(0)
    {}

    Vector3::Vector3(const Vector3& v)
            : x(v[0]), y(v[1]), z(v[2])
    {}

    Vector3::Vector3(double _x, double _y, double _z)
            : x(_x), y(_y), z(_z)
    {}

    Vector3& Vector3::operator=(const Vector3& a) {
        x = a[0];
        y = a[1];
        z = a[2];
        return *this;
    }

    const double& Vector3::operator[](int n) const {
        return (&x)[n];
    }

    double& Vector3::operator[](int n) {
        return (&x)[n];
    }

    Vector3& Vector3::operator+=(const Vector3& a) {
        x += a[0];
        y += a[1];
        z += a[2];
        return *this;
    }

    Vector3& Vector3::operator-=(const Vector3& a) {
        x -= a[0];
        y -= a[1];
        z -= a[2];
        return *this;
    }

    Vector3& Vector3::operator*=(double s) {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    Vector3 Vector3::operator-() const {
        return Vector3(-x, -y, -z);
    }

    Vector3 Vector3::operator+() const {
        return *this;
    }

    Vector3 Vector3::operator+(const Vector3 &v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    Vector3 Vector3::operator-(const Vector3 &v) const {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    Vector3 Vector3::operator/(const double s) const {
        assert(s > 0.0);
        return Vector3(x / s, y / s, z / s);
    }

    Vector3 Vector3::operator*(const double s) const {
        return Vector3(x * s, y * s, z * s);
    }

    // Dot
    double Vector3::operator*(const Vector3 &v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    // Cross product
    Vector3 Vector3::operator^(const Vector3 &v) const {
        return Vector3(y * v.z - z * v.y, z * v.x - x * v.z,
                x * v.y - y * v.x);
    }

    double Vector3::length() const {
        return (double)sqrt(x * x + y * y + z * z);
    }

    double Vector3::lengthSquared() const {
        return x * x + y * y + z * z;
    }

    void Vector3::normalize() {
        double s = 1.0 / (double)sqrt(x * x + y * y + z * z);
        x *= s;
        y *= s;
        z *= s;
    }

    bool Vector3::operator==(const Vector3 &v) const {
        return x == v.x && y == v.y && z == v.z;
    }

    bool Vector3::operator!=(const Vector3 &v) const {
        return x != v.x || y != v.y || z != v.z;
    }

    bool Vector3::approxEqual(const Vector3 &v, double eps) const {
        return isZero(x - v.x, eps) && isZero(y - v.y, eps)
               && isZero(z - v.z, eps);
    }

    void Vector3::print() const {
        std::cout << x << " " << y << " " << z << "\n";
    }


    inline Vector3 operator*(const double s, const Vector3 &v) {
        return Vector3(v[0] * s, v[1] * s, v[2] * s);
    }

    inline double dot(const Vector3 &w, const Vector3 &v) {
        return w * v;
    }

    inline Vector3 cross(const Vector3 &w, const Vector3 &v) {
        return w ^ v;
    }

    inline double length(const Vector3 &v) {
        return v.length();
    }

    inline Vector3 unit(const Vector3 &v) {
        const double len = v.length();
        return v / len;
    }

    inline std::ostream& operator<<(std::ostream& os, const Vector3& v) {
        os << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
        return os;
    }

}


#endif /* SRC_TOOLKIT_MATHTOOLS_VECTOR3_H_ */
