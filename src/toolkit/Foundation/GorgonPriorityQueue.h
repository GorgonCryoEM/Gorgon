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
        GHeap< PQueueElem<TKey, TValue> > heap;
    };

    template <class TKey, class TValue>
    PQueue<TKey, TValue>::PQueue(bool maxIsHighestPriority) {
        heap = GHeap< PQueueElem<TKey, TValue> >(maxIsHighestPriority);
    }

    template <class TKey, class TValue>
    PQueue<TKey, TValue>::~PQueue() {

    }

    template <class TKey, class TValue>
    void PQueue<TKey, TValue>::Add(TKey key, TValue value) {
        heap.AddValue(PQueueElem<TKey, TValue>(key, value));
    }

    template <class TKey, class TValue>
    TValue PQueue<TKey, TValue>::PopFirst() {
        return heap.PopRoot().GetValue();
    }

    template <class TKey, class TValue>
    void PQueue<TKey, TValue>::PopFirst(TKey & key, TValue & value) {
        PQueueElem<TKey, TValue> e = heap.PopRoot();
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
