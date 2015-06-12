/*
Implementation of a graph matching algorithm.

Author: Sasakthi S. Abeysinghe
Date  : 04/02/2006
*/

#ifndef MATCHER7_H
#define MATCHER7_H

#include "StandardGraph.h"
#include "StandardNode.h"
#include "GlobalConstants.h"
#include "Combinatorics.h"
#include "PathGenerator.h"
#include <vector>

struct Matcher7PathHistory {
	unsigned int pathBitMap[RESULT_COUNT];
	double cost[RESULT_COUNT];
	signed char lastPatternHelix[RESULT_COUNT];
	signed char lastBaseNode[RESULT_COUNT];
	signed char lastHistoryTable[RESULT_COUNT];
	int lastHistoryRow[RESULT_COUNT];
	int lastResultRank[RESULT_COUNT];
};

class Matcher7Junction {
public:
	Matcher7Junction(int patternHelixNo, int baseHelixCount, int missingHelixCount) {
		Matcher7PathHistory * hist;
		for(int i = 0; i <= missingHelixCount; i++) {
			unsigned long long count = Combinatorics::Combinations(baseHelixCount, patternHelixNo-i);
			assert(count >= 0);
 			elementCount.push_back(count);
			hist = new Matcher7PathHistory[count];
			for(int j = 0; j < count; j++) {
				for(int k = 0; k < RESULT_COUNT; k++) {
					hist[j].pathBitMap[k] = 0;
					hist[j].cost[k] = 0;
					hist[j].lastBaseNode[k] = -1;
					hist[j].lastPatternHelix[k] = -1;
					hist[j].lastHistoryTable[k] = -1;
					hist[j].lastHistoryRow[k] = -1;
					hist[j].lastResultRank[k] = -1;
				}
			}
			history.push_back(hist);
		}
	}

	~Matcher7Junction() {
		elementCount.clear();
		for(unsigned int i = 0; i < history.size(); i++) {
			delete history[i];
		}
		history.clear();
	}
public:
	vector<Matcher7PathHistory*> history;
	vector<long long> elementCount;
};

class Matcher7 {
public:
	StandardGraph * patternGraph;
	StandardGraph * baseGraph;
public:
	Matcher7(StandardGraph * patternGraph, StandardGraph * baseGraph);
	Matcher7(StandardGraph * patternGraph, StandardGraph * baseGraph, int missingHelixCount, int missingSheetCount);
	~Matcher7();
	void RunMatching();
	void SaveResults();
private:
	double GetPathCost(StandardNode * node);
	void NormalizeGraphs();
	void FindPossiblePaths();
	int GetIndex(int patternHelixNo, int baseHelixNo);
	unsigned int EncodePath(unsigned int currentPath, int baseNode);
	bool IsVisited(unsigned int pathMap, int baseHelix);
	unsigned int GetBitValue(int bitPosition);
	int GetOtherBaseNode(int baseNode);
	int GetHistoryIndex(unsigned int pathBitmap);
	double GetCost(int patternNode1, int patternNode2, int baseNode1, int baseNode2);
	double GetMissingHelixCost(int patternHelix);
	StandardNode * FindPath(int patternHelix, int baseNode, int historyTable, int historyRow, int rank);
	void PrintJunction(int patternHelix, int baseHelix);
	void GetJunctionStats(int patternHelix, int baseNode, int & totalCount, int & usedCount, int & totalRowCount, int & usedRowCount);
	bool PathExists(int p1, int p2, int p3, int b1, int b2, int b3);
	bool SkeletonPathExists(int p1, int p2, int b1, int b2);
	void GetJunctionStats();
private:
	vector<Matcher7Junction*>  junctions;
	vector<StandardNode*> results;
	int dummyHelixNo;
	int missingHelixCount;
	int missingSheetCount;
};

