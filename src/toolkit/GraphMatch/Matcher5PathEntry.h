/*
Storage structure of a path entry in the memoisation
NOTE: Works only when there are less than 32 nodes !!!

Author: Sasakthi S. Abeysinghe
Date  : 04/02/2006
*/

#ifndef MATCHER5PATHENTRY_H
#define MATCHER5PATHENTRY_H

struct Matcher5 {
	unsigned int pathBitMap;
	double	cost;
	signed char lastNode;
};