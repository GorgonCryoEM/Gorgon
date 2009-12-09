/**
 * $Id$
 */

/*
 * Author: Steven Ludtke, 04/10/2003 (sludtke@bcm.edu)
 * Copyright (c) 2000-2006 Baylor College of Medicine
 *
 * This software is issued under a joint BSD/GNU license. You may use the
 * source code in this file under either license. However, note that the
 * complete EMAN2 and SPARX software packages have some GPL dependencies,
 * so you are responsible for compliance with the licenses of these packages
 * if you opt to use BSD licensing. The warranty disclaimer below holds
 * in either instance.
 *
 * This complete copyright notice must be included in any revised version of the
 * source code. Additional authorship citations may be added, but existing
 * author citations must be preserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * */

#ifndef eman_emfft_h__
#define eman_emfft_h__

#include <string>
using std::string;
#include <sstream>
using std::stringstream;
//#include "log.h" //EMAN2 specific
#include <iostream>
using std::cout; //Used to replace log.h stuff
using std::endl;


namespace EMAN {

	/** E2Exception class is the parent class of all EMAN2 E2Exceptions.
	 */
    class E2Exception : public std::exception {
    public:
		/** Contructor.
		 * @param file The name of the file where an E2Exception is
		 * thrown.
		 * @param line The line number in the file where the
		 * E2Exception is thrown.
		 * @param desc_str E2Exception description string.
		 * @param objname_str E2Exception involved object name.
		 */
		explicit E2Exception(const string& file = "", int line = 0,
				  const string& desc_str = "", const string& objname_str = "")
			: filename(file), linenum(line), desc(desc_str), objname(objname_str)
		{
			// Log the message
			stringstream ss;
			string line_str;
			ss << line;
			ss >> line_str;
// We shouldn't log all exceptions, since they are frequently used intentionally, with no implied error
//			string message = "File " + file + " line " + line_str + " description " + desc_str + " " + objname_str;
//			LOGERR( message.c_str() );
		}

		virtual ~E2Exception() throw() {}

		/** The E2Exception information.
		 * @return The E2Exception information including E2Exception
		 * location (filename, line number,function name) and
		 * description.
		 */
		virtual const char *what() const throw();

		/** The name of this E2Exception class.
		 * @return The name of this E2Exception class.
		 */
		virtual const char *name() const { return "Exception"; }
	protected:
		string filename;
		int linenum;
		string desc;
		string objname;
    };

	string int2str(int n)
	{
		char s[32] = {'\0'};
		sprintf(s, "%d", n);
		return string(s);
	}
	
	const char* E2Exception::what() const throw()
	{
		string err1 = "";
		if (objname != "") {
			err1 = "error with '" + objname + "': ";
		}
		
		string msg = string(name()) + " at " + filename + ":" + int2str(linenum);
		msg += ": " + err1 + "'" + desc + "' caught\n";
		return msg.c_str();
	}

	/** Used when an invalid integer value is given.
	 * Parameters:
	 *  1. val  The invalid integer value.
	 *  2. desc Description of the situation.
	 */
	class _InvalidValueException : public E2Exception
	{
	public:
		template <class T>
		_InvalidValueException(T val, const string& file = "unknown",
							   int line = 0, const string& desc_str = "")
			: E2Exception(file, line, desc_str)
		{
			stringstream ss;
			ss << val;
			objname = ss.str();
		}
		const char *name() const { return "InvalidValueException"; }

	};

	#define InvalidValueException(val, desc) \
	 _InvalidValueException(val, __FILE__, __LINE__, desc)

	#ifdef CUDA_FFT
	#include "cuda/cuda_emfft.h"
	#endif //CUDA_FFT

	#ifdef DJBFFT
	extern "C" {
		#include <fftr4.h>
	}
	#endif	//DJBFFT



	namespace {
		int get_rank(int ny, int nz)
		{
			int rank = 3;
			if (ny == 1) {
				rank = 1;
			}
			else if (nz == 1) {
				rank = 2;
			}
			return rank;
		}
	}

}


#ifdef FFTW2

#include <srfftw.h>
namespace EMAN
{
	/** EMfft converts 1d/nd data from real to complex or from complex to real.
     */
	class EMfft
	{
	  public:
		static int real_to_complex_1d(float *real_data, float *complex_data, int n);
		static int complex_to_real_1d(float *complex_data, float *real_data, int n);

		static int real_to_complex_nd(float *real_data, float *complex_data, int nx, int ny,
									  int nz);
		static int complex_to_real_nd(float *complex_data, float *real_data, int nx, int ny,
									  int nz);
		private:
#ifdef  FFTW_PLAN_CACHING
#define EMFFTW2_ND_CACHE_SIZE 20
#define EMFFTW2_1D_CACHE_SIZE 20
		static const int EMAN2_REAL_2_COMPLEX;
		static const int EMAN2_COMPLEX_2_REAL;
		static const int EMAN2_FFTW2_INPLACE;
		static const int EMAN2_FFTW2_OUT_OF_PLACE;
		/** EMfftw2_cache_nd
		 * An ecapsulation of FFTW2 ND plan caching. Works on 2D and 3D arrays only.
		 * Keeps an array of plans and records of important details.
		 * Main interface is get_plan(...)
		 * If asked for a plan that is not currently stored this class will create the plan and then return
		 * it. If asked for a plan that IS stored than the pre-existing plan is returned.
		 * The number of plans that can be cached is determined by the compile time flag EMFFTW2_ND_CACHE_SIZE.
		 * Supports inplace transforms
		 * Using FFTW plan caching usually results in a dramatic performance boost.
		 */
		class EMfftw2_cache_nd
		{
		public:
			EMfftw2_cache_nd();
			~EMfftw2_cache_nd();

			/** Get an FFTW2 plan for 2D or 3D data. Can be complex to real or vice versa, can be in-place or out-of-place
			 * @param rank must be 2 or 3, corresponds to the dimensionality of the Fourier transform
			 * @param x the length of the x dimension of the Fourier transform
			 * @param y the length of the y dimension of the Fourier transform
			 * @param z the length of the z dimension of the Fourier transform, if rank is 2 this should be 1.
			 * @param r2c_flag the real to complex flag, should be either EMAN2_REAL_2_COMPLEX or EMAN2_COMPLEX_2_REAL,depending on the plan you want
			 * @param ip_flag the in-place flag, should be either EMAN2_FFTW2_INPLACE or EMAN2_FFTW2_OUT_OF_PLACE
			 * @exception InvalidValueException when the rank is not 2 or 3
			 * @exception InvalidValueException when the r2c_flag is unrecognized
			 * @return and rfftwnd_plan corresponding to the input arguments
			 */
			rfftwnd_plan get_plan(const int rank, const int x, const int y, const int z, const int r2c_flag, int ip_flag);
		private:
			// Prints useful debug information to standard out
			void debug_plans();

			// Store the number of plans currently contained in this EMfftw3_cache object
			int num_plans;

			// Store the rank of the plan
			int rank[EMFFTW2_ND_CACHE_SIZE];
			// Store the dimensions of the plan (always in 3D, if dimensions are "unused" they are taken to be 1)
			int plan_dims[EMFFTW2_ND_CACHE_SIZE][3];
			// store whether the plan was real to complex or vice versa
			int r2c[EMFFTW2_ND_CACHE_SIZE];
			// Store whether or not the plan was inplace
			int ip[EMFFTW2_ND_CACHE_SIZE];
			// Store the plans themselves
			rfftwnd_plan rfftwnd_plans[EMFFTW2_ND_CACHE_SIZE];
		};

