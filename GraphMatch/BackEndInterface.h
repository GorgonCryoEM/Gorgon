#ifndef BACK_END_INTERFACE_H
#define BACK_END_INTERFACE_H

#include "GlobalConstants.h"
#include "QueryEngine.h"
#include "LinkedNode.h"

using namespace std;

namespace wustl_mm {
	namespace GraphMatch {

		class BackEndInterface {
		public:
			BackEndInterface();
			virtual ~BackEndInterface();
			// Initialization Methods
			void SetConstantsFromFile(char * fileName);
			bool SetConstant(char * token, char * value);
			bool SetConstant(char * token, double value);
			bool SetConstant(char * token, int value);
			bool SetConstant(char * token, bool value);
			// Graph Loading
			void LoadSequenceGraph();
			void LoadSkeletonGraph();
			// Process Execution
			//int ExecuteQuery(StandardGraph * sequenceGraph, StandardGraph * skeletonGraph);
			virtual int ExecuteQuery();
			// Result Retrieval
			virtual SSECorrespondenceResult GetResult(int rank);
			// Cleanup
			void CleanupMemory();
		protected:
			QueryEngine * queryEngine;
			StandardGraph * skeleton;
			StandardGraph * sequence;
		};

		BackEndInterface::BackEndInterface(): skeleton(NULL), sequence(NULL) {
			queryEngine = new QueryEngine();
		}

		BackEndInterface::~BackEndInterface() {
			delete queryEngine;
			if(skeleton != NULL) {
				delete skeleton;
			}
			if(sequence != NULL) {
				delete sequence;
			}
		}

		void BackEndInterface::SetConstantsFromFile(char * fileName) {
			LoadConstantsFromFile(fileName);	
		}

		bool BackEndInterface::SetConstant(char * token, char * value) {
			return SetConstantFromToken(token, value, 0.0, 0, false);
		}

		bool BackEndInterface::SetConstant(char *token, double value) {
			return SetConstantFromToken(token, NULL, value, 0, false);
		}

		bool BackEndInterface::SetConstant(char *token, int value) {
			return SetConstantFromToken(token, NULL, 0.0, value, false);
		}

		bool BackEndInterface::SetConstant(char *token, bool value) {
			return SetConstantFromToken(token, NULL, 0.0, 0, value);
		}

		void BackEndInterface::LoadSequenceGraph() {
			if(sequence != NULL) {
				delete sequence;
			}
			sequence = queryEngine->LoadSequenceGraph();
		}

		void BackEndInterface::LoadSkeletonGraph() {
			if(skeleton != NULL) {
				delete skeleton;
			}
			skeleton = queryEngine->LoadSkeletonGraph();
		}
		/*
		int BackEndInterface::ExecuteQuery(StandardGraph * sequenceGraph, StandardGraph * skeletonGraph) {
			return queryEngine->DoGraphMatching(sequenceGraph, skeletonGraph);
		}
		*/
		int BackEndInterface::ExecuteQuery() {
			if(skeleton != NULL && sequence != NULL)
				return queryEngine->DoGraphMatching(sequence, skeleton);
			else
				return 0;
		}

		SSECorrespondenceResult BackEndInterface::GetResult(int rank) {
			return queryEngine->GetSolution(rank);
		}

		void BackEndInterface::CleanupMemory() {
			queryEngine->FinishGraphMatching();
		}
	}
}

#endif