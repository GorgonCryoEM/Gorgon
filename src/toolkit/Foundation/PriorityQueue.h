#ifndef TOOLKIT_SKELETON_MAKER_PRIORITY_QUEUE_H
#define TOOLKIT_SKELETON_MAKER_PRIORITY_QUEUE_H

#include <cstdlib>
#include <cstdio>
#include <queue>

namespace SkeletonMaker {

    template<class T>
    bool operator<(const pair<T,int> &l, const pair<T,int> &r){
        return l.second < r.second;
    }

//      Template class for a priority queue.
//    The smallest element is at the front
    template<class T>
    class PriorityQueue {
        public:
            int queueLength;
            int maxLength;

            T ** valueQueue;
            int * keyQueue;
            priority_queue<pair<T,int> > q;

        public:
            PriorityQueue(int max) {
                this->maxLength = max;
                this->queueLength = 0;
                this->valueQueue = new T*[max];
                this->keyQueue = new int[max];
            }

            ~PriorityQueue() {
                delete[] keyQueue;
                for(int i = 0; i < queueLength; i++) {
                    delete valueQueue[i];
                }
                delete[] valueQueue;
            }

            int getLength() {
                return this->queueLength;
            }

            bool isEmpty() {
                return (this->queueLength == 0);
            }

            bool isFull() {
                return (this->queueLength == this->maxLength);
            }

            void add(T * v, int k) {
                if(this->isFull()) {
                    printf("PRIORITY QUEUE FILLED UP !!! \n");
                    return;
                }

                int ind = queueLength;
                int tind;
                queueLength++;

                while(ind > 0) {
                    tind = (ind + 1) / 2 - 1;
                    if(k < keyQueue[tind]) {
                        keyQueue[ind] = keyQueue[tind];
                        valueQueue[ind] = valueQueue[tind];
                        ind = tind;
                    }
                    else {
                        break;
                    }
                }

                valueQueue[ind] = v;
                keyQueue[ind] = k;
            }

            void remove(T *& v, int & k) {
                if(this->isEmpty()) {
                    v = NULL;
                    k = 0;
                    return;
                }

                v = valueQueue[0];
                k = keyQueue[0];
                queueLength--;

                if(queueLength == 0) {
                    valueQueue[0] = NULL;
                    return;
                }

                T * vv = valueQueue[queueLength];
                int kk = keyQueue[queueLength], lowk;
                int ind = 0, tind, ind2, ind3;
                while(1) {
                    ind2 = 2 * (ind + 1) - 1;
                    ind3 = ind2 + 1;
                    tind = ind;
                    lowk = kk;

                    if(ind2 >= queueLength) {
                        break;
                    }
                    else {
                        if(keyQueue[ind2] < lowk) {
                            tind = ind2;
                            lowk = keyQueue[ind2];
                        }

                        if(ind3 < queueLength) {
                            if(keyQueue[ind3] < lowk) {
                                tind = ind3;
                            }
                        }

                        if(ind != tind) {
                            valueQueue[ind] = valueQueue[tind];
                            keyQueue[ind] = keyQueue[tind];
                            ind = tind;
                        }
                        else {
                            break;
                        }
                    }
                }

                valueQueue[ind] = vv;
                keyQueue[ind] = kk;
                valueQueue[queueLength] = NULL;
                keyQueue[queueLength] = NULL;
            }
    };
}

#endif
