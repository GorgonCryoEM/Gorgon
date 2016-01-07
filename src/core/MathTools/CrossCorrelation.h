// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Ross Coleman (racolema@bcm.edu)
// Description:   C++ Implementation of SSEHunter


#ifndef MATHTOOLS_CROSS_CORRELATION_H
#define MATHTOOLS_CROSS_CORRELATION_H

#include <complex>
#include <cstdlib>
#include <eman/FFT.h>

using namespace EMAN;

namespace wustl_mm {
	namespace MathTools {

		class OutOfBoundsException {};
		class InvalidComplexNumberArray {};

		// *****************************************************************************
		// getComplexDataAt
		// An array of complex numbers has the real and imaginary parts of each number
		// next to each other in memory. This function interprets the supplied indices
		// (specifically the fast index) as if the array has complex-number values.
		//
		// Parameters medSize and slowSize are the same whether it is real or complex
		// valued. The parameter fastSize is the actual number of floats along that 
		// dimension. If fastSize is odd, the function raises an exception, because the 
		// array can't be interpreted as a complex-number valued array.
		//
		// If you provide a value >= 1 for slowSize, the function performs
		// bounds-checking on your indices.
		// *****************************************************************************
		std::complex<float>& getComplexDataAt(float* data, int fastIx, int medIx, int slowIx,
				int fastSize, int medSize, int slowSize=-1);

		// *****************************************************************************
		// fftInPlace
		// Performs a Fast Fourier Transform on the array.
		// Before doing Fourier transforms, the fast index dimension must be padded
		// with 2 pixels if it had an even size or 1 pixel if it had an odd size.
		// Don't include this padding as part of the fastSizeMinusPadding.
		// Note: this does not normalize the data.
		// *****************************************************************************
		inline void fftInPlace(float* data, int fastSizeMinusPadding, int medSize, int slowSize) {
			EMfft::real_to_complex_nd(data, data, fastSizeMinusPadding, medSize, slowSize);
		}

		// *****************************************************************************
		// iftInPlace
		// Performs a Fast Inverse Fourier Transform on the array
		// Before doing Fourier transforms, the fast index dimension must be padded
		// with 2 pixels if it had an even size or 1 pixel if it had an odd size
		// Don't include this padding as part of the fastSizeMinusPadding.
		// Note: this does not normalize the data.
		// *****************************************************************************
		inline void iftInPlace(float* data, int fastSizeMinusPadding, int medSize, int slowSize) {
			EMfft::complex_to_real_nd(data, data, fastSizeMinusPadding, medSize, slowSize);
		}

		// *****************************************************************************
		// fftOutOfPlace
		// This function assumes that the fast index dimension is not padded with the 
		// 1 or 2 pixels required for in-place FFTs.
		// Note: this does not normalize the data.
		// *****************************************************************************
		inline void fftOutOfPlace(float* in, float* out, int fastSize, int medSize, int slowSize) {
			EMfft::real_to_complex_nd(in, out, fastSize, medSize, slowSize);
		}
		// *****************************************************************************
		// iftOutOfPlace
		// This function assumes that the fast index dimension is padded with the 
		// 1 or 2 pixels required for in-place FFTs.
		// Note: this does not normalize the data.
		// *****************************************************************************
		inline void iftOutOfPlace(float* in, float* out, int fastSize, int medSize, int slowSize) {
			EMfft::complex_to_real_nd(in, out, fastSize, medSize, slowSize);
		}

		// *****************************************************************************
		// cftInPlace
		// This calculates the Cross Fourier Transform: conj(cx_conj_of_this)*as_is
		// It stores the CFT in cx_conj_of_this
		// *****************************************************************************
		void cftInPlace(float* cx_conj_of_this, const float* as_is, int array_size);

		// *****************************************************************************
		// cftOutOfPlace
		// This calculates the Cross Fourier Transform: conj(cx_conj_of_this)*as_is
		// It returns the CFT in a new float array (using malloc)
		// *****************************************************************************
		float* cftOutOfPlace(const float* cx_conj_of_this, const float* as_is, int array_size);