		/** EMfftw2_cache_1d
		 * An ecapsulation of FFTW2 1D plan caching. Keeps an array of plans and records of important details.
		 * Main interface is get_plan(...)
		 * If asked for a plan that is not currently stored this class will create the plan and then return
		 * it. If asked for a plan that IS stored than the pre-existing plan is returned.
		 * The number of plans that can be cached is determined by the compile time flag EMFFTW2_1D_CACHE_SIZE.
		 * Supports inplace transforms
		 * Using FFTW plan caching usually results in a dramatic performance boost.
		 */
		class EMfftw2_cache_1d
		{
		public:
			EMfftw2_cache_1d();
			~EMfftw2_cache_1d();

			/** Get an FFTW2 plan for 1D data. Can be complex to real or vice versa, can be in-place or out-of-place
			 * @param x the length of the x dimension of the Fourier transform
			 * @param r2c_flag the real to complex flag, should be either EMAN2_REAL_2_COMPLEX or EMAN2_COMPLEX_2_REAL,depending on the plan you want
			 * @exception InvalidValueException when the r2c_flag is unrecognized
			 * @return and rfftw_plan corresponding to the input arguments
			 */
			rfftw_plan get_plan(const int x, const int r2c_flag);
		private:
			// Prints useful debug information to standard out
			void debug_plans();

			// Store the number of plans currently contained in this EMfftw3_cache object
			int num_plans;

			// Store the dimensions of the plan (always in 3D, if dimensions are "unused" they are taken to be 1)
			int plan_dims[EMFFTW2_1D_CACHE_SIZE];
			// store whether the plan was real to complex or vice versa
			int r2c[EMFFTW2_1D_CACHE_SIZE];
			// Store the plans themselves
			rfftw_plan rfftw1d_plans[EMFFTW2_1D_CACHE_SIZE];
		};

		static EMfftw2_cache_nd plan_nd_cache;
		static EMfftw2_cache_1d plan_1d_cache;
#endif
	};
}
#endif	//FFTW2

#ifdef FFTW3

#include <fftw3.h>

namespace EMAN
{
	/** EMfft converts 1d/nd data from real to complex or from complex to real.
     */
	class EMfft
	{
	  public:
		static int real_to_complex_1d(float *real_data, float *complex_data, int n);
		static int complex_to_real_1d(float *complex_data, float *real_data, int n);

		static int real_to_complex_nd(float *real_data, float *complex_data, int nx, int ny,
									  int nz);
		static int complex_to_real_nd(float *complex_data, float *real_data, int nx, int ny,
									  int nz);
	  private:
#ifdef FFTW_PLAN_CACHING
#define EMFFTW3_CACHE_SIZE 20
		static const int EMAN2_REAL_2_COMPLEX;
		static const int EMAN2_COMPLEX_2_REAL;
		static const int EMAN2_FFTW2_INPLACE;
		static const int EMAN2_FFTW2_OUT_OF_PLACE;
		/** EMfftw3_cache
		 * An ecapsulation of FFTW3 plan caching. Keeps an array of plans and records of important details.
		 * Main interface is get_plan(...)
		 * If asked for a plan that is not currently stored this class will create the plan and then return
		 * it. If asked for a plan that IS stored than the pre-existing plan is returned.
		 * The number of plans that can be cached is determined by the compile time flag EMFFTW3_CACHE_SIZE.
		 * Although FFTW3 documentation states that plan caching is performed internally, tests on Fedora Core
		 * 6 using rpms indicated that the costs of an associated MD5 algorithm in FFTW3 were prohibitive.
		 * Hence this implementation. Using FFTW plan caching usually results in a dramatic performance boost.
		 * Supports inplace transforms
		 */
		class EMfftw3_cache
		{
		public:
			EMfftw3_cache();
			~EMfftw3_cache();

			/** Get an FFTW3 plan for 1D, 2D or 3D data. Can be complex to real or vice versa, can be in-place or out-of-place
			 * @param rank must be 1, 2 or 3, corresponds to the dimensionality of the Fourier transform
			 * @param x the length of the x dimension of the Fourier transform
			 * @param y the length of the y dimension of the Fourier transform, if rank is 1 this should be 1.
			 * @param z the length of the z dimension of the Fourier transform, if rank is 1 or 2 this should be 1.
			 * @param r2c_flag the real to complex flag, should be either EMAN2_REAL_2_COMPLEX or EMAN2_COMPLEX_2_REAL,depending on the plan you want
			 * @param ip_flag the in-place flag, should be either EMAN2_FFTW2_INPLACE or EMAN2_FFTW2_OUT_OF_PLACE
			 * @param complex_data the complex data, in fftw_complex format
			 * @param real_data the real_data
			 * @exception InvalidValueException when the rank is not 1,2 or 3
			 * @exception InvalidValueException when the r2c_flag is unrecognized
			 * @return and fftwf_plan corresponding to the input arguments
			 */
			fftwf_plan get_plan(const int rank, const int x, const int y, const int z, const int r2c_flag,const int ip_flag, fftwf_complex* complex_data, float* real_data);
		private:
			// Prints useful debug information to standard out
			void debug_plans();

			// Store the number of plans currently contained in this EMfftw3_cache object
			int num_plans;

			// Store the rank of the plan
			int rank[EMFFTW3_CACHE_SIZE];
			// Store the dimensions of the plan (always in 3D, if dimensions are "unused" they are taken to be 1)
			int plan_dims[EMFFTW3_CACHE_SIZE][3];
			// store whether the plan was real to complex or vice versa
			int r2c[EMFFTW3_CACHE_SIZE];
			// Store the plans themselves
			fftwf_plan fftwplans[EMFFTW3_CACHE_SIZE];
			// Store whether or not the plan was inplace
			int ip[EMFFTW3_CACHE_SIZE];

		};

		static EMfftw3_cache plan_cache;
#endif
	};
}
#endif	//FFTW3

#ifdef CUDA_FFT
class EMfft
	{
	public:
		static int real_to_complex_1d(float *real_data, float *complex_data, int n);
		static int complex_to_real_1d(float *complex_data, float *real_data, int n);

		static int real_to_complex_nd(float *real_data, float *complex_data, int nx, int ny,
									  int nz);
		static int complex_to_real_nd(float *complex_data, float *real_data, int nx, int ny,
									  int nz);
	private:
		static const int EMAN2_REAL_2_COMPLEX;
		static const int EMAN2_COMPLEX_2_REAL;
		static const int EMAN2_FFTW2_INPLACE;
		static const int EMAN2_FFTW2_OUT_OF_PLACE;
	};
#endif //CUDA_FFT


#ifdef NATIVE_FFT
namespace EMAN
{
	/** EMfft converts 1d/nd data from real to complex or from complex to real.
     */
	class EMfft
	{
	  public:
		static int real_to_complex_1d(float *real_data, float *complex_data, int n);
		static int complex_to_real_1d(float *complex_data, float *real_data, int n);

		static int real_to_complex_nd(float *real_data, float *complex_data, int nx, int ny, int nz);
		static int complex_to_real_nd(float *complex_data, float *real_data, int nx, int ny, int nz);
	};
}
#endif	//NATIVE_FFT

#ifdef	ACML
#include <acml.h>
#include <functional>

namespace EMAN
{
	/** EMfft converts 1d/nd data from real to complex or from complex to real.
     */
	class EMfft
	{
	  public:
		static int real_to_complex_1d(float *real_data, float *complex_data, int n);
		static int complex_to_real_1d(float *complex_data, float *real_data, int n);

		static int real_to_complex_nd(float *real_data, float *complex_data, int nx, int ny, int nz);
		static int complex_to_real_nd(float *complex_data, float *real_data, int nx, int ny, int nz);

	  private:
		static int real_to_complex_2d(float *real_data, float *complex_data, int nx, int ny);
		static int complex_to_real_2d(float *complex_data, float *real_data, int nx, int ny);
		static int real_to_complex_3d(float *real_data, float *complex_data, int nx, int ny, int nz);
		static int complex_to_real_3d(float *complex_data, float *real_data, int nx, int ny, int nz);

