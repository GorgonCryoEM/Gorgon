/*************************************************************************
AP library 1.3
Copyright (c) 2003-2009 Sergey Bochkanov (ALGLIB project).

>>> LICENSE >>>
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation (www.fsf.org); either version 2 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

A copy of the GNU General Public License is available at
http://www.fsf.org/licensing/licenses

>>> END OF LICENSE >>>
*************************************************************************/

#ifndef AP_H
#define AP_H

#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>

#ifdef __BORLANDC__
#include <list.h>
#include <vector.h>
#else
#include <list>
#include <vector>
#endif

/********************************************************************
Array bounds check
********************************************************************/
#define AP_ASSERT

#ifndef AP_ASSERT     //
#define NO_AP_ASSERT  // This code avoids definition of the
#endif                // both AP_ASSERT and NO_AP_ASSERT symbols
#ifdef NO_AP_ASSERT   //
#ifdef AP_ASSERT      //
#undef NO_AP_ASSERT   //
#endif                //
#endif                //


/********************************************************************
Current environment.
********************************************************************/
#ifndef AP_WIN32
#ifndef AP_UNKNOWN
#define AP_UNKNOWN
#endif
#endif
#ifdef AP_WIN32
#ifdef AP_UNKNOWN
#error Multiple environments are declared!
#endif
#endif

/********************************************************************
This symbol is used for debugging. Do not define it and do not remove
comments.
********************************************************************/
//#define UNSAFE_MEM_COPY


/********************************************************************
Namespace of a standard library AlgoPascal.
********************************************************************/
namespace ap
{

/********************************************************************
Service routines:
    amalloc - allocates an aligned block of size bytes
    afree - frees block allocated by amalloc
    vlen - just alias for n2-n1+1
********************************************************************/
void* amalloc(size_t size, size_t alignment);
void afree(void *block);
int vlen(int n1, int n2);

/********************************************************************
Exception class.
********************************************************************/
class ap_error
{
public:
    ap_error(){};
    ap_error(const char *s){ msg = s; };

    std::string msg;

    static void make_assertion(bool bClause)
        { if(!bClause) throw ap_error(); };
    static void make_assertion(bool bClause, const char *msg)
        { if(!bClause) throw ap_error(msg); };
private:
};

/********************************************************************
Class defining a complex number with double precision.
********************************************************************/
class complex;

class complex
{
public:
    complex():x(0.0),y(0.0){};
    complex(const double &_x):x(_x),y(0.0){};
    complex(const double &_x, const double &_y):x(_x),y(_y){};
    complex(const complex &z):x(z.x),y(z.y){};

    complex& operator= (const double& v){ x  = v; y = 0.0; return *this; };
    complex& operator+=(const double& v){ x += v;          return *this; };
    complex& operator-=(const double& v){ x -= v;          return *this; };
    complex& operator*=(const double& v){ x *= v; y *= v;  return *this; };
    complex& operator/=(const double& v){ x /= v; y /= v;  return *this; };

    complex& operator= (const complex& z){ x  = z.x; y  = z.y; return *this; };
    complex& operator+=(const complex& z){ x += z.x; y += z.y; return *this; };
    complex& operator-=(const complex& z){ x -= z.x; y -= z.y; return *this; };
    complex& operator*=(const complex& z){ double t = x*z.x-y*z.y; y = x*z.y+y*z.x; x = t; return *this; };
    complex& operator/=(const complex& z)
    {
        ap::complex result;
        double e;
        double f;
        if( fabs(z.y)<fabs(z.x) )
        {
            e = z.y/z.x;
            f = z.x+z.y*e;
            result.x = (z.x+z.y*e)/f;
            result.y = (z.y-z.x*e)/f;
        }
        else
        {
            e = z.x/z.y;
            f = z.y+z.x*e;
            result.x = (z.y+z.x*e)/f;
            result.y = (-z.x+z.y*e)/f;
        }
        *this = result;
        return *this;
    };

    double x, y;
};

const complex operator/(const complex& lhs, const complex& rhs);
const bool operator==(const complex& lhs, const complex& rhs);
const bool operator!=(const complex& lhs, const complex& rhs);
const complex operator+(const complex& lhs);
const complex operator-(const complex& lhs);
const complex operator+(const complex& lhs, const complex& rhs);
const complex operator+(const complex& lhs, const double& rhs);
const complex operator+(const double& lhs, const complex& rhs);
const complex operator-(const complex& lhs, const complex& rhs);
const complex operator-(const complex& lhs, const double& rhs);
const complex operator-(const double& lhs, const complex& rhs);
const complex operator*(const complex& lhs, const complex& rhs);
const complex operator*(const complex& lhs, const double& rhs);
const complex operator*(const double& lhs, const complex& rhs);
const complex operator/(const complex& lhs, const complex& rhs);
const complex operator/(const double& lhs, const complex& rhs);
const complex operator/(const complex& lhs, const double& rhs);
const double abscomplex(const complex &z);
const complex conj(const complex &z);
const complex csqr(const complex &z);


/********************************************************************
Templates for vector operations
********************************************************************/
#include "apvt.h"

/********************************************************************
BLAS functions
********************************************************************/
double vdotproduct(const double *v1, const double *v2, int N);
complex vdotproduct(const complex *v1, const complex *v2, int N);

void vmove(double *vdst, const double* vsrc, int N);
void vmove(complex *vdst, const complex* vsrc, int N);

void vmoveneg(double *vdst, const double *vsrc, int N);
void vmoveneg(complex *vdst, const complex *vsrc, int N);

void vmove(double *vdst, const double *vsrc, int N, double alpha);
void vmove(complex *vdst, const complex *vsrc, int N, double alpha);
void vmove(complex *vdst, const complex *vsrc, int N, complex alpha);

void vadd(double *vdst, const double *vsrc, int N);
void vadd(complex *vdst, const complex *vsrc, int N);

void vadd(double *vdst, const double *vsrc, int N, double alpha);
void vadd(complex *vdst, const complex *vsrc, int N, double alpha);
void vadd(complex *vdst, const complex *vsrc, int N, complex alpha);

void vsub(double *vdst, const double *vsrc, int N);
void vsub(complex *vdst, const complex *vsrc, int N);

void vsub(double *vdst, const double *vsrc, int N, double alpha);
void vsub(complex *vdst, const complex *vsrc, int N, double alpha);
void vsub(complex *vdst, const complex *vsrc, int N, complex alpha);

void vmul(double *vdst, int N, double alpha);
void vmul(complex *vdst, int N, double alpha);
void vmul(complex *vdst, int N, complex alpha);


/********************************************************************
Template of a dynamical one-dimensional array
********************************************************************/
template<class T, bool Aligned = false>
class template_1d_array
{
public:
    template_1d_array()
    {
        m_Vec=0;
        m_iVecSize = 0;
        m_iLow = 0;
        m_iHigh = -1;
    };