Matcher7::Matcher7 (StandardGraph * patternGraph, StandardGraph * baseGraph) {
	this->patternGraph = patternGraph;
	this->baseGraph = baseGraph;
	dummyHelixNo = baseGraph->GetNodeCount();
	for(unsigned int i = 0; i < patternGraph->pdbStructures.size() * baseGraph->GetNodeCount() * 2; i++) {
		junctions.push_back(NULL);
	}

	// TODO: Make this more accurate
	missingHelixCount = (patternGraph->GetNodeCount() - baseGraph->GetNodeCount()) / 2;
	missingSheetCount = 0;
	if(!PERFORMANCE_COMPARISON_MODE) {
		NormalizeGraphs();
	}
}

Matcher7::Matcher7(StandardGraph * patternGraph, StandardGraph * baseGraph, int missingHelixCount, int missingSheetCount) {
	this->patternGraph = patternGraph;
	this->baseGraph = baseGraph;
	dummyHelixNo = baseGraph->GetNodeCount();
	for(unsigned int i = 0; i < patternGraph->pdbStructures.size() * (baseGraph->GetNodeCount() + 1) * 2; i++) {
		junctions.push_back(NULL);
	}
	this->missingHelixCount = missingHelixCount;
	this->missingSheetCount = missingSheetCount;
	if(!PERFORMANCE_COMPARISON_MODE) {
		NormalizeGraphs();
	}
}

Matcher7::~Matcher7() {
	for(unsigned int i = 0; i < patternGraph->pdbStructures.size() * (baseGraph->GetNodeCount()) * 2; i++) {
		if(junctions[i] != NULL) {
			delete junctions[i];
		}
	}
	for(unsigned int i = 0; i < results.size(); i++) {
		delete results[i];
	}
	results.clear();
	junctions.clear();
}

bool Matcher7::IsVisited(unsigned int pathMap, int baseHelix) {
	bool isVisited;
	unsigned int bitMask = GetBitValue(baseHelix);
	isVisited = ((pathMap & bitMask) == bitMask);
	return isVisited;
}

unsigned int Matcher7::GetBitValue(int bitPosition) {
	unsigned int bitValue = 1;
	for(int i = 0; i < bitPosition; i++) {
		bitValue *= 2;
	}
	return bitValue;
}

unsigned int Matcher7::EncodePath(unsigned int currentPath, int baseHelix) {
	if(IsVisited(currentPath, baseHelix)) {
		return currentPath;
	} else {
		return currentPath + GetBitValue(baseHelix);
	}
}

int Matcher7::GetIndex(int patternHelixNo, int baseHelixNo) {
	return baseHelixNo * baseGraph->GetNodeCount() + patternHelixNo;
}

void Matcher7::RunMatching() {
	FindPossiblePaths();
}

int Matcher7::GetOtherBaseNode(int baseNode) {
	if((baseNode & 1) == 1) {
		return baseNode - 1;
	} else {
		return baseNode + 1;
	}
}

int Matcher7::GetHistoryIndex(unsigned int pathBitmap) {
	char * X = new char[baseGraph->skeletonHelixes.size() + 1];
	X[0] = 0;
	int count = 0;
	for(unsigned int i = 0; i < baseGraph->skeletonHelixes.size(); i++) {
		if(IsVisited(pathBitmap, i)) {
			count++;
			X[count] = i+1;
		}
	}
	int index = 0;
	int n = baseGraph->skeletonHelixes.size();
	int m = count;
	for(int i = 1; i <= m; i++) {
		for(int j = X[i-1]+2; j <= X[i]; j++) {
			index += Combinatorics::Combinations(n-j+1, m-i);
		}
	}
	delete X;
	return index;
}

double Matcher7::GetMissingHelixCost(int patternHelix) {
	return 0;
	return GetCost(patternHelix*2, patternHelix*2+1, -1, -1);
}

