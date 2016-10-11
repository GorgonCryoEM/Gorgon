/*
 * hello.cpp
 *
 *  Created on: Aug 13, 2015
 *      Author: shadow_walker
 */

#include "hello.h"

std::string hello(){
	return "Hello World!";
}

std::string platform(){

#ifdef __APPLE__
	std::string platf("Mac");
#elif _WIN32
	std::string platf("Windows");
#elif __linux__
	std::string platf("Linux");
#elif __unix__
	std::string platf("Unix");
#else
	std::string platf("UNKNOWN");
#endif

	return std::string("I am running on " + platf + "!");
}
