// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for storing a binary tree ADT

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$

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

			virtual void AddValue(T value);

		private:
			void DeleteSubtree(GorgonBinaryTreeNode<T> * & root);

		protected:
			GorgonBinaryTreeNode<T> * root;
		};

		template <class T>
		GorgonBinaryTree<T>::GorgonBinaryTree() {
			root = NULL;
		}

		template <class T>
		GorgonBinaryTree<T>::~GorgonBinaryTree() {
			DeleteSubTree(root);

		}

		template <class T>
		void GorgonBinaryTree<T>::DeleteSubtree(GorgonBinaryTreeNode<T> * & root) {
			if(root != NULL) {
				DeleteSubTree(root->GetChild(GORGON_BINARY_TREE_LEFT_CHILD));
				DeleteSubTree(root->GetChild(GORGON_BINARY_TREE_RIGHT_CHILD));
				delete root;
			}
			root = NULL;
		}		

		template <class T>
		void GorgonBinaryTree<T>::AddValue(T value) {
			printf("Not implemented.  Please override \n");
		}

	}
}

#endif