    ~template_1d_array()
    {
        if(m_Vec)
        {
            if( Aligned )
                ap::afree(m_Vec);
            else
                delete[] m_Vec;
        }
    };

    template_1d_array(const template_1d_array &rhs)
    {
        m_Vec=0;
        m_iVecSize = 0;
        m_iLow = 0;
        m_iHigh = -1;
        if( rhs.m_iVecSize!=0 )
            setcontent(rhs.m_iLow, rhs.m_iHigh, rhs.getcontent());
    };


    const template_1d_array& operator=(const template_1d_array &rhs)
    {
        if( this==&rhs )
            return *this;

        if( rhs.m_iVecSize!=0 )
            setcontent(rhs.m_iLow, rhs.m_iHigh, rhs.getcontent());
        else
        {
            m_Vec=0;
            m_iVecSize = 0;
            m_iLow = 0;
            m_iHigh = -1;
        }
        return *this;
    };


    const T& operator()(int i) const
    {
        #ifndef NO_AP_ASSERT
        ap_error::make_assertion(i>=m_iLow && i<=m_iHigh);
        #endif
        return m_Vec[ i-m_iLow ];
    };


    T& operator()(int i)
    {
        #ifndef NO_AP_ASSERT
        ap_error::make_assertion(i>=m_iLow && i<=m_iHigh);
        #endif
        return m_Vec[ i-m_iLow ];
    };


    void setbounds( int iLow, int iHigh )
    {
        if(m_Vec)
        {
            if( Aligned )
                ap::afree(m_Vec);
            else
                delete[] m_Vec;
        }
        m_iLow = iLow;
        m_iHigh = iHigh;
        m_iVecSize = iHigh-iLow+1;
        if( Aligned )
            m_Vec = (T*)ap::amalloc(m_iVecSize*sizeof(T), 16);
        else
            m_Vec = new T[m_iVecSize];
    };


    void setlength(int iLen)
    {
        setbounds(0, iLen-1);
    }


    void setcontent( int iLow, int iHigh, const T *pContent )
    {
        setbounds(iLow, iHigh);
        for(int i=0; i<m_iVecSize; i++)
            m_Vec[i] = pContent[i];
    };


    T* getcontent()
    {
        return m_Vec;
    };

    const T* getcontent() const
    {
        return m_Vec;
    };


    int getlowbound(int iBoundNum = 0) const
    {
        return m_iLow;
    };


    int gethighbound(int iBoundNum = 0) const
    {
        return m_iHigh;
    };

    raw_vector<T> getvector(int iStart, int iEnd)
    {
        if( iStart>iEnd || wrongIdx(iStart) || wrongIdx(iEnd) )
            return raw_vector<T>(0, 0, 1);
        else
            return raw_vector<T>(m_Vec+iStart-m_iLow, iEnd-iStart+1, 1);
    };


    const_raw_vector<T> getvector(int iStart, int iEnd) const
    {
        if( iStart>iEnd || wrongIdx(iStart) || wrongIdx(iEnd) )
            return const_raw_vector<T>(0, 0, 1);
        else
            return const_raw_vector<T>(m_Vec+iStart-m_iLow, iEnd-iStart+1, 1);
    };
private:
    bool wrongIdx(int i) const { return i<m_iLow || i>m_iHigh; };

    T         *m_Vec;
    long      m_iVecSize;
    long      m_iLow, m_iHigh;
};



/********************************************************************
Template of a dynamical two-dimensional array
********************************************************************/
template<class T, bool Aligned = false>
class template_2d_array
{
public:
    template_2d_array()
    {
        m_Vec=0;
        m_iVecSize=0;
        m_iLow1 = 0;
        m_iHigh1 = -1;
        m_iLow2 = 0;
        m_iHigh2 = -1;
    };

    ~template_2d_array()
    {
        if(m_Vec)
        {
            if( Aligned )
                ap::afree(m_Vec);
            else
                delete[] m_Vec;
        }
    };

