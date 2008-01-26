#ifndef FILTER_H
#define FILTER_H

template<class T>
void Blur(T* SourceImage, const int Width, const int Height);

template<class T1, class T2>
T1* SobelGradient(T2* SourceImage, const int Width, const int Height);

template<class T1, class T2>
T1* MultiscaleGradient(T2* SourceImage, const int Width, const int Height, const int MaxLevel = 3, const bool Averaged = false);

template<class T>
void ClosingByReconstruction(T* Kernel, const int Width, const int Height, const T H, const int R);

#endif
