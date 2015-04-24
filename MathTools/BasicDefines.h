// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A set of global definitions for the MATHTOOLS module

// CVS Meta Information: 
//   $Source: /project/mm/cvs/graphics/ssa1/source/MathTools/BasicDefines.h,v $
//   $Revision: 1.6 $
//   $Date: 2008/09/29 16:43:15 $
//   $Author: ssa1 $
//   $State: Exp $
//
// History Log: 
//   $Log: BasicDefines.h,v $
//   Revision 1.6  2008/09/29 16:43:15  ssa1
//   Adding in CVS meta information
//

#ifndef BASIC_DEFINES_H
#define BASIC_DEFINES_H

#ifdef _WIN32
	#define round(f) ((f >= 0)?(int)(f + .5):(int)(f - .5))
#endif

namespace wustl_mm {
	namespace MathTools {

		const double MIN_DOUBLE = 1.7E-308;
		const double MAX_DOUBLE = 1.7E+308;
		const float MIN_FLOAT = 3.4E-38f;
		const float MAX_FLOAT = 3.4E+38f;
		const int MAX_INT = 2147483647;

		inline bool isZero(double value, double varience = 1e-16) { 
			return (value < varience && value > -varience)? true : false; 
		}

		inline bool isZero(float value, float varience = 1e-6) { 
			return (value < varience && value > -varience)? true : false; 
		}
		inline bool isEqual(double value1, double value2, double varience = 1e-16) {
			return isZero(value1-value2, varience);
		}
	}
}

#undef round
#endif