    template_2d_array(const template_2d_array &rhs)
    {
        m_Vec=0;
        m_iVecSize=0;
        m_iLow1 = 0;
        m_iHigh1 = -1;
        m_iLow2 = 0;
        m_iHigh2 = -1;
        if( rhs.m_iVecSize!=0 )
        {
            setbounds(rhs.m_iLow1, rhs.m_iHigh1, rhs.m_iLow2, rhs.m_iHigh2);
            for(int i=m_iLow1; i<=m_iHigh1; i++)
                for(int j=m_iLow2; j<=m_iHigh2; j++)
                    operator()(i,j) = rhs(i,j);
                //vmove(&(operator()(i,m_iLow2)), &(rhs(i,m_iLow2)), m_iHigh2-m_iLow2+1);
        }
    };
    const template_2d_array& operator=(const template_2d_array &rhs)
    {
        if( this==&rhs )
            return *this;

        if( rhs.m_iVecSize!=0 )
        {
            setbounds(rhs.m_iLow1, rhs.m_iHigh1, rhs.m_iLow2, rhs.m_iHigh2);
            for(int i=m_iLow1; i<=m_iHigh1; i++)
                for(int j=m_iLow2; j<=m_iHigh2; j++)
                    operator()(i,j) = rhs(i,j);
                //vmove(&(operator()(i,m_iLow2)), &(rhs(i,m_iLow2)), m_iHigh2-m_iLow2+1);
        }
        else
        {
            if(m_Vec)
            {
                if( Aligned )
                    ap::afree(m_Vec);
                else
                    delete[] m_Vec;
            }
            m_Vec=0;
            m_iVecSize=0;
            m_iLow1 = 0;
            m_iHigh1 = -1;
            m_iLow2 = 0;
            m_iHigh2 = -1;
        }
        return *this;
    };

    const T& operator()(int i1, int i2) const
    {
        #ifndef NO_AP_ASSERT
        ap_error::make_assertion(i1>=m_iLow1 && i1<=m_iHigh1);
        ap_error::make_assertion(i2>=m_iLow2 && i2<=m_iHigh2);
        #endif
        return m_Vec[ m_iConstOffset + i2 +i1*m_iLinearMember];
    };

    T& operator()(int i1, int i2)
    {
        #ifndef NO_AP_ASSERT
        ap_error::make_assertion(i1>=m_iLow1 && i1<=m_iHigh1);
        ap_error::make_assertion(i2>=m_iLow2 && i2<=m_iHigh2);
        #endif
        return m_Vec[ m_iConstOffset + i2 +i1*m_iLinearMember];
    };

    void setbounds( int iLow1, int iHigh1, int iLow2, int iHigh2 )
    {
        if(m_Vec)
        {
            if( Aligned )
                ap::afree(m_Vec);
            else
                delete[] m_Vec;
        }
        int n1 = iHigh1-iLow1+1;
        int n2 = iHigh2-iLow2+1;
        m_iVecSize = n1*n2;
        if( Aligned )
        {
            //if( n2%2!=0 )
            while( (n2*sizeof(T))%16!=0 )
            {
                n2++;
                m_iVecSize += n1;
            }
            m_Vec = (T*)ap::amalloc(m_iVecSize*sizeof(T), 16);
        }
        else
            m_Vec = new T[m_iVecSize];
        m_iLow1  = iLow1;
        m_iHigh1 = iHigh1;
        m_iLow2  = iLow2;
        m_iHigh2 = iHigh2;
        m_iConstOffset = -m_iLow2-m_iLow1*n2;
        m_iLinearMember = n2;
    };

    void setlength(int iLen1, int iLen2)
    {
        setbounds(0, iLen1-1, 0, iLen2-1);
    }

    void setcontent( int iLow1, int iHigh1, int iLow2, int iHigh2, const T *pContent )
    {
        setbounds(iLow1, iHigh1, iLow2, iHigh2);
        for(int i=m_iLow1; i<=m_iHigh1; i++, pContent += m_iHigh2-m_iLow2+1)
            for(int j=m_iLow2; j<=m_iHigh2; j++)
                operator()(i,j) = pContent[j-m_iLow2];
            //vmove(&(operator()(i,m_iLow2)), pContent, m_iHigh2-m_iLow2+1);
    };

    int getlowbound(int iBoundNum) const
    {
        return iBoundNum==1 ? m_iLow1 : m_iLow2;
    };

    int gethighbound(int iBoundNum) const
    {
        return iBoundNum==1 ? m_iHigh1 : m_iHigh2;
    };

    raw_vector<T> getcolumn(int iColumn, int iRowStart, int iRowEnd)
    {
        if( (iRowStart>iRowEnd) || wrongColumn(iColumn) || wrongRow(iRowStart) ||wrongRow(iRowEnd) )
            return raw_vector<T>(0, 0, 1);
        else
            return raw_vector<T>(&((*this)(iRowStart, iColumn)), iRowEnd-iRowStart+1, m_iLinearMember);
    };

    raw_vector<T> getrow(int iRow, int iColumnStart, int iColumnEnd)
    {
        if( (iColumnStart>iColumnEnd) || wrongRow(iRow) || wrongColumn(iColumnStart) || wrongColumn(iColumnEnd))
            return raw_vector<T>(0, 0, 1);
        else
            return raw_vector<T>(&((*this)(iRow, iColumnStart)), iColumnEnd-iColumnStart+1, 1);
    };

    const_raw_vector<T> getcolumn(int iColumn, int iRowStart, int iRowEnd) const
    {
        if( (iRowStart>iRowEnd) || wrongColumn(iColumn) || wrongRow(iRowStart) ||wrongRow(iRowEnd) )
            return const_raw_vector<T>(0, 0, 1);
        else
            return const_raw_vector<T>(&((*this)(iRowStart, iColumn)), iRowEnd-iRowStart+1, m_iLinearMember);
    };

    const_raw_vector<T> getrow(int iRow, int iColumnStart, int iColumnEnd) const
    {
        if( (iColumnStart>iColumnEnd) || wrongRow(iRow) || wrongColumn(iColumnStart) || wrongColumn(iColumnEnd))
            return const_raw_vector<T>(0, 0, 1);
        else
            return const_raw_vector<T>(&((*this)(iRow, iColumnStart)), iColumnEnd-iColumnStart+1, 1);
    };
private:
    bool wrongRow(int i) const { return i<m_iLow1 || i>m_iHigh1; };
    bool wrongColumn(int j) const { return j<m_iLow2 || j>m_iHigh2; };

