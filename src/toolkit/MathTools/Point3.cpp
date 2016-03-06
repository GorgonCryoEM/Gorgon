/*
 * Point3.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "Point3.h"

namespace MathTools {

    Point3::Point3() : x(0), y(0), z(0) {}

    Point3::~Point3() {
        // TODO Auto-generated destructor stub
    }

    Point3::Point3(const Point3& p) : x(p[0]), y(p[1]), z(p[2]) {}
    Point3::Point3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}

    Point3& Point3::operator=(const Point3& a) {
        x = a[0]; y = a[1]; z = a[2];
        return *this;
    }

    const double & Point3::operator[](int n) const { return (&x)[n]; }
          double & Point3::operator[](int n)       { return (&x)[n]; }

    Point3& Point3::operator+=(const Vector3<double>& v) {
        x += v[0]; y += v[1]; z += v[2];
        return *this;
    }

    Point3& Point3::operator-=(const Vector3<double>& v) {
        x -= v[0]; y -= v[1]; z -= v[2];
        return *this;
    }

    Point3& Point3::operator*=(double s) {
        x *= s; y *= s; z *= s;
        return *this;
    }

    Vector3<double> Point3::operator-(const Point3 & p) const {
        return Vector3<double>(x - p.x, y - p.y, z - p.z);
    }

    Point3 Point3::operator+(const Vector3<double> & v) const {
        return Point3(x + v[0], y + v[1], z + v[2]);
    }

    Point3 Point3::operator-(const Vector3<double> & v) const {
        return Point3(x - v[0], y - v[1], z - v[2]);
    }

    double Point3::distanceTo(const Point3& p) const {
        return (double) sqrt((p[0] - x) * (p[0] - x) +
                             (p[1] - y) * (p[1] - y) +
                             (p[2] - z) * (p[2] - z));
    }

    double Point3::distanceTo(const Vec3F& v) const {
        return (double) sqrt((v[0] - x) * (v[0] - x) +
                             (v[1] - y) * (v[1] - y) +
                             (v[2] - z) * (v[2] - z));
    }

    double Point3::distanceToSquared(const Point3& p) const {
        return ((p[0] - x) * (p[0] - x) +
                (p[1] - y) * (p[1] - y) +
                (p[2] - z) * (p[2] - z));
    }

    double Point3::distanceFromOrigin() const {
        return (double) sqrt(x * x + y * y + z * z);
    }

    double Point3::distanceFromOriginSquared() const {
        return x * x + y * y + z * z;
    }

    bool Point3::operator==( const Point3 &p ) const {
        return x == p.x && y == p.y && z == p.z;
    }

    bool Point3::operator!=( const Point3 &p ) const {
        return x != p.x || y != p.y || z != p.z;
    }

    bool Point3::approxEqual( const Point3 &p, double eps) const {
        return isZero( x - p.x, eps ) && isZero( y - p.y, eps ) && isZero( z - p.z, eps );
    }

    void Point3::print() const {
        std::cout << x << " " << y << " " << z << "\n";
    }

    inline Point3 lerp( const Point3 &p0, const Point3 &p1, double dT )
    {
        const double dTMinus = 1.0 - dT;
        return Point3( dTMinus * p0[0] + dT * p1[0], dTMinus * p0[1] + dT * p1[1], dTMinus * p0[2] + dT * p1[2] );
    }

    inline std::ostream& operator<<(std::ostream& os, const Point3& p) {
        os << "(" << p[0] << ", " << p[1] << ", " << p[2] << ")";
        return os;
    }

} /* namespace MathTools */
