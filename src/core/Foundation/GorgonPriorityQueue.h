#ifndef CORE_FOUNDATION_GORGONPRIORITYQUEUE_H
#define CORE_FOUNDATION_GORGONPRIORITYQUEUE_H


#include "GorgonPriorityQueueElement.h"
#include "GorgonHeap.h"

namespace Foundation {
    template <class TKey, class TValue>
    class GorgonPriorityQueue {
    public:
        GorgonPriorityQueue(bool maxIsHighestPriority);
        ~GorgonPriorityQueue();

        void Add(TKey key, TValue value);
        TValue PopFirst();
        void PopFirst(TKey & key, TValue & value);
        TValue First();

        bool IsEmpty();

    private:
        GorgonHeap< GorgonPriorityQueueElement<TKey, TValue> > heap;
    };

    template <class TKey, class TValue>
    GorgonPriorityQueue<TKey, TValue>::GorgonPriorityQueue(bool maxIsHighestPriority) {
        heap = GorgonHeap< GorgonPriorityQueueElement<TKey, TValue> >(maxIsHighestPriority);
    }

    template <class TKey, class TValue>
    GorgonPriorityQueue<TKey, TValue>::~GorgonPriorityQueue() {

    }

    template <class TKey, class TValue>
    void GorgonPriorityQueue<TKey, TValue>::Add(TKey key, TValue value) {
        heap.AddValue(GorgonPriorityQueueElement<TKey, TValue>(key, value));
    }

    template <class TKey, class TValue>
    TValue GorgonPriorityQueue<TKey, TValue>::PopFirst() {
        return heap.PopRoot().GetValue();
    }

    template <class TKey, class TValue>
    void GorgonPriorityQueue<TKey, TValue>::PopFirst(TKey & key, TValue & value) {
        GorgonPriorityQueueElement<TKey, TValue> e = heap.PopRoot();
        key = e.GetKey();
        value = e.GetValue();
    }


    template <class TKey, class TValue>
    TValue GorgonPriorityQueue<TKey, TValue>::First() {
        return heap.Root().GetValue();
    }

    template <class TKey, class TValue>
    bool GorgonPriorityQueue<TKey, TValue>::IsEmpty() {
        return heap.IsEmpty();
    }

}

#endif
