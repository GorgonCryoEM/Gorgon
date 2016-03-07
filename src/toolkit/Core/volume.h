#ifndef TOOLKIT_SKELETON_MAKER_VOLUME_H
#define TOOLKIT_SKELETON_MAKER_VOLUME_H

#define MAX_SHEETS 100000
#define MAX_QUEUELEN 5000000
#define MAX_ERODE 1000

#include "VolumeData.h"
#include "Foundation/List.h"
#include "Foundation/PriorityQueue.h"
#include <MathTools/Vector3.h>
#include "GlobalConstants.h"
//#include "ThinningTemplate.h"
//#include <MathTools/MathLib.h>
//#include <GorgonGL.h>
//#include <Core/GlobalDefinitions.h>
//#include <SkeletonMaker/VolumeSkeletonizer.h>
//#include <GraySkeletonCPP/VolumeFormatConverter.h>
//#include "MeshRenderer.h"
//#include <ProteinMorph/NonManifoldMesh.h>
//#include <ProteinMorph/TriangleMesh.h>
//#include <Foundation/Octree.h>

//using namespace GraySkeletonCPP;
using namespace MathTools;
using namespace Visualization;

namespace SkeletonMaker {

    const int neighbor6[6][3]={{0,0,1},{0,0,-1},{0,1,0},{0,-1,0},{1,0,0},{-1,0,0}} ;
    const vector<Vec3I> vneighbor6(neighbor6, neighbor6+6);

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

    class Volume : public VolumeData {
    public:
        Volume();
        Volume(int x, int y, int z, float val=0.0);
        Volume(const Volume& obj);
        virtual ~Volume( );

        int getHashKey(int x, int y, int z, int edge, int iScale) const;
        float getVoxelData(int x, int y, int z) const;
        float getVoxelData(float x, float y, float z) const;
        float getOffset(float fValue1, float fValue2, float fValueDesired) const;

        int getNonZeroVoxelCount();
        void print();
        void subtract(const Volume & vol);
        void applyMask(const Volume & mask, double maskValue, bool keepMaskValue);
        double getMin() const;
        double getMax() const;
        double getMaxValuePosition(int& maxX, int& maxY, int& maxZ);

        float getMean(); // Returns the mean value of all the voxels
        float getStdDev(); // Returns the population standard deviation of the values at all the voxels

        Volume * markCellFace();
        int getNumNeighbor6  (int ox, int oy, int oz);
        int isInternal2      (int ox, int oy, int oz);
        int hasCell          (int ox, int oy, int oz);
        int hasCompleteSheet (int ox, int oy, int oz);
        int hasCompleteHelix (int ox, int oy, int oz);
        int isHelixEnd       (int ox, int oy, int oz);
        int isFeatureFace    (int ox, int oy, int oz);
        int isSheetEnd       (int ox, int oy, int oz);
        int isSimple         (int ox, int oy, int oz);
        int isPiercable      (int ox, int oy, int oz);
        int getNumPotComplex (int ox, int oy, int oz);
        int getNumPotComplex2(int ox, int oy, int oz);
        int components (int vox[3][3][3], const vector<Vec3I> neighbors);
        int components6 (int vox[3][3][3]);
        int components26(int vox[3][3][3]);
        int countNeg( double vox[3][3][3]);
        int countInt( double vox[3][3][3]);
        int countIntEuler( int ox, int oy, int oz );
        void curveSkeleton( const Volume & grayvol, float lowthr, float highthr, const Volume & src);
        void curveSkeleton  (float thr, const Volume & src);
        void curveSkeleton2D(float thr, const Volume & src);
        void pointSkeleton(const Volume & grayvol, float lowthr, float highthr,
                           const Volume & src, const Volume & hvol);
        void skeleton( float thr, const Volume & src, const Volume & hvol );
        void erodeHelix( int disthr );
        int erodeSheet( int disthr );
        void surfaceSkeleton( const Volume & grayvol, float lowthr, float highthr );
        void surfaceSkeletonPres( float thr, const Volume & preserve );
        void threshold( double thr );
        void threshold( double thr, int out, int in );
        void threshold( double thr, int out, int in, int boundary, bool markBoundary);
        void threshold2( double thr, int out, int in );
        void normalize( double min, double max );
        Volume getDataRange(int x, int y, int z, int radius);
        void toMRCFile( string fname );

        void Normalize();
        void downsample();

        virtual void load(string fileName);
        virtual void save(string fileName);
        Volume * PerformBinarySkeletonizationJu2007(double threshold, int minCurveSize, int minSurfaceSize);
        Volume * PerformGrayscaleSkeletonizationAbeysinghe2008(
                double startDensity, int stepCount, int minCurveSize,
                int minSurfaceSize, int curveRadius, int surfaceRadius,
                int skeletonSmoothenRadius);

        void buildHistogram(int binCount);
        int getHistogramBinValue(int binIx);

#ifdef GORGON_DEBUG
        static int id0;
        static int id1;
        static int id3;
#endif


    private:

        VolumeData * getVolumeData();
        vector<int> histogram;

    protected:
        VolumeData * volData;
//----------------------
    public:
        Volume * getVolume();
        void setVolume(Volume *vol);
    };

}

#endif
