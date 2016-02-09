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

    // Returns the mean value of all the surface voxels but no interior voxels
    double Volume::getEdgeMean() const {
        int nx = getSizeX();
        int ny = getSizeY();
        int nz = getSizeZ();

        //Calculate the edge mean -- the average value of all the voxels on the surfaces (1 voxel) of the image
        double edge_sum = 0.0; //The sum of the values on the outer surfaces (1 voxel) of the image
        int num_voxels = 0;

        //sum the values of each voxel on the surfaces of the rectangular prism
        for (int i=0; i<nx; i++)
            for (int j=0; j<ny; j++)
                for (int k=0; k<nz; k++)
                {
                    if (i==0 || i==nx-1 || j==0 || j==ny-1 || k==0 || k==nz-1)
                    {
                        edge_sum += (*this)(i,j,k);
                        num_voxels++;
                    }
                }

        return edge_sum / num_voxels;
    }

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

    // Returns the center of mass of the image in pixels (not angstroms)
//    Vector3DFloat Volume::getCenterOfMass() const {
//
//        double mass    = 0.0;
//        double xmoment = 0.0;
//        double ymoment = 0.0;
//        double zmoment = 0.0;
//
//        for (int i=0; i<getSizeX(); i++)
//            for (int j=0; j<getSizeY(); j++)
//                for (int k=0; k<getSizeZ(); k++) {
//                    double val = (*this)(i,j,k);
//                    mass += val;
//                    xmoment += i*val;
//                    ymoment += j*val;
//                    zmoment += k*val;
//                }
//
//        Vector3DFloat centerOfMass( xmoment/mass, ymoment/mass, zmoment/mass );
//        return centerOfMass;
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

    void Volume::normalize() {
        normalize(0.0, 1.0);
    }

    void Volume::normalize( double y0, double y1 )
    {
        double x0 = getMin();
        double x1 = getMax();
        double scale = (y1 - y0) / (x1 - x0);

        for(iterator it=data.begin(); it!=data.end(); ++it)
            *it = y0 + (*it - x0 ) * scale;
    }

    void Volume::normalize( double min, double max, double thresh, double ithresh )
    {
        double x0 = getMin() ;
        double x1 = getMax() ;
        double irange1 = ithresh - x0 ;
        double irange2 = x1 - ithresh ;
        double range1 = thresh - min;
        double range2 = max - thresh ;

        for(iterator it=data.begin(); it!=data.end(); ++it){
            if (*it < ithresh)
                *it = (*it - x0 ) * range1 / irange1 + min;
            else
                *it = max - ( x1 - *it) * range2 / irange2;
        }
    }



} /* namespace Core */