    T           *m_Vec;
    long        m_iVecSize;
    long        m_iLow1, m_iLow2, m_iHigh1, m_iHigh2;
    long        m_iConstOffset, m_iLinearMember;
};


typedef template_1d_array<int>          integer_1d_array;
typedef template_1d_array<double,true>  real_1d_array;
typedef template_1d_array<complex>      complex_1d_array;
typedef template_1d_array<bool>         boolean_1d_array;

typedef template_2d_array<int>          integer_2d_array;
typedef template_2d_array<double,true>  real_2d_array;
typedef template_2d_array<complex>      complex_2d_array;
typedef template_2d_array<bool>         boolean_2d_array;


/********************************************************************
dataset information.

can store regression dataset, classification dataset, or non-labeled
task:
* nout==0 means non-labeled task (clustering, for example)
* nout>0 && nclasses==0 means regression task
* nout>0 && nclasses>0 means classification task
********************************************************************/
/*class dataset
{
public:
    dataset():nin(0), nout(0), nclasses(0), trnsize(0), valsize(0), tstsize(0), totalsize(0){};

    int nin, nout, nclasses;

    int trnsize;
    int valsize;
    int tstsize;
    int totalsize;

    ap::real_2d_array trn;
    ap::real_2d_array val;
    ap::real_2d_array tst;
    ap::real_2d_array all;
};

bool opendataset(std::string file, dataset *pdataset);

//
// internal functions
//
std::string strtolower(const std::string &s);
bool readstrings(std::string file, std::list<std::string> *pOutput);
bool readstrings(std::string file, std::list<std::string> *pOutput, std::string comment);
void explodestring(std::string s, char sep, std::vector<std::string> *pOutput);
std::string xtrim(std::string s);*/

/********************************************************************
reverse communication state
********************************************************************/
struct rcommstate
{
    int stage;
    ap::integer_1d_array ia;
    ap::boolean_1d_array ba;
    ap::real_1d_array ra;
    ap::complex_1d_array ca;
};


/********************************************************************
Constants and functions introduced for compatibility with AlgoPascal
********************************************************************/
extern const double machineepsilon;
extern const double maxrealnumber;
extern const double minrealnumber;

int sign(double x);
double randomreal();
int randominteger(int maxv);
//int round(double x);
int trunc(double x);
int ifloor(double x);
int iceil(double x);
double pi();
double sqr(double x);
int maxint(int m1, int m2);
int minint(int m1, int m2);
double maxreal(double m1, double m2);
double minreal(double m1, double m2);

}//namespace ap


/*************************************************************************
AP library 1.3
Copyright (c) 2003-2009 Sergey Bochkanov (ALGLIB project).

>>> LICENSE >>>
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation (www.fsf.org); either version 2 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

A copy of the GNU General Public License is available at
http://www.fsf.org/licensing/licenses

>>> END OF LICENSE >>>
*************************************************************************/

//#include "stdafx.h"


/********************************************************************
Optimized ABLAS interface
********************************************************************/
#ifdef AP_WIN32
#include <windows.h>
extern "C"
{
typedef double  (*_ddot1)(const double*, const double*, long);
typedef void    (*_dmove1)(const double*, const double*, long);
typedef void    (*_dmoves1)(const double*, const double*, long, double);
typedef void    (*_dmoveneg1)(const double*, const double*, long);
typedef void    (*_dadd1)(const double*, const double*, long);
typedef void    (*_dadds1)(const double*, const double*, long, double);
typedef void    (*_dsub1)(const double*, const double*, long);
typedef void    (*_dmuls1)(const double*, long, double);
}
HINSTANCE ABLAS = LoadLibrary("ablas.dll");

static _ddot1     ddot1     = ABLAS==NULL ? NULL :     (_ddot1)  GetProcAddress(ABLAS, "ASMDotProduct1");
static _dmove1    dmove1    = ABLAS==NULL ? NULL :    (_dmove1)  GetProcAddress(ABLAS, "ASMMove1");
static _dmoves1   dmoves1   = ABLAS==NULL ? NULL :   (_dmoves1)  GetProcAddress(ABLAS, "ASMMoveS1");
static _dmoveneg1 dmoveneg1 = ABLAS==NULL ? NULL : (_dmoveneg1)  GetProcAddress(ABLAS, "ASMMoveNeg1");
static _dadd1     dadd1     = ABLAS==NULL ? NULL :     (_dadd1)  GetProcAddress(ABLAS, "ASMAdd1");
static _dadds1    dadds1    = ABLAS==NULL ? NULL :    (_dadds1)  GetProcAddress(ABLAS, "ASMAddS1");
static _dsub1     dsub1     = ABLAS==NULL ? NULL :     (_dsub1)  GetProcAddress(ABLAS, "ASMSub1");
static _dmuls1    dmuls1    = ABLAS==NULL ? NULL :     (_dmuls1) GetProcAddress(ABLAS, "ASMMulS1");
#endif

const double ap::machineepsilon = 5E-16;
const double ap::maxrealnumber  = 1E300;
const double ap::minrealnumber  = 1E-300;

/********************************************************************
ap::complex operations
********************************************************************/
const bool ap::operator==(const ap::complex& lhs, const ap::complex& rhs)
{ return lhs.x==rhs.x && lhs.y==rhs.y; }

const bool ap::operator!=(const ap::complex& lhs, const ap::complex& rhs)
{ return lhs.x!=rhs.x || lhs.y!=rhs.y; }

const ap::complex ap::operator+(const ap::complex& lhs)
{ return lhs; }

const ap::complex ap::operator-(const ap::complex& lhs)
{ return ap::complex(-lhs.x, -lhs.y); }

const ap::complex ap::operator+(const ap::complex& lhs, const ap::complex& rhs)
{ ap::complex r = lhs; r += rhs; return r; }

const ap::complex ap::operator+(const ap::complex& lhs, const double& rhs)
{ ap::complex r = lhs; r += rhs; return r; }