		class time_sqrt_n : public std::unary_function<float, float> {
		  public:
			time_sqrt_n(int n) : n_(n), factor(sqrt(float(n_))) {}
			float operator()(float x) const {return x*factor;}
		  private:
		    const int n_;
		    const float factor;
		};

		class divide_sqrt_n : public std::unary_function<float, float> {
		  public:
			divide_sqrt_n(int n) : n_(n), factor(sqrt(float(n_))) {}
			float operator()(float x) const {return x/factor;}
		  private:
			const int n_;
			const float factor;
		};
	};
}
#endif	//ACML












// *************************************************************************************************
// Code below can go in cpp file
// *************************************************************************************************

using namespace EMAN;

#ifdef FFTW_PLAN_CACHING
// The only thing important about these constants is that they don't equal each other
const int EMfft::EMAN2_REAL_2_COMPLEX = 1;
const int EMfft::EMAN2_COMPLEX_2_REAL = 2;

// It is important that these constants are 1 and 0, exactly as they are here
const int EMfft::EMAN2_FFTW2_INPLACE = 1;
const int EMfft::EMAN2_FFTW2_OUT_OF_PLACE=0;


#ifdef FFTW3
EMfft::EMfftw3_cache::EMfftw3_cache() :
		num_plans(0)
{
	for(int i = 0; i < EMFFTW3_CACHE_SIZE; ++i)
	{
		rank[i] = 0;
		plan_dims[i][0] = 0; plan_dims[i][1] = 0; plan_dims[i][2] = 0;
		r2c[i] = -1;
		ip[i] = -1;
		fftwplans[i] = NULL;
	}
}

void EMfft::EMfftw3_cache::debug_plans()
{
	for(int i = 0; i < EMFFTW3_CACHE_SIZE; ++i)
	{
		cout << "Plan " << i << " has dims " << plan_dims[i][0] << " "
				<< plan_dims[i][1] << " " <<
				plan_dims[i][2] << ", rank " <<
				rank[i] << ", rc flag "
				<< r2c[i] << ", ip flag " << ip[i] << endl;
	}
}

EMfft::EMfftw3_cache::~EMfftw3_cache()
{
	for(int i = 0; i < EMFFTW3_CACHE_SIZE; ++i)
	{
		if (fftwplans[i] != NULL)
		{
			fftwf_destroy_plan(fftwplans[i]);
			fftwplans[i] = NULL;
		}
	}
}

fftwf_plan EMfft::EMfftw3_cache::get_plan(const int rank_in, const int x, const int y, const int z, const int r2c_flag, const int ip_flag, fftwf_complex* complex_data, float* real_data )
{

	if ( rank_in > 3 || rank_in < 1 ) throw InvalidValueException(rank_in, "Error, can not get an FFTW plan using rank out of the range [1,3]");
	if ( r2c_flag != EMAN2_REAL_2_COMPLEX && r2c_flag != EMAN2_COMPLEX_2_REAL ) throw InvalidValueException(r2c_flag, "The real two complex flag is not supported");

// 	static int num_added = 0;
// 	cout << "Was asked for " << rank_in << " " << x << " " << y << " " << z << " " << r2c_flag << endl;

	int dims[3];
	dims[0] = z;
	dims[1] = y;
	dims[2] = x;

	// First check to see if we already have the plan
	int i;
	for (i=0; i<num_plans; i++) {
		if (plan_dims[i][0]==x && plan_dims[i][1]==y && plan_dims[i][2]==z
				  && rank[i]==rank_in && r2c[i]==r2c_flag && ip[i]==ip_flag) return fftwplans[i];
	}

	fftwf_plan plan;
	// Create the plan
	if ( y == 1 && z == 1 )
	{
		if ( r2c_flag == EMAN2_REAL_2_COMPLEX )
			plan = fftwf_plan_dft_r2c_1d(x, real_data, complex_data, FFTW_ESTIMATE);
		else // r2c_flag == EMAN2_COMPLEX_2_REAL, this is guaranteed by the error checking at the beginning of the function
			plan = fftwf_plan_dft_c2r_1d(x, complex_data, real_data, FFTW_ESTIMATE);
	}
	else
	{
		if ( r2c_flag == EMAN2_REAL_2_COMPLEX )
			plan = fftwf_plan_dft_r2c(rank_in, dims + (3 - rank_in), real_data, complex_data, FFTW_ESTIMATE);
		else // r2c_flag == EMAN2_COMPLEX_2_REAL, this is guaranteed by the error checking at the beginning of the function
			plan = fftwf_plan_dft_c2r(rank_in, dims + (3 - rank_in), complex_data, real_data, FFTW_ESTIMATE);
	}

	if (fftwplans[EMFFTW3_CACHE_SIZE-1] != NULL )
	{
		fftwf_destroy_plan(fftwplans[EMFFTW3_CACHE_SIZE-1]);
		fftwplans[EMFFTW3_CACHE_SIZE-1] = NULL;
	}

	int upper_limit = num_plans;
	if ( upper_limit == EMFFTW3_CACHE_SIZE ) upper_limit -= 1;
	for (int i=upper_limit-1; i>0; i--)
	{
		fftwplans[i]=fftwplans[i-1];
		rank[i]=rank[i-1];
		r2c[i]=r2c[i-1];
		ip[i]=ip[i-1];
		plan_dims[i][0]=plan_dims[i-1][0];
		plan_dims[i][1]=plan_dims[i-1][1];
		plan_dims[i][2]=plan_dims[i-1][2];
	}
		//dimplan[0]=-1;

	plan_dims[0][0]=x;
	plan_dims[0][1]=y;
	plan_dims[0][2]=z;
	r2c[0]=r2c_flag;
	ip[0]=ip_flag;
	fftwplans[0] = plan;
	rank[0]=rank_in;
	if (num_plans<EMFFTW3_CACHE_SIZE) num_plans++;
// 			debug_plans();
// 			cout << "Created plan 0" << endl;
// 	++num_added;
// 	cout << "I have created " << num_added << " plans" << endl;
	return fftwplans[0];

}

// Static init
EMfft::EMfftw3_cache EMfft::plan_cache;

#endif // FFTW3

#ifdef FFTW2


EMfft::EMfftw2_cache_nd::EMfftw2_cache_nd() :
		num_plans(0)
{
	for(int i = 0; i < EMFFTW2_ND_CACHE_SIZE; ++i)
	{
		rank[i] = 0;
		plan_dims[i][0] = 0; plan_dims[i][1] = 0; plan_dims[i][2] = 0;
		r2c[i] = -1;
		ip[i] = -1;
		rfftwnd_plans[i] = NULL;
	}
}

void EMfft::EMfftw2_cache_nd::debug_plans()
{
	for(int i = 0; i < EMFFTW2_ND_CACHE_SIZE; ++i)
	{
		cout << "Plan " << i << " has dims " << plan_dims[i][0] << " "  << plan_dims[i][1] << " " <<  plan_dims[i][2] << ", rank " <<
				rank[i] << ", rc flag " << r2c[i] << ", ip flag " << ip[i] << endl;
	}
}

EMfft::EMfftw2_cache_nd::~EMfftw2_cache_nd()
{
	for(int i = 0; i < EMFFTW2_ND_CACHE_SIZE; ++i)
	{
		if (rfftwnd_plans[i] != NULL)
		{
			rfftwnd_destroy_plan(rfftwnd_plans[i]);
			rfftwnd_plans[i] = NULL;
		}
	}
}

