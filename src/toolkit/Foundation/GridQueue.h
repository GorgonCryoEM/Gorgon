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

    class GridQueue {
        public:
            GridQueue();
            ~GridQueue();
            gridQueueEle* getNext();
            void reset();
            int getNumElements();
            void prepend(int xx, int yy, int zz);
            gridQueueEle * remove();

        private:
            gridQueueEle* head;
            gridQueueEle* pre;
            gridQueueEle* prepre;
            gridQueueEle* cur;
            int numEles;
    };

    GridQueue::GridQueue() {
        head = NULL;
        cur = NULL;
        pre = NULL;
        prepre = NULL;
        numEles = 0;
    }

    GridQueue::~GridQueue() {
        gridQueueEle* ele;
        reset();
        ele = getNext();
        while( (ele = remove()) != NULL) {
        };
    }

    gridQueueEle* GridQueue::getNext() {
        if(cur == NULL) {
            prepre = NULL;
            pre = NULL;
            cur = head;
        }
        else {
            prepre = pre;
            pre = cur;
            cur = cur->next;
        }

        return cur;
    }

    void GridQueue::reset() {
        prepre = NULL;
        pre = NULL;
        cur = NULL;
    }

    int GridQueue::getNumElements() {
        return numEles;
    }

    void GridQueue::prepend(int xx, int yy, int zz) {
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
    gridQueueEle * GridQueue::remove() {
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
