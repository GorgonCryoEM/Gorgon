/*
 * NormalFinder.h
 *
 *      Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_SKELETONMAKER_NORMALFINDER_H_
#define SRC_TOOLKIT_SKELETONMAKER_NORMALFINDER_H_

//#include <MathTools/Vector3.h>
#include "Core/GlobalDefinitions.h"
#include <Core/volume.h>

//using namespace std;
//using namespace GraphMatch;
using namespace Core;

namespace GraySkeletonCPP {

    const int nodeVectors[6][3] = {{-1,0,0}, {0,1,0}, {1,0,0}, {0,-1,0}, {0,0,-1}, {0,0,1}};

    class NormalFinder {

        public:
            NormalFinder();

            void InitializeGraph(const Volume & vol, int x, int y, int z);
            Vec3F GetSurfaceNormal();

        private:
            void SetCurveIndex(int x, int y, int z, int index);
            int GetCurveIndex(int x, int y, int z);
            bool FindOrdering();
            bool FindOrdering(int level);
            bool IsSameOrdering();
            bool CyclesBackToStart();

        private:
            int edgeCount;
            bool graph[6][6];
            int faceIndex[3][3][3];
            bool usedValidNodes[6];
            int validNodes[6];
            int validNodeCount;
            int ordering[6];
            int correctOrdering[6];
            int solutionCount;
    };

} /* namespace GraySkeletonCPP */

#endif /* SRC_TOOLKIT_SKELETONMAKER_NORMALFINDER_H_ */
