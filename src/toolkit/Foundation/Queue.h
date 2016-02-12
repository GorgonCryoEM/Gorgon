#ifndef TOOLKIT_SKELETON_MAKER_GRID_QUEUE2_H
#define TOOLKIT_SKELETON_MAKER_GRID_QUEUE2_H

namespace SkeletonMaker {

    struct gridQueueEle {
            int x, y, z;
            gridQueueEle* next;

            gridQueueEle(int xx, int yy, int zz)
                    : x(xx), y(yy), z(zz)
            {}

            gridQueueEle() {}
    };

    class Queue {
        public:
            Queue();
            ~Queue();
            gridQueueEle* getNext();
            void reset();
            int getNumElements();
            void prepend(int xx, int yy, int zz);
            gridQueueEle * remove();

        private:
            gridQueueEle* head;
            gridQueueEle* pre;
            gridQueueEle* cur;
            int numEles;
    };

    Queue::Queue() {
        head = NULL;
        cur = NULL;
        pre = NULL;
        numEles = 0;
    }

    Queue::~Queue() {
        gridQueueEle* ele;
        reset();
        ele = getNext();
        while( (ele = remove()) != NULL) {
        };
    }

    gridQueueEle* Queue::getNext() {
        if(cur == NULL) {
            pre = NULL;
            cur = head;
        }
        else {
            pre = cur;
            cur = cur->next;
        }

        return cur;
    }

    void Queue::reset() {
        pre = NULL;
        cur = NULL;
    }

    int Queue::getNumElements() {
        return numEles;
    }

    void Queue::prepend(int xx, int yy, int zz) {
        gridQueueEle* ele = new gridQueueEle;
        ele->x = xx;
        ele->y = yy;
        ele->z = zz;
        ele->next = head;
        head = ele;
        numEles++;

        reset();
    }

    /* Remove current element pointed by cur */
    gridQueueEle * Queue::remove() {
        gridQueueEle* temp = cur;
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
        return cur;
    }
}

#endif
