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

#ifdef _WIN32
	#define FFTW3
#endif

#include <string>
using std::string;
#include <sstream>
using std::stringstream;
//#include "log.h" //EMAN2 specific
#include <iostream>
#include <stdio.h>

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

	inline string int2str(int n)
	{
		char s[32] = {'\0'};
		sprintf(s, "%d", n);
		return string(s);
	}
	
	inline const char* E2Exception::what() const throw()
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












#endif	//eman_emfft_h__
