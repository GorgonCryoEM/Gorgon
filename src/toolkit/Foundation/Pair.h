/*
 * Pair.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_FOUNDATION_PAIR_H_
#define SRC_TOOLKIT_FOUNDATION_PAIR_H_

#include <queue>

namespace Foundation {

    template <class T, class U>
    class Pair {
    public:
        Pair(T key, U value);
    public:
        T first;
        U second;
    };

    template <class T, class U>
    Pair<T, U>::Pair(T key, U value)
        : first(key), second(value)
    {}

    template <class T, class U>
    inline bool operator<(const Pair<T,U>& lhs, const Pair<T, U> &rhs) {
        return (lhs.first < rhs.first);
    }

    template <class T, class U>
    inline bool operator>(const Pair<T,U>& lhs, const Pair<T, U> &rhs) {
        return (lhs.first > rhs.first);
    }

} /* namespace Foundation */

#endif /* SRC_TOOLKIT_FOUNDATION_PAIR_H_ */