double Matcher7::GetCost(int patternNode1, int patternNode2, int baseNode1, int baseNode2) {

	double patternLength = 0.0;
	bool euclideanEstimate;
	double baseLength;
	for(int i  = patternNode1; i < patternNode2; i++) {
		patternLength += patternGraph->adjacencyMatrix[i][i+1][1];
	}

	double weight = 1.0;
	if(baseNode1 == -1) {  // Special handling for missing helixes.
		baseLength = MISSING_HELIX_LENGTH;
	} else {
		baseLength = baseGraph->adjacencyMatrix[baseNode1][baseNode2][1];
		euclideanEstimate = (baseGraph->adjacencyMatrix[baseNode1][baseNode2][0] == GRAPHEDGE_LOOP_EUCLIDEAN);
		switch((int)(baseGraph->adjacencyMatrix[baseNode1][baseNode2][0] + 0.01)) {
			case(GRAPHEDGE_HELIX) : 
				weight = HELIX_WEIGHT_COEFFICIENT;
				break;
			case(GRAPHEDGE_LOOP):
				weight = LOOP_WEIGHT_COEFFICIENT;
				break;
			case(GRAPHEDGE_SHEET):
				weight = SHEET_WEIGHT_COEFFICIENT;
				break;
		}
		weight = euclideanEstimate? weight * EUCLIDEAN_LOOP_PENALTY: weight;
	}	
	switch(COST_FUNCTION)
	{
	case(1):
		return weight * fabs(patternLength - baseLength);
		break;
	case(2):
		return weight * fabs(patternLength - baseLength) / (patternLength + baseLength);
		break;
	case(3):
		return weight * pow((patternLength - baseLength),2);
		break;
	}
	return 0;
}

bool Matcher7::PathExists(int p1, int p2, int p3, int b1, int b2, int b3) {
	//TODO Make this better!!


	return (((patternGraph->adjacencyMatrix[p1][p2][0] == baseGraph->adjacencyMatrix[b1][b2][0]) || 
		((patternGraph->adjacencyMatrix[p1][p2][0] == GRAPHEDGE_LOOP) && 
		(baseGraph->adjacencyMatrix[b1][b2][0] == GRAPHEDGE_LOOP_EUCLIDEAN))) && 
	(patternGraph->adjacencyMatrix[p2][p3][0] == baseGraph->adjacencyMatrix[b2][b3][0] || 
		((patternGraph->adjacencyMatrix[p2][p3][0] == GRAPHEDGE_LOOP) && 
		(baseGraph->adjacencyMatrix[b2][b3][0] == GRAPHEDGE_LOOP_EUCLIDEAN))) &&
	(baseGraph->adjacencyMatrix[b1][b2][1] != MAXINT) && 
	(baseGraph->adjacencyMatrix[b2][b3][1] != MAXINT));
}

bool Matcher7::SkeletonPathExists(int p1, int p2, int b1, int b2) {
	return ((patternGraph->adjacencyMatrix[p1][p2][0] == baseGraph->adjacencyMatrix[b1][b2][0]) && 
		(baseGraph->adjacencyMatrix[b1][b2][1] != MAXINT));
}