const ap::complex ap::operator+(const double& lhs, const ap::complex& rhs)
{ ap::complex r = rhs; r += lhs; return r; }

const ap::complex ap::operator-(const ap::complex& lhs, const ap::complex& rhs)
{ ap::complex r = lhs; r -= rhs; return r; }

const ap::complex ap::operator-(const ap::complex& lhs, const double& rhs)
{ ap::complex r = lhs; r -= rhs; return r; }

const ap::complex ap::operator-(const double& lhs, const ap::complex& rhs)
{ ap::complex r = lhs; r -= rhs; return r; }

const ap::complex ap::operator*(const ap::complex& lhs, const ap::complex& rhs)
{ return ap::complex(lhs.x*rhs.x - lhs.y*rhs.y,  lhs.x*rhs.y + lhs.y*rhs.x); }

const ap::complex ap::operator*(const ap::complex& lhs, const double& rhs)
{ return ap::complex(lhs.x*rhs,  lhs.y*rhs); }

const ap::complex ap::operator*(const double& lhs, const ap::complex& rhs)
{ return ap::complex(lhs*rhs.x,  lhs*rhs.y); }

const ap::complex ap::operator/(const ap::complex& lhs, const ap::complex& rhs)
{
    ap::complex result;
    double e;
    double f;
    if( fabs(rhs.y)<fabs(rhs.x) )
    {
        e = rhs.y/rhs.x;
        f = rhs.x+rhs.y*e;
        result.x = (lhs.x+lhs.y*e)/f;
        result.y = (lhs.y-lhs.x*e)/f;
    }
    else
    {
        e = rhs.x/rhs.y;
        f = rhs.y+rhs.x*e;
        result.x = (lhs.y+lhs.x*e)/f;
        result.y = (-lhs.x+lhs.y*e)/f;
    }
    return result;
}

const ap::complex ap::operator/(const double& lhs, const ap::complex& rhs)
{
    ap::complex result;
    double e;
    double f;
    if( fabs(rhs.y)<fabs(rhs.x) )
    {
        e = rhs.y/rhs.x;
        f = rhs.x+rhs.y*e;
        result.x = lhs/f;
        result.y = -lhs*e/f;
    }
    else
    {
        e = rhs.x/rhs.y;
        f = rhs.y+rhs.x*e;
        result.x = lhs*e/f;
        result.y = -lhs/f;
    }
    return result;
}

const ap::complex ap::operator/(const ap::complex& lhs, const double& rhs)
{ return ap::complex(lhs.x/rhs, lhs.y/rhs); }

const double ap::abscomplex(const ap::complex &z)
{
    double w;
    double xabs;
    double yabs;
    double v;

    xabs = fabs(z.x);
    yabs = fabs(z.y);
    w = xabs>yabs ? xabs : yabs;
    v = xabs<yabs ? xabs : yabs; 
    if( v==0 )
        return w;
    else
    {
        double t = v/w;
        return w*sqrt(1+t*t);
    }
}

const ap::complex ap::conj(const ap::complex &z)
{ return ap::complex(z.x, -z.y); }

const ap::complex ap::csqr(const ap::complex &z)
{ return ap::complex(z.x*z.x-z.y*z.y, 2*z.x*z.y); }

/********************************************************************
BLAS functions
********************************************************************/
double ap::vdotproduct(const double *v1, const double *v2, int N)
{
#ifdef AP_WIN32
    if( ddot1!=NULL )
        return ddot1(v1, v2, N);
#endif
    return ap::_vdotproduct<double>(v1, v2, N);
}

ap::complex ap::vdotproduct(const ap::complex *v1, const ap::complex *v2, int N)
{
    return ap::_vdotproduct<ap::complex>(v1, v2, N);
}

void ap::vmove(double *vdst, const double* vsrc, int N)
{
#ifdef AP_WIN32
    if( dmove1!=NULL )
    {
        dmove1(vdst, vsrc, N);
        return;
    }
#endif
    ap::_vmove<double>(vdst, vsrc, N);
}

void ap::vmove(ap::complex *vdst, const ap::complex* vsrc, int N)
{
    ap::_vmove<ap::complex>(vdst, vsrc, N);
}

void ap::vmoveneg(double *vdst, const double *vsrc, int N)
{
#ifdef AP_WIN32
    if( dmoveneg1!=NULL )
    {
        dmoveneg1(vdst, vsrc, N);
        return;
    }
#endif
    ap::_vmoveneg<double>(vdst, vsrc, N);
}

void ap::vmoveneg(ap::complex *vdst, const ap::complex *vsrc, int N)
{
    ap::_vmoveneg<ap::complex>(vdst, vsrc, N);
}

void ap::vmove(double *vdst, const double *vsrc, int N, double alpha)
{
#ifdef AP_WIN32
    if( dmoves1!=NULL )
    {
        dmoves1(vdst, vsrc, N, alpha);
        return;
    }
#endif
    ap::_vmove<double,double>(vdst, vsrc, N, alpha);
}

void ap::vmove(ap::complex *vdst, const ap::complex *vsrc, int N, double alpha)
{
    ap::_vmove<ap::complex,double>(vdst, vsrc, N, alpha);
}

void ap::vmove(ap::complex *vdst, const ap::complex *vsrc, int N, ap::complex alpha)
{
    ap::_vmove<ap::complex,ap::complex>(vdst, vsrc, N, alpha);
}

void ap::vadd(double *vdst, const double *vsrc, int N)
{
#ifdef AP_WIN32
    if( dadd1!=NULL )
    {
        dadd1(vdst, vsrc, N);
        return;
    }
#endif
    ap::_vadd<double>(vdst, vsrc, N);
}

void ap::vadd(ap::complex *vdst, const ap::complex *vsrc, int N)
{
    ap::_vadd<ap::complex>(vdst, vsrc, N);
}

