#ifndef VECTOR3D_H
#define VECTOR3D_H

#include "Vector3DTemplate.h"

namespace wustl_mm {
	namespace MathTools {

		typedef Vector3DTemplate<int>	 Vector3DInt;
		typedef Vector3DTemplate<float>  Vector3DFloat;
		typedef Vector3DTemplate<double> Vector3DDouble;

		inline Vector3DFloat Vector3DInt2Float(Vector3DInt & v) {
			return Vector3DFloat(v.X(), v.Y(), v.Z());
		}

		inline Vector3DDouble Vector3DInt2Double(Vector3DInt & v) {
			return Vector3DDouble(v.X(), v.Y(), v.Z());
		}



/*
		class Vector3DDouble {
		public:
			Vector3DDouble();
			Vector3DDouble(double x, double y, double z);
			~Vector3DDouble();

			double X();
			double Y();
			double Z();
			double Length();
			double operator*(Vector3DDouble &d ); // Dot Product							
			int XInt();
			int YInt();
			int ZInt();
			Vector3DDouble operator+(Vector3DDouble &d );
			Vector3DDouble operator-();
			Vector3DDouble operator-(Vector3DDouble &d );
			Vector3DDouble operator^(Vector3DDouble &d );		 // Cross Product
			Vector3DDouble operator*(double s);			
			Vector3DDouble& operator=(const Vector3DDouble& d);
			Vector3DDouble& operator=(const Vector3DInt& d);
			Vector3DDouble& operator+=(const Vector3DDouble& d);
			Vector3DDouble& operator-=(const Vector3DDouble& d);
			Vector3DDouble Rotate(Vector3DDouble axis, double angle);
			void Normalize();
			bool IsBadNormal();

			static Vector3DDouble Normalize(Vector3DDouble d);

			double values[3];
		};

		Vector3DDouble::Vector3DDouble() {
			values[0] = 0.0;
			values[1] = 0.0;
			values[2] = 0.0;
		}

		Vector3DDouble::Vector3DDouble(double x, double y, double z) {
			values[0] = x;
			values[1] = y;
			values[2] = z;
		}

		Vector3DDouble::~Vector3DDouble() {
		}

		double Vector3DDouble::X() {
			return values[0];
		}

		double Vector3DDouble::Y() {
			return values[1];
		}

		double Vector3DDouble::Z() {
			return values[2];
		}


		double Vector3DDouble::Length() {
			return sqrt(values[0] * values[0] + values[1] * values[1] + values[2] * values[2]);
		}
		int Vector3DDouble::XInt() {
			return (int)round(values[0]);
		}

		int Vector3DDouble::YInt() {
			return (int)round(values[1]);
		}

		int Vector3DDouble::ZInt() {
			return (int)round(values[2]);
		}


		double Vector3DDouble::operator*(Vector3DDouble &d ) {		// Dot Product
			return X() * d.X() + Y() * d.Y() + Z() * d.Z();
		}

		Vector3DDouble Vector3DDouble::operator+(Vector3DDouble &d ) {
			return Vector3DDouble(X() + d.X(), Y() + d.Y(), Z() + d.Z());	
		}

		Vector3DDouble Vector3DDouble::operator-() {
			return Vector3DDouble(-X(), -Y(), -Z());	
		}

		Vector3DDouble Vector3DDouble::operator-(Vector3DDouble &d ) {
			return Vector3DDouble(X() - d.X(), Y() - d.Y(), Z() - d.Z());	
		}

		Vector3DDouble Vector3DDouble::operator^(Vector3DDouble &d ) { // Cross Product
			return Vector3DDouble(
				values[1] * d.values[2] - values[2] * d.values[1], 
				values[2] * d.values[0] - values[0] * d.values[2], 
				values[0] * d.values[1] - values[1] * d.values[0]);	
		}

		Vector3DDouble Vector3DDouble::operator*(double s) {
			return Vector3DDouble(X()*s, Y()*s, Z()*s);
		}

		Vector3DDouble& Vector3DDouble::operator+=(const Vector3DDouble& d) {
			for(int i = 0; i < 3; i++) {
				values[i] += d.values[i];
			}
			return *this;
		}

		Vector3DDouble& Vector3DDouble::operator-=(const Vector3DDouble& d) {
			for(int i = 0; i < 3; i++) {
				values[i] -= d.values[i];
			}
			return *this;
		}
		Vector3DDouble& Vector3DDouble::operator=(const Vector3DDouble& d) {
			for(int i = 0; i < 3; i++) {
				values[i] = d.values[i];
			}
			return *this;
		}

		Vector3DDouble& Vector3DDouble::operator=(const Vector3DInt& d) {
			for(int i = 0; i < 3; i++) {
				values[i] = d.values[i];
			}
			return *this;
		}

		void Vector3DDouble::Normalize() {
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

		Vector3DDouble Vector3DDouble::Normalize(Vector3DDouble d) {
			Vector3DDouble ret = d;
			d.Normalize();
			return ret;
		}

		Vector3DDouble Vector3DDouble::Rotate(Vector3DDouble axis, double angle) {
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


			Vector3DDouble v = Vector3DDouble(0.0, 0.0, 0.0);
			for(int x = 0; x < 3; x++) {
				for(int y = 0; y < 3; y++) {
					v.values[x] = v.values[x] + R[y][x] * values[y];
				}
			}
			return v;
		}

		bool Vector3DDouble::IsBadNormal() {
			return !isZero(Length() - 1.0, 0.00001);
		}  */
	}
}

#endif