#ifndef BASIC_DEFINES_H
#define BASIC_DEFINES_H

#define round(f) ((f >= 0)?(int)(f + .5):(int)(f - .5))

namespace wustl_mm {
	namespace MatlabInterface {

		inline bool isZero(double value, double varience = 1e-16) 
		{ 
			return (value < varience && value > -varience)? true : false; 
		}
	}
}

#endif