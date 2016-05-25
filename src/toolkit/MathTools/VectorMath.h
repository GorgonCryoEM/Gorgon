#ifndef TOOLKIT_GRAPHMATCH_VECMATH_H
#define TOOLKIT_GRAPHMATCH_VECMATH_H

#include <iostream>
#include <cmath>
#include <cassert>
//#include <MathTools/BasicDefines.h>
#include "Vector3.h"

using namespace MathTools;

namespace GraphMatch {


    // Dot product, subtraction, ofstream, etc., are declared after class
    class ScreenVector {
    public:
        ScreenVector() : x(0), y(0) {}
        ScreenVector(const ScreenVector& v) : x(v[0]), y(v[1]) {}
        ScreenVector(int _x, int _y) : x(_x), y(_y) {}

        ScreenVector& operator=(const ScreenVector& a) {
            x = a[0]; y = a[1];
            return *this;
        }

        const int &operator[](int n) const { return (&x)[n]; }
              int &operator[](int n)       { return (&x)[n]; }

        ScreenVector& operator+=(const ScreenVector& a) {
            x += a[0]; y += a[1];
            return *this;
        }

        ScreenVector& operator-=(const ScreenVector& a) {
            x -= a[0]; y -= a[1];
            return *this;
        }

        ScreenVector& operator*=(int s) {
            x *= s; y *= s;
            return *this;
        }

        ScreenVector operator-() const {
            return ScreenVector(-x, -y);
        }

        ScreenVector operator+() const {
            return *this;
        }

        ScreenVector operator+( const ScreenVector &v ) const {
            return ScreenVector( x + v.x, y + v.y );
        }

        ScreenVector operator-( const ScreenVector &v ) const {
            return ScreenVector( x - v.x, y - v.y );
        }

        ScreenVector operator*( const int s ) const {
            return ScreenVector( x * s, y * s );
        }

        // Dot
        int operator*( const ScreenVector &v ) const {
            return x * v.x + y * v.y;
        }

        double length() const {
            return (double) sqrt( (double) (x * x + y * y) );
        }

        int lengthSquared() const {
            return x * x + y * y;
        }

        bool operator==( const ScreenVector &v ) const {
            return x == v.x && y == v.y;
        }

        bool operator!=( const ScreenVector &v ) const {
            return x != v.x || y != v.y;
        }

        void print() const {
            std::cout << "(" << x << ", " << y << ")";
        }

    private:
        int x, y;
    };

    inline ScreenVector operator*( const int s, const ScreenVector &v ) {
        return ScreenVector( v[0] * s, v[1] * s );
    }

    inline std::ostream& operator<<(std::ostream& os, const ScreenVector& v) {
        os << "(" << v[0] << ", " << v[1] << ")";
        return os;
    }


    class ScreenPoint {
    public:
        ScreenPoint() : x(0), y(0) {}
        ScreenPoint(const ScreenPoint & p) : x(p[0]), y(p[1]) {}
        ScreenPoint(int _x, int _y) : x(_x), y(_y) {}

        ScreenPoint& operator=(const ScreenPoint& a) {
            x = a[0]; y = a[1];
            return *this;
        }

        const int &operator[](int n) const { return (&x)[n]; }
              int &operator[](int n)       { return (&x)[n]; }

        ScreenPoint& operator+=(const ScreenVector& v) {
            x += v[0]; y += v[1];
            return *this;
        }

        ScreenPoint& operator-=(const ScreenVector& v) {
            x -= v[0]; y -= v[1];
            return *this;
        }

        ScreenPoint& operator*=(int s) {
            x *= s; y *= s;
            return *this;
        }

        ScreenPoint operator+(const ScreenVector& v) const {
            return ScreenPoint( x + v[0], y + v[1] );
        }

        ScreenVector operator-(const ScreenPoint& p) const {
            return ScreenVector( x - p.x, y - p.y );
        }

        ScreenPoint operator-(const ScreenVector& v) const {
            return ScreenPoint( x - v[0], y - v[1] );
        }

        bool operator==( const ScreenPoint &p ) const {
            return x == p.x && y == p.y;
        }

        bool operator!=( const ScreenPoint &p ) const {
            return x != p.x || y != p.y;
        }

