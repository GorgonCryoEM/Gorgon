// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   An abstract class for merge sorting

#ifndef FOUNDATION_GORGONMERGESORT_H
#define FOUNDATION_GORGONMERGESORT_H


#include <vector>
#include "GorgonSort.h"

using namespace std;

namespace wustl_mm {
	namespace Foundation { 

		template <class T>
		class GorgonMergeSort : public GorgonSort<T> {
		public:
			static void Sort(vector<T> & values, bool ascending);
		private:
			static void MergeSort(vector<T> & values, int start, int size, bool ascending);
			static void Merge(vector<T> & values, int start1, int size1, int start2, int size2, bool ascending);
		};

		template <class T>
		void GorgonMergeSort<T>::Sort(vector<T> & values, bool ascending) {
			MergeSort(values, 0, values.size(), ascending);			
		}

		template <class T>
		void GorgonMergeSort<T>::MergeSort(vector<T> & values, int start, int size, bool ascending) {
			if(size > 1) {
				int leftSize = size / 2;
				int rightSize = size - leftSize;
				
				MergeSort(values, start, leftSize, ascending);
				MergeSort(values, start+leftSize, rightSize, ascending);

				Merge(values, start, leftSize, start+leftSize, rightSize, ascending);
			}
		}

		template <class T>
		void GorgonMergeSort<T>::Merge(vector<T> & values, int start1, int size1, int start2, int size2, bool ascending) {
			vector<T> merged;
			merged.clear();

			int index1 = 0;
			int index2 = 0;

			while((index1 < size1) && (index2 < size2)) {
				if((ascending && (values[start1+index1] < values[start2+index2])) || (!ascending && (values[start1+index1] > values[start2+index2]))) {
					merged.push_back(values[start1+index1]);
					index1++;
				} else {
					merged.push_back(values[start2+index2]);
					index2++;
				}
			}
			for(unsigned int i = index1; i < size1; i++) {
				merged.push_back(values[start1+i]);
			}
			for(unsigned int i = index2; i < size2; i++) {
				merged.push_back(values[start2+i]);
			}
			for(unsigned int i = 0; i < size1+size2; i++) {
				values[start1+i] = merged[i];
			}
			merged.clear();			
		}


	}
}

#endif