void Matcher7::FindPossiblePaths() {
	// Setting up the first paths
	int totalCellCount = 0;
	int usedCellCount = 0;
	int totalRowCount = 0;
	int usedRowCount = 0;
	int baseHelixCount = baseGraph->skeletonHelixes.size();
	for(int j = 0; j < baseGraph->GetNodeCount(); j++) {
		junctions[GetIndex(0, j)] = new Matcher7Junction(0, baseHelixCount, missingHelixCount);
		#ifdef GET_STATS
		GetJunctionStats(0, j, totalCellCount, usedCellCount, totalRowCount, usedRowCount);
		#endif
		//PrintJunction(0, j);
	}

	Matcher7Junction * currentJunction, * oldJunction;
	Matcher7PathHistory * currentHistoryTable, * oldHistoryTable;
	int currentHistoryCount, oldHistoryCount, oldHistoryTableNum, ob2, obh, b2, bh, hr, rankingCount;
	unsigned int currentPathMap;
	double additionalCost, cost;

	// p-pattern Helix	b-base Node		m-missing count		bh - base helix = b/2	b2 - other end of base node 
	// hr - CurrentHistoryRow;
	// op-oldpattern	ob-oldbase	ohr - oldHistoryRow		obh - Old base helix	ob2 - other end of old base node

	int p, b, m, op, ob, ohr, pp, ohrank, chrank;

	for(p = 1; p < (int)patternGraph->pdbStructures.size(); p++) {
		//printf("\nPDB: %d ", p);
		for(b = 0; b < baseGraph->GetNodeCount(); b++) {
			bh = b/2;
			b2 = GetOtherBaseNode(b);
			currentJunction = new Matcher7Junction(p, baseHelixCount, missingHelixCount);
			junctions[GetIndex(p, b)] = currentJunction;
			for(m = 0; m <= missingHelixCount; m++) {
				currentHistoryTable = (Matcher7PathHistory*)currentJunction->history[m];
				currentHistoryCount = (int)currentJunction->elementCount[m];
				for(op = max(p-m-1,0); op < p; op++) {
					for(ob = 0; ob < baseGraph->GetNodeCount(); ob++) {
						obh = ob /2;
						ob2 = GetOtherBaseNode(ob);
						oldJunction = (Matcher7Junction*)junctions[GetIndex(op, ob)];
						oldHistoryTableNum = m-(p-op-1);
						oldHistoryTable = oldJunction->history[oldHistoryTableNum];
						oldHistoryCount = oldJunction->elementCount[oldHistoryTableNum];
						for(ohr = 0; ohr < oldHistoryCount; ohr++) {	
							additionalCost = GetCost(op*2+1, p*2, ob2, b) + GetCost(p*2, p*2+1, b, b2);
							// If this is the first helix then add the cost of assigning the initial helix.
							if((op <= missingHelixCount)&&(oldHistoryTableNum == op)) {
								rankingCount = 1;
								additionalCost += GetCost(op*2, op*2+1, ob, ob2);
								for(pp = 1; pp <= oldHistoryTableNum; pp++) {
									additionalCost += (MISSING_HELIX_PENALTY + GetMissingHelixCost(pp-1));								
								}				
								if(oldHistoryTableNum > 0) {
									additionalCost += START_END_MISSING_HELIX_PENALTY;
								}
							} else {
								rankingCount = RESULT_COUNT;
							}

							// If the current helix is dummy add penalty
							for(pp = oldHistoryTableNum; pp < m; pp++) {
								additionalCost += (MISSING_HELIX_PENALTY + GetMissingHelixCost(p-pp-1));
							}

							
							for(ohrank = 0;ohrank < rankingCount; ohrank++) 
							{
								if(((oldHistoryTable[ohr].pathBitMap[ohrank] != 0) || ((op <= missingHelixCount)&&(oldHistoryTableNum == op))) &&
								!IsVisited(oldHistoryTable[ohr].pathBitMap[ohrank], bh) &&			// Current node has not been visited								
								PathExists((p-1)*2+1, p*2, p*2+1, ob2, b, b2)) {					// Same typed paths exists
									currentPathMap = EncodePath(oldHistoryTable[ohr].pathBitMap[ohrank], obh);
									hr = GetHistoryIndex(currentPathMap);
								
									cost = oldHistoryTable[ohr].cost[ohrank] + additionalCost;

									for(chrank = 0; chrank < RESULT_COUNT; chrank++) {
										if(currentHistoryTable[hr].lastBaseNode[chrank] == -1) {		// Empty Slot
											currentHistoryTable[hr].cost[chrank] = cost;
											currentHistoryTable[hr].pathBitMap[chrank] = currentPathMap;
											currentHistoryTable[hr].lastBaseNode[chrank] = ob;
											currentHistoryTable[hr].lastPatternHelix[chrank] = op;
											currentHistoryTable[hr].lastHistoryTable[chrank] = oldHistoryTableNum;
											currentHistoryTable[hr].lastHistoryRow[chrank] = ohr;
											currentHistoryTable[hr].lastResultRank[chrank] = ohrank;
											break;
										} else if (cost < currentHistoryTable[hr].cost[chrank]) {		// Should be inserted
											for(int pushIndex = RESULT_COUNT - 2; (pushIndex >= chrank); pushIndex--) {  // Push everything one slot forward
												currentHistoryTable[hr].cost[pushIndex+1] = currentHistoryTable[hr].cost[pushIndex];
												currentHistoryTable[hr].pathBitMap[pushIndex+1] = currentHistoryTable[hr].pathBitMap[pushIndex];
												currentHistoryTable[hr].lastBaseNode[pushIndex+1] = currentHistoryTable[hr].lastBaseNode[pushIndex];
												currentHistoryTable[hr].lastPatternHelix[pushIndex+1] = currentHistoryTable[hr].lastPatternHelix[pushIndex];
												currentHistoryTable[hr].lastHistoryTable[pushIndex+1] = currentHistoryTable[hr].lastHistoryTable[pushIndex];
												currentHistoryTable[hr].lastHistoryRow[pushIndex+1] = currentHistoryTable[hr].lastHistoryRow[pushIndex];
												currentHistoryTable[hr].lastResultRank[pushIndex+1] = currentHistoryTable[hr].lastResultRank[pushIndex];
												assert(currentHistoryTable[hr].lastPatternHelix[pushIndex+1] <= p);
											}

											currentHistoryTable[hr].cost[chrank] = cost;
											currentHistoryTable[hr].pathBitMap[chrank] = currentPathMap;
											currentHistoryTable[hr].lastBaseNode[chrank] = ob;
											currentHistoryTable[hr].lastPatternHelix[chrank] = op;
											currentHistoryTable[hr].lastHistoryTable[chrank] = oldHistoryTableNum;
											currentHistoryTable[hr].lastHistoryRow[chrank] = ohr;
											currentHistoryTable[hr].lastResultRank[chrank] = ohrank;
											break;
										}
									}
								}
							}
						}
					}
				}
			}
			#ifdef GET_STATS
			GetJunctionStats(p, b, totalCellCount, usedCellCount, totalRowCount, usedRowCount);
			#endif
			//PrintJunction(p, b);
		}
	}
	#ifdef GET_STATS
	printf("\n\tOut of a total of %d memory cells, %d cells were used. (%lf %%)", totalCellCount, usedCellCount, (double)usedCellCount * 100.0/ (double)totalCellCount);
	printf("\n\tOut of a total of %d history blocks, %d blocks were used. (%lf %%)", totalRowCount, usedRowCount, (double)usedRowCount * 100.0/ (double)totalRowCount);
	#endif

}

