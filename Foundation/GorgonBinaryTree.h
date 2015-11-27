// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for storing a binary tree ADT


#ifndef FOUNDATION_GORGONBINARYTREE_H
#define FOUNDATION_GORGONBINARYTREE_H


#include "GorgonBinaryTreeNode.h"

namespace wustl_mm {
	namespace Foundation {

		const int GORGON_BINARY_TREE_LEFT_CHILD = 0;
		const int GORGON_BINARY_TREE_RIGHT_CHILD = 1;

		template <class T>
		class GorgonBinaryTree {
		public:
			GorgonBinaryTree();
			~GorgonBinaryTree();
			
			bool IsEmpty();
			virtual void AddValue(T value);
			virtual void Print();
			

		private:
			void Print(GorgonBinaryTreeNode<T> * node);
			void DeleteSubtree(GorgonBinaryTreeNode<T> * root);

		protected:
			GorgonBinaryTreeNode<T> * root;
		};

		template <class T>
		GorgonBinaryTree<T>::GorgonBinaryTree() {
			root = NULL;
		}

		template <class T>
		GorgonBinaryTree<T>::~GorgonBinaryTree() {
			DeleteSubtree(root);
			root = NULL;
		}

		template <class T>
		void GorgonBinaryTree<T>::DeleteSubtree(GorgonBinaryTreeNode<T> * root) {
			if(root != NULL) {
				DeleteSubtree(root->GetChild(GORGON_BINARY_TREE_LEFT_CHILD));
				DeleteSubtree(root->GetChild(GORGON_BINARY_TREE_RIGHT_CHILD));
				delete root;
			}
		}		

		template <class T>
		void GorgonBinaryTree<T>::AddValue(T value) {
			printf("Not implemented.  Please override \n");
		}

		template <class T>
		void GorgonBinaryTree<T>::Print() {
			Print(root);
			printf("\n");
		}

		template <class T>
		void GorgonBinaryTree<T>::Print(GorgonBinaryTreeNode<T> * node) {
			if(node != NULL) {
				Print(node->GetChild(GORGON_BINARY_TREE_LEFT_CHILD));
				printf("%d ", (int)node->GetValue());
				Print(node->GetChild(GORGON_BINARY_TREE_RIGHT_CHILD));
			}			
		}

		template <class T>
		bool GorgonBinaryTree<T>::IsEmpty() {
			return (root == NULL);
		}
	}
}

#endif
