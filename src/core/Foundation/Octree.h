// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for storing an octree.


#ifndef FOUNDATION_OCTREE_H
#define FOUNDATION_OCTREE_H

#include <vector>
#include <MathTools/Vector3D.h>

using namespace std;
using namespace wustl_mm::MathTools;

namespace wustl_mm {
	namespace Foundation {

		// Do not modify these structures.. modification of one implies modification of all three
		unsigned int octreeChildren[8][3] = {{0,0,0}, {1,0,0}, {0,1,0}, {1,1,0}, {0,0,1}, {1,0,1}, {0,1,1}, {1,1,1}};
		//unsigned int octreeCellTestVectors[12][2] = {{0,1}, {0,2}, {1,3}, {2,3}, {4,5}, {4,6}, {5,7}, {6,7}, {0,4}, {1,5}, {2,6}, {3,7}};
		unsigned int octreeCellTestVectors[12][2] = {{0,1}, {0,2}, {0,4}};
		unsigned int octreeChildren1DVals[8][8][2] = {
			{{0,0}, {0,1}, {0,2}, {0,3}, {0,4}, {0,5}, {0,6}, {0,7}},
			{{0,1}, {1,1}, {0,3}, {1,3}, {0,5}, {1,5}, {0,7}, {1,7}},
			{{0,2}, {0,3}, {2,2}, {2,3}, {0,6}, {0,7}, {2,6}, {2,7}},
			{{0,3}, {1,3}, {2,3}, {3,3}, {0,7}, {1,7}, {2,7}, {3,7}},
			{{0,4}, {0,5}, {0,6}, {0,7}, {4,4}, {4,5}, {4,6}, {4,7}},
			{{0,5}, {1,5}, {0,7}, {1,7}, {4,5}, {5,5}, {4,7}, {5,7}},
			{{0,6}, {0,7}, {2,6}, {2,7}, {4,6}, {4,7}, {6,6}, {6,7}},
			{{0,7}, {1,7}, {2,7}, {3,7}, {4,7}, {5,7}, {6,7}, {7,7}}};


		struct OctreeProjectionTestMinMaxStruct {
			float minVal;
			float maxVal;
		};

		template <class TTag> struct OctreeNode {
			unsigned int pos[3];
			unsigned int cellSize;
			OctreeNode<TTag> * parent;
			OctreeNode<TTag> ** children;
			bool isLeaf;
			TTag tag;
		};

		template <class TTag> class Octree {
		public:
			Octree(unsigned int sizeX, unsigned int sizeY, unsigned int sizeZ);
			~Octree();
									
			OctreeNode<TTag> * GetLeaf(unsigned int xPos, unsigned int yPos, unsigned int zPos, OctreeNode<TTag> * node = NULL);
			vector<OctreeNode<TTag> *> GetLeafs(unsigned int xPos, unsigned int yPos, unsigned int zPos, OctreeNode<TTag> * node = NULL);
			OctreeNode<TTag> * GetRoot();
			vector<OctreeNode<TTag> *> GetCells();
			vector<OctreeNode<TTag> *> GetNeighbors(OctreeNode<TTag> * node);
			vector<OctreeNode<TTag> *> IntersectRay(Vector3DFloat ray, Vector3DFloat origin, float rayWidth);
			void AddNewLeaf(unsigned int xPos, unsigned int yPos, unsigned int zPos, unsigned int cellSize = 1, OctreeNode<TTag> * node = NULL);
			void PrintStructure(OctreeNode<TTag> * node = NULL, unsigned int level = 0);
		private:
			bool IsAdjacent(OctreeNode<TTag> * n1, OctreeNode<TTag> * n2);
			OctreeNode<TTag> * CreateNewLeaf(unsigned int xPos, unsigned int yPos, unsigned int zPos, unsigned int cellSize, OctreeNode<TTag> * parent);			
			OctreeNode<TTag> * MakeBlankStructure(unsigned int xPos, unsigned int yPos, unsigned int zPos, unsigned int cellSize, OctreeNode<TTag> * parent);			
			vector<OctreeNode<TTag> *> GetAdjacentLeafs(OctreeNode<TTag> * node, OctreeNode<TTag> * leafParent);
			void CleanNodeRecursively(OctreeNode<TTag> * &node);
			void SplitLeaf(OctreeNode<TTag> * node);
			unsigned int GetLargest2ndPower(unsigned int value);			
			vector<OctreeProjectionTestMinMaxStruct> GetMinMax1DProjectionValues(vector<Vector3DFloat> & testVectors, vector<Vector3DFloat> & points2D);
			vector<OctreeProjectionTestMinMaxStruct> GetMinMax1DProjectionValues(vector<Vector3DFloat> & testVectors, vector< vector<float> > & points1D);
			vector< vector<float> > GetCubeProjectionValues(vector<Vector3DFloat> & testVectors, vector<Vector3DFloat> & points2D);			
			void GetRayIntersectingLeafs(vector<OctreeNode<TTag> *> & intersectingCells, OctreeNode<TTag> * node, vector< vector<float> > & cubePoints1D, vector<Vector3DFloat> & testVectors, vector<OctreeProjectionTestMinMaxStruct> & minMaxRayPoints1D);

		private:
			vector<OctreeNode<TTag> *> cells;
			OctreeNode<TTag> * rootNode;
		public:
			unsigned int size[3];			
		};

