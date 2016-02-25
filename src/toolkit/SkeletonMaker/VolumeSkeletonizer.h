#ifndef TOOLKIT_GRAYSKELETONCPP_VOLUME_SKELETONIZER_H
#define TOOLKIT_GRAYSKELETONCPP_VOLUME_SKELETONIZER_H

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
#include "Visualization/DiscreteMesh.h"
#include <MathTools/Combinatorics.h>

using namespace std;

using namespace MathTools;
//using namespace Foundation;
using namespace SkeletonMaker;

namespace GraySkeletonCPP {

    struct ImmersionBeachElement {
        Vector3Int p;
        int binIndex;
    };

    bool operator<(const pair<ImmersionBeachElement,int> &l, const pair<ImmersionBeachElement,int> &r){
        return l.second < r.second;
    }


    ostream & operator<<(ostream & out, const ImmersionBeachElement & obj){
        return out
                <<"{"
                <<obj.p<<", "
                <<obj.binIndex
                <<"}";
    }


    class VolumeSkeletonizer {
        public:
            VolumeSkeletonizer(int pointRadius, int curveRadius,
                               int surfaceRadius, int skeletonDirectionRadius);
            Volume * PerformImmersionSkeletonizationAndPruning(
                    Volume & sourceVol, Volume * preserveVol, double startGray,
                    double endGray, double stepSize, int smoothingIterations,
                    int smoothingRadius, int minCurveSize, int minSurfaceSize,
                    int maxCurveHole, int maxSurfaceHole, string outputPath,
                    bool doPruning, double pointThreshold,
                    double curveThreshold, double surfaceThreshold);
            Volume * PerformSkeletonizationAndPruning(Volume * imageVol,
                                                      string outputPath);
            Volume * PerformPureJuSkeletonization(Volume * imageVol,
                                                  string outputPath,
                                                  double threshold,
                                                  int minCurveWidth,
                                                  int minSurfaceWidth);
            Volume * GetJuSurfaceSkeleton(Volume & sourceVolume,
                                          Volume & preserve, double threshold);
            Volume * GetJuCurveSkeleton(Volume & sourceVolume,
                                        Volume & preserve, double threshold,
                                        bool is3D);
            Volume * GetJuTopologySkeleton(Volume * sourceVolume,
                                           Volume * preserve, double threshold);
            void PruneCurves(Volume & sourceVolume, int pruneLength);
            void PruneSurfaces(Volume & sourceVolume, int pruneLength);
            void PruneUsingStructureTensor( Volume & skeleton, const Volume & sourceVolume,
                                            Volume * preserveVol, vector<Vector3Float> & volumeGradient,
                                            vector<EigenResults3D> & volumeEigens, ProbDistr3D & filter,
                                            double threshold, char pruningClass, string outputPath);
            void SmoothenVolume(Volume & sourceVolume, double minGrayscale,
                                double maxGrayscale, int stRadius);
            void VoxelBinarySubtract(Volume * sourceAndDestVolume1,
                                     Volume * sourceVolume2);
            void VoxelSubtract(Volume * sourceAndDestVolume1,
                               Volume * sourceVolume2);
            void VoxelOr(Volume * sourceAndDestVolume1, Volume * sourceVolume2);
            Vector3Float GetCurveDirection(const Volume &  skeleton, int x, int y, int z, int radius);
            Vector3Float GetSurfaceNormal(const Volume  & skeleton, int x, int y, int z);
            Vector3Float GetSurfaceNormal(const Volume  & skeleton, int x, int y, int z, int radius,
                                          vector<Vector3Float> & localDirections);
            vector<Vector3Float> GetVolumeGradient(const Volume & sourceVolume);
            vector<Vector3Float> GetSkeletonDirection(const Volume & skeleton, int type);

            void GetEigenResult(EigenResults3D & returnVal,
                                vector<Vector3Float> & imageGradient,
                                ProbDistr3D & gaussianFilter, int x, int y, int z,
                                int sizeX, int sizeY, int sizeZ,
                                int gaussianFilterRadius, bool clear);
            vector<EigenResults3D> GetEigenResults(const Volume & maskVol,
                                                   vector<Vector3Float> & imageGradient,
                                                   ProbDistr3D & gaussianFilter,
                                                   int gaussianFilterRadius,
                                                   bool useMask);

        protected:

            double GetVoxelCost(EigenResults3D imageEigen, Vector3Float skeletonDirection, int type);
            void FindOrthogonalAxes(Vector3Float axis, Vector3Float & res1, Vector3Float & res2);
            void GetSTBasedDistribution(ProbDistr3D & distributionInfo, EigenResults3D eigen);
            Vector3Float XYZtoUVW(Vector3Float vec, Vector3Float u, Vector3Float v, Vector3Float w);
            Volume * FillCurveHoles(Volume * thresholdedSkeleton,
                                    Volume * originalSkeleton, int maxHoleSize);
            Volume FillSurfaceHoles(Volume * thresholdedSkeleton,
                                      Volume * originalSkeleton,
                                      int maxHoleSize);
            Volume * GetJuThinning(Volume * sourceVolume, Volume * preserve,
                                   double threshold, char thinningClass);
            Volume * GetImmersionThinning(Volume * sourceVolume,
                                          Volume * preserve,
                                          double lowGrayscale,
                                          double highGrayscale, double stepSize,
                                          char thinningClass);

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
            ProbDistr3D uniformFilterSkeletonDirectionRadius;
            int pointRadius;
            int curveRadius;
            int surfaceRadius;
            int skeletonDirectionRadius;

    };

    const char VolumeSkeletonizer::THINNING_CLASS_SURFACE_PRESERVATION = 4;
    const char VolumeSkeletonizer::THINNING_CLASS_CURVE_PRESERVATION_2D = 3;
    const char VolumeSkeletonizer::THINNING_CLASS_CURVE_PRESERVATION = 2;
    const char VolumeSkeletonizer::THINNING_CLASS_POINT_PRESERVATION = 1;
    const char VolumeSkeletonizer::THINNING_CLASS_TOPOLOGY_PRESERVATION = 0;
    const char VolumeSkeletonizer::PRUNING_CLASS_PRUNE_SURFACES = 5;
    const char VolumeSkeletonizer::PRUNING_CLASS_PRUNE_CURVES = 6;
    const char VolumeSkeletonizer::PRUNING_CLASS_PRUNE_POINTS = 7;


