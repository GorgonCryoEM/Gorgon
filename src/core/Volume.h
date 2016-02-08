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
//            double getEdgeMean() const; // Returns the mean value of all the surface voxels but no interior voxels
            double getStdDev() const; // Returns the population standard deviation of the values at all the voxels
//            Vector3DFloat getCenterOfMass() const; // Returns the center of mass of the image in pixels (not angstroms)
            void print() const;

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
//            int getNumNeighbor6(int ox, int oy, int oz);
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
//            int getNumCells( int ox, int oy, int oz );
//            int getNumIsolatedEdges( int ox, int oy, int oz );
//            int getNumIsolatedFaces( int ox, int oy, int oz );
//            int isFeatureFace2( int ox, int oy, int oz );
//            int isFeatureFace( int ox, int oy, int oz );
//            int hasFeatureFace( int ox, int oy, int oz );
//            int isSheetEnd( int ox, int oy, int oz );
//            int isSimple( int ox, int oy, int oz );
//            int isPiercable( int ox, int oy, int oz );
//            int isSimple2( int v[3][3][3] );
//            int getNumPotComplex3( int ox, int oy, int oz );
//            int getNumPotComplex4( int ox, int oy, int oz );
//            int getNumPotComplex( int ox, int oy, int oz );
//            int getNumPotComplex2( int ox, int oy, int oz );
//            int getNumNeighbor( int ox, int oy, int oz );
//            void setScoreNeighbor( GridQueue* queue );
//            int components6( int vox[3][3][3] );
//            int components26( int vox[3][3][3] );
//            int countExt( double vox[3][3][3] );
//            int countInt( double vox[3][3][3] );
//            int countIntEuler( int ox, int oy, int oz );
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
//            void addNoise( float thr, float pos );
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
//            void threshold( double thr );
//            void threshold( double thr, int out, int in );
//            void threshold( double thr, int out, int in, int boundary);
//            void threshold( double thr, int out, int in, int boundary, bool markBoundary);
//            void threshold2( double thr, int out, int in );
//            void smooth( float alpha );
//            void normalize( double min, double max );
//            void normalize( double min, double max, double thresh, double ithresh );
//            Volume * getDataRange(int x, int y, int z, int radius);
//            double getInterpDataAt( double x, double y, double z );
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
//            void normalizeVolume();
//            void downsampleVolume();
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
