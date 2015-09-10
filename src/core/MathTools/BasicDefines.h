// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A set of global definitions for the MATHTOOLS module


#ifndef BASIC_DEFINES_H
#define BASIC_DEFINES_H

double round(double f);

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