void ap::vadd(double *vdst, const double *vsrc, int N, double alpha)
{
#ifdef AP_WIN32
    if( dadds1!=NULL )
    {
        dadds1(vdst, vsrc, N, alpha);
        return;
    }
#endif
    ap::_vadd<double,double>(vdst, vsrc, N, alpha);
}

void ap::vadd(ap::complex *vdst, const ap::complex *vsrc, int N, double alpha)
{
    ap::_vadd<ap::complex,double>(vdst, vsrc, N, alpha);
}

void ap::vadd(ap::complex *vdst, const ap::complex *vsrc, int N, ap::complex alpha)
{
    ap::_vadd<ap::complex,ap::complex>(vdst, vsrc, N, alpha);
}

void ap::vsub(double *vdst, const double *vsrc, int N)
{
#ifdef AP_WIN32
    if( dsub1!=NULL )
    {
        dsub1(vdst, vsrc, N);
        return;
    }
#endif
    ap::_vsub<double>(vdst, vsrc, N);
}

void ap::vsub(ap::complex *vdst, const ap::complex *vsrc, int N)
{
    ap::_vsub<ap::complex>(vdst, vsrc, N);
}

void ap::vsub(double *vdst, const double *vsrc, int N, double alpha)
{
#ifdef AP_WIN32
    if( dadds1!=NULL )
    {
        dadds1(vdst, vsrc, N, -alpha);
        return;
    }
#endif
    ap::_vsub<double,double>(vdst, vsrc, N, alpha);
}

void ap::vsub(ap::complex *vdst, const ap::complex *vsrc, int N, double alpha)
{
    ap::_vsub<ap::complex,double>(vdst, vsrc, N, alpha);
}

void ap::vsub(ap::complex *vdst, const ap::complex *vsrc, int N, ap::complex alpha)
{
    ap::_vsub<ap::complex,ap::complex>(vdst, vsrc, N, alpha);
}

void ap::vmul(double *vdst, int N, double alpha)
{
#ifdef AP_WIN32
    if( dmuls1!=NULL )
    {
        dmuls1(vdst, N, alpha);
        return;
    }
#endif
    ap::_vmul<double,double>(vdst, N, alpha);
}

void ap::vmul(ap::complex *vdst, int N, double alpha)
{
    ap::_vmul<ap::complex,double>(vdst, N, alpha);
}

void ap::vmul(ap::complex *vdst, int N, ap::complex alpha)
{
    ap::_vmul<ap::complex,ap::complex>(vdst, N, alpha);
}

/********************************************************************
standard functions
********************************************************************/
int ap::sign(double x)
{
    if( x>0 ) return  1;
    if( x<0 ) return -1;
    return 0;
}

double ap::randomreal()
{
    int i1 = rand();
    int i2 = rand();
    while(i1==RAND_MAX)
        i1 =rand();
    while(i2==RAND_MAX)
        i2 =rand();
    double mx = RAND_MAX;
    return (i1+i2/mx)/mx;
}

int ap::randominteger(int maxv)
{  return rand()%maxv; }

/*int ap::round(double x)
{ return int(floor(x+0.5)); }*/

int ap::trunc(double x)
{ return int(x>0 ? floor(x) : ceil(x)); }

int ap::ifloor(double x)
{ return int(floor(x)); }

int ap::iceil(double x)
{ return int(ceil(x)); }

double ap::pi()
{ return 3.14159265358979323846; }

double ap::sqr(double x)
{ return x*x; }

int ap::maxint(int m1, int m2)
{
    return m1>m2 ? m1 : m2;
}

int ap::minint(int m1, int m2)
{
    return m1>m2 ? m2 : m1;
}

double ap::maxreal(double m1, double m2)
{
    return m1>m2 ? m1 : m2;
}

double ap::minreal(double m1, double m2)
{
    return m1>m2 ? m2 : m1;
}