    VolumeSkeletonizer::VolumeSkeletonizer(int pointRadius, int curveRadius, int surfaceRadius, int skeletonDirectionRadius)
        : math(Matlab()),
          surfNormFinder(NormalFinder())
    {
        this->pointRadius = pointRadius;
        this->curveRadius = curveRadius;
        this->surfaceRadius = surfaceRadius;
        this->skeletonDirectionRadius = skeletonDirectionRadius;

        gaussFiltPtR.R = pointRadius;
        BinomDistr(gaussFiltPtR);

        gaussFiltCrvR.R = curveRadius;
        BinomDistr(gaussFiltCrvR);

        gaussFiltSrfcR.R = surfaceRadius;
        BinomDistr(gaussFiltSrfcR);

        gaussFiltMaxR.R = MAX_GAUSSIAN_FILTER_RADIUS;
        BinomDistr(gaussFiltMaxR);

        uniformFilterSkeletonDirectionRadius.R = skeletonDirectionRadius;
        UniformDistr(uniformFilterSkeletonDirectionRadius);
    }

    double VolumeSkeletonizer::GetVoxelCost(EigenResults3D imageEigen, Vector3Float skeletonDirection, int type) {
        double cost = 1;

        if(!isZero(imageEigen.vals[0])) {
            double theta, a, b;
            Vector3Float temp, skelDirectionST, n;
            float u1 = 1.0;
            float u2 = abs(imageEigen.vals[1]/imageEigen.vals[0]);
            float u3 = abs(imageEigen.vals[2]/imageEigen.vals[0]);
            Vector3Float v1 = imageEigen.vecs[0];
            Vector3Float v2 = imageEigen.vecs[1];
            Vector3Float v3 = imageEigen.vecs[2];
            switch(type) {
                case PRUNING_CLASS_PRUNE_POINTS:

                    cost = u3*u3 / (u1*u2);
                    break;
                case PRUNING_CLASS_PRUNE_CURVES:

                    if(skeletonDirection.IsBadNormal()) {
                        cost = 1.0;
                    } else {
                        n = XYZtoUVW(skeletonDirection, v1, v2, v3);
                        a = u1 * u2 * u3;
                        b = sqrt(u2*u2*u3*u3*n.X()*n.X() + u1*u1*u3*u3*n.Y()*n.Y() + u1*u1*u2*u2*n.Z()*n.Z());
                        temp = n*(a/b);
                        cost = u3/ temp.length();
                    }


                    break;
                case PRUNING_CLASS_PRUNE_SURFACES:

                    {
                        if(skeletonDirection.IsBadNormal()) {
                            cost = 1.0;
                        } else {
                            Vector3Float n1, n2, m1, m2;
                            skelDirectionST = XYZtoUVW(skeletonDirection, imageEigen.vecs[0],imageEigen.vecs[1], imageEigen.vecs[2]);
                            FindOrthogonalAxes(skelDirectionST, n1, n2);

                            m1 = Vector3Float(n1[0]/u1, n1[1]/u2, n1[2]/u3);
                            m2 = Vector3Float(n2[0]/u1, n2[1]/u2, n2[2]/u3);
                            theta = atan((2.0 * (m1 * m2)) / ((m1 * m1) - (m2 * m2))) / 2.0;
                            a = 1.0 / ((m1 * cos(theta)) + (m2 * sin(theta))).length();
                            b = 1.0 / ((m1 * sin(theta)) - (m2 * cos(theta))).length();
                            cost = (u2 * u3) / (a*b);
                        }
                    }
                    break;
            }

        } else {
            cost = 1.0;
            //printf("Zero\n");
        }

        if(cost != cost) {
            cost = 1.0;
        }
        return cost;

    }

    vector<EigenResults3D> VolumeSkeletonizer::GetEigenResults(
            const Volume & maskVol, vector<Vector3Float> & imageGradient,
            ProbDistr3D & gaussianFilter, int gaussianFilterRadius,
            bool useMask)
    {
        vector<EigenResults3D> resultTable(maskVol.getSizeX() * maskVol.getSizeY() * maskVol.getSizeZ());

        for(int x = MAX_GAUSSIAN_FILTER_RADIUS; x < maskVol.getSizeX() - MAX_GAUSSIAN_FILTER_RADIUS; x++) {
            for(int y = MAX_GAUSSIAN_FILTER_RADIUS; y < maskVol.getSizeY() - MAX_GAUSSIAN_FILTER_RADIUS; y++) {
                for(int z = MAX_GAUSSIAN_FILTER_RADIUS; z < maskVol.getSizeZ() - MAX_GAUSSIAN_FILTER_RADIUS; z++) {
                    GetEigenResult(resultTable[maskVol.getIndex(x, y, z)], imageGradient, gaussianFilter, x, y, z,
                            maskVol.getSizeX(), maskVol.getSizeY(), maskVol.getSizeZ(), gaussianFilterRadius, (useMask && (maskVol.getDataAt(x, y, z) == 0)));
                }
            }
        }

        return resultTable;
    }

    Vector3Float VolumeSkeletonizer::XYZtoUVW(Vector3Float vec, Vector3Float u, Vector3Float v, Vector3Float w) {
        float uContri = vec * u;
        float vContri = vec * v;
        float wContri = vec * w;
        Vector3Float inUVW = Vector3Float(uContri, vContri, wContri);
        inUVW.normalize();
        return inUVW;
    }


