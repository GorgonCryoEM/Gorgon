// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for storing a Heap ADT


#ifndef FOUNDATION_GORGONHEAP_H
#define FOUNDATION_GORGONHEAP_H

#include <vector>

using namespace std;

namespace wustl_mm {
	namespace Foundation {

		template <class T>
		class GorgonHeap {
		public:
			GorgonHeap();
			GorgonHeap(bool isMaxHeap);
			~GorgonHeap();

			void BuildHeap(vector<T> & values);
			void AddValue(T value);
			T Root();
			T PopRoot();
			bool IsEmpty();

			void Print();

		private:
			unsigned int GetParent(unsigned int index);
			unsigned int GetLeft (unsigned int index);
			unsigned int GetRight(unsigned int index);
			void Heapify(int rootIndex);

			vector<T> values;
			bool isMaxHeap;

		};


		template <class T>
		GorgonHeap<T>::GorgonHeap() {
			this->isMaxHeap = true;
			values.clear();
		}

		template <class T>
		GorgonHeap<T>::GorgonHeap(bool isMaxHeap) {
			this->isMaxHeap = isMaxHeap;
			values.clear();
		}

		template <class T>
		GorgonHeap<T>::~GorgonHeap() {
			values.clear();
		}

		template <class T>
		inline unsigned int GorgonHeap<T>::GetParent(unsigned int index) {
			return index >> 1;
		}

		template <class T>
		inline unsigned int GorgonHeap<T>::GetLeft(unsigned int index) {
			return (index==0)? 1 :(index << 1);
		}

		template <class T>
		inline unsigned int GorgonHeap<T>::GetRight(unsigned int index) {
			return (index==0)? 2 : ((index << 1) | 1);
		}

		template <class T>
		void GorgonHeap<T>::Heapify(int rootIndex) {
			unsigned int size = values.size();			

			if(rootIndex < size) {
				unsigned int left = GetLeft(rootIndex);
				unsigned int right = GetRight(rootIndex);
				unsigned int properRoot;

				properRoot = rootIndex;
				if((left < size) && ((isMaxHeap && (values[left] > values[properRoot])) || (!isMaxHeap && (values[left] < values[properRoot])))) {
					properRoot = left;
				}

				if((right < size) && ((isMaxHeap && (values[right] > values[properRoot])) || (!isMaxHeap && (values[right] < values[properRoot])))) {
					properRoot = right;
				}

				if(properRoot != rootIndex) {
					T tempValue = values[properRoot];
					values[properRoot] = values[rootIndex];
					values[rootIndex] = tempValue;					

					Heapify(properRoot);
				}
			}
		}


		template <class T>
		void GorgonHeap<T>::AddValue(T value) {
			unsigned int childIndex = values.size();
			values.push_back(value);			

			bool swapped = true;
			unsigned int parentIndex;
			while((childIndex > 0) && swapped) {
				swapped = false;
				parentIndex = GetParent(childIndex);
				if((isMaxHeap && ((values[childIndex] > values[parentIndex]))) || (!isMaxHeap && (values[childIndex] < values[parentIndex]))) {
					T tempValue = values[childIndex];
					values[childIndex] = values[parentIndex];
					values[parentIndex] = tempValue;		
					swapped = true;
					childIndex = parentIndex;
				}
			}
		}


		template <class T>
		T GorgonHeap<T>::Root() {
			return values[0];
		}

		template <class T>
		bool GorgonHeap<T>::IsEmpty() {
			return (values.size() == 0);
		}

		template <class T>
		T GorgonHeap<T>::PopRoot() {
			T val = values[0];

			unsigned int lastElem = values.size()-1;
			values[0] = values[lastElem];
			values.erase(values.begin() + lastElem);
			Heapify(0);
			return val;
		}

		template <class T>
		void GorgonHeap<T>::Print() {
			for(unsigned int i = 0; i < values.size(); i++) {
				printf("%d ", (int)values[i]);
			}
			printf("\n");
		}

		template <class T>
		void GorgonHeap<T>::BuildHeap(vector<T> & values) {
			this->values.clear();
			this->values = values;
			for(int i = (int)values.size() / 2; i >= 0; i--) {
				Heapify(i);
			}


		}
	}
}

#endif
