#ifndef TOOLKIT_FOUNDATION_GORGONPRIORITYQUEUE_H
#define TOOLKIT_FOUNDATION_GORGONPRIORITYQUEUE_H


#include <queue>

namespace Foundation {
    template <class TKey, class TValue>
    class PQueueElem {
    public:
        PQueueElem(TKey key, TValue value);
        bool operator==(const PQueueElem<TKey, TValue> &other) const;
        bool operator!=(const PQueueElem<TKey, TValue> &other) const;
        bool operator>(const PQueueElem<TKey, TValue> &other) const;
        bool operator<(const PQueueElem<TKey, TValue> &other) const;
        bool operator>=(const PQueueElem<TKey, TValue> &other) const;
        bool operator<=(const PQueueElem<TKey, TValue> &other) const;

        TValue GetValue();
        TKey GetKey();
    private:
        TKey key;
        TValue value;
    };

    template <class TKey, class TValue>
    PQueueElem<TKey, TValue>::PQueueElem(TKey key, TValue value) {
        this->key = key;
        this->value = value;
    }

    template <class TKey, class TValue>
    inline bool PQueueElem<TKey, TValue>::operator==(const PQueueElem<TKey, TValue> &other) const {
        return (this->key == other.key);
    }

    template <class TKey, class TValue>
    inline bool PQueueElem<TKey, TValue>::operator!=(const PQueueElem<TKey, TValue> &other) const {
        return (this->key != other.key);
    }

    template <class TKey, class TValue>
    inline bool PQueueElem<TKey, TValue>::operator>(const PQueueElem<TKey, TValue> &other) const {
        return (this->key > other.key);
    }

    template <class TKey, class TValue>
    inline bool PQueueElem<TKey, TValue>::operator<(const PQueueElem<TKey, TValue> &other) const {
        return (this->key < other.key);
    }

    template <class TKey, class TValue>
    inline bool PQueueElem<TKey, TValue>::operator>=(const PQueueElem<TKey, TValue> &other) const {
        return (this->key >= other.key);
    }

    template <class TKey, class TValue>
    inline bool PQueueElem<TKey, TValue>::operator<=(const PQueueElem<TKey, TValue> &other) const {
        return (this->key <= other.key);
    }

    template <class TKey, class TValue>
    inline TValue PQueueElem<TKey, TValue>::GetValue() {
        return value;
    }

    template <class TKey, class TValue>
    inline TKey PQueueElem<TKey, TValue>::GetKey() {
        return key;
    }


    template <class TKey, class TValue>
    class PQueue {
    public:
        void Add(TKey key, TValue value);
        TValue PopFirst();
        void PopFirst(TKey & key, TValue & value);

        bool IsEmpty();

    private:
        typedef PQueueElem<TKey, TValue> Elem;

        priority_queue<Elem> q;
    };

    template <class TKey, class TValue>
    void PQueue<TKey, TValue>::Add(TKey key, TValue value) {
        q.push(Elem(key, value));
    }

    template <class TKey, class TValue>
    TValue PQueue<TKey, TValue>::PopFirst() {
            Elem res = q.top();
            q.pop();
            return res.GetValue();
    }

    template <class TKey, class TValue>
    void PQueue<TKey, TValue>::PopFirst(TKey & key, TValue & value) {
        Elem res = q.top();
        q.pop();
        key = res.GetKey();
        value = res.GetValue();
    }

    template <class TKey, class TValue>
    bool PQueue<TKey, TValue>::IsEmpty() {
            return q.empty();
    }

}

#endif
