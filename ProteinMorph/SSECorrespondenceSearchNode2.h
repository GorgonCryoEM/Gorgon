// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A search node in the A* search used to find the correspondence between feature pairs.


#ifndef PROTEINMORPH_SSE_CORRESPONDENCE_SEARCH_NODE2_H
#define PROTEINMORPH_SSE_CORRESPONDENCE_SEARCH_NODE2_H

#include <vector>
#include <algorithm>
#include <Foundation/GraphBase.h>
using namespace wustl_mm::Foundation;
using namespace std;

namespace wustl_mm {
	namespace Protein_Morph {
		class SSECorrespondenceSearchNode2 {
		public:
			SSECorrespondenceSearchNode2();
			SSECorrespondenceSearchNode2(unsigned int newClique);
			SSECorrespondenceSearchNode2(SSECorrespondenceSearchNode2 & oldNode, unsigned int newClique);
			void AddClique(unsigned int cliqueIndex);
			vector<SSECorrespondenceSearchNode2 *> GetChildNodes();
			vector<SSECorrespondenceSearchNode2 *> GetChildNodes(vector< set<unsigned long long> > & allCliques, vector< set<unsigned long long> > & cliqueFeaturesP, vector< set<unsigned long long> > & cliqueFeaturesQ, int levelSizeTolerence);
			void PrintSolution(vector< set<unsigned long long> > & allCliques, vector<SSECorrespondenceNode> & allNodes, bool useDirection, bool onlyCorr = false);
			float GetCost(float parentCost, vector< set<unsigned long long> > & allCliques, vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> & featureList1, vector<SSECorrespondenceFeature> & featureList2);
			float GetCliqueDistances(vector<unsigned long long> & clique1, vector<unsigned long long> & clique2, vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> & featureList1, vector<SSECorrespondenceFeature> & featureList2);
		private:
			vector<unsigned int> cliqueHistory;
		};

		SSECorrespondenceSearchNode2::SSECorrespondenceSearchNode2() {
			cliqueHistory.clear();
		}

		SSECorrespondenceSearchNode2::SSECorrespondenceSearchNode2(unsigned int newClique) {
			cliqueHistory.clear();
			cliqueHistory.push_back(newClique);
		}

		SSECorrespondenceSearchNode2::SSECorrespondenceSearchNode2(SSECorrespondenceSearchNode2 & oldNode, unsigned int newClique) {
			for(unsigned int i = 0; i < oldNode.cliqueHistory.size(); i++) {
				cliqueHistory.push_back(oldNode.cliqueHistory[i]);
			}
			cliqueHistory.push_back(newClique);
		}

		void SSECorrespondenceSearchNode2::AddClique(unsigned int cliqueIndex) {
			cliqueHistory.push_back(cliqueIndex);
		}

		vector<SSECorrespondenceSearchNode2 *> SSECorrespondenceSearchNode2::GetChildNodes(vector< set<unsigned long long> > & allCliques, vector< set<unsigned long long> > & cliqueFeaturesP, vector< set<unsigned long long> > & cliqueFeaturesQ, int levelSizeTolerence) {
			vector<SSECorrespondenceSearchNode2 *> childNodes;
			childNodes.clear();
			vector<unsigned int> compatibleCliques;						
			bool conflicts;
			int allowedSize = 0;
			
			set<unsigned long long> intersection;
		
			for(unsigned int i = 0; i < allCliques.size(); i++) {
				conflicts = false;
				for(unsigned int j = 0; (j < cliqueHistory.size()) && !conflicts; j++) {
					
					set_intersection(
						cliqueFeaturesP[i].begin(), cliqueFeaturesP[i].end(), 
						cliqueFeaturesP[cliqueHistory[j]].begin(), cliqueFeaturesP[cliqueHistory[j]].end(), 
						inserter(intersection, intersection.begin()));
					conflicts = conflicts || (intersection.size() > 0);
					intersection.clear();

					set_intersection(
						cliqueFeaturesQ[i].begin(), cliqueFeaturesQ[i].end(), 
						cliqueFeaturesQ[cliqueHistory[j]].begin(), cliqueFeaturesQ[cliqueHistory[j]].end(), 
						inserter(intersection, intersection.begin()));
					conflicts = conflicts || (intersection.size() > 0);					
					intersection.clear();
				}
				if(!conflicts){
					compatibleCliques.push_back(i);
					allowedSize = max(allowedSize, (int)allCliques[i].size());
				}
			}

			allowedSize -= levelSizeTolerence;

			for(unsigned int i = 0; i < compatibleCliques.size(); i++) {
				if((int)(allCliques[compatibleCliques[i]].size()) >= allowedSize) {
					childNodes.push_back(new SSECorrespondenceSearchNode2(*this, compatibleCliques[i]));					
				}
			}

			return childNodes;
		}

