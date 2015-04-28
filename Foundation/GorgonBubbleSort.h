// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   An abstract class for sorting

#ifndef FOUNDATION_GORGONBUBBLESORT_H
#define FOUNDATION_GORGONBUBBLESORT_H


#include <vector>
#include "GorgonSort.h"

using namespace std;

namespace wustl_mm {
	namespace Foundation { 

		template <class T>
		class GorgonBubbleSort : public GorgonSort<T> {
		public:
			static void Sort(vector<T> & values, bool ascending);
		};

		template <class T>
		void GorgonBubbleSort<T> :: Sort(vector<T> & values, bool ascending) {
			bool swapped = true;
			while (swapped) {
				swapped = false;
				for(unsigned int i = 0; i < values.size()-1; i++) {
					if((ascending && (values[i] > values[i+1])) || (!ascending && (values[i] < values[i+1]))) {
						Swap(values, i, i+1);
						swapped = true;
					}
				}
			}
		}

	}
}

#endif