    Vector3Float VolumeSkeletonizer::GetCurveDirection(const Volume & skeleton, int x, int y, int z, int radius) {
        Vector3Float direction = Vector3Float(0,0,0);
        if(DiscreteMesh::GetN6Count(skeleton, x, y, z) > 2) {
            direction = Vector3Float(BAD_NORMAL, BAD_NORMAL, BAD_NORMAL);
        } else {

            int margin = 2;
            int size = (radius+margin)*2 + 1;
            Volume * block = new Volume(size, size, size);
            for(int xx = margin; xx <= size-margin; xx++) {
                for(int yy = margin; yy <= size-margin; yy++) {
                    for(int zz = margin; zz <= size-margin; zz++) {
                        block->setDataAt(xx, yy, zz, skeleton.getDataAt(x-radius-margin+xx, y-radius-margin+yy, z-radius-margin+zz));
                    }
                }
            }

            Volume visited(size, size, size);

            vector<Vector3Int> list;
            list.push_back(Vector3Int(margin+radius, margin+radius, margin+radius));
            Vector3Int currentPos;
            vector<Vector3Int> n6;
            int n6Count;

            while(list.size() > 0) {
                currentPos = list[list.size()-1];
                list.pop_back();
                visited.setDataAt(currentPos.X(), currentPos.Y(), currentPos.Z(), 1);
                n6Count = DiscreteMesh::GetN6(n6, block, currentPos.X(), currentPos.Y(), currentPos.Z());

                if(DiscreteMesh::GetN6Count(skeleton, x+currentPos.X()-margin-radius, y+currentPos.Y()-margin-radius, z+currentPos.Z()-margin-radius) <= 2) {
                    for(int i = 0; i < n6Count; i++) {
                        if(visited.getDataAt(n6[i].X(), n6[i].Y(), n6[i].Z()) < 1) {
                            list.push_back(n6[i]);
                        }
                    }
                }
            }

            delete block;

            vector<Vector3Float> gradient = GetVolumeGradient(visited);
            EigenResults3D eigen;
            GetEigenResult(eigen, gradient, uniformFilterSkeletonDirectionRadius,
                margin+radius, margin+radius, margin+radius,
                size, size, size, radius, false);

            direction = eigen.vecs[2];
        }

        return direction;
    }

    Vector3Float VolumeSkeletonizer::GetSurfaceNormal(const Volume & skeleton, int x, int y, int z) {
        surfNormFinder.InitializeGraph(skeleton, x, y, z);
        return surfNormFinder.GetSurfaceNormal();
    }


    Vector3Float VolumeSkeletonizer::GetSurfaceNormal(const Volume & skeleton, int x, int y, int z, int radius, vector<Vector3Float> & localDirections) {
        Vector3Float direction = localDirections[skeleton.getIndex(x, y, z)];


        if(!direction.IsBadNormal()) {
            int margin = 2;
            int size = (radius+margin)*2 + 1;
            Volume * block = new Volume(size, size, size);

            for(int xx = margin; xx <= size-margin; xx++) {
                for(int yy = margin; yy <= size-margin; yy++) {
                    for(int zz = margin; zz <= size-margin; zz++) {
                        block->setDataAt(xx, yy, zz, skeleton.getDataAt(x-radius-margin+xx, y-radius-margin+yy, z-radius-margin+zz));
                    }
                }
            }

            Volume visited(size, size, size);

            vector<Vector3Int> list;
            list.push_back(Vector3Int(margin+radius, margin+radius, margin+radius));
            Vector3Int currentPos, newPos;
            Vector3Float tempDir;

            while(list.size() > 0) {
                currentPos = list[list.size()-1];
                list.pop_back();
                visited.setDataAt(currentPos.X(), currentPos.Y(), currentPos.Z(), 1);
                tempDir = localDirections[skeleton.getIndex(x+currentPos.X()-margin-radius, y+currentPos.Y()-margin-radius, z+currentPos.Z()-margin-radius)];

                if(!tempDir.IsBadNormal()) {
                    for(int i = 0; i < 12; i++) {
                        if((block->getDataAt(currentPos.X() + VOLUME_NEIGHBOR_FACES[i][0][0], currentPos.Y() + VOLUME_NEIGHBOR_FACES[i][0][1], currentPos.Z() + VOLUME_NEIGHBOR_FACES[i][0][2]) > 0) &&
                           (block->getDataAt(currentPos.X() + VOLUME_NEIGHBOR_FACES[i][1][0], currentPos.Y() + VOLUME_NEIGHBOR_FACES[i][1][1], currentPos.Z() + VOLUME_NEIGHBOR_FACES[i][1][2]) > 0) &&
                           (block->getDataAt(currentPos.X() + VOLUME_NEIGHBOR_FACES[i][2][0], currentPos.Y() + VOLUME_NEIGHBOR_FACES[i][2][1], currentPos.Z() + VOLUME_NEIGHBOR_FACES[i][2][2]) > 0)) {
                            for(int j = 0; j < 3; j++) {
                                newPos = currentPos + Vector3Int(VOLUME_NEIGHBOR_FACES[i][j][0], VOLUME_NEIGHBOR_FACES[i][j][1], VOLUME_NEIGHBOR_FACES[i][j][2]);
                                if(visited.getDataAt(newPos.X(), newPos.Y(), newPos.Z()) < 1) {
                                    list.push_back(newPos);
                                }
                            }
                        }
                    }
                }
            }


            delete block;

            vector<Vector3Float> gradient = GetVolumeGradient(visited);

            EigenResults3D eigen;

            GetEigenResult(eigen, gradient, uniformFilterSkeletonDirectionRadius,
                margin+radius, margin+radius, margin+radius,
                size, size, size, radius, false);

            direction = eigen.vecs[0];

        }

        return direction;
    }
    // Gradient = (x+1,y,z) - (x-1,y,z) ....
    vector<Vector3Float> VolumeSkeletonizer::GetVolumeGradient(const Volume & sourceVolume) {
        vector<Vector3Float> gradient(sourceVolume.getSizeX() * sourceVolume.getSizeY() * sourceVolume.getSizeZ());
        int index;

        for(int x = 0; x < sourceVolume.getSizeX(); x = x + sourceVolume.getSizeX()-1) {
            for(int y = 0; y < sourceVolume.getSizeY(); y = y + sourceVolume.getSizeY()-1) {
                for(int z = 0; z < sourceVolume.getSizeZ(); z = z + sourceVolume.getSizeZ()-1) {
                    index = sourceVolume.getIndex(x, y, z);
                    gradient[index] = Vector3Float(0, 0, 0);
                }
            }
        }
        Vector3Float grad1, grad2;

        for(int x = 1; x < sourceVolume.getSizeX()-1; x++) {
            for(int y = 1; y < sourceVolume.getSizeY()-1; y++) {
                for(int z = 1; z < sourceVolume.getSizeZ()-1; z++) {
                    index = sourceVolume.getIndex(x, y, z);
                    grad1 = Vector3Float(sourceVolume.getDataAt(x, y, z) - sourceVolume.getDataAt(x-1, y, z),
                                               sourceVolume.getDataAt(x, y, z) - sourceVolume.getDataAt(x, y-1, z),
                                               sourceVolume.getDataAt(x, y, z) - sourceVolume.getDataAt(x, y, z-1));

                    grad2 = Vector3Float(sourceVolume.getDataAt(x+1, y, z) - sourceVolume.getDataAt(x, y, z),
                                               sourceVolume.getDataAt(x, y+1, z) - sourceVolume.getDataAt(x, y, z),
                                               sourceVolume.getDataAt(x, y, z+1) - sourceVolume.getDataAt(x, y, z));


                    for(int i = 0; i < 3; i++) {
                        if(abs(grad1[i]) > abs(grad2[i])) {
                            gradient[index][i] = grad1[i];
                        } else {
                            gradient[index][i] = grad2[i];
                        }
                    }
                }
            }
        }
        return gradient;
    }

