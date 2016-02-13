#ifndef TOOLKIT_FOUNDATION_GORGONPRIORITYQUEUE_H
#define TOOLKIT_FOUNDATION_GORGONPRIORITYQUEUE_H


#include <queue>

namespace Foundation {
    template <class TKey, class TValue>
    class FakePair {
    public:
        FakePair(TKey key, TValue value);
        bool operator==(const FakePair<TKey, TValue> &other) const;
        bool operator!=(const FakePair<TKey, TValue> &other) const;
        bool operator>(const FakePair<TKey, TValue> &other) const;
        bool operator<(const FakePair<TKey, TValue> &other) const;
        bool operator>=(const FakePair<TKey, TValue> &other) const;
        bool operator<=(const FakePair<TKey, TValue> &other) const;

        TValue GetValue();
        TKey GetKey();
    private:
        TKey key;
        TValue value;
    };

    template <class TKey, class TValue>
    FakePair<TKey, TValue>::FakePair(TKey key, TValue value) {
        this->key = key;
        this->value = value;
    }

    template <class TKey, class TValue>
    inline bool FakePair<TKey, TValue>::operator==(const FakePair<TKey, TValue> &other) const {
        return (this->key == other.key);
    }

    template <class TKey, class TValue>
    inline bool FakePair<TKey, TValue>::operator!=(const FakePair<TKey, TValue> &other) const {
        return (this->key != other.key);
    }

    template <class TKey, class TValue>
    inline bool FakePair<TKey, TValue>::operator>(const FakePair<TKey, TValue> &other) const {
        return (this->key > other.key);
    }

    template <class TKey, class TValue>
    inline bool FakePair<TKey, TValue>::operator<(const FakePair<TKey, TValue> &other) const {
        return (this->key < other.key);
    }

    template <class TKey, class TValue>
    inline bool FakePair<TKey, TValue>::operator>=(const FakePair<TKey, TValue> &other) const {
        return (this->key >= other.key);
    }

    template <class TKey, class TValue>
    inline bool FakePair<TKey, TValue>::operator<=(const FakePair<TKey, TValue> &other) const {
        return (this->key <= other.key);
    }

    template <class TKey, class TValue>
    inline TValue FakePair<TKey, TValue>::GetValue() {
        return value;
    }

    template <class TKey, class TValue>
    inline TKey FakePair<TKey, TValue>::GetKey() {
        return key;
    }
}

#endif
