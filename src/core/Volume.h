/*
 * Volume.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_CORE_VOLUME_H_
#define SRC_CORE_VOLUME_H_

#include "VolumeData.h"

namespace Core {
    /*
     *
     */
    typedef vector<int> VI;
    typedef vector<VI> VVI;

    typedef vector<double> VD;
    typedef vector<VD> VVD;

    const int neighbor6[6][3]={{0,0,1},{0,0,-1},{0,1,0},{0,-1,0},{1,0,0},{-1,0,0}} ;
    const int neighbor4[4][2]={{0,1},{0,-1},{1,0},{-1,0}} ;
    const int neighbor64[6][4][3] = {
        {{0,1,0},{0,-1,0},{1,0,0},{-1,0,0}},
        {{0,1,0},{0,-1,0},{1,0,0},{-1,0,0}},
        {{0,0,1},{0,0,-1},{1,0,0},{-1,0,0}},
        {{0,0,1},{0,0,-1},{1,0,0},{-1,0,0}},
        {{0,0,1},{0,0,-1},{0,1,0},{0,-1,0}},
        {{0,0,1},{0,0,-1},{0,1,0},{0,-1,0}}} ;

    const int sheetNeighbor[12][4][3] = {
        {{0,-1,-1},{0,-1,0},{0,0,-1},{0,0,0}},
        {{0,-1,0},{0,-1,1},{0,0,0},{0,0,1}},
        {{0,0,-1},{0,0,0},{0,1,-1},{0,1,0}},
        {{0,0,0},{0,0,1},{0,1,0},{0,1,1}},

        {{-1,0,-1},{-1,0,0},{0,0,-1},{0,0,0}},
        {{-1,0,0},{-1,0,1},{0,0,0},{0,0,1}},
        {{0,0,-1},{0,0,0},{1,0,-1},{1,0,0}},
        {{0,0,0},{0,0,1},{1,0,0},{1,0,1}},

        {{-1,-1,0},{-1,0,0},{0,-1,0},{0,0,0}},
        {{-1,0,0},{-1,1,0},{0,0,0},{0,1,0}},
        {{0,-1,0},{0,0,0},{1,-1,0},{1,0,0}},
        {{0,0,0},{0,1,0},{1,0,0},{1,1,0}}
        };

    const int faceCells[12][2]={{0,4},{1,5},{2,6},{3,7},{0,2},{1,3},{4,6},{5,7},{0,1},{2,3},{4,5},{6,7}};

    const int cubeFaces[6][4] =
    { {1,5,7,3},{0,2,6,4},{2,3,7,6},{0,4,5,1},{5,4,6,7},{0,1,3,2}};

    const int faceEdges[12][2] = {{3,1},{3,0},{2,1},{2,0},
    {5,1},{5,0},{4,1},{4,0},
    {5,3},{5,2},{4,3},{4,2}};

    const int edgeFaces[6][4] = {{1,3,5,7},{0,2,4,6},{2,3,9,11},{0,1,8,10},{6,7,10,11},{4,5,8,9}} ;

    struct Coordinate {
       int x, y, z;
       Coordinate(){}
       Coordinate(int xx, int yy, int zz)
           : x(xx), y(yy), z(zz)
       {}
       Coordinate(const VI & vi)
                  : x(vi[0]), y(vi[1]), z(vi[2])
              {}
       Coordinate(const int arr[])
                  : x(arr[0]), y(arr[1]), z(arr[2])
              {}

       vector<Coordinate> getNeighbors() const {
           vector<Coordinate> res(6);
           for(int i=0; i<6; ++i) {
               res[i] = Coordinate(neighbor6[i]) + *this;
           }
           return res;
       }

       friend Coordinate operator+(const Coordinate &l, const Coordinate &r) {
           return Coordinate(l.x+r.x, l.y+r.y, l.z+r.z);
       }
    };

    typedef vector<Coordinate> VCOORD;

    class Volume : public VolumeData {
        public:
            Volume();
            Volume(int sizeX, int sizeY, int sizeZ, double val=0.0);
            virtual ~Volume();

            void fill(double val);
            void applyMask(double maskValue, bool keepMaskValue);
            double getMin() const;
            double getMax() const;

            double getMean() const; // Returns the mean value of all the voxels
            double getEdgeMean() const; // Returns the mean value of all the surface voxels but no interior voxels
            double getStdDev() const; // Returns the population standard deviation of the values at all the voxels
//            Vector3DFloat getCenterOfMass() const; // Returns the center of mass of the image in pixels (not angstroms)
            void print() const;

            void threshold( double thr );
            void threshold( double thr, int out, int in );
            void threshold( double thr, int out, int in, int boundary, bool markBoundary=true);
            void threshold2( double thr, int out, int in );

            void smooth( double alpha );

            void normalize();
            void normalize( double min, double max );
            void normalize( double min, double max, double thresh, double ithresh );

//            void addNoise( double thr, double pos );
//            void downsampleVolume();

            int getNumNeighbor6(int ox, int oy, int oz) const;

//            Volume * getDataRange(int x, int y, int z, int radius);
//            double getInterpDataAt( double x, double y, double z );
//            int isSimple( int ox, int oy, int oz );
//            int isPiercable( int ox, int oy, int oz );
//            int isSimple2( int v[3][3][3] );
//            int getNumPotComplex3( int ox, int oy, int oz );
//            int getNumPotComplex4( int ox, int oy, int oz );
//            int getNumPotComplex( int ox, int oy, int oz );
//            int getNumPotComplex2( int ox, int oy, int oz );
//            int getNumNeighbor( int ox, int oy, int oz );
//            int getNumCells( int ox, int oy, int oz );
//            int getNumIsolatedEdges( int ox, int oy, int oz );
//            int components6( int vox[3][3][3] );
//            int components26( int vox[3][3][3] );
//            int countExt( double vox[3][3][3] );
//            int countInt( double vox[3][3][3] );
//            int countIntEuler( int ox, int oy, int oz );

    // Declarations from volume.h START
//            int getNonZeroVoxelCount();
//            void subtract(Volume * vol);
//            double getMaxValuePosition(int& maxX, int& maxY, int& maxZ);
//            double getLocalMax(int x, int y, int z, int radius);
//            double getLocalMin(int x, int y, int z, int radius);
//
//            int isBertrandBorder(int ox, int oy, int oz, int dir);
//            int isBertrandEndPoint(int ox, int oy, int oz);
//            int isHelix(int ox, int oy, int oz);
//            int isSheet(int ox, int oy, int oz);
//            Volume * getSheets(int minSize);
//            Volume * getHelices(int minSize);
//            int isEndPoint(int ox, int oy, int oz);
//            int testIsSheetEnd(int ox, int oy, int oz);
//            int isNoiseSheetEnd(int ox, int oy, int oz);
//            int isInternal(int ox, int oy, int oz);
//            int isInternal2(int ox, int oy, int oz);
//            int hasIsolatedFace(int ox, int oy, int oz);
//            int hasIsolatedEdge(int ox, int oy, int oz);
//            int countFace(int ox, int oy, int oz, int m);
//            int hasCell(int ox, int oy, int oz);
//            Volume * markCellFace();
//            Volume * markFaceEdge();
//            int hasCompleteSheet(int ox, int oy, int oz, Volume * fvol);
//            int hasCompleteSheet(int ox, int oy, int oz);
//            int hasCompleteSheetSlow(int ox, int oy, int oz);
//            int hasCompleteHelix(int ox, int oy, int oz);
//            int hasCompleteHelix(int ox, int oy, int oz, Volume * fvol);
//            int isHelixEnd(int ox, int oy, int oz, Volume * nvol);
//            int isFeature18(int ox, int oy, int oz);
//            int isEdgeEnd(int ox, int oy, int oz);
//            int isFaceEnd(int ox, int oy, int oz);
//            int isNoise(int ox, int oy, int oz, int noise);
//            int isNoiseHelixEnd(int ox, int oy, int oz);
//            int isHelixEnd(int ox, int oy, int oz);
//            int isSheetEnd(int ox, int oy, int oz, Volume * nvol);
//            int getNumFaces( int ox, int oy, int oz );
//            int getNumIsolatedFaces( int ox, int oy, int oz );
//            int isFeatureFace2( int ox, int oy, int oz );
//            int isFeatureFace( int ox, int oy, int oz );
//            int hasFeatureFace( int ox, int oy, int oz );
//            int isSheetEnd( int ox, int oy, int oz );
//            void setScoreNeighbor( GridQueue* queue );
//            void erodeNoTopo( float thr, int wid );
//            void erodeTopo( float thr, int wid );
//            void erode2( float thr, int wid );
//            void erodeShapeTopo( float thr, int wid );
//            void erodeAtom( float thr, int wid, Volume* avol );
//            void curveSkeleton( Volume* grayvol, float lowthr, float highthr, Volume* svol );
//            void curveSkeleton( float thr, Volume* svol );
//            void curveSkeleton2D( float thr, Volume* svol );
//            void skeleton( float thr, int off );
//            void skeleton2( float thr, int off );
//            void pointSkeleton( Volume* grayvol, float lowthr, float highthr, Volume* svol, Volume* hvol );
//            void skeleton( float thr, Volume* svol, Volume* hvol );
//            void erodeHelix( );
//            void erodeHelix( int disthr );
//            int erodeSheet( );
//            int erodeSheet( int disthr );
//            void erodeSheetOld( int disthr );
//            void sequentialSkeleton( float thr, int type, int noise );
//            void dumbsurfaceSkeleton( float thr );
//            void surfaceSkeleton( Volume* grayvol, float lowthr, float highthr );
//            void surfaceSkeleton( float thr );
//            void surfaceSkeleton( float thr, Volume* svol );
//            void surfaceSkeletonOld( float thr );
//            void surfaceSkeletonPres( float thr, Volume * preserve );
//            void bertrandSurfaceSkeleton2( float thr );
//            void bertrandSurfaceSkeleton( float thr );
//            void palagyiSurfaceSkeleton( float thr );
//            void rotateX ( double a );
//            void toMathematicaFile( string fname );
//            void segment( float threshold, Volume* lowvol, Volume* highvol, string mrcfile );
//            void segment( float threshold, Volume* vol, int maxDis, string mrcfile );
//            void writeSegmentation( float threshold, Volume* segvol, string txtfile, string mrcfile );
//            void floodFill( float thr );
//            void reduceComponent( int size );
//            void reduceComponent2( int num );
//            void floodFillPQR( int offset );
//            void writeDistances( string fname, int maxDis );
//            void toMRCFile( string fname );
//
//            void loadFile(string fileName);
//            void saveFile(string fileName);
//            void PerformSmoothLaplacian(double convergenceRate, int iterations);
//            Volume * PerformBinarySkeletonizationJu2007(double threshold, int minCurveSize, int minSurfaceSize);
//            Volume * PerformGrayscaleSkeletonizationAbeysinghe2008(double startDensity, int stepCount, int minCurveSize, int minSurfaceSize, int curveRadius, int surfaceRadius, int skeletonSmoothenRadius);
//
//            string getSupportedLoadFileFormats() const;
//            string getSupportedSaveFileFormats() const;
//            Volume * getVolume();
//            void setVolume(Volume *vol);
//
//            int getHashKey(int x, int y, int z, int edge, int iScale) const;
//            float getOffset(float fValue1, float fValue2, float fValueDesired) const;
//            int Smallest2ndPower(int value) const;
    // Declarations from volume.h END

        private:
//            VolumeData & volume;
    };

} /* namespace Core */

#endif /* SRC_CORE_VOLUME_H_ */
