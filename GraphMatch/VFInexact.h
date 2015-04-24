/*
Implementation of a graph matching algorithm.

Author: Sasakthi S. Abeysinghe
Date  : 04/02/2006
*/

#ifndef VFINEXACT_H
#define VFINEXACT_H

#include "StandardGraph.h"
#include "GlobalConstants.h"
#include <vector>


using namespace std;

class VFStatus {
public:
	VFStatus(){};
	VFStatus(VFStatus * old, int nn, int mm){
		for(int i = 0; i < old->n.size(); i++) {
			n.push_back(old->n[i]);
			m.push_back(old->m[i]);
		}
		n.push_back(nn);
		m.push_back(mm);
	};

	~VFStatus(){
		n.clear();
		m.clear();
	};

	bool IsInN(int nn) {
		bool found = false;
		for(int i = 0; (i < n.size()) && !found; i++) { 
			found = found || (n[i] == nn);
		}
		return found;
	};

	bool IsInM(int mm) {
		bool found = false;
		for(int i = 0; (i < m.size()) && !found; i++) { 
			found = found || (m[i] == mm);
		}
		return found;
	};

public:
	vector<int> n;
	vector<int> m;
};

class VFStateCollection {
public:
	VFStateCollection(){};
	~VFStateCollection(){
		for(int i = 0; i < states.size(); i++) {
			delete states[i];
		}
		states.clear();
	};
public:
	vector<VFStatus*> states;
};

class VFInexact {
public:
	StandardGraph * g1;
	StandardGraph * g2;
public:
	VFInexact(StandardGraph * patternGraph, StandardGraph * baseGraph);
	VFInexact(StandardGraph * patternGraph, StandardGraph * baseGraph, int missingHelixCount, int missingSheetCount);
	~VFInexact() {};
	void PerformMatching();
private:
	VFStatus * GetAllPossiblePairs(VFStatus * s, vector<int> & t1In, vector<int> & t2In, vector<int> & t1Out, vector<int> & t2Out);
	VFStatus * GetCoherentPairs(VFStatus * P, VFStatus * s, vector<int> & t1In, vector<int> & t2In, vector<int> & t1Out, vector<int> & t2Out);
	bool FSyn(int n, int m, VFStatus * s, vector<int> & t1In, vector<int> & t2In, vector<int> & t1Out, vector<int> & t2Out);
	bool FSem(int n, int m, VFStatus * s);
	int GetIntersectionCount(vector<int> & set1, vector<int> & set2);
	vector<int> GetIntersection(vector<int> & set1, vector<int> & set2);
private:
	int missingHelixCount;
	int missingSheetCount;
};

VFInexact::VFInexact (StandardGraph * patternGraph, StandardGraph * baseGraph) {
	this->g1 = patternGraph;
	this->g2 = baseGraph;

	// TODO: Make this more accurate
	missingHelixCount = (g1->GetNodeCount() - g2->GetNodeCount()) / 2;
	missingSheetCount = 0;
}

VFInexact::VFInexact(StandardGraph * patternGraph, StandardGraph * baseGraph, int missingHelixCount, int missingSheetCount) {
	this->g1 = patternGraph;
	this->g2 = baseGraph;
	this->missingHelixCount = missingHelixCount;
	this->missingSheetCount = missingSheetCount;
}