StandardNode * Matcher7::FindPath(int patternHelix, int baseNode, int historyTable, int historyRow, int rank) {
	StandardNode * path = new StandardNode();
	path->cost = -1;

	Matcher7Junction * junction;
	Matcher7PathHistory * histTable;
	int tempRow;
	int tempRank;
	while (patternHelix >= 0) {
		assert((patternHelix >= 0) && (baseNode >= 0) && (historyRow >= 0) && (historyTable >= 0));
		path->n1[path->n1Top] = patternHelix*2 + 1;			path->n1Top++;
		path->n1[path->n1Top] = patternHelix*2;				path->n1Top++;
		path->n2[path->n2Top] = GetOtherBaseNode(baseNode);	path->n2Top++;
		path->n2[path->n2Top] = baseNode;					path->n2Top++;

		junction = (Matcher7Junction*)junctions[GetIndex(patternHelix, baseNode)];
		histTable = (Matcher7PathHistory*)junction->history[historyTable];
		assert(rank >= 0);
		if(histTable[historyRow].lastPatternHelix[rank] != -1) {
			for(int i = patternHelix-2; i >= histTable[historyRow].lastPatternHelix[rank]; i--) {
				patternHelix--;
				path->n1[path->n1Top] = patternHelix*2 + 1;	path->n1Top++;
				path->n1[path->n1Top] = patternHelix*2;		path->n1Top++;
				path->n2[path->n2Top] = -2;				path->n2Top++;
				path->n2[path->n2Top] = -2;				path->n2Top++;
			}
		}
		if(path->cost == -1) {
			path->cost = histTable[historyRow].cost[rank];
		}

		if(histTable[historyRow].lastPatternHelix[rank] == -1) {
			for(int i = patternHelix-1; i >= 0; i--) {
				patternHelix--;
				path->n1[path->n1Top] = patternHelix*2 + 1;	path->n1Top++;
				path->n1[path->n1Top] = patternHelix*2;		path->n1Top++;
				path->n2[path->n2Top] = -2;				path->n2Top++;
				path->n2[path->n2Top] = -2;				path->n2Top++;
			}
		}

		tempRow = historyRow;
		tempRank = rank;

		baseNode = histTable[tempRow].lastBaseNode[tempRank];
		patternHelix = histTable[tempRow].lastPatternHelix[tempRank];
		historyTable = histTable[tempRow].lastHistoryTable[tempRank];
		historyRow = histTable[tempRow].lastHistoryRow[tempRank];
		rank = histTable[tempRow].lastResultRank[tempRank];
	}
	path->SortOnPattern();
	return path;
}

