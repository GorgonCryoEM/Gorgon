/*
 * Point3.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_MATHTOOLS_POINT3_H_
#define SRC_TOOLKIT_MATHTOOLS_POINT3_H_

namespace MathTools {

    /*
     *
     */
    class Point3 {
        public:
            Point3();
            Point3(const Point3& p);
            Point3(double _x, double _y, double _z);
            virtual ~Point3();

            Point3& operator=(const Point3& a);

            const double &operator[](int n) const;
                  double &operator[](int n)      ;

            Point3& operator+=(const Vector3<double>& v);
            Point3& operator-=(const Vector3<double>& v);
            Point3& operator*=(double s);
            Vector3<double> operator-(const Point3 & p) const;
            Point3 operator+(const Vector3<double> & v) const;
            Point3 operator-(const Vector3<double> & v) const;
            double distanceTo(const Point3& p) const;
            double distanceTo(const Vec3F& v) const;
            double distanceToSquared(const Point3& p) const;
            double distanceFromOrigin() const;
            double distanceFromOriginSquared() const;
            bool operator==( const Point3 &p ) const;
            bool operator!=( const Point3 &p ) const;
            bool approxEqual( const Point3 &p, double eps = 1e-12 ) const;
            void print() const;

        private:
            double x, y, z;
    };

} /* namespace MathTools */

#endif /* SRC_TOOLKIT_MATHTOOLS_POINT3_H_ */
