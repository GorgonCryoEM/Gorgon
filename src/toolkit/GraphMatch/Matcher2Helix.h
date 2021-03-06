/*
Storage structure for the helixes being processed

Author: Sasakthi S. Abeysinghe
Date  : 02/12/2006
*/

#ifndef TOOLKIT_GRAPHMATCH_MATCHER2HELIX_H
#define TOOLKIT_GRAPHMATCH_MATCHER2HELIX_H

class Matcher2Helix {
public:
	int length;
	int node1;
	int node2;
public:
	Matcher2Helix();
	Matcher2Helix(int length, int node1, int node2);
};

inline Matcher2Helix::Matcher2Helix() {
};

inline Matcher2Helix::Matcher2Helix(int length, int node1, int node2) {
	this->length = length;
	this->node1 = node1;
	this->node2 = node2;
}
#endif