		template <class TTag> Octree<TTag>::Octree(unsigned int sizeX, unsigned int sizeY, unsigned int sizeZ) {
			unsigned int cellSize = max(max(GetLargest2ndPower(sizeX), GetLargest2ndPower(sizeY)), GetLargest2ndPower(sizeZ));
			size[0] = sizeX;
			size[1] = sizeY;
			size[2] = sizeZ;
			rootNode = MakeBlankStructure(0, 0, 0, cellSize, NULL);
		}

		template <class TTag> Octree<TTag>::~Octree() {
			CleanNodeRecursively(rootNode);
		}

		template <class TTag> bool Octree<TTag>::IsAdjacent(OctreeNode<TTag> * n1, OctreeNode<TTag> * n2) {
			if (n1->cellSize < n2->cellSize) {
				return IsAdjacent(n2, n1);
			} else {
				bool adjacent = false;
				for(int i = 0; (i < 8) && !adjacent; i ++) {
					adjacent = adjacent || 
						((n2->pos[0] + n2->cellSize * octreeChildren[i][0] >= n1->pos[0]) && (n2->pos[0] + n2->cellSize * octreeChildren[i][0] <= n1->pos[0] + n1->cellSize) && 
						 (n2->pos[1] + n2->cellSize * octreeChildren[i][1] >= n1->pos[1]) && (n2->pos[1] + n2->cellSize * octreeChildren[i][1] <= n1->pos[1] + n1->cellSize) && 
						 (n2->pos[2] + n2->cellSize * octreeChildren[i][2] >= n1->pos[2]) && (n2->pos[2] + n2->cellSize * octreeChildren[i][2] <= n1->pos[2] + n1->cellSize));
				}
				return adjacent;
			}
		}

		template <class TTag> OctreeNode<TTag> * Octree<TTag>::CreateNewLeaf(unsigned int xPos, unsigned int yPos, unsigned int zPos, unsigned int cellSize, OctreeNode<TTag> * parent) {
			OctreeNode<TTag> * leaf = new OctreeNode<TTag>();
			cells.push_back(leaf);
			leaf->pos[0] = xPos;
			leaf->pos[1] = yPos;
			leaf->pos[2] = zPos;
			leaf->cellSize = cellSize;
			leaf->children = NULL;
			leaf->isLeaf = true;
			leaf->parent = parent;
			return leaf;
		}

