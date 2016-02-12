#ifndef TOOLKIT_FOUNDATION_GORGONPRIORITYQUEUE_H
#define TOOLKIT_FOUNDATION_GORGONPRIORITYQUEUE_H


#include "GorgonPriorityQueueElement.h"
#include "GorgonHeap.h"

namespace Foundation {
    template <class TKey, class TValue>
    class PQueue {
    public:
        PQueue(bool maxIsHighestPriority);
        ~PQueue();

        void Add(TKey key, TValue value);
        TValue PopFirst();
        void PopFirst(TKey & key, TValue & value);
        TValue First();

        bool IsEmpty();

    private:
        GorgonHeap< GorgonPriorityQueueElement<TKey, TValue> > heap;
    };

    template <class TKey, class TValue>
    PQueue<TKey, TValue>::PQueue(bool maxIsHighestPriority) {
        heap = GorgonHeap< GorgonPriorityQueueElement<TKey, TValue> >(maxIsHighestPriority);
    }

    template <class TKey, class TValue>
    PQueue<TKey, TValue>::~PQueue() {

    }

    template <class TKey, class TValue>
    void PQueue<TKey, TValue>::Add(TKey key, TValue value) {
        heap.AddValue(GorgonPriorityQueueElement<TKey, TValue>(key, value));
    }

    template <class TKey, class TValue>
    TValue PQueue<TKey, TValue>::PopFirst() {
        return heap.PopRoot().GetValue();
    }

    template <class TKey, class TValue>
    void PQueue<TKey, TValue>::PopFirst(TKey & key, TValue & value) {
        GorgonPriorityQueueElement<TKey, TValue> e = heap.PopRoot();
        key = e.GetKey();
        value = e.GetValue();
    }


    template <class TKey, class TValue>
    TValue PQueue<TKey, TValue>::First() {
        return heap.Root().GetValue();
    }

    template <class TKey, class TValue>
    bool PQueue<TKey, TValue>::IsEmpty() {
        return heap.IsEmpty();
    }

}

#endif
