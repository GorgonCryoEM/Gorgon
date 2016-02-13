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
}

#endif