		template <class TTag> OctreeNode<TTag> * Octree<TTag>::GetLeaf(unsigned int xPos, unsigned int yPos, unsigned int zPos, OctreeNode<TTag> * node) {
			if(node == NULL) {
				node = rootNode;
			}

			if (node->isLeaf) {
				return node;
			}

			unsigned int position[3];
			position[0] = xPos;
			position[1] = yPos;
			position[2] = zPos;

			bool found;
			for(int i = 0; i < 8; i++) {
				if(node->children[i] != NULL) {
					found = true;
					for(int j = 0; j < 3; j++) {
						found = found && (position[j] >= node->children[i]->pos[j]) && (position[j] < node->children[i]->pos[j] + node->children[i]->cellSize);
					}
					if(found) {
						return GetLeaf(xPos, yPos, zPos, node->children[i]);
					}
				}
			}
			return NULL;
		}


		template <class TTag> vector<OctreeNode<TTag> *> Octree<TTag>::GetLeafs(unsigned int xPos, unsigned int yPos, unsigned int zPos, OctreeNode<TTag> * node) {
			if(node == NULL) {
				node = rootNode;
			}

			vector<OctreeNode<TTag> *> leafs, childLeafs;
			leafs.clear();

			if (node->isLeaf) {
				leafs.push_back(node);
			} else {

				unsigned int position[3];
				position[0] = xPos;
				position[1] = yPos;
				position[2] = zPos;

				bool found;
				for(int i = 0; i < 8; i++) {
					if(node->children[i] != NULL) {
						found = true;
						for(int j = 0; j < 3; j++) {
							found = found && (position[j] >= node->children[i]->pos[j]) && (position[j] <= node->children[i]->pos[j] + node->children[i]->cellSize);
						}
						if(found) {
							
							childLeafs = GetLeafs(xPos, yPos, zPos, node->children[i]);
							for(unsigned int j = 0; j < childLeafs.size(); j++) {
								leafs.push_back(childLeafs[j]);
							}
						}
					}
				}
			}
			return leafs;
		}


		template <class TTag> OctreeNode<TTag> * Octree<TTag>::GetRoot() {
			return rootNode;
		}

		template <class TTag> OctreeNode<TTag> * Octree<TTag>::MakeBlankStructure(unsigned int xPos, unsigned int yPos, unsigned int zPos, unsigned int cellSize, OctreeNode<TTag> * parent) {
			if((xPos >= size[0]) || (yPos >= size[1]) || (zPos >= size[2])) {
				return NULL;
			}

			OctreeNode<TTag> * node = CreateNewLeaf(xPos, yPos, zPos, cellSize, parent);
			if(cellSize == 1) {
				return node;
			}

			if((xPos + cellSize >= size[0]+1) || (yPos + cellSize >= size[1]+1) || (zPos + cellSize >= size[2]+1)) {
				node->isLeaf = false;
				node->children = new OctreeNode<TTag>*[8];
				unsigned int childCellSize = cellSize / 2;
				for(int i = 0; i < 8; i++) {
					node->children[i] = MakeBlankStructure(xPos + octreeChildren[i][0] * childCellSize, yPos + octreeChildren[i][1] * childCellSize, zPos + octreeChildren[i][2] * childCellSize, childCellSize, node);					
				}
			}
			return node;
		}


		template <class TTag> vector<OctreeNode<TTag> *> Octree<TTag>::GetAdjacentLeafs(OctreeNode<TTag> * node, OctreeNode<TTag> * leafParent) {
			vector<OctreeNode<TTag> *> leafs, childLeafs;
			leafs.clear();

			if(leafParent->isLeaf) {
				if(IsAdjacent(node, leafParent)) {
					leafs.push_back(leafParent);
				}
			} else {
				for(int i = 0; i < 8; i++) {
					if(leafParent->children[i] != NULL) {
						childLeafs = GetAdjacentLeafs(node, leafParent->children[i]);
						for(unsigned int j = 0; j < childLeafs.size(); j++) {
							leafs.push_back(childLeafs[j]);
						}
						childLeafs.clear();
					}
				}
			}

			return leafs;

		}

