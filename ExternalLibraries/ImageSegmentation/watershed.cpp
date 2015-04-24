
#include "watershed.h"
#include "neighbor.h"
#include "stack.h"
#include "queue.h"
#include "sentinel.h"
#include <string.h>
#include <stdlib.h>

// -------------------------------------------------------------------------- //
template<class T> inline
void Swap(T& a, T& b)
{
    T c(a);
    a = b, b = c;
}
// -------------------------------------------------------------------------- //

template<class T> static
int* CountingSort(const T* Gradient, const int Width, const int Height)
// This gradient image must be surrounded with sentinels, so that its actual size is (Width+2)*(Height+2).
{
    const T* scan;
    const int Range(1<<(8*sizeof(T)));
    T MinVal, MaxVal;
    int *Rank, *Hist0, *Hist, index, offset, count, x, y;

    GetMin(MinVal);
    GetMax(MaxVal);

    Rank = new int[Width * Height];
    Hist0 = new int[Range];
    if(!Rank || !Hist0) {
        throw "CountingSort => cannot allocate memory";
    }
    memset(Hist0, 0, sizeof(int) * Range);
    Hist = Hist0 - (int) MinVal;

    // Counting
    scan = Gradient + Width + 2;
    for(y=1; y<=Height; y++) {
        for(x=1; x<=Width; x++) {
            Hist[(int) scan[x]]++;
        }
        scan += Width + 2;
    }

    // Accumulation
    count = 0;
    for(index=(int)MinVal; index<=(int)MaxVal; index++) {
        int temp;
        temp = Hist[index];
        Hist[index] = count;
        count += temp;
    }

    // Indexing
    offset = Width + 2;
    for(y=1; y<=Height; y++) {
        for(x=1; x<=Width; x++) {
            Rank[Hist[(int)Gradient[offset+x]]++] = offset+x;
        }
        offset += Width + 2;
    }

    delete[] Hist0;
    return Rank;
}

// -------------------------------------------------------------------------- //

template<class T> inline
int SortGreater(const T* Gradient, const int Index1, const int Index2)
{
    T x1, x2;

    x1 = Gradient[Index1];
    x2 = Gradient[Index2];

    if(x1 == x2) {
// -------------------------------------------------------------------
// The following code is to make the sorting stable.
// It can make the non-order-invariant watershed algorithm generate
// the same segmentation as the non-order-invariant toboggan algorithm
// in this package.
// -------------------------------------------------------------------
       return Index1 - Index2;
   }
// -------------------------------------------------------------------
    return (x1 > x2) ? +1 : -1;
}

// -------------------------------------------------------------------------- //

template<class T> static
int* QuickSort(const T* Gradient, const int Width, const int Height)
// This gradient image must be surrounded with sentinels, so that its actual size is (Width+2)*(Height+2).
{
    const int QuickSortStackSize = 64;
    const int InsertionSortLength = 16;

    TStack<int> Stack(QuickSortStackSize);
    const int Size(Width * Height);
    int *Rank, i, j, temp, x, y, index, offset, left, right, mid;
    
    // Memory allocation
    Rank = new int[Size];
    if(!Rank) {
        throw "QuickSort => cannot allocate memory";
    }

    // Making indecies
    index = 0;
    offset = Width+3;
    for(y=0; y<Height; y++) {
        for(x=0; x<Width; x++) {
            Rank[index+x] = index+x+offset;
        }
        index += Width;
        offset += 2;
    }

    // Initiation
    Stack.Reset();
    Stack.Push(left = 0);
    Stack.Push(right = Size - 1);

    while(!Stack.IsEmpty()) {

        if(right - left < InsertionSortLength) {
            // InsertionSort
            for(i=left+1; i<=right; i++) {
                temp = Rank[i];
                for(j=i; j>left; j--) {
                    // Search for the proper position of `temp'
                    if(SortGreater(Gradient, Rank[j-1], temp) > 0) {
                        Rank[j] = Rank[j-1];
                    }
                    else {
                        break;
                    }
                }
                Rank[j] = temp;
            }
            Stack.Pop(right);
            Stack.Pop(left);
            continue;
        }

        // QuickSort
        mid = right - 1;
        if(SortGreater(Gradient, Rank[left], Rank[right]) > 0) {
            Swap(Rank[left], Rank[right]);
        }
        if(SortGreater(Gradient, Rank[mid], Rank[right])  > 0) {
            Swap(Rank[mid], Rank[right]);
        }
        if(SortGreater(Gradient, Rank[left], Rank[mid])   > 0) {
            Swap(Rank[left], Rank[mid]);
        }
        // --> Gradient[Rank[left]] <= Gradient[Rank[mid]] <= Gradient[Rank[right]]

        i = left;
        j = mid;
        temp = Rank[mid];
        do {
            while(SortGreater(Gradient, Rank[++i], temp) < 0);
            while(SortGreater(Gradient, Rank[--j], temp) > 0);
            Swap(Rank[i], Rank[j]);
        } while(i <= j);

		temp = Rank[j];
        Rank[j] = Rank[i];
		Rank[i] = Rank[mid];
        Rank[mid] = temp;

        if(j-left+1 > right-i) {
            Stack.Push(left);
            Stack.Push(j);
            left = i+1;
        }
        else {
            Stack.Push(i+1);
            Stack.Push(right);
            right = j;
        }
    }

    return Rank;
}