    vector<Vector3Float> VolumeSkeletonizer::GetSkeletonDirection(const Volume & skeleton, int type) {
        int index;

        vector<Vector3Float> localDirections(skeleton.getSizeX() * skeleton.getSizeY() * skeleton.getSizeZ());

        for(int x = 1; x < skeleton.getSizeX()-1; x++) {
            for(int y = 1; y < skeleton.getSizeY()-1; y++) {
                for(int z = 1; z < skeleton.getSizeZ()-1; z++) {
                    index = skeleton.getIndex(x, y, z);
                    localDirections[index] = Vector3Float(0,0,0);
                    if(skeleton.getDataAt(x,y,z) > 0) {
                        switch(type){
                            case PRUNING_CLASS_PRUNE_SURFACES:
                                localDirections[index] = GetSurfaceNormal(skeleton, x, y, z);
                                break;
                        }
                    }
                }
            }
        }

        vector<Vector3Float> directions(skeleton.getSizeX() * skeleton.getSizeY() * skeleton.getSizeZ());

        for(int x = 1; x < skeleton.getSizeX()-1; x++) {
            for(int y = 1; y < skeleton.getSizeY()-1; y++) {
                for(int z = 1; z < skeleton.getSizeZ()-1; z++) {
                    index = skeleton.getIndex(x, y, z);
                    directions[index] = Vector3Float(0,0,0);
                    if(skeleton.getDataAt(x,y,z) > 0) {
                        switch(type){
                            case PRUNING_CLASS_PRUNE_CURVES:
                                directions[index] = GetCurveDirection(skeleton, x, y, z, skeletonDirectionRadius);
                                break;
                            case PRUNING_CLASS_PRUNE_SURFACES:
                                directions[index] = GetSurfaceNormal(skeleton, x, y, z, skeletonDirectionRadius, localDirections);
                                break;
                        }
                    }
                }
            }
        }
        return directions;
    }


    void VolumeSkeletonizer::FindOrthogonalAxes(Vector3Float axis, Vector3Float & res1, Vector3Float & res2) {
        res1 = Vector3Float(1.0, 0.0, 0.0);
        if(abs(axis * res1) > 0.95) {
            res1 = Vector3Float(0.0, 1.0, 0.0);
        }
        res1 = axis ^ res1;
        res2 = axis ^ res1;
        res1.normalize();
        res2.normalize();
    }

    void VolumeSkeletonizer::GetEigenResult(EigenResults3D & returnVal, vector<Vector3Float> & imageGradient, ProbDistr3D & gaussianFilter, int x, int y, int z, int sizeX, int sizeY, int sizeZ, int gaussianFilterRadius, bool clear) {
        if(clear) {
            for(int r = 0; r < 3; r++) {
                returnVal.vals[r] = 0;
                for(int c = 0; c < 3; c++) {
                    returnVal.vecs[r][c] = 0;
                }
            }
        } else {
            Eigen3D eigenData;
            double probability;
            int index2;

            for(int r = 0; r < 3; r++) {
                for(int c = 0; c < 3; c++) {
                    eigenData.tensor[r][c] = 0;
                }
            }

            for(int xx = -gaussianFilterRadius; xx <= gaussianFilterRadius; xx++) {
                for(int yy = -gaussianFilterRadius; yy <= gaussianFilterRadius; yy++) {
                    for(int zz = -gaussianFilterRadius; zz <= gaussianFilterRadius; zz++) {
                        index2 = (x+xx) * sizeY * sizeZ + (y+yy) * sizeZ + z + zz;
                        probability = gaussianFilter.vals[xx+gaussianFilterRadius][yy+gaussianFilterRadius][zz+gaussianFilterRadius];
                        for(int r = 0; r < 3; r++) {
                            for(int c = 0; c < 3; c++) {
                                eigenData.tensor[r][c] += imageGradient[index2][r] * imageGradient[index2][c] * probability;
                            }
                        }
                    }
                }
            }

            math.EigenAnalysis(eigenData);
            for(int r = 0; r < 3; r++) {
                returnVal.vals[r] = eigenData.vals[r];
                for(int c = 0; c < 3; c++) {
                    returnVal.vecs[r][c] = eigenData.vecs[r][c];
                }
            }

            assert((returnVal.vals[0] >= returnVal.vals[1]) && (returnVal.vals[1] >= returnVal.vals[2]));


        }
    }