		template <class TTag> vector<OctreeNode<TTag> *> Octree<TTag>::GetCells() {
			return cells;
		}
		template <class TTag> vector<OctreeNode<TTag> *> Octree<TTag>::GetNeighbors(OctreeNode<TTag> * node) {
			vector<OctreeNode<TTag> *> neighbors;
			vector<OctreeNode<TTag> *> possibleNeighbors;
			vector<OctreeNode<TTag> *> leafs;
			neighbors.clear();


			OctreeNode<TTag> * neigh;
			bool found = false;
			for(int i = 1; i < 8; i++) {				
				possibleNeighbors = GetLeafs(node->pos[0] + node->cellSize * octreeChildren[i][0], node->pos[1] + node->cellSize * octreeChildren[i][1], node->pos[2] + node->cellSize * octreeChildren[i][2]);
				for(unsigned int k = 0; k < possibleNeighbors.size(); k++) {
					neigh = possibleNeighbors[k];
					if(neigh != node) {
						if(neigh->cellSize == node->cellSize) {
							neighbors.push_back(neigh);
						} else if (neigh->cellSize > node->cellSize) {
							found = false;
							for(unsigned int j = 0; !found && (j < neighbors.size()); j++) {
								found = (neighbors[j] == neigh);
							}
							if(!found) {
								neighbors.push_back(neigh);
							}
						} else {
							while(neigh->cellSize < node->cellSize) {
								neigh = neigh->parent;
							}
							leafs = GetAdjacentLeafs(node, neigh);
							for(unsigned int j = 0; !found && (j < leafs.size()); j++) {
								neighbors.push_back(leafs[j]);
							}
						}
					}
				}
			}

			return neighbors;
		}


		template <class TTag> vector<OctreeNode<TTag> *> Octree<TTag>::IntersectRay(Vector3DFloat ray, Vector3DFloat origin, float rayWidth) {
			ray.Normalize();
			Vector3DFloat planeVec1 = ray.GetOrthogonal();
			planeVec1.Normalize();
			Vector3DFloat planeVec2 = ray ^ planeVec1;
			planeVec2.Normalize();
			//printf("Ray   : %f %f %f \n", ray.X(), ray.Y(), ray.Z());
			//printf("Origin: %f %f %f \n", origin.X(), origin.Y(), origin.Z());
			//flushall();


			vector<Vector3DFloat> rayPoints2D;
			Vector3DFloat rayPoint3D;
			for(int i = 0; i < 4; i ++) {
				rayPoint3D = origin + planeVec1 * rayWidth * ((float)octreeChildren[i][0] * 2.0f - 1.0f) + planeVec2 * rayWidth * ((float)octreeChildren[i][1] * 2.0f - 1.0f);
				rayPoints2D.push_back(Vector3DFloat::Project3Dto2D(rayPoint3D, origin, planeVec1, planeVec2));
			}

			vector<Vector3DFloat> cubePoints2D;
			Vector3DFloat cubePoint3D;

			for(int i = 0; i < 8; i ++) {
				cubePoint3D = Vector3DFloat(rootNode->pos[0] + octreeChildren[i][0] * rootNode->cellSize, rootNode->pos[1] + octreeChildren[i][1] * rootNode->cellSize, rootNode->pos[2] + octreeChildren[i][2] * rootNode->cellSize);
				cubePoints2D.push_back(Vector3DFloat::Project3Dto2D(cubePoint3D, origin, planeVec1, planeVec2));
			}


			vector<Vector3DFloat> testVectors;
			Vector3DFloat vec;

			for(int i = 0; i < 2; i ++) {
				vec = rayPoints2D[octreeCellTestVectors[i][0]] - rayPoints2D[octreeCellTestVectors[i][1]];
				if(!isZero(vec.Length())) {
					testVectors.push_back(vec);
				}
			}
			for(int i = 0; i < 3; i ++) {
				vec = cubePoints2D[octreeCellTestVectors[i][0]] - cubePoints2D[octreeCellTestVectors[i][1]];
				if(!isZero(vec.Length())) {
					testVectors.push_back(vec);
				}
			}

			for(unsigned int i = 0; i < testVectors.size(); i++) {
				testVectors[i].Normalize();
				testVectors[i] = Vector3DFloat(-testVectors[i].Y(), testVectors[i].X(), 0); // Rotate by 90 in 2D				
			}


			vector<OctreeProjectionTestMinMaxStruct> minMaxRayPoints1D = GetMinMax1DProjectionValues(testVectors, rayPoints2D);
			vector< vector<float> > cubePoints1D = GetCubeProjectionValues(testVectors, cubePoints2D);

			vector<OctreeNode<TTag> *> intersectingCells;

			if(rootNode != NULL) {
				GetRayIntersectingLeafs(intersectingCells, rootNode, cubePoints1D, testVectors, minMaxRayPoints1D);
			}

			return intersectingCells;
		}