        void print() const {
            std::cout << "(" << x << ", " << y << ")";
        }

    private:
        int x, y;
    };

    inline std::ostream& operator<<(std::ostream& os, const ScreenPoint& p) {
        os << "(" << p[0] << ", " << p[1] << ")";
        return os;
    }

    class Matrix3 {
    public:
        Matrix3() {
            for ( int i = 0; i < 3; i++ )
                for ( int j = 0; j < 3; j++ )
                    mat[ index(i,j) ] = (i == j) ? 1.0 : 0.0;
        }

        Matrix3(const Vector3<double>& row0, const Vector3<double>& row1, const Vector3<double>& row2) {
            for ( int i = 0; i < 3; i++ ) {
                mat[ index( 0, i ) ] = row0[i];
                mat[ index( 1, i ) ] = row1[i];
                mat[ index( 2, i ) ] = row2[i];
            }
        }

        Matrix3(const Matrix3& m) {
            (*this) = m;
        }

        Matrix3& operator=(const Matrix3& m) {
            memcpy( &mat[0], &m.mat[0], sizeof(double) * 9 );
            return *this;
        }

        // The indexing is this way to match OpenGL
        int index( int row, int col ) const { assert( row >= 0 && row < 3 ); assert( col >= 0 && col < 3 ); return col * 3 + row; }

        const double & operator()( int row, int col ) const { return mat[ index(row,col) ]; }
              double & operator()( int row, int col )       { return mat[ index(row,col) ]; }

        Vector3<double> row(int r) const {
            return Vector3<double>( mat[index(r,0)], mat[index(r,1)], mat[index(r,2)] );
        }

        Vector3<double> column(int c) const {
            return Vector3<double>( mat[index(0,c)], mat[index(1,c)], mat[index(2,c)] );
        }

        Matrix3 transpose() const {
            Matrix3 matRet;
            for ( int i = 0; i < 3; i++ )
                for ( int j = 0; j < 3; j++ )
                    matRet(i,j) = (*this)(j,i);
            return matRet;
        }

        Matrix3 operator+( const Matrix3 &m) const {
            Matrix3 matRet;
            for ( int i = 0; i < 9; i++ )
                matRet.mat[i] = mat[i] + m.mat[i];
            return matRet;
        }

        Matrix3& operator*=(double s) {
            for ( int i = 0; i < 9; i++ )
                mat[i] *= s;
            return *this;
        }

        // pre-multiply column vector by a 3x3 matrix
        Vector3<double> operator*(const Vector3<double>& v) const {
            return Vector3<double>((*this)(0,0) * v[0] + (*this)(0,1) * v[1] + (*this)(0,2) * v[2],
                           (*this)(1,0) * v[0] + (*this)(1,1) * v[1] + (*this)(1,2) * v[2],
                           (*this)(2,0) * v[0] + (*this)(2,1) * v[1] + (*this)(2,2) * v[2]);
        }

        // pre-multiply column point by a 3x3 matrix
        Vec3D operator*(const Vec3D& p) const {
            return Vec3D((*this)(0,0) * p[0] + (*this)(0,1) * p[1] + (*this)(0,2) * p[2],
                          (*this)(1,0) * p[0] + (*this)(1,1) * p[1] + (*this)(1,2) * p[2],
                          (*this)(2,0) * p[0] + (*this)(2,1) * p[1] + (*this)(2,2) * p[2]);
        }

        Matrix3 operator*( const Matrix3 & m ) const {
            Matrix3 matRet;
            for ( int i = 0; i < 3; i++ ) {
                for ( int j = 0; j < 3; j++ ) {
                    matRet(i,j) = 0.0;
                    for ( int k = 0; k < 3; k++ )
                        matRet(i,j) += (*this)(i,k) * m(k,j);
                }
            }
            return matRet;
        }

        static Matrix3 identity() {
            return Matrix3(Vector3<double>(1, 0, 0),
                           Vector3<double>(0, 1, 0),
                           Vector3<double>(0, 0, 1));
        }

        static Matrix3 rotationXYZtoUVW(Vector3<double> u, Vector3<double> v, Vector3<double> w) {
            return Matrix3(Vector3<double>(u[0], v[0], w[0]),
                           Vector3<double>(u[1], v[1], w[1]),
                           Vector3<double>(u[2], v[2], w[2]));
        }

