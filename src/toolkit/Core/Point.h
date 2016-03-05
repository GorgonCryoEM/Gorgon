/*
 * Point.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_CORE_POINT_H_
#define SRC_TOOLKIT_CORE_POINT_H_

namespace Core {

    /*
     *
     */
    template <class T>
    struct Point {
            Point();
            Point(int ii, int jj, int kk, T v=T());
            virtual ~Point();

            void operator=(T r) {
                val = r;
            }

            int i, j, k;
            T val;
    };

    template<class T>
    bool operator<(const Point<T> & l, const Point<T> & r) {
        return l.val < r.val;
    }

    template<class T>
    bool operator<(const Point<T> & l, const T & r) {
        return l.val < r;
    }

    template<class T>
    bool operator<(const T & l, const Point<T> & r) {
        return l < r.val;
    }

    template<class T>
    inline Point<T>::Point(int ii, int jj, int kk, T v)
            : i(ii), j(jj), k(kk), val(v)
    {}

    template<class T>
    inline Point<T>::Point() {
    }

    template<class T>
    inline Point<T>::~Point() {
    }

} /* namespace Core */

#endif /* SRC_TOOLKIT_CORE_POINT_H_ */