rfftwnd_plan EMfft::EMfftw2_cache_nd::get_plan(const int rank_in, const int x, const int y, const int z, const int r2c_flag, const int ip_flag)
{

	if ( rank_in > 3 || rank_in < 1 ) throw InvalidValueException(rank_in, "Error, can not get an FFTW2 plan using rank out of the range [2,3]");
	if ( r2c_flag != EMAN2_REAL_2_COMPLEX && r2c_flag != EMAN2_COMPLEX_2_REAL ) throw InvalidValueException(r2c_flag, "The real two complex flag is not supported");

	int dims[3];
	dims[0] = z;
	dims[1] = y;
	dims[2] = x;

	// First check to see if we already have the plan
	int i;
	for (i=0; i<num_plans; i++) {
		if (plan_dims[i][0]==x && plan_dims[i][1]==y && plan_dims[i][2]==z
				  && rank[i]==rank_in && r2c[i]==r2c_flag && ip[i] == ip_flag) break;
	}

	// If we found the plan above, the i will not equal num_plans
	if (i!=num_plans) {
		return rfftwnd_plans[i];
	}
	else
	{
		rfftwnd_plan plan;
		// Create the plan
		if ( r2c_flag == EMAN2_REAL_2_COMPLEX )
		{
			if (ip_flag == EMAN2_FFTW2_INPLACE)
				plan = rfftwnd_create_plan(rank_in, dims + (3 - rank_in),FFTW_REAL_TO_COMPLEX,FFTW_ESTIMATE|FFTW_IN_PLACE);
			else
				plan = rfftwnd_create_plan(rank_in, dims + (3 - rank_in),FFTW_REAL_TO_COMPLEX, FFTW_ESTIMATE);
		}
		else // r2c_flag == EMAN2_COMPLEX_2_REAL, this is guaranteed by the error checking at the beginning of the function
		{
			if (ip_flag == EMAN2_FFTW2_INPLACE)
				plan = rfftwnd_create_plan(rank_in, dims + (3 - rank_in),FFTW_COMPLEX_TO_REAL,FFTW_ESTIMATE|FFTW_IN_PLACE);
			else
				plan = rfftwnd_create_plan(rank_in, dims + (3 - rank_in),FFTW_COMPLEX_TO_REAL,FFTW_ESTIMATE);
		}


		if (rfftwnd_plans[EMFFTW2_ND_CACHE_SIZE-1] != NULL )
		{
			rfftwnd_destroy_plan(rfftwnd_plans[EMFFTW2_ND_CACHE_SIZE-1]);
			rfftwnd_plans[EMFFTW2_ND_CACHE_SIZE-1] = NULL;
		}

		int upper_limit = num_plans;
		if ( upper_limit == EMFFTW2_ND_CACHE_SIZE ) upper_limit -= 1;
		for (int i=upper_limit-1; i>0; i--)
		{
			rfftwnd_plans[i]=rfftwnd_plans[i-1];
			rank[i]=rank[i-1];
			r2c[i]=r2c[i-1];
			ip[i]=ip[i-1];
			plan_dims[i][0]=plan_dims[i-1][0];
			plan_dims[i][1]=plan_dims[i-1][1];
			plan_dims[i][2]=plan_dims[i-1][2];
		}
		//dimplan[0]=-1;

		plan_dims[0][0]=x;
		plan_dims[0][1]=y;
		plan_dims[0][2]=z;
		r2c[0]=r2c_flag;
		rfftwnd_plans[0] = plan;
		rank[0]=rank_in;
		ip[0]=ip_flag;
		if (num_plans<EMFFTW2_ND_CACHE_SIZE) num_plans++;

		return rfftwnd_plans[0];
	}
}

EMfft::EMfftw2_cache_1d::EMfftw2_cache_1d() :
		num_plans(0)
{
	for(int i = 0; i < EMFFTW2_1D_CACHE_SIZE; ++i)
	{
		plan_dims[i] = 0;
		r2c[i] = -1;
		rfftw1d_plans[i] = NULL;
	}
}

void EMfft::EMfftw2_cache_1d::debug_plans()
{
	for(int i = 0; i < EMFFTW2_1D_CACHE_SIZE; ++i)
	{
		cout << "Plan " << i << " has length " << plan_dims[i] << ", rc flag " << r2c[i] << endl;
	}
}

EMfft::EMfftw2_cache_1d::~EMfftw2_cache_1d()
{
	for(int i = 0; i < EMFFTW2_1D_CACHE_SIZE; ++i)
	{
		if (rfftw1d_plans[i] != NULL)
		{
			rfftw_destroy_plan(rfftw1d_plans[i]);
			rfftw1d_plans[i] = NULL;
		}
	}
}

rfftw_plan EMfft::EMfftw2_cache_1d::get_plan(const int x, const int r2c_flag )
{

	if ( r2c_flag != EMAN2_REAL_2_COMPLEX && r2c_flag != EMAN2_COMPLEX_2_REAL ) throw InvalidValueException(r2c_flag, "The real two complex flag is not supported");

	// First check to see if we already have the plan
	int i;
	for (i=0; i<num_plans; i++) {
		if (plan_dims[i]==x && r2c[i]==r2c_flag ) return rfftw1d_plans[i];
	}

	rfftw_plan plan;
	// Create the plan
	if ( r2c_flag == EMAN2_REAL_2_COMPLEX )
			plan = rfftw_create_plan(x, FFTW_REAL_TO_COMPLEX, FFTW_ESTIMATE);
	else // r2c_flag == EMAN2_COMPLEX_2_REAL, this is guaranteed by the error checking at the beginning of the function
			plan = rfftw_create_plan(x, FFTW_COMPLEX_TO_REAL, FFTW_ESTIMATE);



	if (rfftw1d_plans[EMFFTW2_1D_CACHE_SIZE-1] != NULL )
	{
		rfftw_destroy_plan(rfftw1d_plans[EMFFTW2_1D_CACHE_SIZE-1]);
		rfftw1d_plans[EMFFTW2_1D_CACHE_SIZE-1] = NULL;
	}

	int upper_limit = num_plans;
	if ( upper_limit == EMFFTW2_1D_CACHE_SIZE ) upper_limit -= 1;
	for (int i=upper_limit-1; i>0; i--)
	{
		rfftw1d_plans[i]=rfftw1d_plans[i-1];
		plan_dims[i]=plan_dims[i-1];
		r2c[i] = r2c[i-1];
	}
	//dimplan[0]=-1;

	plan_dims[0]=x;
	r2c[0]=r2c_flag;
	rfftw1d_plans[0] = plan;
	if (num_plans<EMFFTW2_1D_CACHE_SIZE) num_plans++;

	return rfftw1d_plans[0];

}

// Static init
EMfft::EMfftw2_cache_nd EMfft::plan_nd_cache;
EMfft::EMfftw2_cache_1d EMfft::plan_1d_cache;

#endif // FFTW2
#endif // FFTW_PLAN_CACHING

#ifdef FFTW2