        static double det2x2(double a, double b, double c, double d) {
            return a * d - b * c;
        }

        double determinant() const {
            return ((*this)(0,0) * (*this)(1,1) * (*this)(2,2) +
                    (*this)(0,1) * (*this)(1,2) * (*this)(2,0) +
                    (*this)(0,2) * (*this)(1,0) * (*this)(2,1) -
                    (*this)(0,2) * (*this)(1,1) * (*this)(2,0) -
                    (*this)(0,0) * (*this)(1,2) * (*this)(2,1) -
                    (*this)(0,1) * (*this)(1,0) * (*this)(2,2));
        }

        Matrix3 inverse() const {
            Vector3<double> r1(  det2x2((*this)(1,1), (*this)(1,2), (*this)(2,1), (*this)(2,2)),
                         det2x2((*this)(0,2), (*this)(0,1), (*this)(2,2), (*this)(2,1)),
                         det2x2((*this)(0,1), (*this)(0,2), (*this)(1,1), (*this)(1,2)) );
            Vector3<double> r2(  det2x2((*this)(1,2), (*this)(1,0), (*this)(2,2), (*this)(2,0)),
                         det2x2((*this)(0,0), (*this)(0,2), (*this)(2,0), (*this)(2,2)),
                         det2x2((*this)(0,2), (*this)(0,0), (*this)(1,2), (*this)(1,0)) );
            Vector3<double> r3(  det2x2((*this)(1,0), (*this)(1,1), (*this)(2,0), (*this)(2,1)),
                         det2x2((*this)(0,1), (*this)(0,0), (*this)(2,1), (*this)(2,0)),
                         det2x2((*this)(0,0), (*this)(0,1), (*this)(1,0), (*this)(1,1)) );

            Matrix3 adjoint( r1, r2, r3 );

            const double dDet = determinant();

            assert( isZero( dDet ) == false );
            adjoint *= 1.0 / dDet;

            return adjoint;
        }

        bool operator==( const Matrix3 &m ) const {
            for ( int i = 0; i < 9; i++ )
                if ( mat[i] != m.mat[i] )
                    return false;
            return true;
        }

        bool approxEqual( const Matrix3 &m, double eps = 1e-12 ) const {
            for ( int i = 0; i < 9; i++ )
                if ( !isZero( mat[i] - m.mat[i], eps ) )
                    return false;
            return true;
        }

        void print() const {
            std::cout << "( " << (*this)(0,0) << ", " << (*this)(0,1) << ", " << (*this)(0,2) << "\n";
            std::cout << "  " << (*this)(1,0) << ", " << (*this)(1,1) << ", " << (*this)(1,2) << "\n";
            std::cout << "  " << (*this)(2,0) << ", " << (*this)(2,1) << ", " << (*this)(2,2) << ")\n";
        }

    private:
        double mat[9];
    };

    // post-multiply row vector by a 3x3 matrix
    inline Vector3<double> operator*(const Vector3<double>& v, const Matrix3& m) {
        return Vector3<double>(m(0,0) * v[0] + m(1,0) * v[1] + m(2,0) * v[2],
                       m(0,1) * v[0] + m(1,1) * v[1] + m(2,1) * v[2],
                       m(0,2) * v[0] + m(1,2) * v[1] + m(2,2) * v[2]);
    }

    // post-multiply row point by a 3x3 matrix
    inline Vec3D operator*(const Vec3D& p, const Matrix3& m) {
        return Vec3D(m(0,0) * p[0] + m(1,0) * p[1] + m(2,0) * p[2],
                      m(0,1) * p[0] + m(1,1) * p[1] + m(2,1) * p[2],
                      m(0,2) * p[0] + m(1,2) * p[1] + m(2,2) * p[2]);
    }

    inline std::ostream& operator<<(std::ostream& os, const Matrix3& m) {
        os << m.row(0) << std::endl;
        os << m.row(1) << std::endl;
        os << m.row(2) << std::endl;
        return os;
    }


    class Vector4 {
    public:
        Vector4() : x(0), y(0), z(0), w(0) {}
        Vector4(const Vector4& v) : x(v[0]), y(v[1]), z(v[2]), w(v[3]) {}
        Vector4(double _x, double _y, double _z, double _w) : x(_x), y(_y), z(_z), w(_w) {}