/********************************************************************
Dataset functions
********************************************************************/
/*bool ap::readstrings(std::string file, std::list<std::string> *pOutput)
{
    return readstrings(file, pOutput, "");
}

bool ap::readstrings(std::string file, std::list<std::string> *pOutput, std::string comment)
{
    std::string cmd, s;
    FILE *f;
    char buf[32768];
    char *str;

    f = fopen(file.c_str(), "rb");
    if( !f )
        return false;
    s = "";
    pOutput->clear();
    while(str=fgets(buf, sizeof(buf), f))
    {
        // TODO: read file by small chunks, combine in one large string
        if( strlen(str)==0 )
            continue;
            
        //
        // trim trailing newline chars
        //
        char *eos = str+strlen(str)-1;
        if( *eos=='\n' )
        {
            *eos = 0;
            eos--;
        }
        if( *eos=='\r' )
        {
            *eos = 0;
            eos--;
        }
        s = str;

        //
        // skip comments
        //
        if( comment.length()>0 )
            if( strncmp(s.c_str(), comment.c_str(), comment.length())==0 )
            {
                s = "";
                continue;
            }

        //
        // read data
        //
        if( s.length()<1 )
        {
            fclose(f);
            throw ap::ap_error("internal error in read_strings");
        }
        pOutput->push_back(s);
    }
    fclose(f);
    return true;
}

void ap::explodestring(std::string s, char sep, std::vector<std::string> *pOutput)
{
    std::string tmp;
    int i;
    tmp = "";
    pOutput->clear();
    for(i=0; i<s.length(); i++)
    {
        if( s[i]!=sep )
        {
            tmp += s[i];
            continue;
        }
        //if( tmp.length()!=0 )
        pOutput->push_back(tmp);
        tmp = "";
    }
    if( tmp.length()!=0 )
        pOutput->push_back(tmp);
}

std::string ap::strtolower(const std::string &s)
{
    std::string r = s;
    for(int i=0; i<r.length(); i++)
        r[i] = tolower(r[i]);
    return r;
}

std::string ap::xtrim(std::string s)
{
    char *pstr = (char*)malloc(s.length()+1);
    char *p2 = pstr;
    if( pstr==NULL )
        throw "xalloc in xtrim()";
    try
    {
        bool bws;
        int i;

        //
        // special cases:
        // * zero length string
        // * string includes only spaces
        //
        if( s.length()==0 )
        {
            free(pstr);
            return "";
        }
        bws = true;
        for(i=0; i<s.length(); i++)
            if( s[i]!=' ' )
                bws = false;
        if( bws )
        {
            free(pstr);
            return "";
        }

        //
        // merge internal spaces
        //
        bws = false;
        for(i=0; i<s.length(); i++)
        {
            if( s[i]==' ' && bws )
                continue;
            if( s[i]==' ' )
            {
                *p2 = ' ';
                p2++;
                bws = true;
                continue;
            }
            *p2 = s[i];
            bws = false;
            p2++;
        }
        *p2 = 0;

        //
        // trim leading/trailing spaces.
        // we expect at least one non-space character in the string
        //
        p2--;
        while(*p2==' ')
        {
            *p2 = 0;
            p2--;
        }
        p2 = pstr;
        while((*p2)==' ')
            p2++;

        //
        // result
        //
        std::string r = p2;
        free(pstr);
        return r;
    }
    catch(...)
    {
        free(pstr);
        throw "unknown exception in xtrim()";
    }
}

bool ap::opendataset(std::string file, dataset *pdataset)
{
    std::list<std::string> Lines;
    std::vector<std::string> Values, RowsArr, ColsArr, VarsArr, HeadArr;
    std::list<std::string>::iterator i;
    std::string s;
    int TrnFirst, TrnLast, ValFirst, ValLast, TstFirst, TstLast, LinesRead, j;

    //
    // Read data
    //
    if( pdataset==NULL )
        return false;
    if( !readstrings(file, &Lines, "//") )
        return false;
    i = Lines.begin();
    *pdataset = dataset();

    //
    // Read header
    //
    if( i==Lines.end() )
        return false;
    s = ap::xtrim(*i);
    ap::explodestring(s, '#', &HeadArr);
    if( HeadArr.size()!=2 )
        return false;

    //
    // Rows info
    //
    ap::explodestring(ap::xtrim(HeadArr[0]), ' ', &RowsArr);
    if( RowsArr.size()==0 || RowsArr.size()>3 )
        return false;
    if( RowsArr.size()==1 )
    {
        pdataset->totalsize = atol(RowsArr[0].c_str());
        pdataset->trnsize = pdataset->totalsize;
    }
    if( RowsArr.size()==2 )
    {
        pdataset->trnsize = atol(RowsArr[0].c_str());
        pdataset->tstsize = atol(RowsArr[1].c_str());
        pdataset->totalsize = pdataset->trnsize + pdataset->tstsize;
    }
    if( RowsArr.size()==3 )
    {
        pdataset->trnsize = atol(RowsArr[0].c_str());
        pdataset->valsize = atol(RowsArr[1].c_str());
        pdataset->tstsize = atol(RowsArr[2].c_str());
        pdataset->totalsize = pdataset->trnsize + pdataset->valsize + pdataset->tstsize;
    }
    if( pdataset->totalsize<=0 || pdataset->trnsize<0 || pdataset->valsize<0 || pdataset->tstsize<0 )
        return false;
    TrnFirst = 0;
    TrnLast = TrnFirst + pdataset->trnsize;
    ValFirst = TrnLast;
    ValLast = ValFirst + pdataset->valsize;
    TstFirst = ValLast;
    TstLast = TstFirst + pdataset->tstsize;
                
    //
    // columns
    //
    ap::explodestring(ap::xtrim(HeadArr[1]), ' ', &ColsArr);
    if( ColsArr.size()!=1 && ColsArr.size()!=4 )
        return false;
    if( ColsArr.size()==1 )
    {
        pdataset->nin = atoi(ColsArr[0].c_str());
        if( pdataset->nin<=0 )
            return false;
    }
    if( ColsArr.size()==4 )
    {
        if( ap::strtolower(ColsArr[0])!="reg" && ap::strtolower(ColsArr[0])!="cls" )
            return false;
        if( ColsArr[2]!="=>" )
            return false;
        pdataset->nin = atol(ColsArr[1].c_str());
        if( pdataset->nin<1 )
            return false;
        if( ap::strtolower(ColsArr[0])=="reg" )
        {
            pdataset->nclasses = 0;
            pdataset->nout = atol(ColsArr[3].c_str());
            if( pdataset->nout<1 )
                return false;
        }
        else
        {
            pdataset->nclasses = atol(ColsArr[3].c_str());
            pdataset->nout = 1;
            if( pdataset->nclasses<2 )
                return false;
        }
    }

    //
    // initialize arrays
    //
    pdataset->all.setlength(pdataset->totalsize, pdataset->nin+pdataset->nout);
    if( pdataset->trnsize>0 ) pdataset->trn.setlength(pdataset->trnsize, pdataset->nin+pdataset->nout);
    if( pdataset->valsize>0 ) pdataset->val.setlength(pdataset->valsize, pdataset->nin+pdataset->nout);
    if( pdataset->tstsize>0 ) pdataset->tst.setlength(pdataset->tstsize, pdataset->nin+pdataset->nout);

    //
    // read data
    //
    for(LinesRead=0, i++; i!=Lines.end() && LinesRead<pdataset->totalsize; i++, LinesRead++)
    {
        std::string sss = *i;
        ap::explodestring(ap::xtrim(*i), ' ', &VarsArr);
        if( VarsArr.size()!=pdataset->nin+pdataset->nout )
            return false;
        int tmpc = ap::round(atof(VarsArr[pdataset->nin+pdataset->nout-1].c_str()));
        if( pdataset->nclasses>0 && (tmpc<0 || tmpc>=pdataset->nclasses) )
            return false;
        for(j=0; j<pdataset->nin+pdataset->nout; j++)
        {
            pdataset->all(LinesRead,j) = atof(VarsArr[j].c_str());
            if( LinesRead>=TrnFirst && LinesRead<TrnLast )
                pdataset->trn(LinesRead-TrnFirst,j) = atof(VarsArr[j].c_str());
            if( LinesRead>=ValFirst && LinesRead<ValLast )
                pdataset->val(LinesRead-ValFirst,j) = atof(VarsArr[j].c_str());
            if( LinesRead>=TstFirst && LinesRead<TstLast )
                pdataset->tst(LinesRead-TstFirst,j) = atof(VarsArr[j].c_str());
        }
    }
    if( LinesRead!=pdataset->totalsize )
        return false;
    return true;
}*/