void Matcher7::SaveResults() {
	printf("\nTop %d Paths:\n", RESULT_COUNT);
	Matcher7Junction * junction;
	Matcher7PathHistory * historyTable;
	int elementCount;
	StandardNode * node, * oldNode;
	bool inserted;
	results.clear();
	int resultSize = 0;
	for(int p = patternGraph->pdbStructures.size()-1 - missingHelixCount; p < (int)patternGraph->pdbStructures.size(); p++) {
		for(int b = 0; b < baseGraph->GetNodeCount(); b++) {
			junction = (Matcher7Junction*)junctions[GetIndex(p, b)];
			for(int m = 0; m <= missingHelixCount - ((int)patternGraph->pdbStructures.size() - 1 - p); m++) {
				historyTable = junction->history[m];
				elementCount = junction->elementCount[m];
				for(int r = 0; r < elementCount; r++) {
					for(int rank = 0; rank < RESULT_COUNT; rank++) {
						if(historyTable[r].pathBitMap[rank] != 0) {
							node = FindPath(p, b, m, r, rank);		
							// Adding final missing helix penalties.
							for(int pp = p+1; pp <= (int)patternGraph->pdbStructures.size() - 1; pp++) {
								node->cost += (MISSING_HELIX_PENALTY + GetMissingHelixCost(pp));
							}
							if(p < (int)patternGraph->pdbStructures.size() - 1) {
								node->cost += START_END_MISSING_HELIX_PENALTY;
							}
							inserted = false;
							resultSize = results.size();
							for(int i = 0; i < min(resultSize, RESULT_COUNT); i++) {
								oldNode = (StandardNode*)results[i];
								if(oldNode->cost > node->cost) {
									results.insert(results.begin() + i, node);
									if(results.size() > RESULT_COUNT) {
										node = results[results.size() - 1];
										delete node;
										results.erase(results.end() - 1);
									}
									inserted = true;
									break;
								}
							}
							if(!inserted && (results.size() < RESULT_COUNT)) {
								results.push_back(node);
							} else if(!inserted) {
								delete node;
							}
						}
					}
				}
			}
		}
	}
	
	for(unsigned int i = 0; i < results.size(); i++) {
		node = (StandardNode*)results[i];
		for(int j = node->n1Top; j < patternGraph->nodeCount; j++) {
			node->n1[node->n1Top] = j;							node->n1Top++;
			node->n2[node->n2Top] = -2;							node->n2Top++;
		}
		node->PrintNodeConcise(i+1);
	}
}

