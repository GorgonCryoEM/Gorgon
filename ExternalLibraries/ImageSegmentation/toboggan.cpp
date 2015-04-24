
#include "toboggan.h"
#include "neighbor.h"
#include "queue.h"
#include "sentinel.h"
#include <string.h>

// -------------------------------------------------------------------------- //
// -- !!! WARNING !!! -------------------------------------------------------
// If a function template is used instead of the following class template,
// Microsoft Visual C++ 6.0 will generate only one instantiation for a class
// T, no matter `OrderInvariant' is true or false.
// ---------------------------------------------------------------------------//

template<class T, bool OrderInvariant, int Connectivity>
struct GenericToboggan {
    static int* GetLabelImage(const T* OldGradient, const int Width, const int Height);
};

template<class T, bool OrderInvariant, int Connectivity>
int* GenericToboggan<T, OrderInvariant, Connectivity>::
GetLabelImage(const T* OldGradient, const int Width, const int Height)
{
    TNeighbor<Connectivity> Neighbor(1, Width+2);
    TQueue<int> Queue(Width*Height);

    struct {
        int Offset;
        int NextState;
    } TransitionTable[1 << Connectivity];

    T *Gradient, level, min_level;
    int *Label, *Dist, *BacktrackState, *int_scan, LabelCount;
    int offset, index, dist, begin, end, pixel, label, neighbor, state, sliding_list;

    int NegativeMask;

    if(sizeof(int)*8-2 < Connectivity) {
        // See the trick below.
        throw "GenericToboggan_OrderInvariant => the number of bits of integer is not large enough";
    }
    GetMin(NegativeMask); // Setting the most significant bit of `NegativeMask'.
    
    // Surrounding the gradient image with sentinesl.
    Gradient = MakeNewGradient<T>(OldGradient, Width, Height);

    // Memory allocation
    Label = new int[(Width+2)*(Height+2)];
    if(OrderInvariant) { // ---> ADDITIONAL CODE FOR ORDER-INVARIANT ALGORITHM
        Dist  = new int[(Width+2)*(Height+2)];
    } ////////////////////////////////////////////////////////////////////////
    if(!Label || (!Dist && OrderInvariant)) {
        throw "GenericToboggan_OrderInvariant => cannot allocate memory";
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
        // The memory allocated by `Queue' is shared with `BacktrackState'.
        BacktrackState = Dist;
    } ////////////////////////////////////////////////////////////////////////

    LabelCount = 0;
    Queue.Reset();

    // Downward sliding
    begin = Width+3;
    end = (Width+2)*(Height+2)-(Width+3);
    for(pixel=begin; pixel<end; pixel++) {
        if(Label[pixel] != NULL_LABEL) {
            // `pixel' is one of the sentinels.
            continue;
        }
        sliding_list = 0;
        min_level = Gradient[pixel];
        // Search for the steepest direction
        for(Neighbor.Begin(); !Neighbor.End(); Neighbor.Next()) {
            neighbor = Neighbor.Where(pixel);
            level = Gradient[neighbor];
            if(level < min_level) {
                // ************************* Trick ***********************************
                // We set the i-th bit if the label of `pixel' depends on `neighbor',
                // where i is the index of the direction from `pixel' to `neighbor'.
                // The most significant bit of the value will be set, so that it can
                // be identified as a sliding list.
                // Notice that the second-most significant bit must be zero, so that
                // it guarantees the values of sliding lists are less than RIDGE_LABEL.
                // *******************************************************************
                sliding_list = (1 << Neighbor.Direction());
                min_level = level;
                continue;
            }
            if(OrderInvariant) { // ---> ADDITIONAL CODE FOR ORDER-INVARIANT ALGORITHM
                if(level == min_level && sliding_list != 0) {
                    sliding_list |= (1 << Neighbor.Direction());
                }
            } ////////////////////////////////////////////////////////////////////////
        }
        if(sliding_list != 0) {
            Queue.In(pixel);
            Label[pixel] = sliding_list | NegativeMask;
        }
    }

    // Region growing from the lower boundaries
    while(!Queue.IsEmpty()) {
        Queue.Out(pixel);
        level = Gradient[pixel];
        dist = Dist[pixel] + 1;
        for(Neighbor.Begin(); !Neighbor.End(); Neighbor.Next()) {
            // Using `Backtrack' to ensure the correctness of the further use of `Direction'.
            neighbor = Neighbor.Backtrack(pixel);
            if(Gradient[neighbor] != level) {
                // `neighbor' is not at the same level as `pixel'.
                continue;
            }
            if(Label[neighbor] == NULL_LABEL) {
                Label[neighbor] = (1 << Neighbor.Direction()) | NegativeMask; // See the above trick.
                Queue.In(neighbor);
                if(OrderInvariant) { // ---> ADDITIONAL CODE FOR ORDER-INVARIANT ALGORITHM
                    Dist[neighbor] = dist;
                } ////////////////////////////////////////////////////////////////////////
                continue;
            }
            if(OrderInvariant) { // ---> ADDITIONAL CODE FOR ORDER-INVARIANT ALGORITHM
                // `neighbor' has been visited.
                if(Dist[neighbor] == dist) {
                    // and there are multiple ways for it to slide downward.
                    Label[neighbor] |= (1 << Neighbor.Direction());
                }                
            } ////////////////////////////////////////////////////////////////////////
        }
    }

    // Finding the minimal regions
    for(index=begin; index<end; index++) {
        pixel = index;
        if(Label[pixel] != NULL_LABEL) {
            // `pixel' has been visited
            continue;
        }
        Label[pixel] = label = ++ LabelCount;
        Queue.Reset();
        Queue.In(pixel);
        while(!Queue.IsEmpty()) {
            Queue.Out(pixel);
            for(Neighbor.Begin(); !Neighbor.End(); Neighbor.Next()) {
                neighbor = Neighbor.Where(pixel);
                if(Label[neighbor] != NULL_LABEL) {
                    // `neighbor' has been visited.
                    continue;
                }
                Label[neighbor] = label;
                Queue.In(neighbor);
            }
        }
    }

    // Initiation of the transition table
    TransitionTable[0].Offset = 0;
    TransitionTable[0].NextState = 0;
    for(Neighbor.Begin(), index=1; !Neighbor.End(); Neighbor.Next(), index*=2) {
        offset = Neighbor.Where(0);
        for(state=index; state<(1<<Connectivity); state+=index*2) {
            TransitionTable[state].Offset = offset;
            TransitionTable[state].NextState = state - index;
        }
    }

    // Resolving the sliding lists
    for(index=begin; index<end; index++) {
        pixel = index;
        sliding_list = Label[pixel];
        if(sliding_list >= RIDGE_LABEL) {
            // `pixel' has been labeled.
            continue;
        }

        if(OrderInvariant) { // ---> CODE FOR ORDER-INVARIANT ALGORITHM ////////////////////
            // Depth-First-Search (Topological Sort)
            // Non-recursion Implementation

            // Initializing the variables of the top level
            BacktrackState[pixel] = 0;
            state = sliding_list ^ NegativeMask;
            label = NULL_LABEL;

            while(state) {

                neighbor = pixel + TransitionTable[state].Offset;
                sliding_list = Label[neighbor];

                // Has the label of `neighbor' been resolved?
                if(sliding_list < RIDGE_LABEL) {
                    // Starting a new level
                    // Saving the variables of the current level
                    Label[pixel] = label;
                    BacktrackState[neighbor] = state;
                    // Initializing the variables of the next level
                    pixel = neighbor;
                    label = NULL_LABEL;
                    state = sliding_list ^ NegativeMask;
                    continue;
                }

                // Resolving [label(neighbor) == sliding_list]
                if(label == NULL_LABEL) {
                    label = sliding_list;
                }
                else if(label != sliding_list) {
                    label = RIDGE_LABEL;
                    state = 0; // Early-jump-out
                }

                // Transition
                while((state = TransitionTable[state].NextState) == 0) {
                    // (state == 0) --> Backtracking
                    state = BacktrackState[pixel];
                    // Saving the result of this level
                    Label[pixel] = label;

                    if(state == 0) {
                        // Reaching the top level
                        break;
                    }

                    // Restoring the variables of the upper level
                    pixel -= TransitionTable[state].Offset;
                    // Resolving
                    if(Label[pixel] != NULL_LABEL) {
                        if(Label[pixel] != label) {
                            label = RIDGE_LABEL;
                            state = 0; // Early-jump-out
                        }
                    }
                }
            }
        }
        else { // ---> CODE FOR NON-ORDER-INVARIANT ALGORITHM ////////////////////
            Queue.Reset();
            state = sliding_list ^ NegativeMask;
            for(;;) {
                neighbor = pixel + TransitionTable[state].Offset;
                sliding_list = Label[neighbor];
                if(sliding_list >= RIDGE_LABEL) {
                    // label(neighbor) == sliding_list
                    break;
                }
                Queue.In(neighbor);
                pixel = neighbor;
                state = sliding_list ^ NegativeMask;
            }
            Label[index] = label = sliding_list;
            while(!Queue.IsEmpty()) {
                Queue.Out(pixel);
                Label[pixel] = label;
            }
        } ////////////////////////////////////////////////////////////////////////
    }

    // Memory allocated by other functions
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
// This version of the toboggan algorithm is a little bit faster, because it 
// saves the index of the reference neighbor (instead of using the index of
// the relative direction).

template<class T, bool OrderInvariant, int Connectivity>
struct GenericTobogganVer2 {
    static int* GetLabelImage(const T* OldGradient, const int Width, const int Height);
};

template<class T, bool OrderInvariant, int Connectivity>
int* GenericTobogganVer2<T, OrderInvariant, Connectivity>::
GetLabelImage(const T* OldGradient, const int Width, const int Height)
// ** (OrderInvariant == false) **
{
    TNeighbor<Connectivity> Neighbor(1, Width+2);
    TQueue<int> Queue(Width*Height);

    T *Gradient, level, min_level;
    int *Label, *int_scan, LabelCount;
    int offset, index, begin, end, pixel, label, neighbor;

    if(OrderInvariant == true) {
        throw "GenericToboggan::GetLabelImage => OrderInvariant must be false";
    }
    
    // Surrounding the gradient image with sentinesl.
    Gradient = MakeNewGradient<T>(OldGradient, Width, Height);

    // Memory allocation
    Label = new int[(Width+2)*(Height+2)];
    if(!Label) {
        throw "GenericToboggan_OrderVariant => cannot allocate memory";
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

    LabelCount = 0;
    Queue.Reset();

    // Downward sliding
    begin = Width+3;
    end = (Width+2)*(Height+2)-(Width+3);
    for(pixel=begin; pixel<end; pixel++) {
        if(Label[pixel] != NULL_LABEL) {
            // `pixel' is one of the sentinels.
            continue;
        }
        label = NULL_LABEL;
        min_level = Gradient[pixel];
        // Search for the steepest direction
        for(Neighbor.Begin(); !Neighbor.End(); Neighbor.Next()) {
            neighbor = Neighbor.Where(pixel);
            level = Gradient[neighbor];
            if(level < min_level) {
                // ************************* Trick ***********************************
                // Values less than `-1' are used as the pointers to the pixels.
                // (Remind that RIDGE_LABEL is defined as -1 in "sentinel.h".)
                // Thus only the first (pointer=0) and second (pointer=1) pixel in
                // the image are not accessible. It is safe for this function because
                // the first two pixels must be the sentinels.
                // *******************************************************************
                label = - neighbor;
                min_level = level;
            }
        }
        if(label != NULL_LABEL) {
            Queue.In(pixel);
            Label[pixel] = label;
        }
    }

    // Region growing from the lower boundaries
    while(!Queue.IsEmpty()) {
        Queue.Out(pixel);
        level = Gradient[pixel];
        for(Neighbor.Begin(); !Neighbor.End(); Neighbor.Next()) {
            neighbor = Neighbor.Where(pixel);
            if(Label[neighbor] != NULL_LABEL) {
                // `neighbor' has been visited.
                continue;
            }
            if(Gradient[neighbor] != level) {
                // `neighbor' is not at the same level as `pixel'.
                continue;
            }
            Label[neighbor] = - pixel; // Refer to the above trick.
            Queue.In(neighbor);
        }
    }

    // Finding the minimal regions
    for(index=begin; index<end; index++) {
        pixel = index;
        if(Label[pixel] != NULL_LABEL) {
            // `pixel' has been visited
            continue;
        }
        Label[pixel] = label = ++ LabelCount;
        Queue.Reset();
        Queue.In(pixel);
        while(!Queue.IsEmpty()) {
            Queue.Out(pixel);
            for(Neighbor.Begin(); !Neighbor.End(); Neighbor.Next()) {
                neighbor = Neighbor.Where(pixel);
                if(Label[neighbor] != NULL_LABEL) {
                    // `neighbor' has been visited.
                    continue;
                }
                Label[neighbor] = label;
                Queue.In(neighbor);
            }
        }
    }

    // Resolving pointers
    for(pixel=begin; pixel<end; pixel++) {
        label = Label[pixel];
        if(label >= RIDGE_LABEL) {
            // `pixe' is a sentinel, or it has been labelled.
            continue;
        }
        Queue.Reset();
        for(;;) {
            neighbor = - label;
            label = Label[neighbor];
            if(label >= RIDGE_LABEL) {
                break;
            }
            Queue.In(neighbor);
        }
        Label[pixel] = label;
        while(!Queue.IsEmpty()) {
            Queue.Out(neighbor);
            Label[neighbor] = label;
        }
    }

    // Memory allocated by other functions
    delete[] Gradient;

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
int* Toboggan(const T* Gradient, const int Width, const int Height, const bool OrderInvariant, const int Connectivity)
{
    if(Connectivity == 4) {
        if(OrderInvariant) {
            return GenericToboggan<T, true, 4>::GetLabelImage(Gradient, Width, Height);
        }
        else {
            // Using the first version --------------------------------------------------
            // return GenericToboggan<T, false, 4>::GetLabelImage(Gradient, Width, Height);
            // Using the second version -------------------------------------------------
            return GenericTobogganVer2<T, false, 4>::GetLabelImage(Gradient, Width, Height);
            // --------------------------------------------------------------------------
        }
    }
    else if(Connectivity == 8) {
        if(OrderInvariant) {
            return GenericToboggan<T, true, 8>::GetLabelImage(Gradient, Width, Height);
        }
        else {
            // Using the first version --------------------------------------------------
            // return GenericToboggan<T, false, 8>::GetLabelImage(Gradient, Width, Height);
            // Using the second version -------------------------------------------------
            return GenericTobogganVer2<T, false, 8>::GetLabelImage(Gradient, Width, Height);
            // --------------------------------------------------------------------------
        }
    }
    else {
        throw "Toboggan => invalid connectivity";
    }
}

// -------------------------------------------------------------------------- //
// Instantiation 

// unsigned short
template
int* Toboggan<unsigned short>
(const unsigned short*, const int, const int, const bool, const int);

// float
template
int* Toboggan<float>
(const float*, const int, const int, const bool, const int);

// -------------------------------------------------------------------------- //
