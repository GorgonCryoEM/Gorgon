#ifndef TOOLKIT_SKELETON_MAKER_GRID_QUEUE2_H
#define TOOLKIT_SKELETON_MAKER_GRID_QUEUE2_H

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

    List::List() {
        it=q.end();
    }

    QueueNode* List::getNext() {
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

    void List::reset() {
        it = q.end();
    }

    int List::size() {
        return q.size();
    }

    void List::prepend(int xx, int yy, int zz) {
        q.push_front(QueueNode(xx, yy, zz));
        reset();
    }

    /* Remove current element pointed by cur */
    QueueNode * List::remove() {
        if(it != q.end()){
            it = q.erase(it);
            if(it==q.end())
                return NULL;

            return &(*it);
        }
        else
            return NULL;
    }
}

#endif