		// *****************************************************************************
		// mftInPlace
		// This calculates the Mutual Fourier Transform.
		// voxel = [conj(cx_conj_of_this)*as_is]
		// voxel = voxel/sqrt(amplitude(voxel))
		// It stores the MFT in cx_conj_of_this
		//
		// Marin van Heel, Michael Schatz, Elena Orlova, Correlation functions revisited,
		// Ultramicroscopy, Volume 46, Issues 1-4, October 1992, Pages 307-316,
		// ISSN 0304-3991, DOI: 10.1016/0304-3991(92)90021-B.
		// *****************************************************************************
		void mftInPlace(float* cx_conj_of_this, const float* as_is, int array_size);

		// ****************************************************************************
		// corrInPlace: in-place calculation of MCF or CCF
		// @f: complex conjugate of FT of f will be used; results stored in f
		// @g: same dimensions as f; will hold FT(g) when the function returns
		// @fastSizeMinusPadding: the logical size of the fast changing index--does not include fft padding
		// @medSize: the logical size == memory size of middle index
		// @slowSize: the logical size == memory size of slow changing index
		// @useMCF: calculate MCF (Mutual-Correlation Function) or CCF (Cross-Correlation Function)
		// Note: Before calling this function, the fast index dimension of the arrays for
		// f and g should be padded by 1 pixel if odd-sized or 2 pixels if even sized.
		// ****************************************************************************
		void corrInPlace(float* f, float* g, int fastSizeMinusPadding, int medSize,
				int slowSize, bool useMCF=true, bool center = false);


		// ****************************************************************************
		// corrOutOfPlace: out-of-place calculation of MCF or CCF
		// @f: complex conjugate of FT(f) will be used in calculation
		// @g: same dimensions as f
		// @fastSize: size of the fast-changing array index
		// @medSize: size of the middle array index
		// @slowSize: size of the slow-changing array index
		// @return: the MCF or the CCF
		// Note: This function expects f and g not to have any padding in the fast index
		// dimension, because it is not needed for out-of-place FFT's.
		// ****************************************************************************
		float* corrOutOfPlace(float* f, float* g, int fastSize,
				int medSize, int slowSize, bool useMCF=true, bool center = false);









// *************************************************************************************************
// Code below can go in cpp file
// *************************************************************************************************

		// *****************************************************************************
		// getComplexDataAt
		// *****************************************************************************
		std::complex<float>& getComplexDataAt(float* data, int fastIx, int medIx, int slowIx,
				int fastSize, int medSize, int slowSize) {
			if (fastSize % 2) //Must be even if it is to hold complex numbers
				throw InvalidComplexNumberArray();

			//fastIx specifies complex numbers which require two float values
			int index = 2*fastIx + (medIx + slowIx*medSize)*fastSize;

			if (slowSize < 1) //Default value is -1
				return *(reinterpret_cast<std::complex<float>* >(data + index));
			else { //Do bounds checking
				if (fastIx < fastSize/2 || medIx < medSize || slowIx < slowSize)
					return *( reinterpret_cast<std::complex<float>* >(data + index) );
				else
					throw OutOfBoundsException();
			}
		}


		// ****************************************************************************
		// cftInPlace
		// ****************************************************************************
		void cftInPlace(float* cx_conj_of_this, const float* as_is, int array_size) {
			float re1, im1;
			float re2, im2;
			for (int i=0; i<array_size; i+=2) {
				// conj(re1+im1*i) * (re2+im2*i) = (re1-im1*i) * (re2+im2*i) = (re1*re2+im1*im2) + (re1*im2-re2*im1)*i
				re1 = cx_conj_of_this[i];
				im1 = cx_conj_of_this[i+1];
				re2 = as_is[i];
				im2 = as_is[i+1];

				cx_conj_of_this[i] = re1*re2+im1*im2; //real
				cx_conj_of_this[i+1] = re1*im2-re2*im1; //imaginary
			}
		}