    void VolumeSkeletonizer::GetSTBasedDistribution(ProbDistr3D & distributionInfo, EigenResults3D eigen) {
        Vector3Float skeletonDirection;
        double total = 0;
        double cell;
        for(int x = -distributionInfo.R; x <= distributionInfo.R; x++) {
            for(int y = -distributionInfo.R; y <= distributionInfo.R; y++) {
                for(int z = -distributionInfo.R; z <= distributionInfo.R; z++) {
                    if((x!=0) && (y!=0) && (z!=0)) {
                        skeletonDirection = Vector3Float(0,0,0) - Vector3Float(x, y, z);
                        skeletonDirection.normalize();
                        cell = GetVoxelCost(eigen, skeletonDirection, PRUNING_CLASS_PRUNE_CURVES);
                        distributionInfo.vals[x+distributionInfo.R][y+distributionInfo.R][z+distributionInfo.R] = cell;
                        total += cell;
                    }
                }
            }
        }

        for(int x = -distributionInfo.R; x <= distributionInfo.R; x++) {
            for(int y = -distributionInfo.R; y <= distributionInfo.R; y++) {
                for(int z = -distributionInfo.R; z <= distributionInfo.R; z++) {
                    distributionInfo.vals[x+distributionInfo.R][y+distributionInfo.R][z+distributionInfo.R] =
                        distributionInfo.vals[x+distributionInfo.R][y+distributionInfo.R][z+distributionInfo.R] / total;
                }
            }
        }

    }

    void VolumeSkeletonizer::PruneCurves(Volume & sourceVolume, int pruneLength) {
        sourceVolume.erodeHelix(pruneLength);
    }

    void VolumeSkeletonizer::PruneSurfaces(Volume & sourceVolume, int pruneLength) {
        sourceVolume.erodeSheet(pruneLength);
    }

    void VolumeSkeletonizer::PruneUsingStructureTensor(
            Volume &  skeleton, const Volume & sourceVolume, Volume * preserveVol,
            vector<Vector3Float> & volumeGradient, vector<EigenResults3D> & volumeEigens,
            ProbDistr3D & filter, double threshold, char pruningClass,
            string outputPath)
    {
        Volume tempSkel(skeleton);
        Volume costVol(skeleton.getSizeX(), skeleton.getSizeY(), skeleton.getSizeZ());
        vector<Vector3Float> skeletonDirections = GetSkeletonDirection(skeleton, pruningClass);
        int index;
        double cost;
        EigenResults3D eigen;

        for(int x = 0; x < skeleton.getSizeX(); x++) {
            for(int y = 0; y < skeleton.getSizeY(); y++) {
                for(int z = 0; z < skeleton.getSizeZ(); z++) {
                    index = skeleton.getIndex(x, y, z);
                    if(((preserveVol == NULL) || ((preserveVol != NULL) && preserveVol->getDataAt(index) < 0.5)) && (tempSkel.getDataAt(index) > 0)) {
                        if(volumeEigens.empty()) {
                            GetEigenResult(eigen, volumeGradient, filter, x, y, z, skeleton.getSizeX(), skeleton.getSizeY(), skeleton.getSizeZ(), filter.R, false);
                        } else {
                            eigen = volumeEigens[index];
                        }
                        cost = GetVoxelCost(eigen, skeletonDirections[index], pruningClass);
                        if(cost < threshold) {
                            skeleton.setDataAt(index, 0.0);
                        }
                        costVol.setDataAt(index, cost);
                    }
                }
            }
        }


        #ifdef SAVE_INTERMEDIATE_RESULTS
            costVol->toMRCFile((char *)((outputPath + "-Score.mrc").c_str()));
            WriteEigenResultsToVRMLFile(sourceVolume, costVol, tempSkel, volumeEigens, outputPath + "-Eigens.wrl", (pruningClass != PRUNING_CLASS_PRUNE_SURFACES));
            WriteEigenResultsToVRMLFile(sourceVolume, costVol, tempSkel, volumeEigens, outputPath + "-Eigens-inverted.wrl", true);
            WriteEigenResultsToVRMLFile(sourceVolume, costVol, tempSkel, volumeEigens, outputPath + "-Eigens.wrl", false);
            WriteSkeletonDirectionToVRMLFile(tempSkel, costVol, skeletonDirections, outputPath + "-SkeletonDirections.wrl", pruningClass == PRUNING_CLASS_PRUNE_SURFACES, 0.1);
            if(pruningClass == PRUNING_CLASS_PRUNE_CURVES) {
                WriteSkeletonDirectionToVRMLFile(tempSkel, costVol, skeletonDirections, outputPath + "-SkeletonDirections-small.wrl", false, 0.06);
            }
        #endif
    }

    void VolumeSkeletonizer::SmoothenVolume(Volume & sourceVolume, double minGrayscale, double maxGrayscale, int stRadius) {
        ProbDistr3D mask;
        mask.R = 1;

        ProbDistr3D smoothenMask;
        smoothenMask.R = stRadius;
        BinomDistr(smoothenMask);

        sourceVolume.pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);

        Volume maskVolume(sourceVolume.getSizeX(), sourceVolume.getSizeY(), sourceVolume.getSizeZ());
        double data;
        for(int x = 0; x < sourceVolume.getSizeX(); x++) {
            for(int y = 0; y < sourceVolume.getSizeY(); y++) {
                for(int z = 0; z < sourceVolume.getSizeZ(); z++) {
                    data = sourceVolume.getDataAt(x, y, z);
                    if((data >= minGrayscale) && (data <= maxGrayscale)) {
                        maskVolume.setDataAt(x, y, z, 1.0);
                    } else {
                        maskVolume.setDataAt(x, y, z, 0.0);
                    }
                }
            }
        }

        vector<Vector3Float> volumeGradient = GetVolumeGradient(sourceVolume);
        vector<EigenResults3D> eigens = GetEigenResults(maskVolume, volumeGradient, smoothenMask, stRadius, true);
        Volume destVolume (sourceVolume.getSizeX(), sourceVolume.getSizeY(), sourceVolume.getSizeZ());
        double sourceData;

