// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A template class for matrices


#ifndef MATHTOOLS_MATRIX_TEMPLATE_H
#define MATHTOOLS_MATRIX_TEMPLATE_H

#include <alglib/svd.h>
#include <alglib/ap.h>
#include <vector>

using namespace std;

namespace wustl_mm {
	namespace MathTools {
		template <class T> 
		class MatrixTemplate {
		public:
			MatrixTemplate();
			MatrixTemplate(unsigned int rowCount, unsigned int colCount);
			~MatrixTemplate();

			unsigned int GetRowCount() const;
			unsigned int GetColCount() const;
			
			T GetValue(int row, int col) const;
			void SetValue(T value, int row, int col);

			MatrixTemplate<T> operator+(const MatrixTemplate<T> &m );
			MatrixTemplate<T> operator-(const MatrixTemplate<T> &m );
			MatrixTemplate<T> operator*(const MatrixTemplate<T> & m );
			MatrixTemplate<T>& operator=(const MatrixTemplate<T> &m );
			MatrixTemplate<T>& operator+=(const MatrixTemplate<T> &m );
			MatrixTemplate<T>& operator-=(const MatrixTemplate<T> &m );
			MatrixTemplate<T> operator*(const T &s);
			MatrixTemplate<T> Transpose();
			MatrixTemplate<T> ConjugateTranspose();			

			void SingularValueDecomposition(MatrixTemplate <T> &u, MatrixTemplate <T> &w, MatrixTemplate <T> &v);

			T Determinant();

			static MatrixTemplate<T> Identity(int size);
			static MatrixTemplate<T> AllZero(int rowCount, int colCount);
			static MatrixTemplate<T> AllOne(int rowCount, int colCount);
			static MatrixTemplate<T> Random(int rowCount, int colCount);
			static MatrixTemplate<T> Covariance(MatrixTemplate<T> m1, MatrixTemplate<T> m2);
			static T Covariance(vector<T> v1, vector<T> v2);
			void Print(bool isInt);

		private:
			unsigned int GetIndex(int row, int col) const;

		private:
			unsigned int rowCount;
			unsigned int colCount;	
			vector<T> values;
		};

		template <class T>
		MatrixTemplate<T>::MatrixTemplate() {
			rowCount = 0;
			colCount = 0;
			values.clear();
		}

		template <class T>
		MatrixTemplate<T>::MatrixTemplate(unsigned int rowCount, unsigned int colCount) {
			this->rowCount = rowCount;
			this->colCount = colCount;
			values.clear();
			for(unsigned int i = 0; i < rowCount * colCount; i++) {
				values.push_back((T)0);
			}
		}

		template <class T>
		MatrixTemplate<T>::~MatrixTemplate() {
			values.clear();
		}

		template <class T>
		T MatrixTemplate<T>::GetValue(int row, int col) const {
			return values[GetIndex(row, col)];
		}

		template <class T>
		void MatrixTemplate<T>::SetValue(T value, int row, int col) {
			values[GetIndex(row, col)] = value;
		}


		template <class T>
		MatrixTemplate<T> &MatrixTemplate<T>::operator=(const MatrixTemplate<T>& m) {
			if((rowCount != m.GetRowCount()) || (colCount != m.GetColCount())) {
				printf("Error! Assigning matrices of incompatible dimensions!\n");
				exit(0);
			}

			for(unsigned int i = 0; i < rowCount; i++) {
				for(unsigned int j = 0; j < colCount; j++) {
					this->SetValue(m.GetValue(i, j), i, j);
				}							
			}
			return *this;
		}

		template <class T>
		MatrixTemplate<T> MatrixTemplate<T>::operator+(const MatrixTemplate<T> &m ) {
			if((rowCount != m.GetRowCount()) || (colCount != m.GetColCount())) {
				printf("Error! Adding matrices of incompatible dimensions!\n");
				exit(0);
			}

			MatrixTemplate<T> retVal = MatrixTemplate<T>(rowCount, colCount);

			for(unsigned int i = 0; i < rowCount; i++) {
				for(unsigned int j = 0; j < colCount; j++) {
					retVal.SetValue(this->GetValue(i, j) + m.GetValue(i, j), i, j);
				}							
			}
			return retVal;
		}			
		template <class T>
		MatrixTemplate<T> MatrixTemplate<T>::operator*(const MatrixTemplate<T> & m ) {
			if(colCount != m.GetRowCount()) {
				printf("Error! Multiplying matrices of incompatible dimensions!\n");
				exit(0);
			}

			MatrixTemplate<T> retVal = MatrixTemplate<T>(rowCount, m.GetColCount());

			T val;
			for(unsigned int i = 0; i < rowCount; i++) {
				for(unsigned int j = 0; j < m.GetColCount(); j++) {
					val = (T)0;
					for(unsigned int k = 0; k < colCount; k++) {
						val += GetValue(i, k) * m.GetValue(k, j);
					}
					retVal.SetValue(val, i, j);					
				}							
			}
			return retVal; 
		}

