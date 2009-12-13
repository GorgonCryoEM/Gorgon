// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for storing a binary tree node

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$

#ifndef FOUNDATION_GORGONBINARYTREENODE_H
#define FOUNDATION_GORGONBINARYTREENODE_H

namespace wustl_mm {
	namespace Foundation {
		template <class T>

		class GorgonBinaryTreeNode {
		public:
			GorgonBinaryTreeNode(T value);
			GorgonBinaryTreeNode(T value, GorgonBinaryTreeNode<T> * lChild, GorgonBinaryTreeNode<T> * rChild);
			~GorgonBinaryTreeNode();

			T GetValue();
			GorgonBinaryTreeNode<T> * GetChild(int childIndex);
			bool HasChild(int childIndex);
			void AddChild(int childIndex, T childValue);
			void AddChild(int childIndex, GorgonBinaryTreeNode<T> * child);

		private:
			T value;
			GorgonBinaryTreeNode<T> * children[2];
		};

		template <class T>
		GorgonBinaryTreeNode<T>::GorgonBinaryTreeNode(T value) {
			this->value = value;
			this->children[0] = NULL;
			this->children[1] = NULL;
		}

		template <class T>
		GorgonBinaryTreeNode<T>::GorgonBinaryTreeNode(T value, GorgonBinaryTreeNode<T> * lChild, GorgonBinaryTreeNode<T> * rChild) {
			this->value = value;
			this->children[0] = lChild;
			this->children[1] = rChild;
		}

		template <class T>
		GorgonBinaryTreeNode<T>::~GorgonBinaryTreeNode() {
			for(unsigned int i = 0; i < 2; i++) {
				children[i] = NULL;
			}
		}

		template <class T>
		T GorgonBinaryTreeNode<T>::GetValue() {
			return value;
		}

		template <class T>
		GorgonBinaryTreeNode<T> * GorgonBinaryTreeNode<T>::GetChild(int childIndex) {
			return children[childIndex];
		}

		template <class T>
		bool GorgonBinaryTreeNode<T>::HasChild(int childIndex) {
			return (children[childIndex] != NULL);
		}

		template <class T>
		void GorgonBinaryTreeNode<T>::AddChild(int childIndex, T childValue) {
			GorgonBinaryTreeNode<T> * child = new GorgonBinaryTreeNode<T>(childValue);
			children[childIndex] = child;
		}

		template <class T>
		void GorgonBinaryTreeNode<T>::AddChild(int childIndex, GorgonBinaryTreeNode<T> * child) {
			children[childIndex] child;
		}

	}
}

#endif