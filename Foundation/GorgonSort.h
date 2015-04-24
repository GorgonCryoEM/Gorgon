// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   An abstract class for sorting
// CVS Meta Information: 
//   $Source: /project/mm/cvs/graphics/ssa1/source/Foundation/GorgonSort.h,v $
//   $Revision: 1.1 $
//   $Date: 2009/12/13 19:38:37 $
//   $Author: ssa1 $
//   $State: Exp $
//
// History Log: 
//   $Log: GorgonSort.h,v $
//   Revision 1.1  2009/12/13 19:38:37  ssa1
//   Adding in abstract data structures
//

#ifndef FOUNDATION_GORGONSORT_H
#define FOUNDATION_GORGONSORT_H


#include <vector>

using namespace std;

namespace wustl_mm {
	namespace Foundation { 

		template <class T>
		class GorgonSort {
		public:
			static void Sort(vector<T> & values, bool ascending);
		protected:
			static void Swap(vector<T> & values, unsigned int index1, unsigned int index2);

		};

		template <class T>
		void GorgonSort<T>::Sort(vector<T> & values, bool ascending) {
			printf("Not implemented \n");
		}

		template <class T>
		void GorgonSort<T>::Swap(vector<T> & values, unsigned int index1, unsigned int index2) {
			T temp = values[index1];
			values[index1] = values[index2];
			values[index2] = temp;
		}

	}
}

#endif