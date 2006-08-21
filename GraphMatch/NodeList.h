/*
Storage structure for the list of nodes being processed.

Author: Sasakthi S. Abeysinghe
Date  : 01/23/2006
*/

#ifndef NODELIST_H
#define NODELIST_H

#include "StandardNode.h"

class NodeList {
public:
	NodeList();
	~NodeList();
	vector<StandardNode*> nodes;
	void RemoveDuplicates();
public:
	void InsertSorted(StandardNode * node);
	void Insert(StandardNode * node);
};

NodeList::NodeList() {
}

NodeList::~NodeList() {
	for(int i = 0; i < nodes.size(); i++) {
		delete nodes[i];
	}
	nodes.clear();
}
void NodeList::InsertSorted(StandardNode * node) {
	bool inserted = false;
	for(int i = 0; i < (int)nodes.size(); i++) {
		if(node->cost < nodes[i]->cost) {
			nodes.insert(nodes.begin() + i, node);
			inserted = true;
			break;
		}
	}
	if(!inserted) {
		nodes.push_back(node);
	}
}

void NodeList::Insert(StandardNode * node) {
	nodes.push_back(node);
}


void NodeList::RemoveDuplicates() {
	for(int i = (int)nodes.size() - 1; i > 0; i--) {
		if(*nodes[i] == *nodes[i-1]) {
			nodes.erase(nodes.begin() + i);
		}
	}
}

#endif