        Vector4& operator=(const Vector4& a) {
            x = a[0]; y = a[1]; z = a[2]; w = a[3];
            return *this;
        }

        const double &operator[](int n) const { return ((double *) this)[n]; }
              double &operator[](int n)       { return ((double *) this)[n]; }

        Vector4& operator+=(const Vector4& a) {
            x += a[0]; y += a[1]; z += a[2]; w += a[3];
            return *this;
        }

        Vector4& operator-=(const Vector4& a) {
            x -= a[0]; y -= a[1]; z -= a[2]; w -= a[3];
            return *this;
        }

        Vector4& operator*=(double s) {
            x *= s; y *= s; z *= s; w *= s;
            return *this;
        }

        Vector4 operator-() {
            return Vector4(-x, -y, -z, -w);
        }

        Vector4 operator+() {
            return *this;
        }

        Vector4 operator+( const Vector4 &v ) const {
            return Vector4( x + v.x, y + v.y, z + v.z, w + v.w );
        }

        Vector4 operator-( const Vector4 &v ) const {
            return Vector4( x - v.x, y - v.y, z - v.z, w - v.w );
        }

        Vector4 operator/( const double s ) const {
            assert( s > 0.0 );
            return Vector4( x / s, y / s, z / s, w / s );
        }

        Vector4 operator*( const double s ) const {
            return Vector4( x * s, y * s, z * s, w * s );
        }

        // Dot
        double operator*( const Vector4 &v ) const {
            return x * v.x + y * v.y + z * v.z + w * v.w;
        }

        double length() const {
            return (double) sqrt(x * x + y * y + z * z + w * w);
        }

        double lengthSquared() const {
            return x * x + y * y + z * z + w * w;
        }

        void normalize() {
            double s = 1.0 / length();
            x *= s; y *= s; z *= s; w *= s;
        }

        bool operator==( const Vector4 &v ) const {
            return x == v.x && y == v.y && z == v.z && w == v.w;
        }

        bool operator!=( const Vector4 &v ) const {
            return x != v.x || y != v.y || z != v.z || w != v.w;
        }

        bool approxEqual( const Vector4 &v, double eps = 1e-12 ) const {
            return isZero( x - v.x, eps ) && isZero( y - v.y, eps ) && isZero( z - v.z, eps ) && isZero( w - v.w, eps );
        }

        void print() const {
            std::cout << x << " " << y << " " << z << " " << w << "\n";
        }

    private:
        double x, y, z, w;
    };

    inline Vector4 operator*( const double s, const Vector4 &v ) {
        return Vector4( v[0] * s, v[1] * s, v[2] * s, v[3] * s );
    }

    inline double length( const Vector4 &v ) { return v.length(); }
    inline Vector4 unit( const Vector4 &v ) { const double len = v.length(); return v / len; }
    inline std::ostream& operator<<(std::ostream& os, const Vector4& v) {
        os << "(" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ")";
        return os;
    }

    class Matrix4 {
    public:
        Matrix4() {
            for ( int i = 0; i < 4; i++ )
                for ( int j = 0; j < 4; j++ )
                    mat[ index(i,j) ] = (i == j) ? 1.0 : 0.0;
        }

        Matrix4(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3) {
            for ( int i = 0; i < 4; i++ ) {
                mat[ index( 0, i ) ] = row0[i];
                mat[ index( 1, i ) ] = row1[i];
                mat[ index( 2, i ) ] = row2[i];
                mat[ index( 3, i ) ] = row3[i];
            }
        }

        Matrix4(const Vector3<double>& row0, const Vector3<double>& row1, const Vector3<double>& row2 ) {
            for ( int i = 0; i < 3; i++ ) {
                mat[ index( 0, i ) ] = row0[i];
                mat[ index( 1, i ) ] = row1[i];
                mat[ index( 2, i ) ] = row2[i];
                mat[ index(i,3) ] = 0.0;
                mat[ index(3,i) ] = 0.0;
            }
            mat[ index(3,3) ] = 1.0;
        }

        Matrix4(const Matrix4& m) {
            (*this) = m;
        }