		// ****************************************************************************
		// cftOutOfPlace
		// ****************************************************************************
		float* cftOutOfPlace(const float* cx_conj_of_this, const float* as_is, int array_size) {
			float* cft = (float*) malloc(sizeof(float)*array_size);
			float re1, im1;
			float re2, im2;
			for (int i=0; i<array_size; i+=2) {
				// conj(re1+im1*i) * (re2+im2*i) = (re1-im1*i) * (re2+im2*i) = (re1*re2+im1*im2) + (re1*im2-re2*im1)*i
				re1 = cx_conj_of_this[i];
				im1 = cx_conj_of_this[i+1];
				re2 = as_is[i];
				im2 = as_is[i+1];

				cft[i] = re1*re2+im1*im2; //real
				cft[i+1] = re1*im2-re2*im1; //imaginary
			}
			return cft;
		}

		// ****************************************************************************
		// mftInPlace
		// ****************************************************************************
		void mftInPlace(float* cx_conj_of_this, const float* as_is, int array_size) {
			float re, im;
			float re1, im1;
			float re2, im2;
			float temp;
			for (int i=0; i<array_size; i+=2) {
				// conj(re1+im1*i) * (re2+im2*i) = (re1-im1*i) * (re2+im2*i) = (re1*re2+im1*im2) + (re1*im2-re2*im1)*i
				re1 = cx_conj_of_this[i];
				im1 = cx_conj_of_this[i+1];
				re2 = as_is[i];
				im2 = as_is[i+1];

				re = re1*re2+im1*im2;
				im = re1*im2-re2*im1;

				temp = re*re+im*im;
				temp = pow(temp, 0.25f);
				re = re/temp;
				im = im/temp;

				cx_conj_of_this[i] = re;
				cx_conj_of_this[i+1] = im;
			}
		}

		// ****************************************************************************
		// corrInPlace
		// ****************************************************************************
		void corrInPlace(float* f, float* g, int fastSizeMinusPadding,
								int medSize, int slowSize, bool useMCF, bool center) {

			fftInPlace(f, fastSizeMinusPadding, medSize, slowSize);
			fftInPlace(g, fastSizeMinusPadding, medSize, slowSize);

			//Now that we are in Fourier space, all indices (including 1 or 2 pixel padding) are used
			int fastSize = (fastSizeMinusPadding % 2 ? fastSizeMinusPadding+1 : fastSizeMinusPadding+2);
			int array_size = fastSize*medSize*slowSize;
			int N = fastSizeMinusPadding*medSize*slowSize;

			if (useMCF)
				mftInPlace(f, g, array_size);
			else
				cftInPlace(f, g, array_size);
			iftInPlace(f, fastSizeMinusPadding, medSize, slowSize);

			//Normalize the data
			for (int i=0; i < array_size; i++) {
				f[i] /= N;
			}

			if (center) {
				// Placeholder: puts the center of the correlation at the center
				// of the map instead of (0,0,0)
			}
		}

		// ****************************************************************************
		// corrOutOfPlace
		// ****************************************************************************
		float* corrOutOfPlace(float* f, float* g, int fastSize,
						int medSize, int slowSize, bool useMCF, bool center) {
			
			int fastIxPadding = (fastSize % 2 ? 1 : 2);
			int array_size = (fastSize+fastIxPadding)*medSize*slowSize;
			int N = fastSize*medSize*slowSize;

			//FFTW requires arrays to be declared with malloc, not new
			float* ret = (float*) malloc( array_size * sizeof(float) );
			float* G = (float*) malloc( array_size * sizeof(float) );
			
			fftOutOfPlace(f, ret, fastSize, medSize, slowSize);
			fftOutOfPlace(g, G, fastSize, medSize, slowSize);
			
			if (useMCF)
				mftInPlace(ret, G, array_size);
			else
				cftInPlace(ret, G, array_size);
			
			// FFTW will destroy the input array even for an out-of-place complex to real IFT
			// Thus, we might as well do an in-place IFT, which is faster
			iftInPlace(ret, fastSize, medSize, slowSize);
			free(G);
			G = NULL;
			
			//Normalize the data
			for (int i=0; i < array_size; i++) {
				ret[i] /= N;
			}
			
			if (center) {
				// Placeholder: puts the center of the correlation at the center
				// of the map instead of (0,0,0)
			}
			return ret;
		}

	}
}

#endif
