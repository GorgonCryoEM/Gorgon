#ifndef TOOLKIT_FOUNDATION_GORGONPRIORITYQUEUE_H
#define TOOLKIT_FOUNDATION_GORGONPRIORITYQUEUE_H


#include <queue>

namespace Foundation {
    template <class T, class U>
    class FakePair {
    public:
        FakePair(T key, U value);
        bool operator==(const FakePair<T, U> &other) const;
        bool operator!=(const FakePair<T, U> &other) const;
        bool operator>(const FakePair<T, U> &other) const;
        bool operator<(const FakePair<T, U> &other) const;
        bool operator>=(const FakePair<T, U> &other) const;
        bool operator<=(const FakePair<T, U> &other) const;

    public:
        T first;
        U second;
    };

    template <class T, class U>
    FakePair<T, U>::FakePair(T key, U value) {
        first = key;
        second = value;
    }

    template <class T, class U>
    inline bool FakePair<T, U>::operator==(const FakePair<T, U> &other) const {
        return (first == other.first);
    }

    template <class T, class U>
    inline bool FakePair<T, U>::operator!=(const FakePair<T, U> &other) const {
        return (first != other.first);
    }

    template <class T, class U>
    inline bool FakePair<T, U>::operator>(const FakePair<T, U> &other) const {
        return (first > other.first);
    }

    template <class T, class U>
    inline bool FakePair<T, U>::operator<(const FakePair<T, U> &other) const {
        return (first < other.first);
    }

    template <class T, class U>
    inline bool FakePair<T, U>::operator>=(const FakePair<T, U> &other) const {
        return (first >= other.first);
    }

    template <class T, class U>
    inline bool FakePair<T, U>::operator<=(const FakePair<T, U> &other) const {
        return (first <= other.first);
    }
}

#endif
