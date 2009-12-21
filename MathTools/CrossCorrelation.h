// Copyright (C) 2005-2009 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author: Ross Coleman (racolema@bcm.edu)

#ifndef MATHTOOLS_CROSS_CORRELATION_H
#define MATHTOOLS_CROSS_CORRELATION_H

#ifdef FFTW3

#include <complex>
#include <cstdlib>
#include "FFT.h"
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


		// ****************************************************************************
		// ccfInPlace
		// Arrays f and g must have the same dimensions. fastSize is the size of the
		// array for the fast index, slowSize for the slow index.
		// This function will perform an in-place Fourier transform on both of them.
		// At the end of this function, *f = FFT(f) and *g = IFT( conj(FFT(f))*FFT(g) )
		// Before calling this function, the fast index dimension of the arrays for
		// f and g should be padded by 1 pixel if odd-sized or 2 pixels if even sized.
		// Don't include the padding in the fastSizeMinusPadding parameter, though.
		// ****************************************************************************
		void ccfInPlace(float* f, float* g, int fastSizeMinusPadding, int medSize,
				int slowSize, bool center = false);


		// ****************************************************************************
		// ccfOutOfPlace
		// Arrays f and g must have the same dimensions.
		// Returns IFT( conj(FFT(f)) * FFT(g) )
		// This function expects f and g not to have any padding in the fast index 
		// dimension, because it is not needed for out-of-place FFT's.
		// ****************************************************************************
		float* ccfOutOfPlace(float* f, float* g, int fastSize,
				int medSize, int slowSize, bool center = false);









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
		// ccfInPlace
		// ****************************************************************************
		void ccfInPlace(float* f, float* g, int fastSizeMinusPadding,
								int medSize, int slowSize, bool center) {

			fftInPlace(f, fastSizeMinusPadding, medSize, slowSize);
			fftInPlace(g, fastSizeMinusPadding, medSize, slowSize);

			//Now that we are in Fourier space, all indices (including 1 or 2 pixel padding) are used
			int fastSize = (fastSizeMinusPadding % 2 ? fastSizeMinusPadding+1 : fastSizeMinusPadding+2);
			int fastComplexSize = fastSize/2;

			if (center) {
				// Placeholder: puts the center of the correlation at the center
				// of the map instead of (0,0,0)
			}
			else { // center of the correlation is at (0,0,0)
				for (int k = 0; k < slowSize; k++)
					for (int j = 0; j < medSize; j++)
						for (int i=0; i<fastComplexSize; i++) {
							getComplexDataAt(g, i,j,k, fastSize, medSize, slowSize)
							*= conj(getComplexDataAt(f, i,j,k, fastSize, medSize, slowSize));
						}
			}
			iftInPlace(g, fastSizeMinusPadding, medSize, slowSize);

			int N = fastSizeMinusPadding*medSize*slowSize;

			//Normalize the data
			for (int i=0; i < N; i++) {
				g[i] /= N;
			}
		}

		// ****************************************************************************
		// ccfOutOfPlace
		// ****************************************************************************
		float* ccfOutOfPlace(float* f, float* g, int fastSize,
						int medSize, int slowSize, bool center) {
			
			// ******************************************************
			// FIXME: I think that a SIGABORT error (gdb on Mac) is happening somewhere in this function
			// ******************************************************
			
			int fastIxPadding = (fastSize % 2 ? 1 : 2);
			int size = (fastSize+fastIxPadding)*medSize*slowSize;

			//FFTW requires arrays to be declared with malloc, not new
			float* F = (float*) malloc( size * sizeof(float) );
			float* ret = (float*) malloc( size * sizeof(float) );
			
			fftOutOfPlace(f, F, fastSize, medSize, slowSize);
			fftOutOfPlace(g, ret, fastSize, medSize, slowSize);
			
			for (int k=0; k<slowSize; k++)
				for (int j=0; j<medSize; j++)
					for (int i=0; i<fastSize+fastIxPadding; i++) {
						getComplexDataAt(ret, i,j,k, fastSize+fastIxPadding, medSize, slowSize) *= 
							conj( getComplexDataAt(F, i,j,k,fastSize+fastIxPadding, medSize, slowSize ));
					}
			
			// FFTW will destroy the input array even for an out-of-place complex to real IFT
			// Thus, we might as well do an in-place IFT, which is faster
			iftInPlace(ret, fastSize, medSize, slowSize);
			free(F);
			F = NULL;
			return ret;
		}

	}
}

#endif //FFTW3

#endif
