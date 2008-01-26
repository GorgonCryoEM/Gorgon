
#include <string.h>
#include <math.h>
#include "filter.h"

#ifndef _MSC_VER
// Microsfot Visual C++ specified
#    define FORCEINLINE __forceinline
#else
#    define FORCEINLINE inline
#endif

// -------------------------------------------------------------------------- //
// Definitions of filters

template<class T> struct Greater;
template<class T> struct Less;

template<class T>
struct Greater {
    static bool Compare(const T x, const T y) {
        return (x > y);
    }
    static T Evaluate(const T x, const T y) {
        return (x > y) ? x : y; 
    }
    static T Evaluate(const T x, const T y, const T z) {
        return Evaluate(Evaluate(x, y), z);
    }
};

template<class T>
struct Less {
    static bool Compare(const T x, const T y) {
        return (x < y);
    }
    static T Evaluate(const T x, const T y) {
        return (x < y) ? x : y; 
    }
    static T Evaluate(const T x, const T y, const T z) {
        return Evaluate(Evaluate(x, y), z);
    }
};

template<class T>
struct SobelLowPass {
    static T Evaluate(const T x, const T y, const T z) {
        return x + 2 * y + z;
    }
};

template<class T>
struct SobelHighPass {
    static T Evaluate(const T x, const T y, const T z) {
        return z - x;
    }
};

template<class T>
struct Average {
    static T Evaluate(const T x, const T y, const T z) {
        return (x + y + z) / 3;
    }
};

// -------------------------------------------------------------------------- //
// One-dimensional operator for multiple vectors
template<class T, class Filter>
struct Filter_1D {
    FORCEINLINE 
    static void Execute(const T* Kernel, T* Result, const int Stride, const int Length, const int Step, const int nVectors) 
    // This function is in-place safe. (Kernel == Result)
    {
        const int MaxIndex(Step*nVectors);
        int Index, pos, max_pos;
        const T *InVector;
        T *OutVector, predecessor, successor, current_value;

        max_pos = (Length-1)*Stride;
        for(Index=0; Index<MaxIndex; Index+=Step) {
            // Initiation
            InVector = &Kernel[Index];
            OutVector = &Result[Index];
            current_value = InVector[0];
            successor = InVector[Stride];
            // Applying `Filter' to the vector
            OutVector[0] = Filter::Evaluate(current_value, current_value, successor);
            for(pos=Stride; pos<max_pos; pos+=Stride) {
                predecessor = current_value;
                current_value = successor;
                successor = InVector[pos+Stride];
                OutVector[pos] = Filter::Evaluate(predecessor, current_value, successor);
            }
            OutVector[pos] = Filter::Evaluate(current_value, successor, successor);
        }
    }
    static void Execute_2D_Horizontally(const T* Kernel, T* Result, const int Width, const int Height) {
        Execute(Kernel, Result, 1, Width, Width, Height);
    }
    static void Execute_2D_Vertically(const T* Kernel, T* Result, const int Width, const int Height) {
        Execute(Kernel, Result, Width, Height, 1, Width);
    }
};

// -------------------------------------------------------------------------- //

template<class T, class Filter>
class Morphology { // In-place operations...
private:
    T* Kernel;
    const int Width;
    const int Height;
    const int Size;

    void PointwiseSelect(const T* Mask);

public:
    Morphology(T* _Kernel, const int _Width, const int _Height) :
        Kernel(_Kernel), 
        Width(_Width), 
        Height(_Height),
        Size(_Width*_Height) 
        { }

    void DilationErosion();
    void Reconstruction(const T* Mask);
};

template<class T, class Filter> void
Morphology<T, Filter>::DilationErosion()
{
    Filter_1D<T, Filter>::Execute_2D_Horizontally(Kernel, Kernel, Width, Height);
    Filter_1D<T, Filter>::Execute_2D_Vertically(Kernel, Kernel, Width, Height);
}

template<class T, class Filter> void
Morphology<T, Filter>::Reconstruction(const T* Mask)
{
    DilationErosion();
    PointwiseSelect(Mask);
}

template<class T, class Filter> void
Morphology<T, Filter>::PointwiseSelect(const T* Mask)
{
    int index;
    for(index=0; index<Size; index++) {
        if(Filter::Compare(Kernel[index], Mask[index])) {
            Kernel[index] = Mask[index];
        }
    }
}

// -------------------------------------------------------------------------- //

template<class T> static
void Dilate(T* SourceImage, const int Width, const int Height, const int nIterations = 1)
{
    int iter;
    Morphology<T, Greater<T> > M(SourceImage, Width, Height);
    for(iter=0; iter<nIterations; iter++) {
        M.DilationErosion();
    }
}

template<class T> static
void Erode(T* SourceImage, const int Width, const int Height, const int nIterations = 1)
{
    int iter;
    Morphology<T, Less<T> > M(SourceImage, Width, Height);
    for(iter=0; iter<nIterations; iter++) {
        M.DilationErosion();
    }
}

template<class T> static
void ReconstructByDilation(T* Kernel, const int Width, const int Height, const T* Mask, const int R)
{
    int iter;
    Morphology<T, Greater<T> > M(Kernel, Width, Height);
    for(iter=0; iter<R; iter++) {
        M.Reconstruction(Mask);
    }
}

template<class T> static
void ReconstructByErosion(T* Kernel, const int Width, const int Height, const T* Mask, const int R)
{
    int iter;
    Morphology<T, Less<T> > M(Kernel, Width, Height);
    for(iter=0; iter<R; iter++) {
        M.Reconstruction(Mask);
    }
}

