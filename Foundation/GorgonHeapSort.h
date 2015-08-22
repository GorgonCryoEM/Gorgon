// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   An abstract class for heap sorting

#ifndef FOUNDATION_GORGONHEAPSORT_H
#define FOUNDATION_GORGONHEAPSORT_H


#include <vector>
#include "GorgonSort.h"
#include "GorgonHeap.h"

using namespace std;

namespace wustl_mm {
	namespace Foundation { 

		template <class T>
		class GorgonHeapSort : public GorgonSort<T> {
		public:
			static void Sort(vector<T> & values, bool ascending);
		private:
			static void Heapify(vector<T> & values, int size, bool ascending);
			static void BubbleDown(vector<T> & values, int start, int end, bool ascending);
			static unsigned int GetLeftChild(int parent);
			static unsigned int GetRightChild(int parent);
			static unsigned int GetParent(int parent);
		};

		template <class T>
		void GorgonHeapSort<T>::Sort(vector<T> & values, bool ascending) {
			Heapify(values, values.size(), ascending);
			int lastElem = values.size()-1;
			while (lastElem > 0) {
				Swap(values, 0, lastElem);
				lastElem--;
				BubbleDown(values, 0, lastElem, ascending);				
			}
		}

		template <class T>
		void GorgonHeapSort<T>::Heapify(vector<T> & values, int size, bool ascending) {
			unsigned int lastParent = GetParent(size-1);
			for(int i = (int)lastParent; i >= 0; i--) {
				BubbleDown(values, i, size-1, ascending);
			}
		}

		template <class T>
		void GorgonHeapSort<T>::BubbleDown(vector<T> & values, int start, int end, bool ascending) {
			unsigned int lChild = GetLeftChild(start);
			unsigned int rChild = GetRightChild(start);
			unsigned int correctParent = start;
			if((lChild <= end) && 
					((ascending && (values[correctParent] < values[lChild])) || 
					(!ascending && (values[correctParent] > values[lChild])))) {
				correctParent = lChild;	
			}

			if((rChild <= end) && 
					((ascending && (values[correctParent] < values[rChild])) || 
					(!ascending && (values[correctParent] > values[rChild])))) {
				correctParent = rChild;	
			}

			if(correctParent != start) {
				Swap(values, start, correctParent);
				BubbleDown(values, correctParent, end, ascending);
			}
		}

		template <class T>
		inline unsigned int GorgonHeapSort<T>::GetLeftChild(int parent) {
			return (parent == 0)? 1: (parent << 1);
		}

		template <class T>
		inline unsigned int GorgonHeapSort<T>::GetRightChild(int parent){
			return (parent == 0)? 2: ((parent << 1) | 1);
		}

		template <class T>
		inline unsigned int GorgonHeapSort<T>::GetParent(int parent){
			return (parent >> 1);
		}
	}
}

#endif
