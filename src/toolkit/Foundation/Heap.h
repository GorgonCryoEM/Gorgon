#ifndef TOOLKIT_FOUNDATION_GORGONHEAP_H
#define TOOLKIT_FOUNDATION_GORGONHEAP_H

#include <vector>

using namespace std;

namespace Foundation {

    template <class T>
    class Heap {
    public:
        Heap();

        T Root();
        T PopRoot();
        bool IsEmpty();

        void Print();

    private:
        unsigned int GetParent(unsigned int index);
        unsigned int GetLeft (unsigned int index);
        unsigned int GetRight(unsigned int index);
        void Heapify(int rootIndex);

        vector<T> values;
        bool isMaxHeap;

    };


    template <class T>
    Heap<T>::Heap() {
        this->isMaxHeap = true;
    }

    template <class T>
    inline unsigned int Heap<T>::GetParent(unsigned int index) {
        return index >> 1;
    }

    template <class T>
    inline unsigned int Heap<T>::GetLeft(unsigned int index) {
        return (index==0)? 1 :(index << 1);
    }

    template <class T>
    inline unsigned int Heap<T>::GetRight(unsigned int index) {
        return (index==0)? 2 : ((index << 1) | 1);
    }

    template <class T>
    void Heap<T>::Heapify(int rootIndex) {
        unsigned int size = values.size();

        if(rootIndex < size) {
            unsigned int left = GetLeft(rootIndex);
            unsigned int right = GetRight(rootIndex);
            unsigned int properRoot;

            properRoot = rootIndex;
            if((left < size) && ((isMaxHeap && (values[left] > values[properRoot])) || (!isMaxHeap && (values[left] < values[properRoot])))) {
                properRoot = left;
            }

            if((right < size) && ((isMaxHeap && (values[right] > values[properRoot])) || (!isMaxHeap && (values[right] < values[properRoot])))) {
                properRoot = right;
            }

            if(properRoot != rootIndex) {
                T tempValue = values[properRoot];
                values[properRoot] = values[rootIndex];
                values[rootIndex] = tempValue;

                Heapify(properRoot);
            }
        }
    }


    template <class T>
    T Heap<T>::Root() {
        return values[0];
    }

    template <class T>
    bool Heap<T>::IsEmpty() {
        return (values.size() == 0);
    }

    template <class T>
    T Heap<T>::PopRoot() {
        T val = values[0];

        unsigned int lastElem = values.size()-1;
        values[0] = values[lastElem];
        values.erase(values.begin() + lastElem);
        Heapify(0);
        return val;
    }

    template <class T>
    void Heap<T>::Print() {
        for(unsigned int i = 0; i < values.size(); i++) {
            printf("%d ", (int)values[i]);
        }
        printf("\n");
    }

}

#endif
