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

            T  *valueQueue;
            int  *keyQueue;
            typedef pair<T,int> Elem;
            priority_queue<Elem, vector<Elem>, greater<Elem> > q;

        public:
            PriorityQueue(int max) {
                maxLength = max;
                queueLength = 0;
                valueQueue = new T[max];
                keyQueue = new int[max];
            }

            ~PriorityQueue() {
                delete[] keyQueue;
                delete[] valueQueue;
            }

            int getLength() {
                return queueLength;
            }

            bool isEmpty() {
                return q.empty();
            }

            bool isFull() {
                return (queueLength == this->maxLength);
            }

            void add(T v, int k) {
                if(isFull()) {
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

                q.push(make_pair(v,k));
            }

            void remove(T & v, int & k) {

                if(isEmpty()) {
//                    v = NULL;
                    k = 0;
                    return;
                }

                pair<T,int> res = q.top();
                q.pop();

                v = valueQueue[0];
                k = keyQueue[0];
                queueLength--;

                if(isEmpty()) {
                    valueQueue[0] = T();
                    return;
                }

                T  vv = valueQueue[queueLength];
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
                valueQueue[queueLength] = T();
                keyQueue[queueLength] = 0;
            }
    };
}

#endif
