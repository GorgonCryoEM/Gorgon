#ifndef TOOLKIT_FOUNDATION_GORGONPRIORITYQUEUE_H
#define TOOLKIT_FOUNDATION_GORGONPRIORITYQUEUE_H


#include <queue>

namespace Foundation {
    template <class T, class U>
    class FakePair {
    public:
        FakePair(T key, U value);
    public:
        T first;
        U second;
    };

    template <class T, class U>
    FakePair<T, U>::FakePair(T key, U value)
        : first(key), second(value)
    {}

    template <class T, class U>
    inline bool operator==(const FakePair<T,U>& lhs, const FakePair<T, U> &rhs) {
        return (lhs.first == rhs.first);
    }

    template <class T, class U>
    inline bool operator!=(const FakePair<T,U>& lhs, const FakePair<T, U> &rhs) {
        return (lhs.first != rhs.first);
    }

    template <class T, class U>
    inline bool operator>(const FakePair<T,U>& lhs, const FakePair<T, U> &rhs) {
        return (lhs.first > rhs.first);
    }

    template <class T, class U>
    inline bool operator<(const FakePair<T,U>& lhs, const FakePair<T, U> &rhs) {
        return (lhs.first < rhs.first);
    }

    template <class T, class U>
    inline bool operator>=(const FakePair<T,U>& lhs, const FakePair<T, U> &rhs) {
        return (lhs.first >= rhs.first);
    }

    template <class T, class U>
    inline bool operator<=(const FakePair<T,U>& lhs, const FakePair<T, U> &rhs) {
        return (lhs.first <= rhs.first);
    }
}

#endif
