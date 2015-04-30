// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   An abstract class for a binary search tree

#ifndef FOUNDATION_GORGONBINARYSEARCHTREE_H
#define FOUNDATION_GORGONBINARYSEARCHTREE_H


#include "GorgonBinaryTree.h"
#include "GorgonBinaryTreeNode.h"

namespace wustl_mm {
	namespace Foundation { 

		template <class T>
		class GorgonBinarySearchTree : public GorgonBinaryTree<T> {
		public:
			GorgonBinarySearchTree();
			~GorgonBinarySearchTree();
			virtual void AddValue(T value);
			virtual void RemoveValue(T value);
			bool IsElementPresent(T searchQuery);
			T GetElement(T searchQuery);
		private:
			bool IsElementPresent(T searchQuery, GorgonBinaryTreeNode<T> * node);
			T GetElement(T searchQuery, GorgonBinaryTreeNode<T> * node);
			GorgonBinaryTreeNode<T> * GetNode(T searchQuery, GorgonBinaryTreeNode<T> * node);
			void AddValue(T value, GorgonBinaryTreeNode<T> * node);
			void RemoveNode(GorgonBinaryTreeNode<T> * node);
			GorgonBinaryTreeNode<T> * GetLargestElement(GorgonBinaryTreeNode<T> * node);

		};

		template <class T>
		GorgonBinarySearchTree<T>::GorgonBinarySearchTree() {
		}

		template <class T>
		GorgonBinarySearchTree<T>::~GorgonBinarySearchTree() {
		}

		template <class T>
		bool GorgonBinarySearchTree<T>::IsElementPresent(T searchQuery) {
			return IsElementPresent(T, root);
		}

		template <class T>
		bool GorgonBinarySearchTree<T>::IsElementPresent(T searchQuery, GorgonBinaryTreeNode<T> * node) {			
			if(node == NULL) {
				return false; 
			} else {
				T nodeValue = node->GetValue();
				if(nodeValue == searchQuery) {
					return true;
				} else if(nodeValue < searchQuery) {
					return IsElementPresent(searchQuery, node->GetChild(GORGON_BINARY_TREE_RIGHT_CHILD));
				} else {
					return IsElementPresent(searchQuery, node->GetChild(GORGON_BINARY_TREE_LEFT_CHILD));
				}
			}
		}


		template <class T>
		T GorgonBinarySearchTree<T>::GetElement(T searchQuery) {
			return GetElement(searchQuery, root);
		}

		template <class T>
		T GorgonBinarySearchTree<T>::GetElement(T searchQuery, GorgonBinaryTreeNode<T> * node) {			
			if(node == NULL) {
				return NULL; 
			} else {
				T nodeValue = node->GetValue();
				if(nodeValue == searchQuery) {
					return nodeValue;
				} else if(nodeValue < searchQuery) {
					return GetElement(searchQuery, node->GetChild(GORGON_BINARY_TREE_RIGHT_CHILD));
				} else {
					return GetElement(searchQuery, node->GetChild(GORGON_BINARY_TREE_LEFT_CHILD));
				}
			}

		}

		template <class T>
		GorgonBinaryTreeNode<T> * GorgonBinarySearchTree<T>::GetNode(T searchQuery, GorgonBinaryTreeNode<T> * node) {			
			if(node == NULL) {
				return NULL; 
			} else {
				T nodeValue = node->GetValue();
				if(nodeValue == searchQuery) {
					return node;
				} else if(nodeValue < searchQuery) {
					return GetNode(searchQuery, node->GetChild(GORGON_BINARY_TREE_RIGHT_CHILD));
				} else {
					return GetNode(searchQuery, node->GetChild(GORGON_BINARY_TREE_LEFT_CHILD));
				}
			}

		}

		template <class T>
		void GorgonBinarySearchTree<T>::AddValue(T value) {
			if(root == NULL) {
				root = new GorgonBinaryTreeNode<T>(value, NULL);
			} else {
				AddValue(value, root);
			}
		}


