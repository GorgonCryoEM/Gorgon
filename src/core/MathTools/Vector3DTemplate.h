// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A template class for 3D vectors


#ifndef MATHTOOLS_VECTORT3DTEMPLATE_H
#define MATHTOOLS_VECTORT3DTEMPLATE_H

#include <cstdio>
#include <cmath>
#include "BasicDefines.h"
#include "Matrix.h"

namespace wustl_mm {
	namespace MathTools {
		template <class T> class Vector3DTemplate {
		public:
			Vector3DTemplate();
			Vector3DTemplate(T x, T y, T z);
			Vector3DTemplate(const vector<T>& vec);
			Vector3DTemplate(const Vector3DTemplate<T>& rhs);
			~Vector3DTemplate();

			T X() const;
			T Y() const;
			T Z() const;
			T& operator[](int subscript);
			T operator[](int subscript) const;
			double Length();
			T operator*(const Vector3DTemplate<T> &d ); // Dot Product							
			int XInt();
			int YInt();
			int ZInt();

			bool operator!=(Vector3DTemplate<T> &d);
			bool operator==(Vector3DTemplate<T> &d);	
			bool operator>(Vector3DTemplate<T> &d);
			bool operator<(Vector3DTemplate<T> &d);
			bool operator>=(Vector3DTemplate<T> &d);
			bool operator<=(Vector3DTemplate<T> &d);

			Vector3DTemplate<T> operator+(const Vector3DTemplate<T> &d);
			Vector3DTemplate<T> operator-();
			Vector3DTemplate<T> operator-(const Vector3DTemplate<T> &d );
			Vector3DTemplate<T> operator^(const Vector3DTemplate<T> &d );		 // Cross Product
			Vector3DTemplate<T> operator*(double s);			
			Vector3DTemplate<T>& operator=(const Vector3DTemplate<T>& d);
			Vector3DTemplate<T>& operator+=(const Vector3DTemplate<T>& d);
			Vector3DTemplate<T>& operator-=(const Vector3DTemplate<T>& d);
			Vector3DTemplate<T> GetOrthogonal();
			Vector3DTemplate<T> Rotate(Vector3DTemplate<T> axis, double angle);			
			Vector3DTemplate<T> Transform(MatrixTemplate<T> transformation);
			T * begin();
			T * end();

			void Normalize();
			bool IsBadNormal();
			void Print();

			static Vector3DTemplate<T> Normalize(Vector3DTemplate<T> d);
			static Vector3DTemplate<T> Project3Dto2D(Vector3DTemplate<T> point, Vector3DTemplate<T> planePt, Vector3DTemplate<T> planeVec1, Vector3DTemplate<T> planeVec2);

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
		
		template <class T> Vector3DTemplate<T>::Vector3DTemplate(const vector<T>& vec) {
			values[0] = vec[0];
			values[1] = vec[1];
			values[2] = vec[2];
		}

		template <class T> Vector3DTemplate<T>::Vector3DTemplate(const Vector3DTemplate<T>& rhs) {
			values[0] = rhs.values[0];
			values[1] = rhs.values[1];
			values[2] = rhs.values[2];
		}		

		template <class T> Vector3DTemplate<T>::~Vector3DTemplate() {
		}


		template <class T> bool Vector3DTemplate<T>::operator!=(Vector3DTemplate<T> &d) {
			return (X() != d.X()) || (Y() != d.Y()) || (Z() != d.Z());
		}

		template <class T> bool Vector3DTemplate<T>::operator==(Vector3DTemplate<T> &d) {
			return (X() == d.X()) && (Y() == d.Y()) && (Z() == d.Z());
		}

		template <class T> bool Vector3DTemplate<T>::operator>(Vector3DTemplate<T> &d) {
			return (X() > d.X()) || 
				((X() == d.X()) && (Y() > d.Y())) ||
				((X() == d.X()) && (Y() == d.Y()) && (Z() > d.Y()));
		}

		template <class T> bool Vector3DTemplate<T>::operator<(Vector3DTemplate<T> &d) {
			return d > *this;
		}

		template <class T> bool Vector3DTemplate<T>::operator>=(Vector3DTemplate<T> &d) {
			return (*this > d) || (*this == d);
		}

		template <class T> bool Vector3DTemplate<T>::operator<=(Vector3DTemplate<T> &d) {
			return (*this < d) || (*this == d);
		}

		template <class T> T Vector3DTemplate<T>::X() const {
			return values[0];
		}

		template <class T> T Vector3DTemplate<T>::Y() const {
			return values[1];
		}

		template <class T> T Vector3DTemplate<T>::Z() const {
			return values[2];
		}
		template <class T> T& Vector3DTemplate<T>::operator[](int subscript) {
			if (subscript < 0 || subscript > 2)
				throw "Vector3D [] operator BoundsError!\n";
			else
				return values[subscript];
		}
		template <class T> T Vector3DTemplate<T>::operator[](int subscript) const {
			if (subscript < 0 || subscript > 2)
				throw "Vector3D [] operator BoundsError!\n";
			else
				return values[subscript];
		}

		template <class T> double Vector3DTemplate<T>::Length() {
			return (double)sqrt((double)(values[0] * values[0] + values[1] * values[1] + values[2] * values[2]));
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

		template <class T> T Vector3DTemplate<T>::operator*(const Vector3DTemplate<T> &d ) {		// Dot Product
			return X() * d.X() + Y() * d.Y() + Z() * d.Z();
		}

		template <class T> Vector3DTemplate<T> Vector3DTemplate<T>::operator+(const Vector3DTemplate<T> &d) {
			return Vector3DTemplate<T>(X() + d.X(), Y() + d.Y(), Z() + d.Z());	
		}		
		
		template <class T> Vector3DTemplate<T> Vector3DTemplate<T>::operator-() {
			return Vector3DTemplate<T>(-X(), -Y(), -Z());	
		}

		template <class T> Vector3DTemplate<T> Vector3DTemplate<T>::operator-(const Vector3DTemplate<T> &d ) {
			return Vector3DTemplate<T>(X() - d.X(), Y() - d.Y(), Z() - d.Z());	
		}

		template <class T> Vector3DTemplate<T> Vector3DTemplate<T>::operator^(const Vector3DTemplate<T> &d ) { // Cross Product
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

		/** Add this function to make it iterable in Python,
		 * so we can call list() or tuple() to convert Vec3f in python to a list or tuple.
		 *
		 * @return the iterator (here is the pointer) of the first element
		 * */
		template <class T> T * Vector3DTemplate<T>::begin() {
			return &values[0];
		}

		/** Add this function to make it iterable in Python,
		 * so we can call list() or tuple() to convert Vec3f in python to a list or tuple.
		 *
		 * @return the iterator (here is the pointer) of the one beyond the last element.
		 * */
		template <class T> T * Vector3DTemplate<T>::end() {
			return &values[2];
		}

		template <class T> void Vector3DTemplate<T>::Normalize() {
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

		template <class T> Vector3DTemplate<T> Vector3DTemplate<T>::Normalize(Vector3DTemplate<T> d) {
			Vector3DTemplate<T> ret = d;
			ret.Normalize();
			return ret;  
		}

		template <class T> Vector3DTemplate<T> Vector3DTemplate<T>::GetOrthogonal() {
			Vector3DTemplate<T> orthVec = Vector3DTemplate<T>(1, 1, 1);
			orthVec = Vector3DTemplate<T>(this->X(), this->Y(), this->Z()) ^ orthVec;
			if(isZero(orthVec.Length())) {
				orthVec = Vector3DTemplate<T>(1, -1, -1);
				orthVec = Vector3DTemplate<T>(this->X(), this->Y(), this->Z()) ^ orthVec;
			}
			return orthVec;
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

		template <class T> Vector3DTemplate<T> Vector3DTemplate<T>::Project3Dto2D(Vector3DTemplate<T> point, Vector3DTemplate<T> planePt, Vector3DTemplate<T> planeVec1, Vector3DTemplate<T> planeVec2) {
			Vector3DTemplate<T> ray = point - planePt;
			return Vector3DTemplate<T>(ray * planeVec1, ray * planeVec2, 0);
			//return Vector3DTemplate<T>(point * planeVec1, point * planeVec2, 0);
			
		}

		template <class T> void Vector3DTemplate<T>::Print() {
			printf("{%f, %f, %f} \n", X(), Y(), Z());
		}

		template <class T> Vector3DTemplate<T> Vector3DTemplate<T>::Transform(MatrixTemplate<T> transformation) {
			MatrixTemplate<T> p = MatrixTemplate<T>(4, 1);
			p.SetValue(values[0], 0, 0);
			p.SetValue(values[1], 1, 0);
			p.SetValue(values[2], 2, 0);
			p.SetValue((T)1, 3, 0);
			
			p = transformation * p;
			return Vector3DTemplate<T>(p.GetValue(0,0), p.GetValue(1,0), p.GetValue(2,0));
		}
	}
}

#endif

