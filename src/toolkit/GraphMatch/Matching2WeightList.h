/*
Storage structure for the list of weights being processed.

Author: Sasakthi S. Abeysinghe
Date  : 02/12/2006
*/


#ifndef MATCHING2WEIGHTLIST_H
#define MATCHING2WEIGHTLIST_H

class Matching2WeightList {
public:
	vector<double> weights;
	vector<int> patternHelixes;
	vector<int> baseHelixes;
	vector<bool> used;
public:
	Matching2WeightList(int magicBaseHelix);
	~Matching2WeightList();
	void AddWeightSorted(double weight, int patternHelix, int baseHelix);
	bool AddBestWeightToNode(Node * currentNode);
	void ClearUsed();
	void PrintWeightList();
private:
	int magicBaseHelix;
private:
	bool Matching2WeightList::CanUseWeight(int weightIndex, Node * currentNode);
	bool PopWeight(double &weight, int &patternHelix, int &baseHelix, Node * currentNode);
};


Matching2WeightList::~Matching2WeightList() {
	weights.clear();
	patternHelixes.clear();
	baseHelixes.clear();
	used.clear();
}

Matching2WeightList::Matching2WeightList(int magicBaseHelix) {
	this->magicBaseHelix = magicBaseHelix;
}

void Matching2WeightList::AddWeightSorted(double weight, int patternHelix, int baseHelix) {
	bool inserted = false;
	for(int i = 0; i < (int)weights.size(); i++) {
		if(weight < weights[i]) {
			weights.insert(weights.begin() + i, weight);
			patternHelixes.insert(patternHelixes.begin() + i, patternHelix);
			baseHelixes.insert(baseHelixes.begin() + i, baseHelix);
			used.insert(used.begin() + i, false);
			inserted = true;
			break;
		}
	}
	if(!inserted) {
		weights.push_back(weight);
		patternHelixes.push_back(patternHelix);
		baseHelixes.push_back(baseHelix);
		used.push_back(false);
	}	
}

bool Matching2WeightList::PopWeight(double &weight, int &patternHelix, int &baseHelix, Node * currentNode) {
	for(int i = 0; i < (int)weights.size(); i++) {
		if(CanUseWeight(i, currentNode)) {
			weight = weights[i];
			patternHelix = patternHelixes[i];
			baseHelix = baseHelixes[i];
			used[i] = true;
			return true;
		}
	}
	return false;
}

bool Matching2WeightList::CanUseWeight(int weightIndex, Node * currentNode) {
	if(used[weightIndex]) {
		return false;
	}

	for(int i = 0; i < (int)currentNode->n1.size(); i++) {
		int b = currentNode->n2[i];
		int p = currentNode->n1[i];
		if(((baseHelixes[weightIndex] == currentNode->n2[i]) && (baseHelixes[weightIndex] != magicBaseHelix)) || (patternHelixes[weightIndex] == currentNode->n1[i])) {
			return false;
		}
	}

	return true;
}

bool Matching2WeightList::AddBestWeightToNode(Node * currentNode) {
	double weight;
	int patternHelix;
	int baseHelix;
	bool added = PopWeight(weight, patternHelix, baseHelix, currentNode);
	if(added) {
		currentNode->n1.push_back(patternHelix);
		currentNode->n2.push_back(baseHelix);
		currentNode->cost += weight;
	}
	return added;
}

void Matching2WeightList::ClearUsed() {
	for(int i = 0; i < (int)used.size(); i++) {
		used[i] = false;
	}
}

void Matching2WeightList::PrintWeightList() {
	for(int i = 0; i < (int)weights.size(); i++) {
		printf("(%f, %d, %d) ", weights[i], patternHelixes[i], baseHelixes[i]);
	}
	printf("\n");
}

#endif