		template <class TTag> void Octree<TTag>::AddNewLeaf(unsigned int xPos, unsigned int yPos, unsigned int zPos, unsigned int cellSize, OctreeNode<TTag> * node) {
			OctreeNode<TTag> * parent = GetLeaf(xPos, yPos, zPos, node);
			if(parent->cellSize > cellSize) {
				SplitLeaf(parent);
				AddNewLeaf(xPos, yPos, zPos, cellSize, parent);
			}
		}

		template <class TTag> void Octree<TTag>::CleanNodeRecursively(OctreeNode<TTag> * &node) {
			if(node != NULL) {
				if (!node->isLeaf) {
					for(int i = 0; i < 8; i++) {
						CleanNodeRecursively(node->children[i]);
					}
				}
				delete node;
				node = NULL;
			}			
		}

		template <class TTag> void Octree<TTag>::PrintStructure(OctreeNode<TTag> * node, unsigned int level) {
			if((node == NULL) || (level == 0)) {
				node = rootNode;
			}
			if(node != NULL) {
				for(int i = 0; i < level; i++) {
					printf("  ");
				}
				printf("%ld) {%ld, %ld, %ld}, %ld\n", level, node->pos[0], node->pos[1], node->pos[2], node->cellSize);
				if(!node->isLeaf) {
					for(int i = 0; i < 8; i++) {
						if(node->children[i] != NULL) {
							PrintStructure(node->children[i], level+1);
						}
					}
				}
			}
			

		}
		template <class TTag> void Octree<TTag>::SplitLeaf(OctreeNode<TTag> * node) {
			if((!node->isLeaf) || (node->cellSize == 1)) {
				printf("Error! attempting to split an unsplittable node! \n");
				return;
			}

			node->isLeaf = false;
			unsigned int childCellSize = node->cellSize / 2;
			node->children = new OctreeNode<TTag>*[8];
			for(int i = 0; i < 8; i++) {
				node->children[i] = CreateNewLeaf(node->pos[0] + octreeChildren[i][0] * childCellSize, node->pos[1] + octreeChildren[i][1] * childCellSize, node->pos[2] + octreeChildren[i][2] * childCellSize, childCellSize, node);					
			}
		}

		template <class TTag> unsigned int Octree<TTag>::GetLargest2ndPower(unsigned int value) {
			unsigned int pow = 1;
			while (pow < value) {
				pow *= 2;
			}
			return pow;
		}

		template <class TTag> vector<OctreeProjectionTestMinMaxStruct> Octree<TTag>::GetMinMax1DProjectionValues(vector<Vector3DFloat> & testVectors, vector<Vector3DFloat> & points2D) {
			vector<OctreeProjectionTestMinMaxStruct> retVal;
			OctreeProjectionTestMinMaxStruct minMaxVals;
			float val;

			for(unsigned int i = 0; i < testVectors.size(); i++)  {
				minMaxVals.maxVal = MIN_FLOAT;
				minMaxVals.minVal = MAX_FLOAT;
				for(unsigned int j = 0; j < points2D.size(); j++) {
					val = testVectors[i] * points2D[j];
					minMaxVals.minVal = min(val, minMaxVals.minVal);
					minMaxVals.maxVal = max(val, minMaxVals.maxVal);					
				}
				retVal.push_back(minMaxVals);
			}
			return retVal;
		}