        for(int x = mask.R; x < sourceVolume.getSizeX()-mask.R; x++) {
            for(int y = mask.R; y < sourceVolume.getSizeY()-mask.R; y++) {
                for(int z = mask.R; z < sourceVolume.getSizeZ()-mask.R; z++) {
                    sourceData = sourceVolume.getDataAt(x, y, z);
                    if((sourceData >= minGrayscale) && (sourceData <= maxGrayscale)) {
                        GetSTBasedDistribution(mask, eigens[sourceVolume.getIndex(x, y, z)]);

                        for(int xx = -mask.R; xx <= mask.R; xx++) {
                            for(int yy = -mask.R; yy <= mask.R; yy++) {
                                for(int zz = -mask.R; zz <= mask.R; zz++) {
                                    destVolume.setDataAt(x, y, z,
                                        destVolume.getDataAt(x, y, z) + sourceVolume.getDataAt(x+xx, y+yy, z+zz) *  mask.vals[xx+mask.R][yy+mask.R][zz+mask.R]);
                                }
                            }
                        }
                        destVolume.setDataAt(x, y, z, sourceVolume.getDataAt(x, y, z) * 0.5 + destVolume.getDataAt(x, y, z) * 0.5);
                    } else {
                        destVolume.setDataAt(x, y, z, sourceVolume.getDataAt(x, y, z));
                    }
                }
            }
        }

