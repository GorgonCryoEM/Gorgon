// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for storing a stack ADT

// CVS Meta Information: 
//   $Source: /project/mm/cvs/graphics/ssa1/source/Foundation/GorgonStack.h,v $
//   $Revision: 1.1 $
//   $Date: 2009/12/13 19:38:37 $
//   $Author: ssa1 $
//   $State: Exp $
//
// History Log: 
//   $Log: GorgonStack.h,v $
//   Revision 1.1  2009/12/13 19:38:37  ssa1
//   Adding in abstract data structures
//

#ifndef FOUNDATION_GORGONSTACK_H
#define FOUNDATION_GORGONSTACK_H


#include <vector>

using namespace std;

namespace wustl_mm {
	namespace Foundation {

		template <class T>
		class GorgonStack {
		public:
			GorgonStack();
			~GorgonStack();
			void Push(T value);
			T Pop();
			T First();		
			bool IsEmpty();

			void Print();
		private:
			vector<T> values;
		};

		template <class T>
		GorgonStack<T>::GorgonStack() {
			values.clear();
		}

		template <class T>
		GorgonStack<T>::~GorgonStack() {
			values.clear();
		}
		template <class T>
		void GorgonStack<T>::Push(T value) {
			values.push_back(value);
		}

		template <class T>
		T GorgonStack<T>::Pop() {
			T value = values[values.size()-1];
			values.erase(values.begin() + values.size() - 1);
			return value;			
		}

		template <class T>
		T GorgonStack<T>::First() {
			return values[values.size() -1];
		}

		template <class T>
		bool GorgonStack<T>::IsEmpty() {
			return (values.size() == 0);
		}

		template <class T>
		void GorgonStack<T>::Print() {
			for(unsigned int i = 0; i < values.size(); i++){ 
				printf("%f ", (double)values[i]);
			}
			printf("\n");
		}
	}
}

#endif