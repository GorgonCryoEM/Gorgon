// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   An interface to start the graph matching engine.



#ifndef BACK_END_INTERFACE_H
#define BACK_END_INTERFACE_H

#include "GlobalConstants.h"
#include "QueryEngine.h"
#include "LinkedNode.h"

#ifdef DEBUG
	#include <iostream>
#endif

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
			bool GetConstant(char * token, char * value);
			string GetConstantString(char * token);
			double GetConstantDouble(char * token);
			int GetConstantInt(char * token);
			bool GetConstantBool(char * token);
			void ClearAllConstraints();
			void SetHelixConstraint(int sequenceHelix, int skeletonHelix);
			void SetNodeConstraint(int sequenceNode, int skeletonNode);
			int GetStrandConstraint(int sequenceNode, int constraintNum);
			int GetHelixConstraintFwd(int sequenceNode);
			int GetHelixConstraintRev(int sequenceNode);
			int GetHelixConstraintUnk(int sequenceNode);

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

		bool BackEndInterface::GetConstant(char * token, char * value) {
			int iVal;
			double dVal;
			bool bVal;
			return GetConstantFromToken(token, value, dVal, iVal, bVal);
		}

		string BackEndInterface::GetConstantString(char * token) {
			char sVal[100];
			int iVal;
			double dVal;
			bool bVal;
			GetConstantFromToken(token, sVal, dVal, iVal, bVal);
			return sVal;
		}

		double BackEndInterface::GetConstantDouble(char *token) {
			int iVal;
			double dVal;
			bool bVal;
			GetConstantFromToken(token, NULL, dVal, iVal, bVal);
			return dVal;
		}

		int BackEndInterface::GetConstantInt(char *token) {
			int iVal;
			double dVal;
			bool bVal;
			GetConstantFromToken(token, NULL, dVal, iVal, bVal);
			return iVal;
		}

		bool BackEndInterface::GetConstantBool(char *token) {
			int iVal;
			double dVal;
			bool bVal;
			GetConstantFromToken(token, NULL, dVal, iVal, bVal);
			return bVal;
		}

		void BackEndInterface::ClearAllConstraints() {
			ClearAllowedConstraints();
			ClearNotAllowedConstraints();
		}

		void BackEndInterface::SetHelixConstraint(int sequenceHelix, int skeletonHelix) {
			AddHelixConstraint(sequenceHelix, skeletonHelix);
		}

		void BackEndInterface::SetNodeConstraint(int sequenceNode, int skeletonNode) {
			AddNodeConstraint(sequenceNode, skeletonNode);
		}

		int BackEndInterface::GetStrandConstraint(int sequenceNode, int constraintNum=0) {
			// get # of helices
			// check that seqNode > numH
			// return first constraint, or zero if none
			return GetNodeConstraint(sequenceNode, constraintNum);
		}

		int BackEndInterface::GetHelixConstraintFwd(int firstHelixNode) {
			if (GetNodeConstraint(firstHelixNode, 1) != 0 || GetNodeConstraint(firstHelixNode+1, 1) != 0) {
				return 0; // more than one constraint per node
			}
			int c1 = GetNodeConstraint(firstHelixNode, 0);
			int c2 = GetNodeConstraint(firstHelixNode+1, 0);
			//cout << "c1=" << c1 << ", c2=" << c2 << endl;

			// constrained as missing
			if (c1==-1 || c2==-1) {
				return -1;
			}

			// three cases for forward match:
			// c1 odd and c2 == c1+1
			if (c1>0 && c1%2==1 && c2==c1+1) {
				return c1;
			}
			// c1 odd and c2 zero
			if (c1>0 && c1%2==1 && c2==0) {
				return c1;
			}
			// c1 zero and c2 even
			if (c1==0 && c2>0 && c2%2==0) {
				return c2-1;
			}
			// not a forward helix constraint
			return 0;
		}

		int BackEndInterface::GetHelixConstraintRev(int firstHelixNode) {
			if (GetNodeConstraint(firstHelixNode, 1) != 0 || GetNodeConstraint(firstHelixNode+1, 1) != 0) {
				return 0; // more than one constraint per node
			}
			int c1 = GetNodeConstraint(firstHelixNode, 0);
			int c2 = GetNodeConstraint(firstHelixNode+1, 0);
			//cout << "c1=" << c1 << ", c2=" << c2 << endl;

			// constrained as missing
			if (c1==-1 || c2==-1) {
				return -1;
			}

			// three cases for reverse match:
			// c1 even and c2 == c1-1
			if (c1>0 && c1%2==0 && c2==c1-1) {
				return c1;
			}
			// c1 even and c2 zero
			if (c1>0 && c1%2==0 && c2==0) {
				return c1;
			}
			// c1 zero and c2 odd
			if (c1==0 && c2>0 && c2%2==1) {
				return c2+1;
			}
			// not a reverse helix constraint
			return 0;
		}

		int BackEndInterface::GetHelixConstraintUnk(int firstHelixNode) {
			if (GetNodeConstraint(firstHelixNode, 2) != 0 || GetNodeConstraint(firstHelixNode+1, 2) != 0) {
				return 0; // more than two constraints per node
			}
			int c11 = GetNodeConstraint(firstHelixNode, 0);
			int c12 = GetNodeConstraint(firstHelixNode, 1);
			int c21 = GetNodeConstraint(firstHelixNode+1, 0);
			int c22 = GetNodeConstraint(firstHelixNode+1, 1);
			//cout << "c1=" << c1 << ", c2=" << c2 << endl;

			// constrained as missing
			if (c11==-1 || c12==-1 || c21==-1 || c22==-1) {
				return -1;
			}

			// for unknown match, both nodes must store both numbers
			if (c11<=0 || c12<=0 || c21<=0 || c22<=0) {
				return 0;
			}
			if ((c11==c21 && c12==c22) || (c11==c22 && c12==c21)) {
				if (c11<c12) {
					return c11;
				} else {
					return c12;
				}
			}
			// not a reverse helix constraint
			return 0;
		}

		void BackEndInterface::LoadSequenceGraph() {
			#ifdef DEBUG
			cout << "In BackEndInterface::LoadSequenceGraph" << endl;
			#endif
			if(sequence != NULL) {
				delete sequence;
			}
			sequence = queryEngine->LoadSequenceGraph();
			//sequence->PrintGraph();
		}

		void BackEndInterface::LoadSkeletonGraph() {
			if(skeleton != NULL) {
				delete skeleton;
			}
			skeleton = queryEngine->LoadSkeletonGraph();
			//skeleton->PrintGraph();
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