VFStatus * VFInexact::GetAllPossiblePairs(VFStatus * s, vector<int> & t1In, vector<int> & t2In, vector<int> & t1Out, vector<int> & t2Out) {
	VFStatus * P = new VFStatus();

	t1In.clear();
	t2In.clear();
	t1Out.clear();
	t2Out.clear();

	bool insertIn, insertOut;
	for(int i = 0; i < g1->nodeCount; i++) {	
		insertIn = false;
		insertOut = false;
		if(!s->IsInN(i)) {
			for(int j = 0; j < s->n.size(); j++) {
				if(g1->EdgeExists(i, s->n[j])) {
					insertIn = true;					
				}
				if(g1->EdgeExists(s->n[j], i)) {
					insertOut = true;					
				}
			}
		}

		if(insertIn) {
			t1In.push_back(i);
		}
		if(insertOut) {
			t1Out.push_back(i);
		}
	}


	for(int i = 0; i < g2->nodeCount; i++) {	
		insertIn = false;
		insertOut = false;
		if(!s->IsInN(i)) {
			for(int j = 0; j < s->n.size(); j++) {
				if(g2->EdgeExists(i, s->m[j])) {
					insertIn = true;					
				}
				if(g2->EdgeExists(s->m[j], i)) {
					insertOut = true;					
				}
			}
		}

		if(insertIn) {
			t2In.push_back(i);
		}
		if(insertOut) {
			t2Out.push_back(i);
		}
	}

	if((t1Out.size() > 0) && (t2Out.size() > 0)) {
		for(int i = 0; i < t1Out.size(); i++) {
			for(int j = 0; j < t2Out.size(); j++) {
				P->n.push_back(t1Out[i]);
				P->m.push_back(t2Out[j]);
			}
		}
	} else if((t1In.size() > 0) && (t2In.size() > 0)) {
		for(int i = 0; i < t1In.size(); i++) {
			for(int j = 0; j < t2In.size(); j++) {
				P->n.push_back(t1In[i]);
				P->m.push_back(t2In[j]);
			}
		}
	} else {
		for(int i = 0; i < g1->nodeCount; i++) {
			if(!s->IsInN(i)) {
				for(int j = 0; j < g2->nodeCount; j++) {
					if(!s->IsInM(j)) {
						P->n.push_back(i);
						P->m.push_back(j);
					}
				}
			}
		}
	}

	return P;
}

bool VFInexact::FSyn(int n, int m, VFStatus * s, vector<int> & t1In, vector<int> & t2In, vector<int> & t1Out, vector<int> & t2Out) {
	vector<int> Pred1;
	vector<int> Pred2;
	vector<int> Succ1;
	vector<int> Succ2;

	for(int i = 0; i < g1->nodeCount; i++) {
		if(g1->EdgeExists(i, n)) {
			Pred1.push_back(i);
		}
		if(g1->EdgeExists(n, i)) {
			Succ1.push_back(i);
		}
	}

	for(int i = 0; i < g2->nodeCount; i++) {
		if(g2->EdgeExists(i, m)) {
			Pred2.push_back(i);
		}
		if(g2->EdgeExists(m, i)) {
			Succ2.push_back(i);
		}
	}

	bool rPred, rSucc, rIn, rOut, rNew, temp;
	vector<int> tempSet;

	int nn, mm;

	// Find rPred
	rPred = true;

	tempSet = GetIntersection(s->n, Pred1);
	for(int i= 0 ; i < tempSet.size(); i++) {
		nn = tempSet[i];
		temp = false;
		for(int j = 0; j < Pred2.size(); j++) {
			mm = Pred2[j];
			for(int k = 0; k < s->n.size(); k++) {
				temp = temp || ((nn == s->n[k]) && (mm == s->m[k]));
			}
		}
		rPred = rPred && temp;
	}
	tempSet.clear();


	tempSet = GetIntersection(s->m, Pred2);
	for(int i= 0 ; i < tempSet.size(); i++) {
		mm = tempSet[i];
		temp = false;
		for(int j = 0; j < Pred1.size(); j++) {
			nn = Pred1[j];
			for(int k = 0; k < s->n.size(); k++) {
				temp = temp || ((nn == s->n[k]) && (mm == s->m[k]));
			}
		}
		rPred = rPred && temp;
	}



	// Find RSucc
	rSucc = true;
	tempSet = GetIntersection(s->n, Succ1);
	for(int i= 0 ; i < tempSet.size(); i++) {
		nn = tempSet[i];
		temp = false;
		for(int j = 0; j < Succ2.size(); j++) {
			mm = Succ2[j];
			for(int k = 0; k < s->n.size(); k++) {
				temp = temp || ((nn == s->n[k]) && (mm == s->m[k]));
			}
		}
		rSucc = rSucc&& temp;
	}
	tempSet.clear();


	tempSet = GetIntersection(s->m, Succ2);
	for(int i= 0 ; i < tempSet.size(); i++) {
		mm = tempSet[i];
		temp = false;
		for(int j = 0; j < Succ1.size(); j++) {
			nn = Succ1[j];
			for(int k = 0; k < s->n.size(); k++) {
				temp = temp || ((nn == s->n[k]) && (mm == s->m[k]));
			}
		}
		rSucc = rSucc && temp;
	}
	rIn = (GetIntersectionCount(Succ1, t1In) == GetIntersectionCount(Succ2, t2In)) && (GetIntersectionCount(Pred1, t1In) == GetIntersectionCount(Pred2, t2In));
	rOut = (GetIntersectionCount(Succ1, t1Out) == GetIntersectionCount(Succ2, t2Out)) && (GetIntersectionCount(Pred1, t1Out) == GetIntersectionCount(Pred2, t2Out));
	rNew = true;

	return rPred && rSucc && rIn && rOut && rNew;
}

