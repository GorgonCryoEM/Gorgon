/*
 * Volume.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "Volume.h"
#include <cmath>

#include <algorithm>
#include <numeric>

namespace Core {

    Volume::Volume()
            : VolumeData()//,
//              volume(dynamic_cast<VolumeData &>(*this))
    {}

    Volume::Volume(int sizeX, int sizeY, int sizeZ, double val)
            : VolumeData(sizeX, sizeY, sizeZ, val)//,
//              volume(dynamic_cast<VolumeData &>(*this))
    {    }

    Volume::~Volume() {}

    void Volume::fill(double val)
    {
        for(iterator it=data.begin(); it!=data.end(); ++it)
                    *it = val;
    }

    void Volume::applyMask(double maskValue, bool keepMaskValue) {
        for(iterator it=data.begin(); it!=data.end(); ++it)
            if((*it== maskValue && !keepMaskValue) ||
               (*it != maskValue && keepMaskValue))
                *it = 0;
    }

    double Volume::getMin() const {
        return *min_element(data.begin(), data.end());
    }

    double Volume::getMax() const {
        return *max_element(data.begin(), data.end());
    }

    double Volume::getMean() const {
        return accumulate(data.begin(), data.end(), 0.0) / data.size();
    }

//    double Volume::getEdgeMean() const {
//    }

    void Volume::print() const {
        for(int i = 0; i < getSizeX(); i++) {
            cout<<"{ ";
            for(int j = 0; j < getSizeY(); j++) {
                cout<<"{ ";
                for(int k = 0; k < getSizeZ(); k++) {
                    cout<<(*this)(i, j, k)<<" ";
                }
                cout<<"} ";
            }
            cout<<"} ";
        }
        cout<<endl;
    }

    double Volume::getStdDev() const {
        //Calculate the standard deviation of all the voxels in the image
        double sum  = 0.0;
        double sum2 = 0.0;

        for(const_iterator it=data.begin(); it!=data.end(); ++it) {
            double val = *it;
            sum  += val;
            sum2 += val*val;
        }

        return sqrt( (sum2 - sum*sum/data.size()) / data.size() );
    }

//    Vector3DFloat Volume::getCenterOfMass() const {
//    }

    void Volume::threshold( double thr )
    {
        threshold( thr, 0, 1, 0) ;
    }

    void Volume::threshold( double thr, int out, int in )
    {
        threshold( thr, out, in, out) ;
    }

    void Volume::threshold( double thr, int out, int in, int boundary, bool markBoundary)
    {
        for ( int i = 0 ; i < getSizeX() ; i ++ )
            for ( int j = 0 ; j < getSizeY() ; j ++ )
                for ( int k = 0 ; k < getSizeZ() ; k ++ )
                {
                    double val = (*this)(i, j, k);
                    if(markBoundary) {
                        if ( i > 1 && i < getSizeX() - 2 && j > 1 && j < getSizeY() - 2 && k > 1 && k < getSizeZ() - 2 ) {
                            if(val < thr) {
                                (*this)(i, j, k) = out;
                            } else {
                                (*this)(i, j, k) = in;
                            }
                        }
                        else
                        {
                            (*this)(i, j, k) = boundary;
                        }
                    } else {
                        if(val < thr) {
                            (*this)(i, j, k) = out;
                        } else {
                            (*this)(i, j, k) = in;
                        }
                    }
                }
    }

    void Volume::threshold2( double thr, int out, int in )
    {
        for ( int i = 0 ; i < getSizeX() ; i ++ )
            for ( int j = 0 ; j < getSizeY() ; j ++ )
                for ( int k = 0 ; k < getSizeZ() ; k ++ ) {
                    double val = (*this)(i, j, k);
                    if(val <= thr) {
                        (*this)(i, j, k) = out;
                    } else {
                        (*this)(i, j, k) = in;
                    }
                }
    }

    void Volume::smooth( double alpha )
    {
        Volume backup(*this);

        for (int i = 1; i < getSizeX() - 1; i++)
            for (int j = 1; j < getSizeY() - 1; j++)
                for (int k = 1; k < getSizeZ() - 1; k++) {
                    double val = backup( i - 1, j    , k     ) +
                                 backup( i + 1, j    , k     ) +
                                 backup( i    , j - 1, k     ) +
                                 backup( i    , j + 1, k     ) +
                                 backup( i    , j    , k - 1 ) +
                                 backup( i    , j    , k + 1 ) ;
                    (*this)(i, j, k) = (*this)(i, j, k) * alpha + ( 1.0 - alpha ) * val / 6.0;
                }
    }


    void Volume::normalize( double min, double max )
    {
        double imin = getMin() ;
        double imax = getMax() ;
        double irange = imax - imin ;
        double range = max - min ;

        int size = volData->getMaxIndex();
        for(int i = 0 ; i < size ; i ++) {
            setDataAt(i, ((getDataAt(i) - (float)imin ) / (float)irange) * (float)range + (float)min);
        }
    }

    void Volume::normalize( double min, double max, double thresh, double ithresh )
    {
        double imin = getMin() ;
        double imax = getMax() ;
        double irange1 = ithresh - imin ;
        double irange2 = imax - ithresh ;
        double range1 = thresh - min;
        double range2 = max - thresh ;

        int size = volData->getMaxIndex();
        for (int i = 0; i < size; i++) {
            if (getDataAt(i) < ithresh) {
                setDataAt(i, ((getDataAt(i) - (float)imin ) / (float)irange1) * (float)range1 + (float)min);
            }
            else
            {
                setDataAt(i, (float)max - (( (float)imax - getDataAt(i)) / (float)irange2) * (float)range2);
            }
        }
    }



} /* namespace Core */
