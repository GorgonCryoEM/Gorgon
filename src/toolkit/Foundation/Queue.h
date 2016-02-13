#ifndef TOOLKIT_SKELETON_MAKER_GRID_QUEUE2_H
#define TOOLKIT_SKELETON_MAKER_GRID_QUEUE2_H

#include <list>

namespace SkeletonMaker {

    struct QueueNode {
            int x, y, z;
            QueueNode* next;

            QueueNode(int xx, int yy, int zz)
                    : x(xx), y(yy), z(zz)
            {}

            QueueNode() {}
    };

    class Queue {
        public:
            Queue();
            ~Queue();
            QueueNode* getNext();
            void reset();
            int getNumElements();
            void prepend(int xx, int yy, int zz);
            QueueNode * remove();

        private:
            QueueNode* head;
            QueueNode* pre;
            QueueNode* cur;
            int numEles;

            typedef list<QueueNode> Cont;
            Cont q;
            Cont::iterator it;
    };

    Queue::Queue() {
        head = NULL;
        cur = NULL;
        pre = NULL;
        numEles = 0;
        it=q.end();
    }

    Queue::~Queue() {
        QueueNode* ele;
        reset();
        ele = getNext();
        while( (ele = remove()) != NULL) {
        };
    }

    QueueNode* Queue::getNext() {
        if(cur == NULL) {
            pre = NULL;
            cur = head;
        }
        else {
            pre = cur;
            cur = cur->next;
        }

        if(it==q.end())
            it=q.begin();
        else
            ++it;

        return &(*it);
//        return cur;
    }

    void Queue::reset() {
        pre = NULL;
        cur = NULL;
        it = q.end();
    }

    int Queue::getNumElements() {
        return q.size();
//        return numEles;
    }

    void Queue::prepend(int xx, int yy, int zz) {
        QueueNode* ele = new QueueNode;
        ele->x = xx;
        ele->y = yy;
        ele->z = zz;
        ele->next = head;
        head = ele;
        numEles++;

        q.push_front(QueueNode(xx, yy, zz));

        reset();
    }

    /* Remove current element pointed by cur */
    QueueNode * Queue::remove() {
        QueueNode* temp = cur;
        if(cur != NULL) {
            cur = cur->next;
            delete temp;

            if(pre != NULL) {
                pre->next = cur;
            }
            else {
                head = cur;
            }
            numEles--;
        }

        if(it != q.end())
            it = q.erase(it);

        return &(*it);
//        return cur;
    }
}

#endif