// -------------------------------------------------------------------------- //

template<class T>
class StdLibSort {
private:
    static const T* Gradient;
public:
    static int CompareFunction(const void* p1, const void* p2);
    static int* Execute(const T* Gradient, const int Width, const int Height);
};

template<class T>
const T* StdLibSort<T>::Gradient;

template<class T>
int StdLibSort<T>::CompareFunction(const void* p1, const void* p2)
{
    return SortGreater(StdLibSort<T>::Gradient, *(const int*)p1, *(const int*)p2);
}

template<class T>
int* StdLibSort<T>::Execute(const T* Gradient, const int Width, const int Height) 
{
    const int Size(Width*Height);
    int *Rank, x, y, index, offset;

    // Memory allocation
    Rank = new int[Size];
    if(!Rank) {
        throw "StdLibSort::Execute => cannot allocate memory";
    }

    // Making indecies
    index = 0;
    offset = Width+3;
    for(y=0; y<Height; y++) {
        for(x=0; x<Width; x++) {
            Rank[index+x] = index+x+offset;
        }
        index += Width;
        offset += 2;
    }

    StdLibSort<T>::Gradient = Gradient;
    qsort(Rank, Size, sizeof(int), StdLibSort<T>::CompareFunction);

    return Rank;
}

// -------------------------------------------------------------------------- //

template<class T> static
int* Sort(const T* Gradient, const int Width, const int Height)
// This gradient image must be surrounded with sentinels, so that its actual size is (Width+2)*(Height+2).
{
    if(CountableAndSmallRange(Gradient)) {
        return CountingSort<T>(Gradient, Width, Height);
    }
    else
    {
        return QuickSort<T>(Gradient, Width, Height);
        // Another choice is to use the standard C library ---------
        // -- WARNING ----------------------------------------------
        // Because this sorting function uses a static member
        // variable, it is not a thread-safe operation.
        // --------------------------------------------------------
        // return StdLibSort<T>::Execute(Gradient, Width, Height);
        // --------------------------------------------------------
    }
}

// -------------------------------------------------------------------------- //
// -- !!! WARNING !!! -------------------------------------------------------
// If a function template is used instead of the following class template,
// Microsoft Visual C++ 6.0 will generate only one instantiation for a class
// T, no matter `OrderInvariant' is true or false.
// ---------------------------------------------------------------------------//

template<class T, bool OrderInvariant, int Connectivity>
struct GenericWatershed {
    static int* GetLabelImage(const T* OldGradient, const int Width, const int Height);
};