        Matrix4& operator=(const Matrix4& m) {
            memcpy( &mat[0], &m.mat[0], sizeof(double) * 16 );
            return *this;
        }

        // The indexing is this way to match OpenGL
        int index( int row, int col ) const { assert( row >= 0 && row < 4 ); assert( col >= 0 && col < 4 ); return col * 4 + row; }

        const double & operator()( int row, int col ) const { return mat[ index(row,col) ]; }
              double & operator()( int row, int col )       { return mat[ index(row,col) ]; }

        Vector4 row(int r) const {
            return Vector4( mat[index(r,0)], mat[index(r,1)], mat[index(r,2)], mat[index(r,3)] );
        }

        Vector4 column(int c) const {
            return Vector4( mat[index(0,c)], mat[index(1,c)], mat[index(2,c)], mat[index(3,c)] );
        }

        Matrix4 & operator *=( const Matrix4 &m )  {
            const Matrix4 matRet = (*this) * m;
            (*this) = matRet;
            return *this;
        }

        Matrix4 & operator +=( const Matrix4 &m )  {
            const Matrix4 matRet = (*this) + m;
            (*this) = matRet;
            return *this;
        }

        Matrix4 & operator -=( const Matrix4 &m )  {
            const Matrix4 matRet = (*this) - m;
            (*this) = matRet;
            return *this;
        }

        Matrix4 transpose() const {
            Matrix4 matRet;
            for ( int i = 0; i < 4; i++ )
                for ( int j = 0; j < 4; j++ )
                    matRet(i,j) = (*this)(j,i);
            return matRet;
        }

        Matrix4 operator+( const Matrix4 &m) const {
            Matrix4 matRet;
            for ( int i = 0; i < 16; i++ )
                matRet.mat[i] = mat[i] + m.mat[i];
            return matRet;
        }

        Matrix4 operator-( const Matrix4 &m) const {
            Matrix4 matRet;
            for ( int i = 0; i < 16; i++ )
                matRet.mat[i] = mat[i] - m.mat[i];
            return matRet;
        }

        Vector3<double> operator*(const Vector3<double>& v) const {
            return Vector3<double>((*this)(0,0) * v[0] + (*this)(0,1) * v[1] + (*this)(0,2) * v[2],
                           (*this)(1,0) * v[0] + (*this)(1,1) * v[1] + (*this)(1,2) * v[2],
                           (*this)(2,0) * v[0] + (*this)(2,1) * v[1] + (*this)(2,2) * v[2]);
        }

         Vec3D operator*(const Vec3D& p) const {
            const Vec3D pt((*this)(0,0) * p[0] + (*this)(0,1) * p[1] + (*this)(0,2) * p[2] + (*this)(0,3),
                            (*this)(1,0) * p[0] + (*this)(1,1) * p[1] + (*this)(1,2) * p[2] + (*this)(1,3),
                            (*this)(2,0) * p[0] + (*this)(2,1) * p[1] + (*this)(2,2) * p[2] + (*this)(2,3));
            const double w = (*this)(3,0) * p[0] + (*this)(3,1) * p[1] + (*this)(3,2) * p[2] + (*this)(3,3);
            assert( isZero( w ) == false );
            return Vec3D( pt[0] / w, pt[1] / w, pt[2] / w );
        }

        Vector4 operator*(const Vector4& v) const {
            return Vector4((*this)(0,0) * v[0] + (*this)(0,1) * v[1] + (*this)(0,2) * v[2] + (*this)(0,3) * v[3],
                           (*this)(1,0) * v[0] + (*this)(1,1) * v[1] + (*this)(1,2) * v[2] + (*this)(1,3) * v[3],
                           (*this)(2,0) * v[0] + (*this)(2,1) * v[1] + (*this)(2,2) * v[2] + (*this)(2,3) * v[3],
                           (*this)(3,0) * v[0] + (*this)(3,1) * v[1] + (*this)(3,2) * v[2] + (*this)(3,3) * v[3]);
        }

        inline Matrix4 operator*(const Matrix4& b) const{
            Matrix4 matRet;
            for ( int i = 0; i < 4; i++ ) {
                for ( int j = 0; j < 4; j++ ) {
                    matRet(i,j) = 0.0;
                    for ( int k = 0; k < 4; k++ )
                        matRet(i,j) += (*this)(i,k) * b(k,j);
                }
            }
            return matRet;
        }