		template <class T>
		void GorgonBinarySearchTree<T>::AddValue(T value, GorgonBinaryTreeNode<T> * node) {
			T nodeValue = node->GetValue();
			if(value <= nodeValue) {
				if(node->HasChild(GORGON_BINARY_TREE_LEFT_CHILD)) {
					AddValue(value, node->GetChild(GORGON_BINARY_TREE_LEFT_CHILD));
				} else {
					node->AddChild(GORGON_BINARY_TREE_LEFT_CHILD, value);
				}
			} else {
				if(node->HasChild(GORGON_BINARY_TREE_RIGHT_CHILD)) {
					AddValue(value, node->GetChild(GORGON_BINARY_TREE_RIGHT_CHILD));
				} else {
					node->AddChild(GORGON_BINARY_TREE_RIGHT_CHILD, value);
				}
			}			
		}

		template <class T>
		void GorgonBinarySearchTree<T>::RemoveValue(T value) {
			RemoveNode(GetNode(value, root));
		}

		template <class T>
		void GorgonBinarySearchTree<T>::RemoveNode(GorgonBinaryTreeNode<T> * node) {
			GorgonBinaryTreeNode<T> * parent = node->GetParent();
			GorgonBinaryTreeNode<T> * child;
			GorgonBinaryTreeNode<T> * pred;
			int childIndex;

			if(node->HasChild(GORGON_BINARY_TREE_LEFT_CHILD) && node->HasChild(GORGON_BINARY_TREE_RIGHT_CHILD)) {
				pred = GetLargestElement(node->GetChild(GORGON_BINARY_TREE_LEFT_CHILD));
				node->SetValue(pred->GetValue());
				parent = pred->GetParent();
				if(parent != NULL) {
					childIndex = (pred->GetValue() <= parent->GetValue()) ? GORGON_BINARY_TREE_LEFT_CHILD : GORGON_BINARY_TREE_RIGHT_CHILD;
					parent->SetChild(childIndex, NULL);
				} else {
					root = pred;
				}
				RemoveNode(pred);


			} else if (node->HasChild(GORGON_BINARY_TREE_LEFT_CHILD)) {
				child = node->GetChild(GORGON_BINARY_TREE_LEFT_CHILD);
				child->SetParent(parent);
				if(parent != NULL) {
					childIndex = (node->GetValue() <= parent->GetValue()) ? GORGON_BINARY_TREE_LEFT_CHILD : GORGON_BINARY_TREE_RIGHT_CHILD;
					parent->SetChild(childIndex, child);
				} else {
					root = child;
				}
				delete node;

			} else if (node->HasChild(GORGON_BINARY_TREE_RIGHT_CHILD)) {
				child = node->GetChild(GORGON_BINARY_TREE_RIGHT_CHILD);
				child->SetParent(parent);
				if(parent != NULL) {
					childIndex = (node->GetValue() <= parent->GetValue()) ? GORGON_BINARY_TREE_LEFT_CHILD : GORGON_BINARY_TREE_RIGHT_CHILD;
					parent->SetChild(childIndex, child);
				} else {
					root = child;
				}
				delete node;

			} else {
				if(parent != NULL) {
					childIndex = (node->GetValue() <= parent->GetValue()) ? GORGON_BINARY_TREE_LEFT_CHILD : GORGON_BINARY_TREE_RIGHT_CHILD;
					parent->SetChild(childIndex, NULL);
				} else {
					root = NULL;
				}
				delete node;				
			}
			
		}

		template <class T>
		GorgonBinaryTreeNode<T> * GorgonBinarySearchTree<T>::GetLargestElement(GorgonBinaryTreeNode<T> * node) {
			if(node->HasChild(GORGON_BINARY_TREE_RIGHT_CHILD)) {
				return GetLargestElement(node->GetChild(GORGON_BINARY_TREE_RIGHT_CHILD));
			} else {
				return node;
			}
		}
	}
}

#endif
