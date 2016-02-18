// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   An implementation of complex numbers


#ifndef COMPLEX_NUMBER_H
#define COMPLEX_NUMBER_H

#include "BasicDefines.h"
#include <cmath>

namespace wustl_mm {
	namespace MathTools {
		class ComplexNumber {
		public:
			ComplexNumber();
			ComplexNumber(double realComponent, double imaginaryComponent);

			ComplexNumber& operator=(const double& d);
			ComplexNumber& operator=(const ComplexNumber& d);
			ComplexNumber& operator+=(const double& d);
			ComplexNumber& operator+=(const ComplexNumber& d);
			ComplexNumber& operator-=(const double& d);
			ComplexNumber& operator-=(const ComplexNumber& d);
			ComplexNumber& operator*=(const double& d);
			ComplexNumber& operator*=(const ComplexNumber& d);
			ComplexNumber& operator/=(const double& d);
			ComplexNumber& operator/=(const ComplexNumber& d);
			ComplexNumber operator-() const;
			ComplexNumber operator+( const ComplexNumber &d ) const;
			ComplexNumber operator+( const double &d ) const;
			ComplexNumber operator-( const ComplexNumber &d ) const;
			ComplexNumber operator-( const double &d ) const;
			ComplexNumber operator*( const ComplexNumber &d ) const;
			ComplexNumber operator*( const double &d ) const;
			ComplexNumber operator/( const ComplexNumber &d ) const;
			ComplexNumber operator/( const double &d ) const;
			void SquareRoot();
			void CubeRoot(int rootCount);
			bool IsZero();
		private:
			ComplexNumber Pow(ComplexNumber n, double p, int piTimes = 0);
		public:
			double value;
			double iValue;
		};

		ComplexNumber::ComplexNumber() {
			iValue = 0;
			value = 0;
		}

		ComplexNumber::ComplexNumber(double realComponent, double imaginaryComponent) {
			iValue = imaginaryComponent;
			value = realComponent;
		}


		ComplexNumber& ComplexNumber::operator=(const double& d) {
			value = d;
			iValue = 0;
			return *this;
		}
		ComplexNumber& ComplexNumber::operator=(const ComplexNumber& d) {
			iValue = d.iValue;
			value = d.value;
			return *this;
		}

		ComplexNumber& ComplexNumber::operator+=(const double& d) {
			value += d;
			return *this;
		}

		ComplexNumber& ComplexNumber::operator+=(const ComplexNumber& d) {
			value += d.value;
			iValue += d.iValue;
			return *this;
		}

		ComplexNumber& ComplexNumber::operator-=(const double& d) {
			value -= d;
			return *this;
		}

		ComplexNumber& ComplexNumber::operator-=(const ComplexNumber& d) {
			value -= d.value;
			iValue -= d.iValue;
			return *this;
		}

		ComplexNumber& ComplexNumber::operator*=(const double& d) {
			value *= d;
			iValue *= d;
			return *this;
		}

		ComplexNumber& ComplexNumber::operator*=(const ComplexNumber& d) {
			value = value * d.value + iValue * d.iValue;
			iValue = value * d.iValue + iValue * d.value;
			return *this;
		}

		ComplexNumber& ComplexNumber::operator/=(const double& d) {
			if(isZero(d)) {
				value = 0;
				iValue = 0;
				printf("ComplexNumber::Division by zero!!!\n");
			} else {
				value /= d;
				iValue /= d;
			}
			return *this;
		}

		ComplexNumber& ComplexNumber::operator/=(const ComplexNumber& d) {
			if(isZero(d.value) && isZero(d.iValue)) {
				value = 0;
				iValue = 0;
				printf("ComplexNumber::Division by zero!!!\n");
			}

			if(isZero(iValue) && isZero(d.iValue)) {
				value = value / d.value;
			} else if(isZero(value) && isZero(d.value)) {
				iValue = iValue / d.iValue;
			} else {
				value = 0;
				iValue = 0;
				printf("ComplexNumber:: Cannot Divide {%f + %f i} by {%f + %f i}\n", value, iValue, d.value, d.iValue);
			}
			return *this;
		}

		ComplexNumber ComplexNumber::operator-() const {
			return ComplexNumber(-value, -iValue);
		}

