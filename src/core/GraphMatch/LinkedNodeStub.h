// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Storage structure for a node in the graph traversal.


#ifndef LINKEDNODESTUB_H
#define LINKEDNODESTUB_H

#include "LinkedNode.h"

using namespace std;
namespace wustl_mm {
	namespace GraphMatch {

		class LinkedNodeStub {
		public:
			char n1Node;
			char n2Node;
			LinkedNodeStub * parentNode;
		public:
			LinkedNodeStub();
			LinkedNodeStub(LinkedNodeStub * oldNode);
			~LinkedNodeStub();
		};

		LinkedNodeStub::LinkedNodeStub() {
			n1Node = 0;
			n2Node = 0;
			parentNode = NULL;
		}

		LinkedNodeStub::LinkedNodeStub(LinkedNodeStub * oldNode) {
			n1Node = oldNode->n1Node;
			n2Node = oldNode->n2Node;
			parentNode = oldNode->parentNode;
		}

		LinkedNodeStub::~LinkedNodeStub() {
		}
	}
}
#endif
