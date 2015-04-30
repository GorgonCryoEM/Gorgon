// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for storing a queue ADT


#ifndef FOUNDATION_GORGONQUEUE_H
#define FOUNDATION_GORGONQUEUE_H


#include <vector>
using namespace std;

namespace wustl_mm {
	namespace Foundation {

		template <class T>
		class GorgonQueue {
		public:
			GorgonQueue();
			~GorgonQueue();
			void PushBack(T value);			
			T PopFront();
			T First();
			bool IsEmpty();		
			void Print();
		private:
			vector<T> values;
		};

		template <class T>
		GorgonQueue<T>::GorgonQueue() {
			values.clear();
		}

		template <class T>
		GorgonQueue<T>::~GorgonQueue() {
			values.clear();
		}

		template <class T>
		void GorgonQueue<T>::PushBack(T value) {
			values.push_back(value);
		}

		template <class T>
		T GorgonQueue<T>::PopFront() {
			T value = values[0];
			values.erase(values.begin());
			return value;
		}

		template <class T>
		T GorgonQueue<T>::First() {
			return values[0];
		}

		template <class T>
		bool GorgonQueue<T>::IsEmpty() {
			return (values.size() == 0);
		}

		template <class T>
		void GorgonQueue<T>::Print() {
			for(unsigned int i = 0; i < values.size(); i++) {
				printf("%f ", (double)values[i]);
			}
			printf("\n");
		}

	}
}

#endif
