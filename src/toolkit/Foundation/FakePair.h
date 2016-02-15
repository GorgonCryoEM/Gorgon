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

    public:
        TKey first;
        TValue second;
    };

    template <class TKey, class TValue>
    FakePair<TKey, TValue>::FakePair(TKey key, TValue value) {
        first = key;
        second = value;
    }

    template <class TKey, class TValue>
    inline bool FakePair<TKey, TValue>::operator==(const FakePair<TKey, TValue> &other) const {
        return (first == other.first);
    }

    template <class TKey, class TValue>
    inline bool FakePair<TKey, TValue>::operator!=(const FakePair<TKey, TValue> &other) const {
        return (first != other.first);
    }

    template <class TKey, class TValue>
    inline bool FakePair<TKey, TValue>::operator>(const FakePair<TKey, TValue> &other) const {
        return (first > other.first);
    }

    template <class TKey, class TValue>
    inline bool FakePair<TKey, TValue>::operator<(const FakePair<TKey, TValue> &other) const {
        return (first < other.first);
    }

    template <class TKey, class TValue>
    inline bool FakePair<TKey, TValue>::operator>=(const FakePair<TKey, TValue> &other) const {
        return (first >= other.first);
    }

    template <class TKey, class TValue>
    inline bool FakePair<TKey, TValue>::operator<=(const FakePair<TKey, TValue> &other) const {
        return (first <= other.first);
    }
}

#endif