template<class T, bool OrderInvariant, int Connectivity>
int* GenericWatershed<T, OrderInvariant, Connectivity>::
GetLabelImage(const T* OldGradient, const int Width, const int Height)
{
    TNeighbor<Connectivity> Neighbor(1, Width+2);
    TQueue<int> Queue(Width*Height);

    T *Gradient, level, min_level, gradient;
    int *Rank, *Dist, *Label, *int_scan, LabelCount;
    int offset, index, begin, end, pixel, dist, label, neighbor;
    
    // Surrounding the gradient image with sentinesl.
    Gradient = MakeNewGradient<T>(OldGradient, Width, Height);
    Rank = Sort<T>(Gradient, Width, Height);

    // Memory allocation
    Label = new int[(Width+2)*(Height+2)];
    if(OrderInvariant) { // ---> ADDITIONAL CODE FOR ORDER-INVARIANT ALGORITHM
        Dist  = new int[(Width+2)*(Height+2)];
    } ////////////////////////////////////////////////////////////////////////
    if(!Label || (!Dist && OrderInvariant)) {
        throw "GenericWatershed => cannot allocate memory";
    }

    // Initialization
    // Setting the sentinels as ridges
    memset(Label+0, -1, sizeof(int)*(Width+2)); // RIDGE_LABEL == -1
    offset = Width+2;
    for(index=1; index<=Height; index++) {
        memset(Label+offset+1, 0, sizeof(int)*Width); // NULL_LABEL == 0
        Label[offset+0] = Label[offset+Width+1] = RIDGE_LABEL;
        offset += Width+2;
    }
    memset(Label+offset, -1, sizeof(int)*(Width+2)); // RIDGE_LABEL == -1

    if(OrderInvariant) { // ---> ADDITIONAL CODE FOR ORDER-INVARIANT ALGORITHM
        memcpy(Dist, Label, sizeof(int)*(Width+2)*(Height+2));
    } ////////////////////////////////////////////////////////////////////////
    LabelCount = 0;

    for(begin=0; begin<Width*Height; begin=end) {
        
        Queue.Reset();
        // Search for the growable pixels
        level = Gradient[Rank[begin]];
        for(index=begin; index<Width*Height; index++) {
            if(Gradient[pixel=Rank[index]] != level) {
                break;
            }
            min_level = level;
            label = NULL_LABEL;
            for(Neighbor.Begin(); !Neighbor.End(); Neighbor.Next()) {
                neighbor = Neighbor.Where(pixel);
                gradient = Gradient[neighbor];
                if(gradient >= level) {
                    continue;
                }
                if(gradient < min_level) {
                    // The label of the steepest direction should be chosen.
                    label = Label[neighbor];
                    min_level = gradient;
                    continue;
                }
                if(OrderInvariant) { // ---> ADDITIONAL CODE FOR ORDER-INVARIANT ALGORITHM
                    if(gradient == min_level && Label[neighbor] != label) {
                        // There are multiple markers reachable to `pixel'.
                        label = RIDGE_LABEL;
                    }
                } ////////////////////////////////////////////////////////////////////////
            }
            if(label != NULL_LABEL) {
                Label[pixel] = label;
                Queue.In(pixel);
            }
        }
        end = index;

        // Flooding in the flat regions
        while(!Queue.IsEmpty()) {
            Queue.Out(pixel);
            label = Label[pixel];
            if(OrderInvariant) { // ---> ADDITIONAL CODE FOR ORDER-INVARIANT ALGORITHM
                dist = Dist[pixel] + 1;
            } ////////////////////////////////////////////////////////////////////////
            for(Neighbor.Begin(); !Neighbor.End(); Neighbor.Next()) {
                neighbor = Neighbor.Where(pixel);
                if(Gradient[neighbor] == level) { 
                    // `neighbor' is in the flat regions.
                    if(Label[neighbor] == NULL_LABEL) { 
                        // `neighbor' has not been visited.
                        Label[neighbor] = label;
                        Queue.In(neighbor);
                        if(OrderInvariant) { // ---> ADDITIONAL CODE FOR ORDER-INVARIANT ALGORITHM
                            Dist[neighbor] = dist;
                        } ////////////////////////////////////////////////////////////////////////
                        continue;
                    }
                    if(OrderInvariant) { // ---> ADDITIONAL CODE FOR ORDER-INVARIANT ALGORITHM
                        if(Dist[neighbor] == dist && Label[neighbor] != label) {
                            // There are multiple markers in the same distance to `neighbor'.
                            Label[neighbor] = RIDGE_LABEL;
                        }
                    } ////////////////////////////////////////////////////////////////////////
                }
            }
        }

        // Creating new markers in the minimal regions
        for(index=begin; index<end; index++) {
            pixel = Rank[index];
            if(Label[pixel] != NULL_LABEL) {
                continue;
            }
            label = ++ LabelCount;
            Label[pixel] = label;
            Queue.Reset();
            Queue.In(pixel);
            while(!Queue.IsEmpty()) {
                Queue.Out(pixel);
                for(Neighbor.Begin(); !Neighbor.End(); Neighbor.Next()) {
                    neighbor = Neighbor.Where(pixel);
                    if(Label[neighbor] == NULL_LABEL && Gradient[neighbor] == level) {
                        Label[neighbor] = label;
                        Queue.In(neighbor);
                    }
                }                
            }
        }
    }

    // Memory allocated by other functions
    delete[] Rank;
    delete[] Gradient;

    // Memory allocated by this function
    if(OrderInvariant) { // ---> ADDITIONAL CODE FOR ORDER-INVARIANT ALGORITHM
        delete[] Dist;
    } ////////////////////////////////////////////////////////////////////////

    // Removing the sentinels
    offset = Width + 3;
    int_scan = Label;
    for(index=0; index<Height; index++) {
        memcpy(int_scan, int_scan+offset, sizeof(int) * Width);
        offset += 2;
        int_scan += Width;
    }

    return Label;
}

// -------------------------------------------------------------------------- //

template<class T>
int* Watershed(const T* Gradient, const int Width, const int Height, const bool OrderInvariant, const int Connectivity)
{
    if(Connectivity == 4) {
        if(OrderInvariant) {
            return GenericWatershed<T, true, 4>::GetLabelImage(Gradient, Width, Height);
        }
        else {
            return GenericWatershed<T, false, 4>::GetLabelImage(Gradient, Width, Height);
        }
    }
    else if(Connectivity == 8) {
        if(OrderInvariant) {
            return GenericWatershed<T, true, 8>::GetLabelImage(Gradient, Width, Height);
        }
        else {
            return GenericWatershed<T, false, 8>::GetLabelImage(Gradient, Width, Height);
        }
    }
    else {
        throw "Watershed => invalid connectivity";
    }
}

// -------------------------------------------------------------------------- //
// Instantiation 

// unsigned short
template
int* Watershed<unsigned short>
(const unsigned short*, const int, const int, const bool, const int);

// unsigned float
template
int* Watershed<float>
(const float*, const int, const int, const bool, const int);

// -------------------------------------------------------------------------- //
