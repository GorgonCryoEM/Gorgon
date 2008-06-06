#ifndef FOUNDATION_OCTREE_H
#define FOUNDATION_OCTREE_H

#include <vector>

using namespace std;

namespace wustl_mm {
	namespace Foundation {

		unsigned int octreeChildren[8][3] = {{0,0,0}, {1,0,0}, {0,1,0}, {1,1,0}, {0,0,1}, {1,0,1}, {0,1,1}, {1,1,1}};

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
			vector<OctreeNode<TTag> *> GetCells();
			vector<OctreeNode<TTag> *> GetNeighbors(OctreeNode<TTag> * node);
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
			vector<OctreeNode<TTag> *> leafs;
			neighbors.clear();


			OctreeNode<TTag> * neigh;
			bool found = false;
			for(int i = 1; i < 8; i++) {				
				neigh = GetLeaf(node->pos[0] + node->cellSize * octreeChildren[i][0], node->pos[1] + node->cellSize * octreeChildren[i][1], node->pos[2] + node->cellSize * octreeChildren[i][2]);
				if(neigh != NULL) {
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

			return neighbors;
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
		
	}
}

#endif