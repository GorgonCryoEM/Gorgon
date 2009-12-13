// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for storing a priority queue ADT

// CVS Meta Information: 
//   $Source$
//   $Revision$
//   $Date$
//   $Author$
//   $State$
//
// History Log: 
//   $Log$

#ifndef FOUNDATION_GORGONPRIORITYQUEUEELEMENT_H
#define FOUNDATION_GORGONPRIORITYQUEUEELEMENT_H


namespace wustl_mm {
	namespace Foundation {
		template <class TKey, class TValue>
		class GorgonPriorityQueueElement {
		public:
			GorgonPriorityQueueElement(TKey key, TValue value);
			bool operator==(const GorgonPriorityQueueElement<TKey, TValue> &other) const;
			bool operator!=(const GorgonPriorityQueueElement<TKey, TValue> &other) const;
			bool operator>(const GorgonPriorityQueueElement<TKey, TValue> &other) const;
			bool operator<(const GorgonPriorityQueueElement<TKey, TValue> &other) const;
			bool operator>=(const GorgonPriorityQueueElement<TKey, TValue> &other) const;
			bool operator<=(const GorgonPriorityQueueElement<TKey, TValue> &other) const;

			TValue GetValue();
		private:
			TKey key;
			TValue value;
		};

		template <class TKey, class TValue>
		GorgonPriorityQueueElement<TKey, TValue>::GorgonPriorityQueueElement(TKey key, TValue value) {
			this->key = key;
			this->value = value;
		}

		template <class TKey, class TValue>
		bool GorgonPriorityQueueElement<TKey, TValue>::operator==(const GorgonPriorityQueueElement<TKey, TValue> &other) const {
			return (this->key == other.key);
		}

		template <class TKey, class TValue>
		bool GorgonPriorityQueueElement<TKey, TValue>::operator!=(const GorgonPriorityQueueElement<TKey, TValue> &other) const {
			return (this->key != other.key);
		}

		template <class TKey, class TValue>
		bool GorgonPriorityQueueElement<TKey, TValue>::operator>(const GorgonPriorityQueueElement<TKey, TValue> &other) const {
			return (this->key > other.key);
		}

		template <class TKey, class TValue>
		bool GorgonPriorityQueueElement<TKey, TValue>::operator<(const GorgonPriorityQueueElement<TKey, TValue> &other) const {
			return (this->key < other.key);
		}

		template <class TKey, class TValue>
		bool GorgonPriorityQueueElement<TKey, TValue>::operator>=(const GorgonPriorityQueueElement<TKey, TValue> &other) const {
			return (this->key >= other.key);
		}

		template <class TKey, class TValue>
		bool GorgonPriorityQueueElement<TKey, TValue>::operator<=(const GorgonPriorityQueueElement<TKey, TValue> &other) const {
			return (this->key <= other.key);
		}

		template <class TKey, class TValue>
		TValue GorgonPriorityQueueElement<TKey, TValue>::GetValue() {
			return value;
		}
	}
}

#endif