        destVolume.pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);

        sourceVolume = destVolume;
    }
    void VolumeSkeletonizer::VoxelBinarySubtract(Volume * sourceAndDestVolume1, Volume * sourceVolume2){
        for(int x = 0; x < sourceAndDestVolume1->getSizeX(); x++) {
            for(int y = 0; y < sourceAndDestVolume1->getSizeY(); y++) {
                for(int z = 0; z < sourceAndDestVolume1->getSizeZ(); z++) {
                    if(sourceVolume2->getDataAt(x, y, z) > 0) {
                        sourceAndDestVolume1->setDataAt(x, y, z, 0);
                    }
                }
            }
        }
    }

    void VolumeSkeletonizer::VoxelSubtract(Volume * sourceAndDestVolume1, Volume * sourceVolume2){
        for(int x = 0; x < sourceAndDestVolume1->getSizeX(); x++) {
            for(int y = 0; y < sourceAndDestVolume1->getSizeY(); y++) {
                for(int z = 0; z < sourceAndDestVolume1->getSizeZ(); z++) {
                    sourceAndDestVolume1->setDataAt(x, y, z, sourceAndDestVolume1->getDataAt(x, y, z) - sourceVolume2->getDataAt(x, y, z));
                }
            }
        }
    }

    void VolumeSkeletonizer::VoxelOr(Volume * sourceAndDestVolume1, Volume * sourceVolume2){
        if(sourceVolume2 != NULL) {
            for(int x = 0; x < sourceAndDestVolume1->getSizeX(); x++) {
                for(int y = 0; y < sourceAndDestVolume1->getSizeY(); y++) {
                    for(int z = 0; z < sourceAndDestVolume1->getSizeZ(); z++) {
                        sourceAndDestVolume1->setDataAt(x, y, z, max(sourceAndDestVolume1->getDataAt(x, y, z), sourceVolume2->getDataAt(x, y, z)));
                    }
                }
            }
        }
    }


    Volume * VolumeSkeletonizer::FillCurveHoles(Volume * thresholdedSkeleton, Volume * originalSkeleton, int maxHoleSize) {
        Volume * holes = new Volume(*originalSkeleton);
        VoxelSubtract(holes, thresholdedSkeleton);
        PruneCurves(*holes, maxHoleSize);

        Volume * filledSkeleton = new Volume(*originalSkeleton);
        VoxelSubtract(filledSkeleton, holes);
        delete holes;
        return filledSkeleton;
    }

    Volume VolumeSkeletonizer::FillSurfaceHoles(Volume * thresholdedSkeleton, Volume * originalSkeleton, int maxHoleSize) {
        Volume holes(*originalSkeleton);

        VoxelSubtract(&holes, thresholdedSkeleton);
        PruneSurfaces(holes, maxHoleSize);

        Volume filledSkeleton(*originalSkeleton);
        VoxelSubtract(&filledSkeleton, &holes);

        return filledSkeleton;
    }

    Volume * VolumeSkeletonizer::GetImmersionThinning(Volume * sourceVolume, Volume * preserve, double lowGrayscale, double highGrayscale, double stepSize, char thinningClass) {
        Volume * thinnedVolume = new Volume(*sourceVolume);
        thinnedVolume->threshold2(lowGrayscale, 0, 1) ;
        double t;
        switch(thinningClass) {
            case THINNING_CLASS_SURFACE_PRESERVATION :
                for (t = lowGrayscale ; t <= highGrayscale; t+= stepSize) {
                    thinnedVolume->surfaceSkeleton(sourceVolume, t, t + stepSize) ;
                }
                break;
            case THINNING_CLASS_CURVE_PRESERVATION :
                for (t = lowGrayscale ; t <= highGrayscale; t+= stepSize) {
                    thinnedVolume->curveSkeleton(sourceVolume, t, t + stepSize, preserve);
                }
                break;
            case THINNING_CLASS_CURVE_PRESERVATION_2D :
                for (t = lowGrayscale ; t <= highGrayscale; t+= stepSize) {
                    thinnedVolume->curveSkeleton(sourceVolume, t, t + stepSize, preserve);
                }
                break;
            case THINNING_CLASS_POINT_PRESERVATION :
                for (t = lowGrayscale ; t <= highGrayscale; t+= stepSize) {
                    thinnedVolume->pointSkeleton(sourceVolume, t, t + stepSize, preserve, preserve);
                }
        }
        return thinnedVolume;
    }


    Volume * VolumeSkeletonizer::GetJuCurveSkeleton(Volume & sourceVolume, Volume & preserve, double threshold, bool is3D){
        char thinningClass = is3D ? THINNING_CLASS_CURVE_PRESERVATION : THINNING_CLASS_CURVE_PRESERVATION_2D;
        return GetJuThinning(&sourceVolume, &preserve, threshold, thinningClass);
    }

    Volume * VolumeSkeletonizer::GetJuSurfaceSkeleton(Volume & sourceVolume, Volume & preserve, double threshold){
        return GetJuThinning(&sourceVolume, &preserve, threshold, THINNING_CLASS_SURFACE_PRESERVATION);
    }

    Volume * VolumeSkeletonizer::GetJuTopologySkeleton(Volume * sourceVolume, Volume * preserve, double threshold){
        return GetJuThinning(sourceVolume, preserve, threshold, THINNING_CLASS_TOPOLOGY_PRESERVATION);
    }


    Volume * VolumeSkeletonizer::GetJuThinning(Volume * sourceVolume, Volume * preserve, double threshold, char thinningClass) {
        Volume * thinnedVolume = new Volume(*sourceVolume);
        switch(thinningClass) {
            case THINNING_CLASS_SURFACE_PRESERVATION :
                thinnedVolume->surfaceSkeletonPres(threshold, preserve);
                break;
            case THINNING_CLASS_CURVE_PRESERVATION :
                thinnedVolume->curveSkeleton(threshold, preserve);
                break;
            case THINNING_CLASS_CURVE_PRESERVATION_2D :
                thinnedVolume->curveSkeleton2D(threshold, preserve);
                break;
            case THINNING_CLASS_TOPOLOGY_PRESERVATION :
                thinnedVolume->skeleton(threshold, preserve, preserve);
        }

        return thinnedVolume;
    }

    Volume * VolumeSkeletonizer::PerformImmersionSkeletonizationAndPruning(
            Volume & sourceVol, Volume * preserveVol, double startGray,
            double endGray, double stepSize, int smoothingIterations,
            int smoothingRadius, int minCurveSize, int minSurfaceSize,
            int maxCurveHole, int maxSurfaceHole, string outputPath,
            bool doPruning, double pointThreshold, double curveThreshold,
            double surfaceThreshold)
    {
        appTimeManager.PushCurrentTime();
        for(int i = 0; i < smoothingIterations; i++) {
            SmoothenVolume(sourceVol, startGray, endGray, smoothingRadius);
        }
        appTimeManager.PopAndDisplayTime("Smoothing : %f seconds!\n");
        vector<Vector3Float> volumeGradient;
        vector<EigenResults3D> volumeEigens;
        sourceVol.pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);

        if(preserveVol != NULL)
            preserveVol->pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);

        if(doPruning) {
            volumeGradient = GetVolumeGradient(sourceVol);
        }

        Volume nullVol(sourceVol.getSizeX(), sourceVol.getSizeY(), sourceVol.getSizeZ());
        appTimeManager.PushCurrentTime();
        Volume * surfaceVol = GetImmersionThinning(&sourceVol, preserveVol, startGray, endGray, stepSize, THINNING_CLASS_SURFACE_PRESERVATION);
        appTimeManager.PopAndDisplayTime("Surface Thinning : %f seconds!\n");

        #ifdef SAVE_INTERMEDIATE_RESULTS
            surfaceVol->toMRCFile((char *)(outputPath + "-S-Pre-Prune-Pre-Erode.mrc").c_str());
        #endif

        PruneSurfaces(*surfaceVol, minSurfaceSize);

        appTimeManager.PushCurrentTime();
        if(doPruning) {
            #ifdef SAVE_INTERMEDIATE_RESULTS
                surfaceVol->toMRCFile((char *)(outputPath + "-S-Pre-Prune.mrc").c_str());
                WriteVolumeToVRMLFile(surfaceVol, outputPath + "-S-Pre-Prune.wrl");
            #endif
            appTimeManager.PushCurrentTime();
            volumeEigens = GetEigenResults(*surfaceVol, volumeGradient, gaussFiltSrfcR, surfaceRadius, true);
            appTimeManager.PopAndDisplayTime("  Getting Eigens : %f seconds!\n");

            appTimeManager.PushCurrentTime();
            Volume prunedSurfaceVol(*surfaceVol);
            appTimeManager.PopAndDisplayTime("  Getting Copy of surface : %f seconds!\n");


            appTimeManager.PushCurrentTime();
            PruneUsingStructureTensor(prunedSurfaceVol, sourceVol, preserveVol, volumeGradient, volumeEigens, gaussFiltSrfcR, surfaceThreshold, PRUNING_CLASS_PRUNE_SURFACES, outputPath + "-S");
            appTimeManager.PopAndDisplayTime("  Pruning: %f seconds!\n");

            appTimeManager.PushCurrentTime();

            #ifdef SAVE_INTERMEDIATE_RESULTS
                prunedSurfaceVol->toMRCFile((char *)(outputPath + "-S-Post-Prune.mrc").c_str());
            #endif

            *surfaceVol = prunedSurfaceVol;
            appTimeManager.PopAndDisplayTime("  Memory Cleanup: %f seconds!\n");

        }

        PruneSurfaces(*surfaceVol, minSurfaceSize);
        appTimeManager.PopAndDisplayTime("Surface Pruning  : %f seconds!\n");

        #ifdef SAVE_INTERMEDIATE_RESULTS
            surfaceVol->toMRCFile((char *)(outputPath + "-S-Post-Erosion.mrc").c_str());
        #endif

        Volume cleanedSurfaceVol = *GetJuSurfaceSkeleton(*surfaceVol, nullVol, 0.5);
        PruneSurfaces(cleanedSurfaceVol, minSurfaceSize);
        #ifdef SAVE_INTERMEDIATE_RESULTS
            cleanedSurfaceVol->toMRCFile((char *)(outputPath + "-S-Cleaned.mrc").c_str());
        #endif

        *surfaceVol = cleanedSurfaceVol;
        VoxelOr(surfaceVol, preserveVol);

        appTimeManager.PushCurrentTime();

        Volume curveVol = *GetImmersionThinning(&sourceVol, surfaceVol, startGray, endGray, stepSize, THINNING_CLASS_CURVE_PRESERVATION);
        appTimeManager.PopAndDisplayTime("Curve Thinning   : %f seconds!\n");

        #ifdef SAVE_INTERMEDIATE_RESULTS
            curveVol->toMRCFile((char *)(outputPath + "-C-Pre-Prune_Pre-Erode.mrc").c_str());
        #endif

        PruneCurves(curveVol, minCurveSize);
        VoxelBinarySubtract(&curveVol, surfaceVol);

        appTimeManager.PushCurrentTime();
        if(doPruning) {
            #ifdef SAVE_INTERMEDIATE_RESULTS
                curveVol->toMRCFile((char *)(outputPath + "-C-Pre-Prune.mrc").c_str());
            #endif

            volumeEigens = GetEigenResults(curveVol, volumeGradient, gaussFiltCrvR, curveRadius, true);
            Volume prunedCurveVol(curveVol);
            PruneUsingStructureTensor(prunedCurveVol, sourceVol, preserveVol, volumeGradient, volumeEigens, gaussFiltCrvR, curveThreshold, PRUNING_CLASS_PRUNE_CURVES, outputPath + "-C");

            #ifdef SAVE_INTERMEDIATE_RESULTS
                prunedCurveVol->toMRCFile((char *)(outputPath + "-C-Post-Prune.mrc").c_str());
            #endif

            Volume * filledCurveVol = FillCurveHoles(&prunedCurveVol, &curveVol, maxCurveHole);
            #ifdef SAVE_INTERMEDIATE_RESULTS
                filledCurveVol->toMRCFile((char *)(outputPath + "-C-Post-Fill.mrc").c_str());
            #endif

            curveVol = *filledCurveVol;
        }

        VoxelOr(&curveVol, surfaceVol);
        PruneCurves(curveVol, minCurveSize);
        appTimeManager.PopAndDisplayTime("Curve Pruning    : %f seconds!\n");
        #ifdef SAVE_INTERMEDIATE_RESULTS
            curveVol->toMRCFile((char *)(outputPath + "-C-Post-Erosion.mrc").c_str());
        #endif

        Volume cleanedCurveVol = *GetJuCurveSkeleton(curveVol, *surfaceVol, 0.5, true);
        PruneCurves(cleanedCurveVol, minCurveSize);
        #ifdef SAVE_INTERMEDIATE_RESULTS
            cleanedCurveVol->toMRCFile((char *)(outputPath + "-C-Cleaned.mrc").c_str());
        #endif

        curveVol = cleanedCurveVol;

        VoxelOr(&curveVol, surfaceVol);
        #ifdef SAVE_INTERMEDIATE_RESULTS
            curveVol->toMRCFile((char *)(outputPath + "-SC.mrc").c_str());
        #endif

        delete surfaceVol;
        //delete curveVol;

        #ifdef SAVE_INTERMEDIATE_RESULTS
            curveVol->toOFFCells2((char *)(outputPath + "-SC.off").c_str());
        #endif

        sourceVol.pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);
        curveVol.pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);

        Volume * res = new Volume(curveVol);

        return res;
    }


    Volume * VolumeSkeletonizer::PerformPureJuSkeletonization(Volume * imageVol, string outputPath, double threshold, int minCurveWidth, int minSurfaceWidth) {
        #ifdef GORGON_DEBUG
              cout<<"\033[33mDEBUG: File:   VolumeSkeletonizer.h"<<endl;
              cout<<"DEBUG: Method: VolumeSkeletonizer::PerformPureJuSkeletonization\033[0m"<<endl;
              cout<<"DEBUG: Args: Volume*, string, double, int, int\033[0m"<<endl;
              cout<<imageVol->getSize()<<endl;
        #endif
        imageVol->pad(MAX_GAUSSIAN_FILTER_RADIUS, 0);
        #ifdef GORGON_DEBUG
              cout<<"imageVol->getSize(): "<<imageVol->getSize()<<endl;
        #endif


        Volume * preservedVol = new Volume(imageVol->getSizeX(), imageVol->getSizeY(), imageVol->getSizeZ());
        Volume * surfaceVol;
        Volume * curveVol;
        Volume * topologyVol;

        //printf("\t\t\tUSING THRESHOLD : %f\n", threshold);
        // Skeletonizing while preserving surface features curve features and topology
        surfaceVol = GetJuSurfaceSkeleton(*imageVol, *preservedVol, threshold);
        #ifdef GORGON_DEBUG
              cout<<"surfaceVol->getSize(): "<<surfaceVol->getSize()<<endl;
        #endif

        PruneSurfaces(*surfaceVol, minSurfaceWidth);
        VoxelOr(preservedVol, surfaceVol);

        curveVol = VolumeSkeletonizer::GetJuCurveSkeleton(*imageVol, *preservedVol, threshold, true);
        #ifdef GORGON_DEBUG
              cout<<"curveVol->getSize(): "<<curveVol->getSize()<<endl;
        #endif
        VolumeSkeletonizer::PruneCurves(*curveVol, minCurveWidth);
        VoxelOr(preservedVol, curveVol);
#ifdef GORGON_DEBUG
        cout<<"preservedVol->getSize(): "<<preservedVol->getSize()<<endl;
        cout<<"curveVol->getSize(): "<<curveVol->getSize()<<endl;
#endif

        topologyVol = VolumeSkeletonizer::GetJuTopologySkeleton(imageVol, preservedVol, threshold);
#ifdef GORGON_DEBUG
        cout<<"1: topologyVol->getSize(): "<<topologyVol->getSize()<<endl;
#endif

        imageVol->pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);
        topologyVol->pad(-MAX_GAUSSIAN_FILTER_RADIUS, 0);

        delete preservedVol;
        delete surfaceVol;
        delete curveVol;

        #ifdef GORGON_DEBUG
              cout<<"\033[34mDEBUG: File:   VolumeSkeletonizer.h"<<endl;
              cout<<"DEBUG: Method: VolumeSkeletonizer::PerformPureJuSkeletonization\033[0m"<<endl;
              cout<<"DEBUG: Args: Volume*, string, double, int, int\033[0m"<<endl;
              cout<<"2: topologyVol->getSize(): "<<topologyVol->getSize()<<endl;
        #endif

        return topologyVol;
    }
}

#endif