// -------------------------------------------------------------------------- //

template<class T>
void Blur(T* SourceImage, const int Width, const int Height)
{
    Filter_1D<T, Average<T> >::
        Execute_2D_Horizontally(SourceImage, SourceImage, Width, Height);    
    Filter_1D<T, Average<T> >::
        Execute_2D_Vertically(SourceImage, SourceImage, Width, Height);
}

template<class T1, class T2>
T1* SobelGradient(T2* SourceImage, const int Width, const int Height)
{
    typedef int IntermediateType;

    IntermediateType *GradientX, *GradientY;
    T1 *Gradient;
    int index, Size;

    Size = Width * Height;
    Gradient = new T1[Size];
    GradientX = new IntermediateType[Size];
    GradientY = new IntermediateType[Size];
    if(!Gradient, !GradientX || !GradientY) {
        throw "SobelGradient => cannot allocate memory";
    }

    for(index=0; index<Size; index++) {
        GradientX[index] = (IntermediateType) SourceImage[index];
    }
    memcpy(GradientY, GradientX, sizeof(IntermediateType) * Size);

    Filter_1D<IntermediateType, SobelHighPass<IntermediateType> >::
        Execute_2D_Horizontally(GradientX, GradientX, Width, Height);
    Filter_1D<IntermediateType, SobelLowPass<IntermediateType> >::
        Execute_2D_Vertically(GradientX, GradientX, Width, Height);

    Filter_1D<IntermediateType, SobelHighPass<IntermediateType> >::
        Execute_2D_Vertically(GradientY, GradientY, Width, Height);
    Filter_1D<IntermediateType, SobelLowPass<IntermediateType> >::
        Execute_2D_Horizontally(GradientY, GradientY, Width, Height);

    for(index=0; index<Size; index++) {
        double dx, dy;
        dx = (double) GradientX[index];
        dy = (double) GradientY[index];
        Gradient[index] = (T1) sqrt(dx * dx + dy * dy);
    }

    delete[] GradientX;
    delete[] GradientY;
    return Gradient;
}

template<class T1, class T2>
T1* MultiscaleGradient(T2* SourceImage, const int Width, const int Height, const int MaxLevel, const bool Averaged)
{
    T1 *Gradient;
    int level, index, Size;
    T2 *Max, *Min, *Diff;

    if(MaxLevel <= 0) {
        return NULL;
    }

    Size = Width * Height;
    Gradient = new T1[Size];
    Max  = new T2[Size];
    Min  = new T2[Size];
    Diff = new T2[Size];

    if(!Gradient || !Max || !Min || !Diff) {
        throw "MultiscaleGradient => cannot allocate memory";
    }

    memset(Gradient, 0, Size * sizeof(T1));
    memcpy(Min, SourceImage, Size * sizeof(T2));
    memcpy(Max, SourceImage, Size * sizeof(T2));

    for(level=1; level<=MaxLevel; level++) {
        // Max := SourceImage (+) B_{ixi}, where i := 2 * level + 1
        Dilate(Max, Width, Height); 
        // Min := SourceImage (-) B_{ixi}, where i := 2 * level + 1
        Erode(Min, Width, Height);

        for(index=0; index<Size; index++) {
            Diff[index] = Max[index] - Min[index];
        }
        // Diff := (Max - Min) (-) B_{ixi}, where i := 2 * level - 1
        Erode(Diff, Width, Height, level-1); 

        for(index=0; index<Size; index++) {
            Gradient[index] += Diff[index];
        }
    }

    if(Averaged && MaxLevel > 1) {
        for(index=0; index<Size; index++) {
            // This can be improved by multipling the inverse of `MaxLevel'
            // if class T1 is float or double.
            Gradient[index] /= MaxLevel;
        }
    }

    delete[] Max;
    delete[] Min;
    delete[] Diff;

    return Gradient;
}

template<class T>
void ClosingByReconstruction(T* Kernel, const int Width, const int Height, const T H, const int R)
{
    T *Mask;
    int index, Size;

    Size = Width * Height;
    Mask = new T[Size];
    if(!Mask) {
        throw "GenericClosingByReconstruction => cannot allocate memory";
    }

    for(index=0; index<Size; index++) {
        // Overflow??
        Mask[index] = Kernel[index] + H;
    }
    ReconstructByDilation(Kernel, Width, Height, Mask, R);

    for(index=0; index<Size; index++) {
        // Underflow??
        Mask[index] -= H;
    }
    ReconstructByErosion(Kernel, Width, Height, Mask, R);

    delete[] Mask;
}

// -------------------------------------------------------------------------- //
// Instantiation 

template void
Blur<unsigned char>
(unsigned char*, const int, const int);

// GradientType => unsigned short
template unsigned short*
SobelGradient<unsigned short, unsigned char>
(unsigned char*, const int, const int);

template unsigned short* 
MultiscaleGradient<unsigned short, unsigned char>
(unsigned char*, const int, const int, const int, const bool);

template void 
ClosingByReconstruction<unsigned short>
(unsigned short*, const int, const int, const unsigned short, const int);

// GradientType => float
template float*
SobelGradient<float, unsigned char>
(unsigned char*, const int, const int);

template float* 
MultiscaleGradient<float, unsigned char>
(unsigned char*, const int, const int, const int, const bool);

template void 
ClosingByReconstruction<float>
(float*, const int, const int, const float, const int);

// -------------------------------------------------------------------------- //