int EMfft::real_to_complex_1d(float *real_data, float *complex_data, int n)
{
#ifdef DJBFFT
	switch(n)
	{
		if ( n==2 || n==4 || n==8 || n==16 || n==32 || n==64 || n==128
			|| n==256 || n==512 || n==1024 || n==2048 || n==4096 || n==8192 )
		{
			memcpy( complex_data, real_data, n * sizeof(float) );
		}

		case 2:
			fftr4_2( (real4 *)complex_data );

		case 4:
			fftr4_4( (real4 *)complex_data );

		case 8:
			fftr4_8( (real4 *)complex_data );

		case 16:
			fftr4_16( (real4 *)complex_data );

		case 32:
			fftr4_32( (real4 *)complex_data );

		case 64:
			fftr4_64( (real4 *)complex_data );

		case 128:
			fftr4_128( (real4 *)complex_data );

		case 256:
			fftr4_256( (real4 *)complex_data );

		case 512:
			fftr4_512( (real4 *)complex_data );

		case 1024:
			fftr4_1024( (real4 *)complex_data );

		case 2048:
			fftr4_2048( (real4 *)complex_data );

		case 4096:
			fftr4_4096( (real4 *)complex_data );

		case 8192:
			fftr4_8192( (real4 *)complex_data );

		default:
			const int complex_n = n + 2 - n%2;
			float * fft_data = new float[n];
			rfftw_plan p = rfftw_create_plan(n, FFTW_REAL_TO_COMPLEX, FFTW_ESTIMATE);
			rfftw_one(p, (fftw_real *) real_data, (fftw_real *) complex_data);
			rfftw_destroy_plan(p);
			for(int i=0; i<complex_n; ++i) {
				if(i%2==0) {	//copy real part of complex array
					complex_data[i] = fft_data[i/2];
				}
				else {	//copy imaginary part of complex array
					if(i==1) {
						complex_data[i] = 0.0f;
					}
					else {
						if(n%2 == 0 && i == complex_n-1 ) {
							complex_data[i] = 0.0f;
						}
						else {
							complex_data[i] = fft_data[n-i/2];
						}
					}
				}
			}

			delete [] fft_data;
	}
#else
	const int complex_n = n + 2 - n%2;
	float * fft_data = new float[n];
#ifdef FFTW_PLAN_CACHING
	rfftw_plan plan_1d = plan_1d_cache.get_plan(n,EMAN2_REAL_2_COMPLEX);
	rfftw_one(plan_1d, (fftw_real *) real_data, (fftw_real *) fft_data);
#else
	rfftw_plan	p = rfftw_create_plan(n, FFTW_REAL_TO_COMPLEX, FFTW_ESTIMATE);
	rfftw_one(p, (fftw_real *) real_data, (fftw_real *) fft_data);
	rfftw_destroy_plan(p);
#endif //FFTW_PLAN_CACHING

	for(int i=0; i<complex_n; ++i) {
		if(i%2==0) {	//copy real part of complex array
			complex_data[i] = fft_data[i/2];
		}
		else {	//copy imaginary part of complex array
			if(i==1) {
				complex_data[i] = 0.0f;
			}
			else {
				if(n%2 == 0 && i == complex_n-1 ) {
					complex_data[i] = 0.0f;
				}
				else {
					complex_data[i] = fft_data[n-i/2];
				}
			}
		}
	}

	delete [] fft_data;
#endif	//DJBFFT

	return 0;
}

int EMfft::complex_to_real_1d(float *complex_data, float *real_data, int n)
{
#ifdef DJBFFT
	switch(n)
	{
		if ( n==2 || n==4 || n==8 || n==16 || n==32 || n==64 || n==128
			|| n==256 || n==512 || n==1024 || n==2048 || n==4096 || n==8192 )
		{
			memcpy( real_data, complex_data, n * sizeof(float) );
		}

		case 2:
			fftr4_scale2( (real4 *)real_data );
     		fftr4_un2( (real4 *)real_data );

		case 4:
			fftr4_scale4( (real4 *)real_data );
     		fftr4_un4( (real4 *)real_data );

		case 8:
			fftr4_scale8( (real4 *)real_data );
     		fftr4_un8( (real4 *)real_data );

		case 16:
			fftr4_scale16( (real4 *)real_data );
     		fftr4_un16( (real4 *)real_data );

		case 32:
			fftr4_scale32( (real4 *)real_data );
     		fftr4_un32( (real4 *)real_data );

		case 64:
			fftr4_scale64( (real4 *)real_data );
     		fftr4_un64( (real4 *)real_data );

		case 128:
			fftr4_scale128( (real4 *)real_data );
     		fftr4_un128( (real4 *)real_data );

		case 256:
			fftr4_scale256( (real4 *)real_data );
     		fftr4_un256( (real4 *)real_data );

		case 512:
			fftr4_scale512( (real4 *)real_data );
     		fftr4_un512( (real4 *)real_data );

		case 1024:
			fftr4_scale1024( (real4 *)real_data );
     		fftr4_un1024( (real4 *)real_data );

		case 2048:
			fftr4_scale2048( (real4 *)real_data );
     		fftr4_un2048( (real4 *)real_data );

		case 4096:
			fftr4_scale4096( (real4 *)real_data );
     		fftr4_un4096( (real4 *)real_data );

		case 8192:
			fftr4_scale8192( (real4 *)real_data );
     		fftr4_un8192( (real4 *)real_data );

		default:
			const int complex_n = n + 2 - n%2;
			float * fft_data = new float[n];

			for(int i=0; i<complex_n; ++i) {
				if(i%2 == 0) {	//copy real part of complex array
					fft_data[i/2] = complex_data[i];
				}
				else {	//copy imaginary part of complex array
					if(i==1) {continue;}
					if(!(n%2 == 0 && i == complex_n-1)) {
						fft_data[n-i/2] = complex_data[i];
					}
				}
			}

			rfftw_plan p = rfftw_create_plan(n, FFTW_COMPLEX_TO_REAL, FFTW_ESTIMATE);
			rfftw_one(p, (fftw_real *) fft_data, (fftw_real *) real_data);
			rfftw_destroy_plan(p);
	}
#else
	const int complex_n = n + 2 - n%2;
	float * fft_data = new float[n];

	for(int i=0; i<complex_n; ++i) {
		if(i%2 == 0) {	//copy real part of complex array
			fft_data[i/2] = complex_data[i];
		}
		else {	//copy imaginary part of complex array
			if(i==1) {continue;}
			if(!(n%2 == 0 && i == complex_n-1)) {
				fft_data[n-i/2] = complex_data[i];
			}
		}
	}
#ifdef FFTW_PLAN_CACHING
	rfftw_plan plan_1d = plan_1d_cache.get_plan(n,EMAN2_COMPLEX_2_REAL);
	rfftw_one(plan_1d, (fftw_real *) fft_data, (fftw_real *) real_data);
#else
	rfftw_plan p = rfftw_create_plan(n, FFTW_COMPLEX_TO_REAL, FFTW_ESTIMATE);
	rfftw_one(p, (fftw_real *) fft_data, (fftw_real *) real_data);
	rfftw_destroy_plan(p);
#endif // FFTW_PLAN_CACHING
	delete [] fft_data;
#endif	//DJBFFT

	return 0;

}

int EMfft::real_to_complex_nd(float *real_data, float *complex_data, int nx, int ny, int nz)
{
//	const int complex_nx = nx + 2 - nx%2;
	const int rank = get_rank(ny, nz);
	int dims[3];
	dims[0] = nz;
	dims[1] = ny;
	dims[2] = nx;

	switch(rank) {
		case 1:
			real_to_complex_1d(real_data, complex_data, nx);
			break;

		case 2:
		case 3:
		{
#ifdef FFTW_PLAN_CACHING
			bool ip = ( complex_data == real_data );
			rfftwnd_plan plan_nd = plan_nd_cache.get_plan(rank,nx,ny,nz,EMAN2_REAL_2_COMPLEX,ip);
			rfftwnd_one_real_to_complex(plan_nd, (fftw_real *) real_data, (fftw_complex *) complex_data);
#else
			rfftwnd_plan plan_nd;
			if(real_data == complex_data) {
				plan_nd = rfftwnd_create_plan(rank, dims + (3 - rank),
						FFTW_REAL_TO_COMPLEX, FFTW_ESTIMATE | FFTW_IN_PLACE);
			}
			else {
				plan_nd = rfftwnd_create_plan(rank, dims + (3 - rank),
						FFTW_REAL_TO_COMPLEX, FFTW_ESTIMATE);
			}
			rfftwnd_one_real_to_complex(plan_nd, (fftw_real *) real_data,
										(fftw_complex *) complex_data);
			rfftwnd_destroy_plan(plan_nd);
#endif // FFTW_PLAN_CACHING
		}
			break;

		default:
			LOGERR("Should NEVER be here!!!");
			break;
	}

	return 0;
}