		float SSECorrespondenceSearchNode2::GetCliqueDistances(vector<unsigned long long> & clique1, vector<unsigned long long> & clique2, vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> & featureList1, vector<SSECorrespondenceFeature> & featureList2) {
			float dist = MAX_FLOAT, dist1, dist2;
						
			if((clique1.size() == 0) || (clique2.size() == 0)) {
				return 0;
			} else {
				for(unsigned int i = 0; i < clique1.size(); i++) {
					for(unsigned int j = 0; j < clique2.size(); j++) {
						dist1 = (float)(featureList1[allNodes[clique1[i]].GetPIndex()].GetCentroid() - featureList1[allNodes[clique2[j]].GetPIndex()].GetCentroid()).Length();
						dist2 = (float)(featureList2[allNodes[clique1[i]].GetQIndex()].GetCentroid() - featureList2[allNodes[clique2[j]].GetQIndex()].GetCentroid()).Length();
						dist = min(dist, abs(dist1-dist2));
					}
				}
			}
			return dist;
		}



		float SSECorrespondenceSearchNode2::GetCost(float parentCost, vector< set<unsigned long long> > & allCliques, vector<SSECorrespondenceNode> & allNodes, vector<SSECorrespondenceFeature> & featureList1, vector<SSECorrespondenceFeature> & featureList2) {
			float currCost = 0;

			if(cliqueHistory.size() > 1) {
	
				float minDist = MAX_FLOAT;
				float dist;
				vector<unsigned long long> finalClique = GraphBase<bool,bool>::VertexSetToVector(allCliques[cliqueHistory[cliqueHistory.size()-1]]);
				vector<unsigned long long> currClique;

				for(unsigned int i = 0; i < cliqueHistory.size() - 1; i++) {
					currClique = GraphBase<bool,bool>::VertexSetToVector(allCliques[cliqueHistory[i]]);
					dist = GetCliqueDistances(finalClique, currClique, allNodes, featureList1, featureList2);
					minDist = min(minDist, dist);
				}

				currCost = minDist * (float)(1.0/(1.0-exp(-(float)finalClique.size()/10.0)));;

			}

			return parentCost + currCost;
		}


		void SSECorrespondenceSearchNode2::PrintSolution(vector< set<unsigned long long> > & allCliques, vector<SSECorrespondenceNode> & allNodes, bool useDirection, bool onlyCorr) {
			printf("corr= {\n");
			for(unsigned int i = 0; i < cliqueHistory.size(); i++) {
				if(i != 0) {
					printf(", \n");
				}
				printf("\t{");
				vector<unsigned long long> clique = GraphBase<bool,bool>::VertexSetToVector(allCliques[cliqueHistory[i]]);				
				for(unsigned int j = 0; j < clique.size(); j++) {
					if(j != 0) {
						printf(", ");
					}
					if(useDirection) {
						if(allNodes[clique[j]].IsForward()) {
							printf("{{%d, %d}, {%d, %d}}", allNodes[clique[j]].GetPIndex(), 0, allNodes[clique[j]].GetQIndex(), 0);
						} else {
							printf("{{%d, %d}, {%d, %d}}", allNodes[clique[j]].GetPIndex(), 0, allNodes[clique[j]].GetQIndex(), 1);
						}
					} else {
						printf("{%d, %d}", allNodes[clique[j]].GetPIndex(), allNodes[clique[j]].GetQIndex());
					}
				}
				printf("}");
			}
			printf("};\n\n");
			if(!onlyCorr) {
				printf("corr = Sort[corr, Length[#1] > Length[#2] &];\n");
				printf("printFinalOutput[corr, fl1, fl2, 1, groundTruth]\n");
			}
		}

	}
}
#endif
