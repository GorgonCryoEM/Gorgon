// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   The storage structure for the correspondence result between the SSE element and the sequence.

// CVS Meta Information: 
//   $Source: /project/mm/cvs/graphics/ssa1/source/GraphMatch/SSECorrespondenceResult.h,v $
//   $Revision: 1.5 $
//   $Date: 2009/12/22 01:03:06 $
//   $Author: schuhs $
//   $State: Exp $
//
// History Log: 
//   $Log: SSECorrespondenceResult.h,v $
//   Revision 1.5  2009/12/22 01:03:06  schuhs
//   Adding support for beta sheets to the SSE correspondence search algorithm
//
//   Revision 1.4  2008/09/29 16:19:30  ssa1
//   Adding in CVS meta information
//

#ifndef GRAPHMATCH_SSE_CORRESPONDENCE_RESULT_H
#define GRAPHMATCH_SSE_CORRESPONDENCE_RESULT_H

#include "LinkedNode.h"
#include <vector>
#include <cstdio>

using namespace std;

namespace wustl_mm {
	namespace GraphMatch {

		class SSECorrespondenceResult {
		public:
			SSECorrespondenceResult();
			SSECorrespondenceResult(LinkedNode * node, int helixCount);
			SSECorrespondenceResult(vector<int> correspondence, double cost, int helixCount);
			~SSECorrespondenceResult();

			string GetNodeString();
			double GetCost();
			int GetNodeCount();
			int GetHelixCount();
			int GetSheetCount();
			int GetSkeletonNode(int sequenceNode);
			int NodeToHelix(int nodeId);
		private:
			vector<int> correspondence;
			double cost;
			int helixCount;
			int sheetCount;
		};

		SSECorrespondenceResult::SSECorrespondenceResult(){
			correspondence.clear();
			cost = 0;
			helixCount = 0;
			sheetCount = 0;
		}

		SSECorrespondenceResult::SSECorrespondenceResult(LinkedNode * node, int helixCount){
			correspondence = node->GetNodeCorrespondence();
			cost = node->GetCost();
			this->helixCount = helixCount;
		}

		SSECorrespondenceResult::SSECorrespondenceResult(vector<int> correspondence, double cost, int helixCount){
			this->correspondence = correspondence;
			this->cost = cost;
			this->helixCount = helixCount;
		}

		SSECorrespondenceResult::~SSECorrespondenceResult(){
			correspondence.clear();
		}

		string SSECorrespondenceResult::GetNodeString(){
			string nodeString = string("");
			char text[100];
			for(unsigned int i = 0; i < correspondence.size(); i++) {
				sprintf(text, "%d", correspondence[i]);
				nodeString.append(text);
				nodeString.append(" ");
			}
			return nodeString;
		}

		double SSECorrespondenceResult::GetCost() {
			return cost;
		}

		int SSECorrespondenceResult::GetNodeCount() {
			return correspondence.size();
		}

		int SSECorrespondenceResult::GetHelixCount() {
			return helixCount;
		}

		int SSECorrespondenceResult::GetSheetCount() {
			// TODO: fix
			return -1;
		}

		int SSECorrespondenceResult::GetSkeletonNode(int sequenceNode) {
			return correspondence[sequenceNode];
		}

		int SSECorrespondenceResult::NodeToHelix(int nodeId) {
			if(nodeId >= 0) {
				if (nodeId < 2 * helixCount) {
					return (int)(nodeId / 2);
				} else {
					return (int)(nodeId - helixCount);
				}
			} else {
				return nodeId;
			}
		}

	}
}

#endif