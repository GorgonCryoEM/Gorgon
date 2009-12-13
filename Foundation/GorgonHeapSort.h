// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   An abstract class for heap sorting
// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$

#ifndef FOUNDATION_GORGONBUBBLESORT_H
#define FOUNDATION_GORGONBUBBLESORT_H


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
		};

		template <class T>
		void GorgonHeapSort<T>::Sort(vector<T> & values, bool ascending) {
			GorgonHeap<T> heap = GorgonHeap<T>(!ascending);
			for(unsigned int i = 0; i < values.size(); i++) {
				heap.AddValue(values[i]);
			}
			values.clear();
			while(!heap.IsEmpty()) {
				values.push_back(heap.PopRoot());
			}
		}
	}
}

#endif