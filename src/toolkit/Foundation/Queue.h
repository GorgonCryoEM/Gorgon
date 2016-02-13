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

    class Queue {
        public:
            Queue();
            QueueNode* getNext();
            void reset();
            int getNumElements();
            void prepend(int xx, int yy, int zz);
            QueueNode * remove();

        private:
            typedef list<QueueNode> Cont;
            Cont q;
            Cont::iterator it;
    };

    Queue::Queue() {
        it=q.end();
    }

    QueueNode* Queue::getNext() {
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

    void Queue::reset() {
        it = q.end();
    }

    int Queue::getNumElements() {
        return q.size();
    }

    void Queue::prepend(int xx, int yy, int zz) {
        q.push_front(QueueNode(xx, yy, zz));
        reset();
    }

    /* Remove current element pointed by cur */
    QueueNode * Queue::remove() {
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
