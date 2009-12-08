// Copyright (C) 2005-2009 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Austin D. Abrams (austin.abrams@gmail.com)
// Description:   A function to create a sheet from a set of 3D points.

#ifndef SHEETGENERATOR_H
#define SHEETGENERATOR_H

#include <vector>
#include <set>
#include <map>
#include <math.h>
#include "../MathTools/Vector3D.h"

using namespace std;
using namespace wustl_mm::MathTools;

namespace wustl_mm {
	namespace SheetGeometry{
		struct Triangle {
			int a,b,c;
		};

		struct Edge {
			int a,b;
		};

		bool operator==(Edge left, Edge right){
			return (left.a == right.a and left.b == right.b) or (left.b == right.a and left.a == right.b);
		}
		bool operator< (Edge left, Edge right){
			if (left.a == right.a)
				return left.b < right.b;
			else
				return left.a < right.a;
		}

/*
 Returns true if the edge set forms a simple path or cycle.
 */
bool simplePathOrCycle(const vector<Edge> & edgeSet){
	vector<Edge> edges = edgeSet;
	vector<Edge>::iterator edgeIt;

	// returns true if the edges in edgeSet forms a simple path or cycle.
	Edge currEdge = edges.front();
	int head = currEdge.a;
	int tail = currEdge.b;
	edges.erase(edges.begin());

	// for each vertex in the edge set, this holds how many times
	// each vertex is used.
	map<int, unsigned int> used;

	used[head] = 1;
	used[tail] = 1;
	bool found;
	int edgeA;
	int edgeB;
	while (edges.size() > 0){
		found = false;
		for(edgeIt = edges.begin(); edgeIt != edges.end(); edgeIt++){
			edgeA = edgeIt->a;
			edgeB = edgeIt->b;
			if (head == edgeA and used[edgeA] < 2 and used[edgeB] < 2){
				head = edgeB;
				found = true;
			} else if (head == edgeB and used[edgeA] < 2 and used[edgeB] < 2){
				head = edgeA;
				found = true;
			} else if (tail == edgeA and used[edgeA] < 2 and used[edgeB] < 2){
				tail = edgeB;
				found = true;
			} else if (tail == edgeB and used[edgeA] < 2 and used[edgeB] < 2){
				head = edgeA;
				found = true;
			}

			if (found){
				used[edgeA]++;
				used[edgeB]++;
				edges.erase(edgeIt);
				break;
			}
		}
		if (not found){
			return false;
		}
	}
	return true;
}


/*
 Returns a true-false vector, whose ith element is true if all of the ith
 triangle's vertices satisfy the manifold condition:  If p is a point in
 the triangulation, where {A_1 B_1 p, ..., A_k B_k p} is the set of triangles and
 {A_1 B_1, ..., A_k B_k} forms a simple path or cycle, then p satisfies the
 manifold condition.
 */
vector<bool> obeysManifoldCondition(const vector<Triangle> & triangles){
	vector<Triangle>::const_iterator it;
	vector<bool> retVector;

	set<int> points;
	set<int>::iterator p;
	map<int,bool> meetsCondition;

	for (it = triangles.begin(); it != triangles.end(); it++){
		points.insert(it->a);
		points.insert(it->b);
		points.insert(it->c);
	}

	for (p=points.begin(); p != points.end(); p++){
		// find all the triangles that include p, and collect the opposite
		// edges of p.
		vector<Edge> oppositeEdges;
		Edge newEdge;
		for (it=triangles.begin(); it != triangles.end(); it++){
			if (it->a == *p){
				newEdge.a = it->b;
				newEdge.b = it->c;
				oppositeEdges.push_back(newEdge);
			} else if (it->b == *p){
				newEdge.a = it->a;
				newEdge.b = it->c;
				oppositeEdges.push_back(newEdge);
		    } else if (it->c == *p){
				newEdge.a = it->a;
				newEdge.b = it->b;
				oppositeEdges.push_back(newEdge);
		    }
		}

		// verify that the opposite edges of p form a simple path or cycle.
		meetsCondition[*p] = simplePathOrCycle(oppositeEdges);
	}

	for (it = triangles.begin(); it != triangles.end(); it++){

		if (meetsCondition[it->a] and meetsCondition[it->b] and meetsCondition[it->c]){
			retVector.push_back(true);
		}else{
			retVector.push_back(false);
		}
	}

	return retVector;

}

/*
 Returns a true-false vector whose ith element is true if the ith triangle
 shares each of its edges with at most 2 triangles.
 */
vector<bool> obeysEdgeConnectivity(const vector<Triangle> & triangles){
	vector<Triangle>::const_iterator it;
	vector<bool> retVector;

	map<Edge,unsigned int> used;
	Edge edge1;
	Edge edge2;
	Edge edge3;

	for (it = triangles.begin(); it != triangles.end(); it++){
		edge1.a = it->a;
		edge1.b = it->b;
		edge2.a = it->b;
		edge2.b = it->c;
		edge3.a = it->a;
		edge3.b = it->c;
		used[edge1]++;
		used[edge2]++;
		used[edge3]++;

		edge1.a = it->b;
		edge1.b = it->a;
		edge2.a = it->c;
		edge2.b = it->b;
		edge3.a = it->c;
		edge3.b = it->a;
		used[edge1]++;
		used[edge2]++;
		used[edge3]++;
	}

	for (it = triangles.begin(); it != triangles.end(); it++){
		edge1.a = it->a;
		edge1.b = it->b;
		edge2.a = it->b;
		edge2.b = it->c;
		edge3.a = it->a;
		edge3.b = it->c;
		if (used[edge1] <= 2 and used[edge2] <= 2 and used[edge3] <= 2){
			retVector.push_back(true);
		}else{
			retVector.push_back(false);
		}
	}

	return retVector;
}


// returns the 3D Gabriel graph of a set of vertices.
// It would probably be better if this took from a subset of the
// Delaunay tetrahedralization, but for small point sets, it's quick
// enough.
vector<Triangle> gabrielGraph(const vector<Vector3DFloat> vertices, const vector<int> vertexIxs){
	vector<Vector3DFloat>::const_iterator itI,itJ,itK;
	vector<Triangle> solution;
	unsigned int i,j,k;
	i = 0;
	for (itI = vertices.begin(); itI != vertices.end(); itI++){
		j = i+1;
		for (itJ = itI + 1; itJ != vertices.end(); itJ++){
			k = j+1;
			for (itK = itJ + 1; itK != vertices.end(); itK++){
				// find the radius and center of this triangle's smallest
				// circumscribed sphere.
				Vector3DFloat a = *itI;
				Vector3DFloat b = *itJ;
				Vector3DFloat c = *itK;

				Vector3DFloat aMinusB = a - b;
				Vector3DFloat bMinusC = b - c;
				Vector3DFloat aMinusC = a - c;
				Vector3DFloat bMinusA = b - a;
				Vector3DFloat cMinusB = c - b;
				Vector3DFloat cMinusA = c - a;
				float bottom = (aMinusB^bMinusC).Length();
				if (bottom == 0){
					k++;
					continue;
				}
				float radius = aMinusB.Length() * bMinusC.Length() * cMinusA.Length() / (2 * bottom);
				float alpha  = bMinusC.Length() * bMinusC.Length() * (aMinusB * aMinusC) / (2 * bottom * bottom);
				float beta   = aMinusC.Length() * aMinusC.Length() * (bMinusA * bMinusC) / (2 * bottom * bottom);
				float gamma  = aMinusB.Length() * aMinusB.Length() * (cMinusA * cMinusB) / (2 * bottom * bottom);

				Vector3DFloat center = a * alpha + b * beta + c * gamma;

				// if i,j, and k form a triangle such that no other
				// points fall in its circumscribed sphere, then this
				// triangle is in the Gabriel graph.
				bool works = true;
				for (unsigned int l = 0; l < vertices.size(); l++){
					if (l == i ||l == j || l == k)
						continue;
					Vector3DFloat otherVert = vertices[l];
					Vector3DFloat otherVec = otherVert - center;
					if (otherVec.Length() < radius){
						works = false;
						break;
					}
				}
				if (works){
					Triangle gabrielTriangle;
					gabrielTriangle.a = vertexIxs[i];
					gabrielTriangle.b = vertexIxs[j];
					gabrielTriangle.c = vertexIxs[k];
					solution.push_back(gabrielTriangle);
				}

				k++;
			}
			j++;
		}
		i++;
	}
	return solution;
}

vector<Triangle> sheetGenerator(const vector<Vector3DFloat> points, const vector<int> vertexIxs){
	vector<Triangle> triangles = gabrielGraph(points, vertexIxs);

	vector<bool> manifold      = obeysManifoldCondition(triangles);
	vector<bool> edgeConnect   = obeysEdgeConnectivity(triangles);

	unsigned int badTriangles = 0;
	for (unsigned int i = 0; i < manifold.size(); i++){
		if ((not manifold[i]) or (not edgeConnect[i])){
			badTriangles++;
		}
	}

	vector<bool>::iterator manifoldIt;
	vector<bool>::iterator edgeIt;

	while (badTriangles > 0){
		unsigned int bestScore = 0;
		unsigned int bestChoice = 0;
		for (unsigned int i=0; i < manifold.size(); i++){
			if ((not manifold[i]) or (not edgeConnect[i])){
				// this is a triangle that's eligible for deletion.
				vector<Triangle> newtriangles = triangles;
				newtriangles.erase(newtriangles.begin() + i);

				// count up the score of this new triangulation.
				vector<bool> newManifold = obeysManifoldCondition(newtriangles);
				vector<bool> newEdgeConnect = obeysEdgeConnectivity(newtriangles);
				unsigned int score = 0;
				for (manifoldIt = newManifold.begin(), edgeIt = newEdgeConnect.begin();
					 manifoldIt != newManifold.end() and edgeIt != newEdgeConnect.end();
					 manifoldIt++, edgeIt++){
					if (*manifoldIt){
						score++;
					}
					if (*edgeIt){
						score++;
					}
				}

				// see if this is the best score.
				if (score > bestScore){
					bestScore = score;
					bestChoice = i;
				}
			}
		}

		// remove the best choice.
		triangles.erase(triangles.begin() + bestChoice);
		manifold = obeysManifoldCondition(triangles);
		edgeConnect = obeysEdgeConnectivity(triangles);

		// recalculate bad triangles.
		badTriangles = 0;
		for (manifoldIt = manifold.begin(), edgeIt = edgeConnect.begin();
			 manifoldIt != manifold.end() and edgeIt != edgeConnect.end();
			 manifoldIt++, edgeIt++){
			if ((not *manifoldIt) or (not *edgeIt)){
				badTriangles++;
			}
		}
	}

	return triangles;
}
}
}
#endif