        Matrix4 inverse() const;

        bool operator==( const Matrix4 &m ) const {
            for ( int i = 0; i < 16; i++ )
                if ( mat[i] != m.mat[i] )
                    return false;
            return true;
        }

        bool approxEqual( const Matrix4 &m, double eps = 1e-12 ) const {
            for ( int i = 0; i < 16; i++ )
                if ( !isZero( mat[i] - m.mat[i], eps ) )
                    return false;
            return true;
        }

        void print() const {
            std::cout << "( " << (*this)(0,0) << ", " << (*this)(0,1) << ", " << (*this)(0,2) << ", " << (*this)(0,3) << "\n";
            std::cout << "  " << (*this)(1,0) << ", " << (*this)(1,1) << ", " << (*this)(1,2) << ", " << (*this)(0,3) << "\n";
            std::cout << "  " << (*this)(2,0) << ", " << (*this)(2,1) << ", " << (*this)(2,2) << ", " << (*this)(0,3) << "\n";
            std::cout << "  " << (*this)(3,0) << ", " << (*this)(3,1) << ", " << (*this)(3,2) << ", " << (*this)(0,3) << ")\n";
        }

        static Matrix4 identity() {
            return Matrix4(Vector4(1, 0, 0, 0),
                           Vector4(0, 1, 0, 0),
                           Vector4(0, 0, 1, 0),
                           Vector4(0, 0, 0, 1));
        }

        static Matrix4 translation(const Vec3D& p) {
            return Matrix4(Vector4(1, 0, 0, p[0]),
                           Vector4(0, 1, 0, p[1]),
                           Vector4(0, 0, 1, p[2]),
                           Vector4(0, 0, 0, 1));
        }

        static Matrix4 translation(const Vector3<double>& v) {
            return Matrix4(Vector4(1, 0, 0, v[0]),
                           Vector4(0, 1, 0, v[1]),
                           Vector4(0, 0, 1, v[2]),
                           Vector4(0, 0, 0, 1));
        }

        static Matrix4 rotation(const Vector3<double>& u, const Vector3<double>& v, const Vector3<double>& w) {
            return Matrix4(Vector4(u[0], u[1], u[2], 0),
                           Vector4(v[0], v[1], v[2], 0),
                           Vector4(w[0], w[1], w[2], 0),
                           Vector4(0  , 0  , 0  , 1));
        }

        static Matrix4 rotation(const Vector3<double>& axis, double angle) {
            Vector3<double> a = axis;
            a.normalize();
            const double c = cos(angle);
            const double s = sin(angle);
            const double t = 1 - c;

            return Matrix4(Vector4(t * a[0] * a[0] + c,
                                   t * a[0] * a[1] - s * a[2],
                                   t * a[0] * a[2] + s * a[1],
                                   0),
                           Vector4(t * a[0] * a[1] + s * a[2],
                                   t * a[1] * a[1] + c,
                                   t * a[1] * a[2] - s * a[0],
                                   0),
                           Vector4(t * a[0] * a[2] - s * a[1],
                                   t * a[1] * a[2] + s * a[0],
                                   t * a[2] * a[2] + c,
                                   0),
                           Vector4(0, 0, 0, 1));
            //
            //Vector3 a = axis;
            //a.normalize();
            //double u = a[0];
            //double v = a[1];
            //double w = a[2];
            //double u2 = u*u;
            //double v2 = v*v;
            //double w2 = w*w;
            //double b = u2 + v2 + w2;
            //double sq = sqrt(u2 + v2 + w2);

            //double c = cos(angle);
            //double s = sin(angle);
            //return Matrix4(
            //	Vector4((u2 + (v2+w2)*c)/b,			(u*v*(1-c) - w*sq*s)/b,		(u*v*(1-c) + v*sq*s)/b,		0),
            //	Vector4((u*v*(1-c) + w*sq*s)/b,		(v2+(u2+w2)*c)/b,			(v*w*(1-c) - u*sq*s)/b,		0),
            //	Vector4((u*w*(1-c) - v*sq*s)/b,		(v*w*(1-c) + u*sq*s)/b,		(w2 + (u2+v2)*c)/b,			0),
            //	Vector4(0,							0,							0,							1));

        }

