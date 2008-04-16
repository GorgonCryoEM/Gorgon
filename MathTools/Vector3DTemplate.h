#ifndef VECTORT3DTEMPLATE_H
#define VECTORT3DTEMPLATE_H

#include <stdio.h>
#include <math.h>
#include "BasicDefines.h"

namespace wustl_mm {
	namespace MathTools {
		template <class T> class Vector3DTemplate {
		public:
			Vector3DTemplate();
			Vector3DTemplate(T x, T y, T z);
			~Vector3DTemplate();

			T X();
			T Y();
			T Z();
			T Length();
			T operator*(Vector3DTemplate<T> &d ); // Dot Product							
			int XInt();
			int YInt();
			int ZInt();

			bool operator!=(Vector3DTemplate<T> &d);
			bool operator==(Vector3DTemplate<T> &d);	

			Vector3DTemplate<T> operator+(Vector3DTemplate<T> &d );
			Vector3DTemplate<T> operator-();
			Vector3DTemplate<T> operator-(Vector3DTemplate<T> &d );
			Vector3DTemplate<T> operator^(Vector3DTemplate<T> &d );		 // Cross Product
			Vector3DTemplate<T> operator*(double s);			
			Vector3DTemplate<T>& operator=(const Vector3DTemplate<T>& d);
			Vector3DTemplate<T>& operator+=(const Vector3DTemplate<T>& d);
			Vector3DTemplate<T>& operator-=(const Vector3DTemplate<T>& d);
			Vector3DTemplate<T> Rotate(Vector3DTemplate<T> axis, double angle);
			void Normalize();
			bool IsBadNormal();

			static Vector3DTemplate<T> Normalize(Vector3DTemplate<T> d);

			T values[3];
		};

		template <class T> Vector3DTemplate<T>::Vector3DTemplate() {
			values[0] = (T)0.0;
			values[1] = (T)0.0;
			values[2] = (T)0.0;
		}

		template <class T> Vector3DTemplate<T>::Vector3DTemplate(T x, T y, T z) {
			values[0] = x;
			values[1] = y;
			values[2] = z;
		}

		template <class T> Vector3DTemplate<T>::~Vector3DTemplate() {
		}


		template <class T> bool Vector3DTemplate<T>::operator!=(Vector3DTemplate<T> &d) {
			return (X() != d.X()) || (Y() != d.Y()) || (Z() != d.Z());
		}

		template <class T> bool Vector3DTemplate<T>::operator==(Vector3DTemplate<T> &d) {
			return (X() == d.X()) && (Y() == d.Y()) && (Z() == d.Z());
		}

		template <class T> T Vector3DTemplate<T>::X() {
			return values[0];
		}

		template <class T> T Vector3DTemplate<T>::Y() {
			return values[1];
		}

		template <class T> T Vector3DTemplate<T>::Z() {
			return values[2];
		}


		template <class T> T Vector3DTemplate<T>::Length() {
			return sqrt(values[0] * values[0] + values[1] * values[1] + values[2] * values[2]);
		}

		template <class T> int Vector3DTemplate<T>::XInt() {
			return (int)round(values[0]);
		}

		template <class T> int Vector3DTemplate<T>::YInt() {
			return (int)round(values[1]);
		}

		template <class T> int Vector3DTemplate<T>::ZInt() {
			return (int)round(values[2]);
		}

		template <class T> T Vector3DTemplate<T>::operator*(Vector3DTemplate<T> &d ) {		// Dot Product
			return X() * d.X() + Y() * d.Y() + Z() * d.Z();
		}

		template <class T> Vector3DTemplate<T> Vector3DTemplate<T>::operator+(Vector3DTemplate<T> &d ) {
			return Vector3DTemplate<T>(X() + d.X(), Y() + d.Y(), Z() + d.Z());	
		}

		template <class T> Vector3DTemplate<T> Vector3DTemplate<T>::operator-() {
			return Vector3DTemplate<T>(-X(), -Y(), -Z());	
		}

		template <class T> Vector3DTemplate<T> Vector3DTemplate<T>::operator-(Vector3DTemplate<T> &d ) {
			return Vector3DTemplate<T>(X() - d.X(), Y() - d.Y(), Z() - d.Z());	
		}

		template <class T> Vector3DTemplate<T> Vector3DTemplate<T>::operator^(Vector3DTemplate<T> &d ) { // Cross Product
			return Vector3DTemplate<T>(
				values[1] * d.values[2] - values[2] * d.values[1], 
				values[2] * d.values[0] - values[0] * d.values[2], 
				values[0] * d.values[1] - values[1] * d.values[0]);	
		}

		template <class T> Vector3DTemplate<T> Vector3DTemplate<T>::operator*(double s) {
			return Vector3DTemplate<T>((T)(X()*s), (T)(Y()*s), (T)(Z()*s));
		}

		template <class T> Vector3DTemplate<T>& Vector3DTemplate<T>::operator+=(const Vector3DTemplate<T>& d) {
			for(int i = 0; i < 3; i++) {
				values[i] += d.values[i];
			}
			return *this;
		}

		template <class T> Vector3DTemplate<T>& Vector3DTemplate<T>::operator-=(const Vector3DTemplate<T>& d) {
			for(int i = 0; i < 3; i++) {
				values[i] -= d.values[i];
			}
			return *this;
		}
		template <class T> Vector3DTemplate<T>& Vector3DTemplate<T>::operator=(const Vector3DTemplate<T>& d) {
			for(int i = 0; i < 3; i++) {
				values[i] = d.values[i];
			}
			return *this;
		}

		template <class T> void Vector3DTemplate<T>::Normalize() {
			T base = 0;
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

		template <class T> Vector3DTemplate<T> Vector3DTemplate<T>::Normalize(Vector3DTemplate<T> d) {
			Vector3DTemplate<T> ret = d;
			d.Normalize();
			return ret;
		}

		template <class T> Vector3DTemplate<T> Vector3DTemplate<T>::Rotate(Vector3DTemplate<T> axis, double angle) {
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


			Vector3DTemplate<T> v = Vector3DTemplate(0.0, 0.0, 0.0);
			for(int x = 0; x < 3; x++) {
				for(int y = 0; y < 3; y++) {
					v.values[x] = v.values[x] + R[y][x] * values[y];
				}
			}
			return v;
		}

		template <class T> bool Vector3DTemplate<T>::IsBadNormal() {
			return !isZero(Length() - 1.0, 0.00001);
		}
	}
}

#endif