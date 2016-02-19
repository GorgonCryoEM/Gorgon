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
        threshold( thr, 0, 1, 0, true) ;
    }

    void Volume::threshold( double thr, int out, int in )
    {
        threshold( thr, out, in, out, true) ;
    }

    void Volume::threshold( double thr, int out, int in, int boundary)
    {
        threshold(thr, out, in, boundary, true);
    }

    void Volume::threshold( double thr, int out, int in, int boundary, bool markBoundary)
    {
        float val;
        for ( int i = 0 ; i < getSizeX() ; i ++ )
            for ( int j = 0 ; j < getSizeY() ; j ++ )
                for ( int k = 0 ; k < getSizeZ() ; k ++ )
                {
                    val = (*this)(i, j, k);
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

} /* namespace Core */