		template <class T>
		MatrixTemplate<T> MatrixTemplate<T>::operator-(const MatrixTemplate<T> &m ) {
			if((rowCount != m.GetRowCount()) || (colCount != m.GetColCount())) {
				printf("Error! Adding matrices of incompatible dimensions!\n");
				exit(0);
			}

			MatrixTemplate<T> retVal = MatrixTemplate<T>(rowCount, colCount);

			for(unsigned int i = 0; i < rowCount; i++) {
				for(unsigned int j = 0; j < colCount; j++) {
					retVal.SetValue(this->GetValue(i, j) - m.GetValue(i, j), i, j);
				}							
			}
			return retVal;
		}

		template <class T>
		MatrixTemplate<T>& MatrixTemplate<T>::operator+=(const MatrixTemplate<T> &m ) {
			if((rowCount != m.GetRowCount()) || (colCount != m.GetColCount())) {
				printf("Error! Adding matrices of incompatible dimensions!\n");
				exit(0);
			}

			for(unsigned int i = 0; i < rowCount; i++) {
				for(unsigned int j = 0; j < colCount; j++) {
					SetValue(GetValue(i, j) + m.GetValue(i, j), i, j);
				}							
			}
			return *this;
		}

		template <class T>
		MatrixTemplate<T>& MatrixTemplate<T>::operator-=(const MatrixTemplate<T> &m ) {
			if((rowCount != m.GetRowCount()) || (colCount != m.GetColCount())) {
				printf("Error! Adding matrices of incompatible dimensions!\n");
				exit(0);
			}

			for(unsigned int i = 0; i < rowCount; i++) {
				for(unsigned int j = 0; j < colCount; j++) {
					this->SetValue(this->GetValue(i, j) - m.GetValue(i, j), i, j);
				}							
			}
			return *this;
		}

		template <class T>
		MatrixTemplate<T> MatrixTemplate<T>::operator*(const T &s) {
			MatrixTemplate<T> retVal = MatrixTemplate(rowCount, colCount);
			for(unsigned int i = 0; i < rowCount; i++) {
				for(unsigned int j = 0; j < colCount; j++) {
					retVal.SetValue(this->GetValue(i, j) * s, i, j);
				}							
			}
			return retVal;
		}

		template <class T>
		MatrixTemplate<T> MatrixTemplate<T>::Transpose() {
			MatrixTemplate<T> retVal = MatrixTemplate(colCount, rowCount);
			for(unsigned int i = 0; i < rowCount; i++) {
				for(unsigned int j = 0; j < colCount; j++) {
					retVal.SetValue(this->GetValue(i, j), j, i);
				}							
			}
			return retVal;
		}


		template <class T>
		MatrixTemplate<T> MatrixTemplate<T>::ConjugateTranspose() {			
			return Transpose();
		}


		template <class T>
		unsigned int MatrixTemplate<T>::GetRowCount() const  {
			return rowCount;
		}

		template <class T>
		unsigned int MatrixTemplate<T>::GetColCount() const {
			return colCount;
		}

		template <class T>
		unsigned int MatrixTemplate<T>::GetIndex(int row, int col) const  {
			return row*colCount + col;
		}

		template <class T>
		T MatrixTemplate<T>::Determinant() {
			if(rowCount != colCount) {
				printf("Error! Determinant can be obtained of square matrices ONLY.");
				exit(0);
			}

			T a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p;
			switch(rowCount) {
				case 1:
					return GetValue(0,0);
				case 2: 
					a = GetValue(0,0);	b = GetValue(0,1);
					c = GetValue(1,0);	d = GetValue(1,1);
					return a*d - b*c;
				case 3: 
					a = GetValue(0,0);	b = GetValue(0,1);	c = GetValue(0,2);
					d = GetValue(1,0);	e = GetValue(1,1);	f = GetValue(1,2);
					g = GetValue(2,0);	h = GetValue(2,1);	i = GetValue(2,2);
					return a*e*i - a*f*h + b*f*g - b*d*i + c*d*h - c*e*g;
				case 4:
					a = GetValue(0,0);	b = GetValue(0,1);	c = GetValue(0,2);	d = GetValue(0,3);
					e = GetValue(1,0);	f = GetValue(1,1);	g = GetValue(1,2);	h = GetValue(1,3);
					i = GetValue(2,0);	j = GetValue(2,1);	k = GetValue(2,2);	l = GetValue(2,3);
					m = GetValue(3,0);	n = GetValue(3,1);	o = GetValue(3,2);	p = GetValue(3,3);
					return d*g*j*m - c*h*j*m - d*f*k*m + b*h*k*m + c*f*l*m - b*g*l*m - d*g*i*n + 
						c*h*i*n + d*e*k*n - a*h*k*n - c*e*l*n + a*g*l*n + d*f*i*o - b*h*i*o -
						d*e*j*o + a*h*j*o + b*e*l*o - a*f*l*o - c*f*i*p + b*g*i*p + 
						c*e*j*p - a*g*j*p - b*e*k*p + a*f*k*p;
				default:
					printf("Error! Determinant of matrixes larger than 4x4 not implemented!\n");
					exit(0);
			}								
		}