int EMfft::complex_to_real_nd(float *complex_data, float *real_data, int nx, int ny, int nz)
{
//	const int complex_nx = nx + 2 - nx%2;
	const int rank = get_rank(ny, nz);
	int dims[3];
	dims[0] = nz;
	dims[1] = ny;
	dims[2] = nx;

	switch(rank) {
		case 1:
			complex_to_real_1d(complex_data, real_data, nx);
			break;

		case 2:
		case 3:
		{
#ifdef FFTW_PLAN_CACHING
			bool ip = ( complex_data == real_data );
			rfftwnd_plan plan_nd = plan_nd_cache.get_plan(rank,nx,ny,nz,EMAN2_COMPLEX_2_REAL,ip);
			rfftwnd_one_complex_to_real(plan_nd, (fftw_complex *) complex_data,	(fftw_real *) real_data);
#else
			rfftwnd_plan plan_nd;
			if(real_data == complex_data) {
				plan_nd = rfftwnd_create_plan(rank, dims + (3 - rank),
						FFTW_COMPLEX_TO_REAL, FFTW_ESTIMATE | FFTW_IN_PLACE);
			}
			else {
				plan_nd = rfftwnd_create_plan(rank, dims + (3 - rank),
						FFTW_COMPLEX_TO_REAL, FFTW_ESTIMATE);
			}
			rfftwnd_one_complex_to_real(plan_nd, (fftw_complex *) complex_data,
				(fftw_real *) real_data);
			rfftwnd_destroy_plan(plan_nd);
#endif // FFTW_PLAN_CACHING
		}
			break;

		default:
			LOGERR("Should NEVER be here!!!");
			break;
	}

	return 0;
}
#endif	//FFTW2

#ifdef CUDA_FFT
int EMfft::real_to_complex_1d(float *real_data, float *complex_data, int n)
{
	return  cuda_fft_real_to_complex_1d(real_data,complex_data,n);
}

int EMfft::complex_to_real_1d(float *complex_data, float *real_data, int n)
{
	return cuda_fft_complex_to_real_1d(complex_data,real_data,n);
}

int EMfft::real_to_complex_nd(float *real_data, float *complex_data, int nx, int ny, int nz)
{
	return cuda_fft_real_to_complex_nd(real_data,complex_data,nx,ny,nz);
}

int EMfft::complex_to_real_nd(float *complex_data, float *real_data, int nx, int ny, int nz)
{
	return cuda_fft_complex_to_real_nd(complex_data,real_data,nx,ny,nz);
}

#endif

#ifdef FFTW3

int EMfft::real_to_complex_1d(float *real_data, float *complex_data, int n)
{//cout<<"doing fftw3"<<endl;
#ifdef FFTW_PLAN_CACHING
	bool ip = ( complex_data == real_data );
	fftwf_plan plan = plan_cache.get_plan(1,n,1,1,EMAN2_REAL_2_COMPLEX,ip,(fftwf_complex *) complex_data, real_data);
	// According to FFTW3, this is making use of the "guru" interface - this is necessary if plans are to be reused
	fftwf_execute_dft_r2c(plan, real_data,(fftwf_complex *) complex_data);
#else
	fftwf_plan plan = fftwf_plan_dft_r2c_1d(n, real_data, (fftwf_complex *) complex_data,
											FFTW_ESTIMATE);
	fftwf_execute(plan);
	fftwf_destroy_plan(plan);
#endif // FFTW_PLAN_CACHING
	return 0;
}

int EMfft::complex_to_real_1d(float *complex_data, float *real_data, int n)
{
#ifdef FFTW_PLAN_CACHING
	bool ip = ( complex_data == real_data );
	fftwf_plan plan = plan_cache.get_plan(1,n,1,1,EMAN2_COMPLEX_2_REAL,ip,(fftwf_complex *) complex_data, real_data);
	// According to FFTW3, this is making use of the "guru" interface - this is necessary if plans are to be reused
	fftwf_execute_dft_c2r(plan, (fftwf_complex *) complex_data, real_data);
#else
	fftwf_plan plan = fftwf_plan_dft_c2r_1d(n, (fftwf_complex *) complex_data, real_data,
											FFTW_ESTIMATE);
	fftwf_execute(plan);
	fftwf_destroy_plan(plan);
#endif // FFTW_PLAN_CACHING

	return 0;
}

int EMfft::real_to_complex_nd(float *real_data, float *complex_data, int nx, int ny, int nz)
{
	const int rank = get_rank(ny, nz);
	int dims[3];
	dims[0] = nz;
	dims[1] = ny;
	dims[2] = nx;

	switch(rank) {
		case 1:
			real_to_complex_1d(real_data, complex_data, nx);
			break;

		case 2:
		case 3:
		{
#ifdef FFTW_PLAN_CACHING
			bool ip = ( complex_data == real_data );
			fftwf_plan plan = plan_cache.get_plan(rank,nx,ny,nz,EMAN2_REAL_2_COMPLEX,ip,(fftwf_complex *) complex_data, real_data);
			// According to FFTW3, this is making use of the "guru" interface - this is necessary if plans are to be re-used
			fftwf_execute_dft_r2c(plan, real_data,(fftwf_complex *) complex_data );
#else
			fftwf_plan plan = fftwf_plan_dft_r2c(rank, dims + (3 - rank),
					real_data, (fftwf_complex *) complex_data, FFTW_ESTIMATE);
			fftwf_execute(plan);
			fftwf_destroy_plan(plan);
#endif // FFTW_PLAN_CACHING
		}
			break;

		default:
			//LOGERR("Should NEVER be here!!!");
			cout << "The rank of the array is not 1D, 2D, or 3D!\n";
			break;
	}

	return 0;
}

int EMfft::complex_to_real_nd(float *complex_data, float *real_data, int nx, int ny, int nz)
{
	const int rank = get_rank(ny, nz);
	int dims[3];
	dims[0] = nz;
	dims[1] = ny;
	dims[2] = nx;

	switch(rank) {
		case 1:
			complex_to_real_1d(complex_data, real_data, nx);
			break;

		case 2:
		case 3:
		{
#ifdef FFTW_PLAN_CACHING
			bool ip = ( complex_data == real_data );
			fftwf_plan plan = plan_cache.get_plan(rank,nx,ny,nz,EMAN2_COMPLEX_2_REAL,ip,(fftwf_complex *) complex_data, real_data);
			// According to FFTW3, this is making use of the "guru" interface - this is necessary if plans are to be re-used
			fftwf_execute_dft_c2r(plan, (fftwf_complex *) complex_data, real_data);
#else
			fftwf_plan plan = fftwf_plan_dft_c2r(rank, dims + (3 - rank),
					(fftwf_complex *) complex_data, real_data, FFTW_ESTIMATE);
			fftwf_execute(plan);
			fftwf_destroy_plan(plan);
#endif // FFTW_PLAN_CACHING


		}
			break;

		default:
			//LOGERR("Should NEVER be here!!!");
			cout << "The rank of the array is not 1D, 2D, or 3D!\n";
			break;
	}

	return 0;
}

#endif	//FFTW3