/*
previous variant
bool ap::opendataset(std::string file, dataset *pdataset)
{
    std::list<std::string> Lines;
    std::vector<std::string> Values;
    std::list<std::string>::iterator i;
    int nCol, nRow, nSplitted;
    int nColumns, nRows;

    //
    // Read data
    //
    if( pdataset==NULL )
        return false;
    if( !readstrings(file, &Lines, "//") )
        return false;
    i = Lines.begin();
    *pdataset = dataset();

    //
    // Read columns info
    //
    if( i==Lines.end() )
        return false;
    if( sscanf(i->c_str(), " columns = %d %d ", &pdataset->nin, &pdataset->nout)!=2 )
        return false;
    if( pdataset->nin<=0 || pdataset->nout==0 || pdataset->nout==-1)
        return false;
    if( pdataset->nout<0 )
    {
        pdataset->nclasses = -pdataset->nout;
        pdataset->nout = 1;
        pdataset->iscls = true;
    }
    else
    {
        pdataset->isreg = true;
    }
    nColumns = pdataset->nin+pdataset->nout;
    i++;

    //
    // Read rows info
    //
    if( i==Lines.end() )
        return false;
    if( sscanf(i->c_str(), " rows = %d %d %d ", &pdataset->trnsize, &pdataset->valsize, &pdataset->tstsize)!=3 )
        return false;
    if( (pdataset->trnsize<0) || (pdataset->valsize<0) || (pdataset->tstsize<0) )
        return false;
    if( (pdataset->trnsize==0) && (pdataset->valsize==0) && (pdataset->tstsize==0) )
        return false;
    nRows = pdataset->trnsize+pdataset->valsize+pdataset->tstsize;
    pdataset->size = nRows;
    if( Lines.size()!=nRows+2 )
        return false;
    i++;

    //
    // Read all cases
    //
    ap::real_2d_array &arr = pdataset->all;
    arr.setbounds(0, nRows-1, 0, nColumns-1);
    for(nRow=0; nRow<nRows; nRow++)
    {
        ap::ap_error::make_assertion(i!=Lines.end());
        explodestring(*i, '\t', &Values);
        if( Values.size()!=nColumns )
            return false;
        for(nCol=0; nCol<nColumns; nCol++)
        {
            double v;
            if( sscanf(Values[nCol].c_str(), "%lg", &v)!=1 )
                return false;
            if( (nCol==nColumns-1) && pdataset->iscls && ((round(v)<0) || (round(v)>=pdataset->nclasses)) )
                return false;
            if( (nCol==nColumns-1) && pdataset->iscls )
                arr(nRow, nCol) = round(v);
            else
                arr(nRow, nCol) = v;
        }
        i++;
    }

    //
    // Split to training, validation and test sets
    //
    if( pdataset->trnsize>0 )
        pdataset->trn.setbounds(0, pdataset->trnsize-1, 0, nColumns-1);
    if( pdataset->valsize>0 )
        pdataset->val.setbounds(0, pdataset->valsize-1, 0, nColumns-1);
    if( pdataset->tstsize>0 )
        pdataset->tst.setbounds(0, pdataset->tstsize-1, 0, nColumns-1);
    nSplitted=0;
    for(nRow=0; nRow<=pdataset->trnsize-1; nRow++, nSplitted++)
        for(nCol=0; nCol<=nColumns-1; nCol++)
            pdataset->trn(nRow,nCol) = arr(nSplitted,nCol);
    for(nRow=0; nRow<=pdataset->valsize-1; nRow++, nSplitted++)
        for(nCol=0; nCol<=nColumns-1; nCol++)
            pdataset->val(nRow,nCol) = arr(nSplitted,nCol);
    for(nRow=0; nRow<=pdataset->tstsize-1; nRow++, nSplitted++)
        for(nCol=0; nCol<=nColumns-1; nCol++)
            pdataset->tst(nRow,nCol) = arr(nSplitted,nCol);
    return true;
}*/

/********************************************************************
Service routines:
********************************************************************/
void* ap::amalloc(size_t size, size_t alignment)
{
    if( alignment<=1 )
    {
        //
        // no alignment, just call malloc
        //
        void *block = malloc(sizeof(void*)+size);
        void **p = (void**)block;
        *p = block;
        return (void*)((char*)block+sizeof(void*));
    }
    else
    {
        //
        // align.
        //
        void *block = malloc(alignment-1+sizeof(void*)+size);
        char *result = (char*)block+sizeof(void*);
        //if( ((unsigned int)(result))%alignment!=0 )
        //    result += alignment - ((unsigned int)(result))%alignment;
        if( (result-(char*)0)%alignment!=0 )
            result += alignment - (result-(char*)0)%alignment;
        *((void**)(result-sizeof(void*))) = block;
        return result;
    }
}

void ap::afree(void *block)
{
    void *p = *((void**)((char*)block-sizeof(void*)));
    free(p);
}

int ap::vlen(int n1, int n2)
{
    return n2-n1+1;
}



#endif