		ComplexNumber ComplexNumber::operator+( const ComplexNumber &d ) const {
			return ComplexNumber(value + d.value, iValue + d.iValue);
		}	  

		ComplexNumber ComplexNumber::operator+( const double &d ) const {
			return ComplexNumber(value + d, iValue);
		}	
		ComplexNumber ComplexNumber::operator-( const ComplexNumber &d ) const {
			return ComplexNumber(value - d.value, iValue - d.iValue);
		}	  

		ComplexNumber ComplexNumber::operator-( const double &d ) const {
			return ComplexNumber(value - d, iValue);
		}	
		ComplexNumber ComplexNumber::operator*( const ComplexNumber &d ) const {
			return ComplexNumber(value * d.value + iValue * d.iValue, value * d.iValue + iValue * d.value);
		}	  

		ComplexNumber ComplexNumber::operator*( const double &d ) const {
			return ComplexNumber(value * d, iValue * d);
		}	

		ComplexNumber ComplexNumber::operator/( const ComplexNumber &d ) const {
			double v = value, i = iValue;

			if(isZero(d.value) && isZero(d.iValue)) {
				v = 0;
				i = 0;
				printf("ComplexNumber::Division by zero!!!\n");
			}

			if(isZero(iValue) && isZero(d.iValue)) {
				v = value / d.value;
			} else if(isZero(value) && isZero(d.value)) {
				i = iValue / d.iValue;
			} else {
				ComplexNumber t1(value, iValue);
				ComplexNumber t2(d.value, -d.iValue);
				t1 = t1*t2;
				t1 = t1/(d.value*d.value + d.iValue*d.iValue);
				v = t1.value;
				i = t1.iValue;
			}

			return ComplexNumber(v, i);
		}	  

		ComplexNumber ComplexNumber::operator/( const double &d ) const {
			double v = value, i = iValue;
			if(isZero(d)) {
				v = 0;
				i = 0;
				printf("ComplexNumber::Division by zero!!!\n");
			} else {
				v = v / d;
				i = i / d;
			}
			return ComplexNumber(v, i);
		}	
		void ComplexNumber::SquareRoot() {
			
			if(isZero(value) && isZero(iValue)) {
				value = 0;
				iValue = 0;
			} else if(isZero(iValue)) {
				if(value > 0) {
					value = sqrt(value);
				} else {
					iValue = sqrt(-value);
					value = 0;
				}
			} else {
				ComplexNumber temp = ComplexNumber(value, iValue);
				temp = Pow(temp, 0.5);
				value = temp.value;
				iValue = temp.iValue;
			}

			//printf("SQRT(%f + %fi)    = %f + %fi\n", v, i, value, iValue);

		}

		void ComplexNumber::CubeRoot(int rootCount) {
			
			if(isZero(value) && isZero(iValue)) {
				value = 0;
				iValue = 0;
			} else if(isZero(iValue)) {
				if(value > 0) {
					value = pow(value, 1.0/3.0);
				} else {
					value = -pow(-value, 1.0/3.0);
				}
			} else {
				ComplexNumber temp = ComplexNumber(value, iValue);
				temp = Pow(temp, 1.0/3.0, rootCount-1);
				value = temp.value;
				iValue = temp.iValue;
			}

			//printf("CRT(%f + %fi)    = %f + %fi\n", v, i, value, iValue);
		}

		ComplexNumber ComplexNumber::Pow(ComplexNumber n, double p, int piTimes) {
			double r = sqrt(n.value*n.value + n.iValue*n.iValue);
			double t;
			double pi = 3.1415926535897932385;
			if((n.value) == 0) {
				if(n.iValue > 0) { 
					t = pi/2.0 + 2*pi * (double)piTimes; 
				} else {
					t = -pi/2.0 + 2*pi * (double)piTimes; 
				}
			} else {
				t = tanh(n.iValue/n.value) + 2*pi * (double)piTimes;
			}
				
			r = pow(r, p);
			t = t*p;
			//printf("POW(%f + %fi)%f    = %f + %fi\n", n.value, n.iValue, p, r*cos(t), r*sin(t));
			return ComplexNumber(r*cos(t), r*sin(t));
		}


		bool ComplexNumber::IsZero() {
			return isZero(value) && isZero(iValue);
		}
	}
}

#endif
