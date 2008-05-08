#ifndef FOUNDATION_OCTREE_H
#define FOUNDATION_OCTREE_H

namespace wustl_mm {
	namespace Foundation {

		unsigned int octreeChildren[8][3] = {{0,0,0}, {1,0,0}, {0,1,0}, {1,1,0}, {0,0,1}, {1,0,1}, {0,1,1}, {1,1,1}};

		struct OctreeNode {
			unsigned int pos[3];
			unsigned int cellSize;
			OctreeNode * parent;
			OctreeNode ** children;
			bool isLeaf;
		};

		class Octree {
		public:
			Octree(unsigned int sizeX, unsigned int sizeY, unsigned int sizeZ);
			~Octree();
						
			void AddNewLeaf(unsigned int xPos, unsigned int yPos, unsigned int zPos, unsigned int cellSize = 1, OctreeNode * node = NULL);
			void PrintStructure(OctreeNode * node = NULL, unsigned int level = 0);
		private:
			OctreeNode * CreateNewLeaf(unsigned int xPos, unsigned int yPos, unsigned int zPos, unsigned int cellSize, OctreeNode * parent);			
			OctreeNode * GetLeaf(unsigned int xPos, unsigned int yPos, unsigned int zPos, OctreeNode * node = NULL);
			OctreeNode * MakeBlankStructure(unsigned int xPos, unsigned int yPos, unsigned int zPos, unsigned int cellSize, OctreeNode * parent);						
			void CleanNodeRecursively(OctreeNode * &node);
			void SplitLeaf(OctreeNode * node);
			unsigned int GetLargest2ndPower(unsigned int value);
		private:
			OctreeNode * rootNode;
			unsigned int size[3];			
		};

		Octree::Octree(unsigned int sizeX, unsigned int sizeY, unsigned int sizeZ) {
			unsigned int cellSize = max(max(GetLargest2ndPower(sizeX), GetLargest2ndPower(sizeY)), GetLargest2ndPower(sizeZ));
			size[0] = sizeX;
			size[1] = sizeY;
			size[2] = sizeZ;
			rootNode = MakeBlankStructure(0, 0, 0, cellSize, NULL);
		}

		Octree::~Octree() {
			CleanNodeRecursively(rootNode);
		}

		OctreeNode * Octree::CreateNewLeaf(unsigned int xPos, unsigned int yPos, unsigned int zPos, unsigned int cellSize, OctreeNode * parent) {
			OctreeNode * leaf = new OctreeNode();
			leaf->pos[0] = xPos;
			leaf->pos[1] = yPos;
			leaf->pos[2] = zPos;
			leaf->cellSize = cellSize;
			leaf->children = NULL;
			leaf->isLeaf = true;
			leaf->parent = parent;
			return leaf;
		}

		OctreeNode * Octree::GetLeaf(unsigned int xPos, unsigned int yPos, unsigned int zPos, OctreeNode * node) {
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
				found = true;
				for(int j = 0; j < 3; j++) {
					found = found && (position[j] >= node->children[i]->pos[j]) && (position[j] < node->children[i]->pos[j] + node->children[i]->cellSize);
				}
				if(found) {
					return GetLeaf(xPos, yPos, zPos, node->children[i]);
				}
			}
			return NULL;
		}

		OctreeNode * Octree::MakeBlankStructure(unsigned int xPos, unsigned int yPos, unsigned int zPos, unsigned int cellSize, OctreeNode * parent) {
			if((xPos >= size[0]) || (yPos >= size[1]) || (zPos >= size[2])) {
				return NULL;
			}

			OctreeNode * node = CreateNewLeaf(xPos, yPos, zPos, cellSize, parent);
			if(cellSize == 1) {
				return node;
			}

			if((xPos + cellSize >= size[0]+1) || (yPos + cellSize >= size[1]+1) || (zPos + cellSize >= size[2]+1)) {
				node->isLeaf = false;
				node->children = new OctreeNode*[8];
				unsigned int childCellSize = cellSize / 2;
				for(int i = 0; i < 8; i++) {
					node->children[i] = MakeBlankStructure(xPos + octreeChildren[i][0] * childCellSize, yPos + octreeChildren[i][1] * childCellSize, zPos + octreeChildren[i][2] * childCellSize, childCellSize, node);					
				}
			}
			return node;
		}

		void Octree::AddNewLeaf(unsigned int xPos, unsigned int yPos, unsigned int zPos, unsigned int cellSize, OctreeNode * node) {
			OctreeNode * parent = GetLeaf(xPos, yPos, zPos, node);
			if(parent->cellSize > cellSize) {
				SplitLeaf(parent);
				AddNewLeaf(xPos, yPos, zPos, cellSize, parent);
			}
		}

		void Octree::CleanNodeRecursively(OctreeNode * &node) {
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

		void Octree::PrintStructure(OctreeNode * node, unsigned int level) {
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
		void Octree::SplitLeaf(OctreeNode * node) {
			if((!node->isLeaf) || (node->cellSize == 1)) {
				printf("Error! attempting to split an unsplittable node! \n");
				return;
			}

			node->isLeaf = false;
			unsigned int childCellSize = node->cellSize / 2;
			node->children = new OctreeNode*[8];
			for(int i = 0; i < 8; i++) {
				node->children[i] = CreateNewLeaf(node->pos[0] + octreeChildren[i][0] * childCellSize, node->pos[1] + octreeChildren[i][1] * childCellSize, node->pos[2] + octreeChildren[i][2] * childCellSize, childCellSize, node);					
			}
		}

		unsigned int Octree::GetLargest2ndPower(unsigned int value) {
			unsigned int pow = 1;
			while (pow < value) {
				pow *= 2;
			}
			return pow;
		}
		
	}
}

#endif