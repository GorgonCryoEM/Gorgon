// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   An abstract class for sorting

#ifndef FOUNDATION_GORGONQUICKSORT_H
#define FOUNDATION_GORGONQUICKSORT_H


#include <vector>
#include "GorgonSort.h"

using namespace std;

namespace wustl_mm {
	namespace Foundation { 

		template <class T>
		class GorgonQuickSort : public GorgonSort<T> {
		public:
			static void Sort(vector<T> & values, bool ascending);
		private:
			static void QuickSort(vector<T> & values, int start, int end, bool ascending);
			static int Partition(vector<T> & values, int start, int end, bool ascending);
		};

		template <class T>
		void GorgonQuickSort<T>::Sort(vector<T> & values, bool ascending) {
			QuickSort(values, 0, values.size()-1, ascending);
			
		}

		template <class T>
		void GorgonQuickSort<T>::QuickSort(vector<T> & values, int start, int end, bool ascending) {
			if(start < end) {
				int partitionIndex = Partition(values, start, end, ascending);
				QuickSort(values, start, partitionIndex, ascending);
				QuickSort(values, partitionIndex + 1, end, ascending);
			}
		}

		template <class T>
		int GorgonQuickSort<T>::Partition(vector<T> & values, int start, int end, bool ascending) {
			int pivot = values[start];
			int i = start-1;
			int j = end+1;
			while(i < j) {				
				do {
					i++;
				} while ((ascending && (values[i] < pivot)) || (!ascending && (values[i] > pivot)));

				do {
					j--;
				} while ((ascending && (values[j] > pivot)) || (!ascending && (values[j] < pivot)));

				if(i < j) {
					Swap(values, i, j);
				}
			}
			return j;
		}
	}
}

#endif