		template <class TTag> vector<OctreeProjectionTestMinMaxStruct> Octree<TTag>::GetMinMax1DProjectionValues(vector<Vector3DFloat> & testVectors, vector< vector<float> > & points1D) {
			vector<OctreeProjectionTestMinMaxStruct> retVal;
			OctreeProjectionTestMinMaxStruct minMaxVals;

			for(unsigned int i = 0; i < testVectors.size(); i++)  {
				minMaxVals.maxVal = MIN_FLOAT;
				minMaxVals.minVal = MAX_FLOAT;
				for(unsigned int j = 0; j < points1D[i].size(); j++) {					
					minMaxVals.minVal = min(points1D[i][j], minMaxVals.minVal);
					minMaxVals.maxVal = max(points1D[i][j], minMaxVals.maxVal);					
				}
				retVal.push_back(minMaxVals);
			}
			return retVal;
		}

		template <class TTag> vector< vector<float> > Octree<TTag>::GetCubeProjectionValues(vector<Vector3DFloat> & testVectors, vector<Vector3DFloat> & points2D) {
			vector< vector<float> > retVal;
			vector<float> values;

			for(unsigned int i = 0; i < testVectors.size(); i++)  {
				values.clear();				
				for(unsigned int j = 0; j < points2D.size(); j++) {
					values.push_back(testVectors[i] * points2D[j]);
				}
				retVal.push_back(values);
			}
			return retVal;
		}

		template <class TTag> void Octree<TTag>::GetRayIntersectingLeafs(vector<OctreeNode<TTag> *> & intersectingCells, OctreeNode<TTag> * node, vector< vector<float> > & cubePoints1D, vector<Vector3DFloat> & testVectors, vector<OctreeProjectionTestMinMaxStruct> & minMaxRayPoints1D) {
			vector<OctreeProjectionTestMinMaxStruct> minMaxCubePoints1D = GetMinMax1DProjectionValues(testVectors, cubePoints1D);
			bool intersecting = true;

			for(unsigned int i = 0; i < testVectors.size(); i++) {
				intersecting = intersecting && 
					(((minMaxCubePoints1D[i].minVal <= minMaxRayPoints1D[i].minVal) && (minMaxCubePoints1D[i].maxVal >= minMaxRayPoints1D[i].minVal)) ||
					((minMaxCubePoints1D[i].minVal <= minMaxRayPoints1D[i].maxVal) && (minMaxCubePoints1D[i].maxVal >= minMaxRayPoints1D[i].maxVal)) || 
					((minMaxCubePoints1D[i].minVal >= minMaxRayPoints1D[i].minVal) && (minMaxCubePoints1D[i].maxVal <= minMaxRayPoints1D[i].maxVal)));
			}
		
			if(intersecting) {
				if(node->isLeaf) {
					intersectingCells.push_back(node);
				} else {
					vector< vector<float> > childCubePoints1D;
					vector<float> childCubePoints1DTest;

					for(int i = 0; i < 8; i++) {
						if(node->children[i] != NULL) {						
							childCubePoints1D.clear();
							for(unsigned int j = 0; j < testVectors.size(); j++) {
								childCubePoints1DTest.clear();
								for(int k = 0; k < 8; k++) {
									childCubePoints1DTest.push_back((cubePoints1D[j][octreeChildren1DVals[i][k][0]] + cubePoints1D[j][octreeChildren1DVals[i][k][1]]) / 2.0f);										
								}
								childCubePoints1D.push_back(childCubePoints1DTest); 
							}								
							GetRayIntersectingLeafs(intersectingCells, node->children[i], childCubePoints1D, testVectors, minMaxRayPoints1D);
						}
					}
				}
			}
		}


	}
}

#endif