void Matcher7::PrintJunction(int patternHelix, int baseNode) {
	Matcher7Junction * junc = junctions[GetIndex(patternHelix, baseNode)];
	if(junc != NULL) {
		printf("\nJunction History (%d, %d)\n", patternHelix+1, baseNode+1);
		Matcher7PathHistory * historyTable;
		Matcher7PathHistory historyElement;
		for(unsigned int i = 0; i < junc->history.size(); i++) {
			printf("\t%d missing helixes\n", i);
			historyTable = (Matcher7PathHistory*)junc->history[i];
			for(unsigned int j = 0; j < junc->elementCount[i]; j++) {
				historyElement = historyTable[j];
				for(int k = 0; k < RESULT_COUNT; k++) {
					if(historyElement.lastBaseNode[k] != -1) {
						printf("\t%d.) Bit map : %d | ", k, historyElement.pathBitMap, historyElement.cost);
						StandardNode * node = FindPath(patternHelix, baseNode, i, j, k);
						node->PrintNodeConcise();
						delete node;
					}				
				}
			}
		}
	}
}
void Matcher7::GetJunctionStats(int patternHelix, int baseNode, int & totalCount, int & usedCount, int & totalRowCount, int & usedRowCount) {
	Matcher7Junction * junc = junctions[GetIndex(patternHelix, baseNode)];
	if(junc != NULL) {
		Matcher7PathHistory * historyTable;
		Matcher7PathHistory historyElement;
		for(unsigned int i = 0; i < junc->history.size(); i++) {
			historyTable = (Matcher7PathHistory*)junc->history[i];
			for(unsigned int j = 0; j < junc->elementCount[i]; j++) {
				totalRowCount++;
				historyElement = historyTable[j];
				for(int k = 0; k < RESULT_COUNT; k++) {
					if(historyElement.lastBaseNode[k] != -1) {
						usedCount++;
						if(k == 0) {
							usedRowCount++;
						}
					}				
					totalCount++;
				}
			}
		}
	}
}
void Matcher7::NormalizeGraphs() {

	printf("Normalizing the base graph based on helix length ratio\nNormalized Graph:\n");
	double ratio = 0;

	for(int i = 0; i < (int)baseGraph->skeletonHelixes.size(); i++) {
		ratio += (double)baseGraph->skeletonHelixes[i]->length / (double)baseGraph->adjacencyMatrix[i*2][i*2+1][1];
	}
	ratio = ratio / (double)baseGraph->skeletonHelixes.size();

	for(int i = 0; i < baseGraph->nodeCount; i++) {
		for(int j = 0; j < baseGraph->nodeCount; j++) {
			if(baseGraph->adjacencyMatrix[i][j][1] != MAXINT) {
				baseGraph->SetCost(i+1,j+1, baseGraph->adjacencyMatrix[i][j][1] * ratio);
			}

		}
	}	
	baseGraph->PrintGraph();
}
double Matcher7::GetPathCost(StandardNode * node) {
	double cost = 0;
	int b1 = -1, b2 = -1, p1, p2;
	for(int i = 0; i < node->n1Top-1; i++) {
		if(node->n2[i] != baseGraph->GetNodeCount()) {
			b1 = node->n2[i];
			p1 = node->n1[i];
		} else {
			cost += MISSING_HELIX_PENALTY / 2.0;
			cost += GetMissingHelixCost(i / 2) / 2.0;
			if((i == 0) || (i == patternGraph->GetNodeCount()-1))
			{
				cost += START_END_MISSING_HELIX_PENALTY;
			}
		}

		if(node->n2[i+1] != baseGraph->GetNodeCount()) {
			b2 = node->n2[i+1];
			p2 = node->n1[i+1];
			cost += GetCost(p1, p2, b1, b2);
			//printf("(%d %d) (%d %d)\n", p1+1, p2+1, b1+1, b2+1);
		}	
	}
	return cost;
}
void Matcher7::GetJunctionStats() {
	Matcher7Junction * currentJunction;
	Matcher7PathHistory * pathTable;
	int count;
	int tot;
	for(unsigned int p = 0; p < patternGraph->pdbStructures.size(); p++) {
		printf("\n[%d] : ", p + 1);
		for(int b = 0; b < baseGraph->GetNodeCount(); b++) {
			currentJunction = junctions[GetIndex(p, b)];
			count = 0;
			tot = 0;
			for(int m = 0; m <= missingHelixCount; m++) {
				pathTable = currentJunction->history[m];
				for(int r = 0; r < currentJunction->elementCount[m]; r++) {
					for(int k = 0; k < RESULT_COUNT; k++) {
						tot++;
						if(pathTable[r].lastPatternHelix[k] != -1) {
							count++;
						}
					}
				}
			}
			printf(" (%d : %d / %d) \t", b+1, count, tot);
		}
	}
}
#endif