#ifdef NATIVE_FFT
#include "sparx/native_fft.h"
int EMfft::real_to_complex_1d(float *real_data, float *complex_data, int n)
{
	//int complex_size = n + 2 - n%2;
	float * work = (float*) malloc((2*n+15)*sizeof(float));
	if (!work) {
		fprintf(stderr,"real_to_complex_1d: failed to allocate work\n");
		LOGERR("real_to_complex_1d: failed to allocate work\n");
	}

	rffti(n, work);
	memcpy(&complex_data[1], real_data, n * sizeof(float));
	rfftf(n, &complex_data[1], work);
	complex_data[0] = complex_data[1] ;
	complex_data[1] = 0.0f ;
	if (n%2 == 0)  complex_data[n+1] = 0.0f ;

	free(work);
	return 0;
}
/*{
	//int complex_size = n + 2 - n%2;

	//memcpy(complex_data, real_data, n * sizeof(float));
	//float * work = (float*) malloc((2*n+15)*sizeof(float));
	//if (!work) {
	//	fprintf(stderr,"real_to_complex_1d: failed to allocate work\n");
	//	LOGERR("real_to_complex_1d: failed to allocate work\n");
	//}
	//Nativefft::fmrs_1rf(complex_data, work, n);
	//cout<<"doing rfftf"<<endl;
	//rffti(n, work);

	//rfftf(n, real_data, work);

	//cout<<"doing fftr_q"<<endl;
	int l=(int)(log2(n));
        Util::fftr_q(real_data,l);

	//free(work);
	return 0;
}//
{
	int complex_size = n + 2 - n%2;

	memcpy(complex_data, real_data, n * sizeof(float));
	float * work = (float*) malloc(complex_size*sizeof(float));
	if (!work) {
		fprintf(stderr,"real_to_complex_1d: failed to allocate work\n");
		LOGERR("real_to_complex_1d: failed to allocate work\n");
	}

	Nativefft::fmrs_1rf(complex_data, work, n);

	free(work);
	return 0;
}*/

int EMfft::complex_to_real_1d(float *complex_data, float *real_data, int n)
{
	//here, n is the "logical" size of DFT, not the array size
	float * work = (float*) malloc((2*n+15)*sizeof(float));
	if (!work) {
		fprintf(stderr,"real_to_complex_1d: failed to allocate work\n");
		LOGERR("complex_to_real_1d: failed to allocate work\n");
	}

	rffti(n, work);

	memcpy(&real_data[1], &complex_data[2], (n-1) * sizeof(float));
	real_data[0] = complex_data[0] ;
	rfftb(n, real_data, work);
	//  Normalize
	float nrm = 1.0f/float(n);
	for (int i = 0; i<n; i++) real_data[i] *= nrm;
	free(work);
	return 0;
}
/*{
	int complex_size = n + 2 - n%2;

	//here, n is the "logical" size of DFT, not the array size
	memcpy(real_data, complex_data, complex_size * sizeof(float));
	float * work = (float*)malloc(complex_size*sizeof(float));
	if (!work) {
		fprintf(stderr,"real_to_complex_1d: failed to allocate work\n");
		LOGERR("complex_to_real_1d: failed to allocate work\n");
	}

	Nativefft::fmrs_1rb(real_data, work, n);

	free(work);
	return 0;
}*/

int EMfft::real_to_complex_nd(float *real_data, float *complex_data, int nx, int ny, int nz)
{
	const int rank = get_rank(ny, nz);
	const int complex_nx = nx + 2 - nx%2;

	switch(rank) {
		case 1:		//for 1D fft
			real_to_complex_1d(real_data, complex_data, nx);
			return 0;
		case 2:		//for 2D fft
		{
			/*if(real_data != complex_data) {
				for (int j = 0; j < ny; j++) {
					memcpy(&complex_data[complex_nx*j], &real_data[nx*j], nx*sizeof(float));
				}
			}
			float * work = (float*) malloc(complex_nx*sizeof(float));
	   		if (!work) {
	   			fprintf(stderr,"real_to_complex_nd(2df): failed to allocate work\n");
	   			LOGERR("real_to_complex_nd(2df): failed to allocate work\n");
	   		}

	   		// 2d inplace fft, overwrite y
	   		int status = Nativefft::fmrs_2rf(complex_data, work, complex_nx, nx, ny);
	   		if (status !=0) {
	      		fprintf(stderr, "real_to_complex_nd(2df): status = %d\n", status);
	      		LOGWARN("real_to_complex_nd(2df): status = %d\n", status);
	   		}

	   		free(work);*/

	   		int status = Nativefft::ftp_2rf(real_data, complex_data, complex_nx, nx, ny);
	   		if (status !=0) {
	      		fprintf(stderr, "real_to_complex_nd(2df): status = %d\n", status);
	      		LOGWARN("real_to_complex_nd(2df): status = %d\n", status);
	   		}
	   		return 0;
		}
	   	case 3:		//for 3D fft
	   	{
			/*if(real_data != complex_data) {
				for (int k = 0; k<nz; k++) {
		      		for (int j = 0; j < ny; j++) {
		         		memcpy(&complex_data[complex_nx*ny*k+j*complex_nx], &real_data[nx*ny*k+j*nx], nx*sizeof(float));
		      		}
				}
			}
			float * work = (float*) malloc(1*sizeof(float));//malloc(complex_nx*sizeof(float));
	   		if (!work) {
	   			fprintf(stderr,"real_to_complex_nd(3df): failed to allocate work\n");
	   			LOGERR("real_to_complex_nd(3df): failed to allocate work\n");
	   		}*/

	   		// 3d inplace fft, overwrite complex_data
	   		int status = Nativefft::ftp_3rf(real_data, complex_data, complex_nx, nx, ny, nz);
	   		if (status !=0) {
	      		fprintf(stderr, "real_to_complex_nd(3df): status = %d\n", status);
	      		LOGWARN("real_to_complex_nd(3df): status = %d\n", status);
	   		}

	   		//free(work);
	   		return 0;
	   	}
	   	default:
	   		LOGERR("Never should be here...\n");
	   		return -1;
	}
}

int EMfft::complex_to_real_nd(float *complex_data, float *real_data, int nx, int ny, int nz)
{
	const int rank = get_rank(ny, nz);
	const int complex_nx = nx + 2 - nx%2;

	switch(rank) {
		case 1:		//for 1D ift
			complex_to_real_1d(complex_data, real_data, nx);
			return 0;
		case 2:		//for 2D ift
		/*{
			if(real_data != complex_data) {
				memcpy(real_data, complex_data, complex_nx*ny*sizeof(float));
			}

			float * work = (float*) malloc(complex_nx*sizeof(float));
			if (!work) {
				fprintf(stderr,"complex_to_real_nd(2db): failed to allocate work\n");
				LOGERR("complex_to_real_nd(2db): failed to allocate work\n");
			}

			// 2d inplace ift, overwrite real_data
	   		int status = Nativefft::fmrs_2rb(real_data, work, complex_nx, nx, ny);
	   		if (status !=0) {
	      		fprintf(stderr, "complex_to_real_nd(2db): status = %d\n", status);
	      		LOGWARN("complex_to_real_nd(2db): status = %d\n", status);
			}

			free(work);
			return 0;
		}*/
		{	//  Only out of place!
			memcpy(real_data, complex_data, complex_nx*ny*sizeof(float));

			// 2d inplace ift, overwrite real_data
	   		int status = Nativefft::ftp_2rb(real_data, complex_nx, nx, ny);

	   		if (status !=0) {
	      		fprintf(stderr, "complex_to_real_nd(2db): status = %d\n", status);
	      		LOGWARN("complex_to_real_nd(2db): status = %d\n", status);
			}
			return 0;
		}
		case 3:		//for 3D ift
		{	// Only out of place!
			memcpy(real_data, complex_data, complex_nx*ny*nz*sizeof(float));

	   		// 3d inplace fft, overwrite real_data
	   		int status = Nativefft::ftp_3rb(real_data, complex_nx, nx, ny, nz);
	   		if (status !=0) {
	      		fprintf(stderr, "complex_to_real_nd(3db): status = %d\n", status);
	      		LOGWARN("complex_to_real_nd(3db): status = %d\n", status);
	   		}
	   		return 0;
		}
	   	default:
	   		LOGERR("Never should be here...\n");
	   		return -1;
	}
}
#endif	//NATIVE_FFT