		template <class T>
		void MatrixTemplate<T>::Print(bool isInt) {
			printf("\n");
			for(unsigned int i = 0; i < rowCount; i++) {
				for(unsigned int j = 0; j < colCount; j++) {
					if(isInt) {
						printf("%d ", GetValue(i, j));
					} else {
						printf("%f ", GetValue(i, j));
					}
				}
				printf("\n");
			}

		}

		template <class T>
		MatrixTemplate<T> MatrixTemplate<T>::Identity(int size){
			MatrixTemplate<T> retVal = MatrixTemplate<T>(size, size);
			for(int i = 0; i < size; i++) {
				retVal.SetValue((T)1, i, i);
			}
			return retVal;
		}

		template <class T>
		MatrixTemplate<T> MatrixTemplate<T>::AllZero(int rowCount, int colCount) {
			MatrixTemplate<T> retVal = MatrixTemplate<T>(rowCount, colCount);
			return retVal;
		}

		template <class T>
		MatrixTemplate<T> MatrixTemplate<T>::AllOne(int rowCount, int colCount) {
			MatrixTemplate<T> retVal = MatrixTemplate<T>(rowCount, colCount);
			for(unsigned int i = 0; i < rowCount; i++) {
				for(unsigned int j = 0; j < colCount; j++) {
					retVal.SetValue((T)1, i, j);
				}
			}
			return retVal;
		}

		template <class T>
		MatrixTemplate<T> MatrixTemplate<T>::Random(int rowCount, int colCount) {
			MatrixTemplate<T> retVal = MatrixTemplate<T>(rowCount, colCount);
			for(unsigned int i = 0; i < rowCount; i++) {
				for(unsigned int j = 0; j < colCount; j++) {
					retVal.SetValue((T)rand(), i, j);
				}
			}
			return retVal;
		}
	
		template <class T>
		void MatrixTemplate<T>::SingularValueDecomposition(MatrixTemplate <T> &u, MatrixTemplate <T> &w, MatrixTemplate <T> &v) {
			ap::real_2d_array a;
			a.setlength(rowCount, colCount);
			for(unsigned int i = 0; i < rowCount; i++) {
				for(unsigned int j = 0; j < colCount; j++) {
					a(i, j) = GetValue(i, j);
				}
			}		

			ap::real_2d_array retU;
			ap::real_1d_array retW;
			ap::real_2d_array retVT;

			rmatrixsvd(a, rowCount, colCount, 2, 2, 2, retW, retU, retVT);

			for(unsigned int i = 0; i < rowCount; i++) {
				for(unsigned int j = 0; j < rowCount; j++) {
					u.SetValue((T)retU(i, j), i, j);
				}
			}

			for(unsigned int i = 0; i < rowCount; i++) {
				w.SetValue((T)retW(i), i, i);
			}

			for(unsigned int i = 0; i < colCount; i++) {
				for(unsigned int j = 0; j < colCount; j++) {
					v.SetValue((T)retVT(i, j), j, i);
				}
			}
		}

		template <class T>
		MatrixTemplate<T> MatrixTemplate<T>::Covariance(MatrixTemplate<T> m1, MatrixTemplate<T> m2) {
			if(m1.GetRowCount() != m2.GetRowCount()) {
				printf("Error! Unable to get covariance of two matrices with incompatible row counts\n");
				exit(0);
			}

			MatrixTemplate<T> retVal = MatrixTemplate<T>(m1.GetColCount(), m2.GetColCount());
			vector<T> c1, c2;

			for(unsigned int i = 0; i < m1.GetColCount(); i++) {
				c1.clear();
				for(unsigned int k = 0; k < m1.GetRowCount(); k++) {
					c1.push_back(m1.GetValue(k, i));
				}
				for(unsigned int j = 0; j < m2.GetColCount(); j++) {				
					c2.clear();
					for(unsigned int k = 0; k < m2.GetRowCount(); k++) {
						c2.push_back(m2.GetValue(k, j));
					}
					retVal.SetValue(Covariance(c1, c2), i, j);
				}
			}
			return retVal.Transpose();
		}

		template <class T>
		T MatrixTemplate<T>::Covariance(vector<T> v1, vector<T> v2) {
			if(v1.size() != v2.size()) {
				printf("Error! Covariance can be found of only vectors of same size.\n");
				exit(0);
			}

			double covar = 0;
			double multiplier;
			unsigned int n = v1.size();
			switch(n) {
				case 0: 
					covar = 0;
					break;
				case 1:
					covar = 0;
					break;
				case 2:
					covar = (double)(v1[0] - v1[1]) * (double)(v2[0] - v2[1]) * 0.5;
					break;
				default:					
					for(unsigned int i = 0; i < n; i++) {
						multiplier = 0;
						for(unsigned int j = 0; j < n; j++) {
							if(i == j) {
								multiplier += (double)n * (double)v1[j];
							} else {
								multiplier -= (double)v1[j];
							}
						}
						covar += multiplier * (double)v2[i];
					}
					covar = covar / (double)(n * n-1);
					break;
			}								

			return (T)covar;	
			
		}
	}
}

#endif
