#ifndef QUEUE_H
#define QUEUE_H

#include <assert.h>

template<class T>
class TQueue
{
private:
    const int MaxSize;
    T* Array;
    int head, tail;

public:
    void In(const T value) {
        assert(tail < MaxSize);
        Array[tail++] = value;
    }
    void Out(T& reference)  {
        assert(head < tail);
        reference = Array[head++];
    }
    bool IsEmpty() {
        return (head >= tail);
    }
    void Reset() {
        head = tail = 0;
    }

public:
    TQueue(const int Size):MaxSize(Size) {
        Array = new T[MaxSize];
        if(!Array) {
            throw "Queue::Queue => cannot allocate memory";
        }
        Reset();
    }
    ~TQueue() {
        if(Array) {
            delete[] Array;
        }
    }
};

#endif