#ifdef 	ACML
#include <iostream>
using std::cout;
using std::endl;

int EMfft::real_to_complex_1d(float *real_data, float *complex_data, int n)
{
	const int complex_n = n + 2 - n%2;	//the size for 1D complex array
	int info;

	/* Allocate communication work array */
	float * comm = (float *)malloc((3*n+100)*sizeof(float));
	/* Allocate work array to store ACML complex array*/
	float * fft_data = (float *)malloc(n * sizeof(float));

	//copy real_data to complex_data then apply inplace FFT on complex data
	memcpy(fft_data, real_data, n * sizeof(float));

	/* Initialize communication work array */
	scfft(0, n, fft_data, comm, &info);
	if(info != 0) {
		LOGERR("Error happening in Initialize communication work array: %d", info);
	}

	/* Compute a real --> Hermitian transform */
	scfft(1, n, fft_data, comm, &info);
	if(info != 0) {
		LOGERR("Error happening in Initialize communication work array: %d", info);
	}

	/**ACML fft real to complex 1D result store as:
	 * let X be an array of length N and with first index 0,
	 * - X(i) contains the real part of Z(i) for i = 0, ..., N/2
	 * - X(N-i) contains the imaginary part of Z(i) for i=1, ..., (N-1)/2
	 * so we need re-organize the data layout and time all data by sqrt(N)
	 * to make the reault consistent with FFTW  */
	transform(fft_data, fft_data+n, fft_data, time_sqrt_n(n));

	for(int i=0; i<complex_n; ++i) {
		if(i%2==0) {	//copy real part of complex array
			complex_data[i] = fft_data[i/2];
		}
		else {	//copy imaginary part of complex array
			if(i==1) {
				complex_data[i] = 0.0f;
			}
			else {
				if(n%2 == 0 && i == complex_n-1 ) {
					complex_data[i] = 0.0f;
				}
				else {
					complex_data[i] = fft_data[n-i/2];
				}
			}
		}
	}

	free(fft_data);
	free(comm);
	return 0;
}

int EMfft::complex_to_real_1d(float *complex_data, float *real_data, int n)
{
	int complex_n = n + 2 - n%2;	//the size for 1D complex array
	int info;

	/* Allocate communication work array */
	float * comm = (float *)malloc((3*n+100)*sizeof(float));

	for(int i=0; i<complex_n; ++i) {
		if(i%2 == 0) {	//copy real part of complex array
			real_data[i/2] = complex_data[i];
		}
		else {	//copy imaginary part of complex array
			if(i==1) {continue;}
			if(!(n%2 == 0 && i == complex_n-1)) {
				real_data[n-i/2] = complex_data[i];
			}
		}
	}
	transform(real_data, real_data+n, real_data, divide_sqrt_n(n));

	/* Initialize communication work array */
	csfft(0, n, real_data, comm, &info);
	if(info != 0) {
		LOGERR("Error happening in Initialize communication work array: %d", info);
	}

	/* Conjugate the Vector X to simulate inverse transform */
	for (int j = n/2+1; j < n; j++) {
    	real_data[j] = -real_data[j];
	}

	/* Compute a Hermitian --> real transform */
	csfft(1, n, real_data, comm, &info);
	if(info != 0) {
		LOGERR("Error happening in Initialize communication work array: %d", info);
	}

	free(comm);
	return 0;
}

int EMfft::real_to_complex_2d(float *real_data, float *complex_data, int nx, int ny)
{
	const int complex_nx = nx + 2 - nx%2;
	int info;
	/* Allocate communication work array */
	float * comm = (float *)malloc((3*nx+100)*sizeof(float));

	/* Allocate work array to store ACML complex array*/
	float * fft_data = (float *)malloc(complex_nx * ny * sizeof(float));
	cout << "fft_data after allocation:" << endl;
	for(int j=0; j<ny; ++j) {
		for(int i=0; i<complex_nx; ++i) {
			cout << "data[" << i << "][" << j << "] = " << fft_data[i+j*complex_nx] << "\t";
		}
		cout << endl;
	}

	/*copy real_data to complex_data then apply inplace FFT on complex data*/
	for(int i=0; i<ny; ++i) {
		memcpy(fft_data+i*complex_nx, real_data+i*nx, nx*sizeof(float));
	}
	//memcpy(fft_data, real_data, nx * ny * sizeof(float));
	cout << endl << "real_data array: " << endl;
	for(int j=0; j<ny; ++j) {
		for(int i=0; i<nx; ++i) {
			cout << real_data[i+j*nx] << "\t";
		}
		cout << endl;
	}
	cout << endl << "the fft_data array: " << endl;
	for(int j=0; j<ny; ++j) {
		for(int i=0; i<complex_nx; ++i) {
			cout << fft_data[i+j*complex_nx] << "\t";
		}
		cout << endl;
	}

	//do a multiple 1d real-to-complex transform on x direction
	scfftm(ny, nx, fft_data, comm, &info);

	cout << endl << "the fft_data array after x dim transform: " << endl;
	for(int j=0; j<ny; ++j) {
		for(int i=0; i<complex_nx; ++i) {
			cout << fft_data[i+j*complex_nx] << "\t";
		}
		cout << endl;
	}

/*	cout << "original fft array" << endl;
/	cout << "complex_nx = " << complex_nx << " ny = " << n*(fft_data2+i+j*2*ny+1) << endl;
	for(int i=0; i<ny; ++i) {
		for(int j=0; j<complex_nx; ++j) {
			cout << *(fft_data+j+i*complex_nx) << "\t";
		}
		cout << endl;
	}
*/
	//do a multiple 1d complex to complex transformation on y direction
	float * fft_data2 = (float *)malloc(complex_nx * ny * sizeof(float));
/*	cout << "fft array rearranged in y dimension" << endl;
	for(int i=0; i<complex_nx; ++i) {
		for(int j=0; j<ny; ++j) {
			*(fft_data2+i+j*2*ny) = *(fft_data+i+j*complex_nx);
			*(fft_data2+i+j*2*ny+1) = *(fft_data+i+complex_nx/2+j*complex_nx);
			cout << *(fft_data2+i+j*2*ny) << "\t" << *(fft_data2+i+j*2*ny+1) << "\t";
		}
		cout << endl;
	}
*/
	if(info != 0) {
		LOGERR("Error happening in scfftm: %d", info);
	}

	return 0;
}

int EMfft::complex_to_real_2d(float *complex_data, float *real_data, int nx, int ny)
{
	return 0;
}

int EMfft::real_to_complex_3d(float *real_data, float *complex_data, int nx, int ny, int nz)
{
	return 0;
}

int EMfft::complex_to_real_3d(float *complex_data, float *real_data, int nx, int ny, int nz)
{
	return 0;
}

int EMfft::real_to_complex_nd(float *real_data, float *complex_data, int nx, int ny, int nz)
{
	const int rank = get_rank(ny, nz);

	switch(rank) {
		case 1:
			return real_to_complex_1d(real_data, complex_data, nx);
		case 2:
			return real_to_complex_2d(real_data, complex_data, nx, ny);
		case 3:
			return real_to_complex_3d(real_data, complex_data, nx, ny, nz);
		default:
			LOGERR("Never should be here...\n");
	   		return -1;
	}
}

int EMfft::complex_to_real_nd(float *complex_data, float *real_data, int nx, int ny, int nz)
{
	const int rank = get_rank(ny, nz);

	switch(rank) {
		case 1:
			return complex_to_real_1d(complex_data, real_data, nx);
		case 2:
			return complex_to_real_2d(complex_data, real_data, nx, ny);
		case 3:
			return complex_to_real_3d(complex_data, real_data, nx, ny, nz);
		default:
			LOGERR("Never should be here...\n");
	   		return -1;
	}
}


#endif	//ACML

#endif	//eman_emfft_h__
