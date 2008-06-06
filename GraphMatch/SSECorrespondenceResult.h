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
			SSECorrespondenceResult(LinkedNode * node);
			SSECorrespondenceResult(vector<int> correspondence, double cost);
			~SSECorrespondenceResult();

			string GetNodeString();
			double GetCost();
			int GetNodeCount();
			int GetSkeletonNode(int sequenceNode);
			int NodeToHelix(int nodeId);
		private:
			vector<int> correspondence;
			double cost;
		};

		SSECorrespondenceResult::SSECorrespondenceResult(){
			correspondence.clear();
			cost = 0;
		}

		SSECorrespondenceResult::SSECorrespondenceResult(LinkedNode * node){
			correspondence = node->GetNodeCorrespondence();
			cost = node->GetCost();
		}

		SSECorrespondenceResult::SSECorrespondenceResult(vector<int> correspondence, double cost){
			this->correspondence = correspondence;
			this->cost = cost;
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

		int SSECorrespondenceResult::GetSkeletonNode(int sequenceNode) {
			return correspondence[sequenceNode];
		}

		int SSECorrespondenceResult::NodeToHelix(int nodeId) {
			if(nodeId >= 0) {
				return (int)(nodeId / 2);
			} else {
				return nodeId;
			}
		}

	}
}

#endif