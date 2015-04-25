// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Storage structure for the list of nodes being processed.

// CVS Meta Information: 
//   $Source: /project/mm/cvs/graphics/ssa1/source/GraphMatch/NodeList.h,v $
//   $Revision: 1.4 $
//   $Date: 2008/09/29 16:19:30 $
//   $Author: ssa1 $
//   $State: Exp $
//
// History Log: 
//   $Log: NodeList.h,v $
//   Revision 1.4  2008/09/29 16:19:30  ssa1
//   Adding in CVS meta information
//


#ifndef NODELIST_H
#define NODELIST_H

#include "StandardNode.h"
namespace wustl_mm {
	namespace GraphMatch {

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
			for(unsigned int i = 0; i < nodes.size(); i++) {
				delete nodes[i];
			}
			nodes.clear();
		}
		void NodeList::InsertSorted(StandardNode * node) {
			bool inserted = false;
			for(unsigned int i = 0; i < nodes.size(); i++) {
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
	}
}
#endif