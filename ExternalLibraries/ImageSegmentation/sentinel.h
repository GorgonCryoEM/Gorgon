#ifndef SENTINEL_H
#define SENTINEL_H

#include <limits.h>
#include <float.h>
#include <string.h>

#define RIDGE_LABEL (-1)
#define NULL_LABEL  (0)

// GetMax: return the maximal value of a data type
// GetMin: return the minimal value of a data type
// CountableAndSmallRange: whether a data type is suitable to distribution counting sort
#define SET_TYPE_ATTRIBUTE(TYPE,MIN_VALUE,MAX_VALUE,COUNTABLE) \
        inline void GetMax(TYPE& ref) { ref = MAX_VALUE; }     \
        inline void GetMin(TYPE& ref) { ref = MIN_VALUE; }     \
        inline bool CountableAndSmallRange(const TYPE*) { return COUNTABLE; }

SET_TYPE_ATTRIBUTE(char,           CHAR_MIN,  CHAR_MAX,   true)
SET_TYPE_ATTRIBUTE(signed char,    SCHAR_MIN, SCHAR_MAX,  true)
SET_TYPE_ATTRIBUTE(unsigned char,  0,         UCHAR_MAX,  true)
SET_TYPE_ATTRIBUTE(signed short,   SHRT_MIN,  SHRT_MAX,   true)
SET_TYPE_ATTRIBUTE(unsigned short, 0,         USHRT_MAX,  true)
SET_TYPE_ATTRIBUTE(int,            INT_MIN,   INT_MAX,    false)
SET_TYPE_ATTRIBUTE(unsigned int,   0,         UINT_MAX,   false)
SET_TYPE_ATTRIBUTE(long,           LONG_MIN,  LONG_MAX,   false)
SET_TYPE_ATTRIBUTE(unsigned long,  0,         ULONG_MAX,  false)
SET_TYPE_ATTRIBUTE(double,         -DBL_MAX,  DBL_MAX,    false)
SET_TYPE_ATTRIBUTE(float,          -FLT_MAX,  FLT_MAX,    false)

template<class T> inline
T* MakeNewGradient(const T* OldGradient, const int Width, const int Height)
// Making a new gradient image surrounded with sentinels.
{
    T *NewGradient, *new_scan;
    const T *old_scan;
    int x, y;
    T SentinelValue;

    NewGradient = new T[(Width+2)*(Height+2)];
    if(!NewGradient) {
        throw "MakeNewGradient => cannot allocate memory";
    }

    GetMax(SentinelValue);

    new_scan = NewGradient;
    old_scan = OldGradient;

    for(x=0; x<Width+2; x++) {
        new_scan[x] = SentinelValue;
    }
    new_scan += Width+2;
    for(y=1; y<=Height; y++) {
        memcpy(new_scan+1, old_scan, sizeof(T) * Width);
        new_scan[0]       = SentinelValue;
        new_scan[Width+1] = SentinelValue;
        new_scan += Width+2;
        old_scan += Width;
    }
    for(x=0; x<Width+2; x++) {
        new_scan[x] = SentinelValue;
    }

    return NewGradient;
}

#endif
