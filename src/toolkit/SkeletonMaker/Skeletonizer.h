/*
 * Skeletonizer.h
 *
 *      Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_SKELETONMAKER_SKELETONIZER_H_
#define SRC_TOOLKIT_SKELETONMAKER_SKELETONIZER_H_

////#include "ImageReaderMRC.h"
////#include "Core/GlobalDefinitions.h"
////#include "VolumeDeltaAnalyzer.h"
////#include <MathTools/Combinatorics.h>
////#include <SkeletonMaker/PriorityQueue.h>
#include <MathTools/Matlab.h>
//#include <MathTools/DataStructures.h>
////#include <MathTools/Vector3.h>
#include "NormalFinder.h"
////#include <string>
////#include <Foundation/TimeManager.h>
////#include <new>
////#include <functional>
//#include <Core/volume.h>
//#include "MathTools/BasicDefines.h"
#include <MathTools/Combinatorics.h>

using namespace std;

using namespace MathTools;
//using namespace Foundation;
using namespace Core;

namespace GraySkeletonCPP {

    class Skeletonizer {
        public:
            Skeletonizer(int pointR, int curveR,
                               int surfaceR, int skelDirR);
            Volume * PerformImmersionSkeletonizationAndPruning(
                    Volume & sourceVol, Volume * preserved, double startGray,
                    double endGray, double stepSize, int smoothingIterations,
                    int smoothingR, int minCurveSize, int minSurfaceSize,
                    int maxCurveHole, int maxSurfaceHole, string outPath,
                    bool doPruning, double pointThreshold,
                    double curveThreshold, double surfaceThreshold);

            Volume * PerformPureJuSkeletonization(Volume & imageVol, string outPath,
                                                  double threshold, int minCurveWidth, int minSurfaceWidth);
            Volume * GetJuSurfaceSkeleton (Volume & src, Volume & preserve, double threshold);
            Volume * GetJuCurveSkeleton   (Volume & src, Volume & preserve, double threshold, bool is3D);
            Volume * GetJuTopologySkeleton(Volume & src, Volume & preserve, double threshold);
            void PruneCurves  (Volume & src, int pruneLength);
            void PruneSurfaces(Volume & src, int pruneLength);
            void PruneUsingStructureTensor( Volume & skel, const Volume & src, Volume * preserved,
                                            vector<Vec3F> & volGrad,
                                            vector<EigenResults3D> & volumeEigens, ProbDistr3D & filter,
                                            double threshold, char pruningClass, string outPath);
            void SmoothenVolume(Volume & src, double minGrayscale,
                                double maxGrayscale, int stR);

            void VoxelBinarySubtract(Volume & dest, const Volume & src);
            void VoxelSubtract      (Volume & dest, const Volume & src);
            void VoxelOr            (Volume & dest, const Volume * src);

            Vec3F GetCurveDirection(const Volume & skel, int x, int y, int z, int R);
            Vec3F GetSurfaceNormal (const Volume & skel, int x, int y, int z);
            Vec3F GetSurfaceNormal (const Volume & skel, int x, int y, int z, int R, vector<Vec3F> & localDirs);
            vector<Vec3F> GetVolumeGradient   (const Volume & src);
            vector<Vec3F> GetSkeletonDirection(const Volume & skel, int type);

            void GetEigenResult(EigenResults3D & returnVal,
                                vector<Vec3F> & imgGrad, ProbDistr3D & gaussFilt,
                                int x, int y, int z,
                                int sizeX, int sizeY, int sizeZ,
                                int gaussFiltR, bool clear);
            vector<EigenResults3D> GetEigenResults(const Volume & mask, vector<Vec3F> & imgGrad,
                                                   ProbDistr3D & gaussFilt,
                                                   int gaussFiltR, bool useMask);

        protected:

            double GetVoxelCost(EigenResults3D imgEigen, Vec3F skelDir, int type);
            void FindOrthogonalAxes(Vec3F axis, Vec3F & res1, Vec3F & res2);
            void GetSTBasedDistribution(ProbDistr3D & distr, EigenResults3D eigen);
            Vec3F XYZtoUVW(Vec3F vec, Vec3F u, Vec3F v, Vec3F w);

            Volume FillCurveHoles  (Volume & dest, const Volume & src, int maxHoleSize);
            Volume FillSurfaceHoles(Volume & dest, const Volume & src, int maxHoleSize);

            Volume * GetJuThinning       (      Volume & src, const Volume & preserve, double threshold, char thinningClass);
            Volume * GetImmersionThinning(const Volume & src, const Volume & preserve,
                                          double lowGrayscale, double highGrayscale,
                                          double stepSize, char thinningClass);

            static const char THINNING_CLASS_SURFACE_PRESERVATION;
            static const char THINNING_CLASS_CURVE_PRESERVATION_2D;
            static const char THINNING_CLASS_CURVE_PRESERVATION;
            static const char THINNING_CLASS_POINT_PRESERVATION;
            static const char THINNING_CLASS_TOPOLOGY_PRESERVATION;
            static const char PRUNING_CLASS_PRUNE_SURFACES;
            static const char PRUNING_CLASS_PRUNE_CURVES;
            static const char PRUNING_CLASS_PRUNE_POINTS;

        public:
            Matlab math;
            NormalFinder surfNormFinder;
            ProbDistr3D gaussFiltPtR;
            ProbDistr3D gaussFiltCrvR;
            ProbDistr3D gaussFiltSrfcR;
            ProbDistr3D gaussFiltMaxR;
            ProbDistr3D uniformFiltSkelDirR;
            int pointR;
            int curveR;
            int surfR;
            int skelDirR;
    };

} /* namespace GraySkeletonCPP */

#endif /* SRC_TOOLKIT_SKELETONMAKER_SKELETONIZER_H_ */
