/*
 * List.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_FOUNDATION_LIST_H_
#define SRC_TOOLKIT_FOUNDATION_LIST_H_

#include <list>

namespace SkeletonMaker {

    struct QueueNode {
            int x, y, z;

            QueueNode(int xx, int yy, int zz)
                    : x(xx), y(yy), z(zz)
            {}

            QueueNode() {}
    };

    class List {
        public:
            List();
            QueueNode* getNext();
            void reset();
            int size();
            void prepend(int xx, int yy, int zz);
            QueueNode * remove();

        private:
            typedef list<QueueNode> Cont;
            Cont q;
            Cont::iterator it;
    };

    inline List::List() {
        it=q.end();
    }

    inline QueueNode* List::getNext() {
        if(it==q.end()){
            it=q.begin();
            if(it==q.end())
                return NULL;
        }
        else {
            ++it;
            if(it==q.end())
                return NULL;
        }

        return &(*it);
    }

    inline void List::reset() {
        it = q.end();
    }

    inline int List::size() {
        return q.size();
    }

    inline void List::prepend(int xx, int yy, int zz) {
        q.push_front(QueueNode(xx, yy, zz));
        reset();
    }

    /* Remove current element pointed by cur */
    inline QueueNode * List::remove() {
        if(it != q.end()){
            it = q.erase(it);
            if(it==q.end())
                return NULL;

            return &(*it);
        }
        else
            return NULL;
    }

} /* namespace SkeletonMaker */

#endif /* SRC_TOOLKIT_FOUNDATION_LIST_H_ */
