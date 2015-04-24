#ifndef STACK_H
#define STACK_H

#include <assert.h>

template<class T>
class TStack
{
private:
    const int MaxSize;
    T* Array;
    int sptr;

public:
    void Push(const T value) {
        assert(sptr < MaxSize);
        Array[sptr++] = value;
    }
    void Pop(T& reference)  {
        assert(sptr > 0);
        reference = Array[--sptr];
    }
    bool IsEmpty() {
        return (sptr == 0);
    }
    void Reset() {
        sptr = 0;
    }

public:
    TStack(const int Size):MaxSize(Size) {
        Array = new T[MaxSize];
        if(!Array) {
            throw "Stack::Stack => cannot allocate memory";
        }
        Reset();
    }
    ~TStack() {
        if(Array) {
            delete[] Array;
        }
    }
};

#endif
