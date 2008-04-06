#ifndef BASIC_DEFINES_H
#define BASIC_DEFINES_H

#define round(f) ((f >= 0)?(int)(f + .5):(int)(f - .5))

namespace wustl_mm {
	namespace MathTools {

		const double MIN_DOUBLE = 1.7E-308;
		const double MAX_DOUBLE = 1.7E+308;

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

#endif