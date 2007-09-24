#ifndef VECTOR3D_H
#define VECTOR3D_H

#define round(f) ((f >= 0)?(int)(f + .5):(int)(f - .5))

#include <stdio.h>
#include <math.h>

namespace wustl_mm {
	namespace MatlabInterface {
		class Vector3D {
		public:
			Vector3D();
			Vector3D(double x, double y, double z);
			~Vector3D();

			double X();
			double Y();
			double Z();
			int XInt();
			int YInt();
			int ZInt();

			double Vector3D::operator*(Vector3D &d ); // Dot Product
						
			Vector3D operator+(Vector3D &d );
			Vector3D operator-();
			Vector3D operator-(Vector3D &d );
			Vector3D operator^(Vector3D &d );		 // Cross Product
			Vector3D operator*(double s);			
			Vector3D& operator=(const Vector3D& d);
			Vector3D& operator+=(const Vector3D& d);
			Vector3D& operator-=(const Vector3D& d);
			Vector3D Rotate(Vector3D axis, double angle);
			void Normalize();

			static Vector3D Normalize(Vector3D d);

			double values[3];
		};

		Vector3D::Vector3D() {
			values[0] = 0.0;
			values[1] = 0.0;
			values[2] = 0.0;
		}

		Vector3D::Vector3D(double x, double y, double z) {
			values[0] = x;
			values[1] = y;
			values[2] = z;
		}

		Vector3D::~Vector3D() {
		}

		double Vector3D::X() {
			return values[0];
		}

		double Vector3D::Y() {
			return values[1];
		}

		double Vector3D::Z() {
			return values[2];
		}


		int Vector3D::XInt() {
			return (int)round(values[0]);
		}

		int Vector3D::YInt() {
			return (int)round(values[1]);
		}

		int Vector3D::ZInt() {
			return (int)round(values[2]);
		}


		double Vector3D::operator*(Vector3D &d ) {		// Dot Product
			return X() * d.X() + Y() * d.Y() + Z() * d.Z();
		}

		Vector3D Vector3D::operator+(Vector3D &d ) {
			return Vector3D(X() + d.X(), Y() + d.Y(), Z() + d.Z());	
		}

		Vector3D Vector3D::operator-() {
			return Vector3D(-X(), -Y(), -Z());	
		}

		Vector3D Vector3D::operator-(Vector3D &d ) {
			return Vector3D(X() - d.X(), Y() - d.Y(), Z() - d.Z());	
		}

		Vector3D Vector3D::operator^(Vector3D &d ) { // Cross Product
			return Vector3D(
				values[1] * d.values[2] - values[2] * d.values[1], 
				values[2] * d.values[0] - values[0] * d.values[2], 
				values[0] * d.values[1] - values[1] * d.values[0]);	
		}

		Vector3D Vector3D::operator*(double s) {
			return Vector3D(X()*s, Y()*s, Z()*s);
		}

		Vector3D& Vector3D::operator+=(const Vector3D& d) {
			for(int i = 0; i < 3; i++) {
				values[i] += d.values[i];
			}
			return *this;
		}

		Vector3D& Vector3D::operator-=(const Vector3D& d) {
			for(int i = 0; i < 3; i++) {
				values[i] -= d.values[i];
			}
			return *this;
		}
		Vector3D& Vector3D::operator=(const Vector3D& d) {
			for(int i = 0; i < 3; i++) {
				values[i] = d.values[i];
			}
			return *this;
		}

		void Vector3D::Normalize() {
			double base = 0;
			for(int i = 0; i < 3; i++) {
				base += (values[i] * values[i]);
			}
			if(base == 0) {
				values[0] = 0;
				values[1] = 0;
				values[2] = 0;
			} else {				
				base = sqrt(base);
				values[0] = values[0]/base;
				values[1] = values[1]/base;
				values[2] = values[2]/base;
			}
		}

		Vector3D Vector3D::Normalize(Vector3D d) {
			Vector3D ret = d;
			d.Normalize();
			return ret;
		}

		Vector3D Vector3D::Rotate(Vector3D axis, double angle) {
			double r = angle;
			double a = axis.values[0];
			double b = axis.values[1];
			double c = axis.values[2];
			
			double q0 = cos(r/2.0);
			double q1 = sin(r/2.0)* a;
			double q2 = sin(r/2.0)* b;
			double q3 = sin(r/2.0)* c;


			double R[3][3] = {{q0*q0 + q1*q1 - q2*q2 - q3*q3,		2*(q1*q2 - q0*q3),					2*(q1*q3 + q0*q2)},
							  {2*(q2*q1 + q0*q3),					q0*q0 - q1*q1 + q2*q2 - q3*q3,		2*(q2*q3 - q0*q1)},
							  {2*(q3*q1 - q0*q2),					2*(q3*q2 + q0*q1),					q0*q0 - q1*q1 - q2*q2 + q3*q3}};


			Vector3D v = Vector3D(0.0, 0.0, 0.0);
			for(int x = 0; x < 3; x++) {
				for(int y = 0; y < 3; y++) {
					v.values[x] = v.values[x] + R[y][x] * values[y];
				}
			}
			return v;
		}
	}
}

#endif