int VFInexact::GetIntersectionCount(vector<int> & set1, vector<int> & set2) {
	int count = 0;
	for(int i = 0; i < set1.size(); i++) {
		for(int j = 0; j < set2.size(); j++) {
			if(set1[i] == set2[j]) {
				count++;
			}
		}
	}
	return count;
}

vector<int> VFInexact::GetIntersection(vector<int> & set1, vector<int> & set2) {
	vector<int> intersection;
	for(int i = 0; i < set1.size(); i++) {
		for(int j = 0; j < set2.size(); j++) {
			if(set1[i] == set2[j]) {
				intersection.push_back(set1[i]);
			}
		}
	}
	return intersection;
}

bool VFInexact::FSem(int n, int m, VFStatus * s) {
	return true;
	for(int i = 0; i < g1->nodeCount; i++) {
		
	}
}

VFStatus * VFInexact::GetCoherentPairs(VFStatus * P, VFStatus * s, vector<int> & t1In, vector<int> & t2In, vector<int> & t1Out, vector<int> & t2Out){
	VFStatus * Q = new VFStatus();
	VFStatus * QPotential;
	int nn, mm;

	for(int i = 0; i < P->n.size(); i++) {
		nn = P->n[i];
		mm = P->m[i];
		QPotential = new VFStatus(s, nn, mm);
		
		if(FSyn(nn, mm, QPotential, t1In, t2In, t1Out, t2Out) && FSem(nn, mm, QPotential)) {
			Q->n.push_back(nn);
			Q->m.push_back(mm);
		}
		delete QPotential;
	}
	return Q;
}

void VFInexact::PerformMatching(){
	vector<VFStateCollection*> S;
	VFStatus * s0 = new VFStatus();
	VFStatus * s, * t;
	VFStatus * P;
	VFStatus * Q;
	VFStateCollection * cStates = new VFStateCollection();
	vector<int> t1In;
	vector<int> t2In;
	vector<int> t1Out;
	vector<int> t2Out;

	cStates->states.push_back(s0);
	S.push_back(cStates);
	int k = 0;

	do {
		printf("%d : %d\n", k, S[k]->states.size());
		S.push_back(new VFStateCollection());
		for(int i = 0; i < S[k]->states.size(); i++) {
			s = S[k]->states[i];
			P = GetAllPossiblePairs(s, t1In, t2In, t1Out, t2Out);
			Q = GetCoherentPairs(P, s, t1In, t2In, t1Out, t2Out);
			delete P;
			for(int j = 0; j < Q->n.size(); j++) {
				t = new VFStatus(s, Q->n[j], Q->m[j]);
				S[k+1]->states.push_back(t);
			}
			delete Q;
		}
		k++;
	} while ((k != g1->nodeCount) && (S[k]->states.size() != 0));

	if((k == g1->nodeCount) && (S[k]->states.size() > 0)) {
		for(int i = 0; i < S[k]->states[0]->n.size(); i++) {
			printf("(%d - %d) ", S[k]->states[0]->n[i], S[k]->states[0]->m[i]);
		}
		printf("\n");
	} else 	{
		printf("Not found\n");
	}
	

	for(int i = 0; i < S.size(); i++) {
		delete S[i];
	}

}

#endif