        static Matrix4 xrotation(double angle) {
            const double c = cos(angle);
            const double s = sin(angle);

            return Matrix4(Vector4(1, 0,  0, 0),
                           Vector4(0, c, -s, 0),
                           Vector4(0, s,  c, 0),
                           Vector4(0, 0,  0, 1));
        }

        static Matrix4 yrotation(double angle) {
            double c = cos(angle);
            double s = sin(angle);

            return Matrix4(Vector4( c, 0, s, 0),
                           Vector4( 0, 1, 0, 0),
                           Vector4(-s, 0, c, 0),
                           Vector4( 0, 0, 0, 1));
        }

        static Matrix4 zrotation(double angle) {
            const double c = cos(angle);
            const double s = sin(angle);

            return Matrix4(Vector4(c, -s, 0, 0),
                           Vector4(s,  c, 0, 0),
                           Vector4(0,  0, 1, 0),
                           Vector4(0,  0, 0, 1));
        }

        static Matrix4 scaling(const Vector3<double>& s) {
            return Matrix4(Vector4(s[0], 0  , 0  , 0),
                           Vector4(0  , s[1], 0  , 0),
                           Vector4(0  , 0  , s[2], 0),
                           Vector4(0  , 0  , 0  , 1));
        }

        static Matrix4 scaling( double x, double y, double z) {
            return Matrix4(Vector4(x, 0, 0, 0),
                           Vector4(0, y, 0, 0),
                           Vector4(0, 0, z, 0),
                           Vector4(0, 0, 0, 1));
        }

        static Matrix4 scaling(double scale) {
            return scaling(Vector3<double>(scale, scale, scale));
        }

        double mat[16];
    };






    // **** Matrix4 operators ****

    inline std::ostream& operator<<(std::ostream& os, const Matrix4& m) {
        os << m.row(0) << std::endl;
        os << m.row(1) << std::endl;
        os << m.row(2) << std::endl;
        os << m.row(3) << std::endl;
        return os;
    }

    inline Matrix4 Matrix4::inverse() const {
        // Compute inverse using Gauss-Jordan elimination; caller is responsible
        // for ensuring that the matrix isn't singular.
        Matrix4 a(*this);
        Matrix4 b(Matrix4::identity());
        int i, j;
        int p;

        for (j = 0; j < 4; j++) {
            p = j;
            for (i = j + 1; i < 4; i++) {
                if (fabs(a(i,j)) > fabs(a(p,j)))
                    p = i;
            }
            // Swap rows p and j
            if ( p != j ) {
                for ( i = 0; i < 4; i++ ) {
                    const double ta = a(p,i);
                    a(p,i) = a(j,i);
                    a(j,i) = ta;

                    const double tb = b(p,i);
                    b(p,i) = b(j,i);
                    b(j,i) = tb;
                }
            }

            const double s = a(j,j);  // if s == 0, the matrix is singular
            assert( isZero( s ) == false );
            for ( i = 0; i < 4; i++ ) {
                a(j,i) *= (1.0 / s);
                b(j,i) *= (1.0 / s);
            }
            // Eliminate off-diagonal elements
            for (i = 0; i < 4; i++) {
                if (i != j) {
                    for ( int k = 0; k < 4; k++ ) {
                        b(i,k) -= a(i,j) * b(j,k);
                        a(i,k) -= a(i,j) * a(j,k);
                    }
                }
            }
        }
        return b;
    }

    // Class used to store stack points..
    class Point3Pair {
    public:
        int x, y, z, node;
        float distance;
        Point3Pair(int x, int y, int z, float distance) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->distance = distance;
            this->node = 0;
        }
        bool operator==(Point3Pair &other) {
            return ((this->x == other.x) && (this->y == other.y) && (this->z == other.z));
        }

        static double EuclideanDistance(Point3Pair p1, Point3Pair p2) {
            return sqrt(pow((double)(p1.x - p2.x), 2) + pow((double)(p1.y - p2.y), 2) + pow((double)(p1.z - p2.z), 2));
        }
    };
}
#endif /* _VECMATH_H_ */


