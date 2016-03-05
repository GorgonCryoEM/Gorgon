/*
 * DiscreteMesh.h
 *
 *      Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_DISCRETEMESH_H_
#define SRC_TOOLKIT_VISUALIZATION_DISCRETEMESH_H_

//#include "Core/GlobalDefinitions.h"
#include <Core/volume.h>
//#include <MathTools/DataStructures.h>
#include <MathTools/Vector3.h>
//#include <string>
//#include <list>
//#include <functional>
#include <vector>

using namespace std;
using namespace MathTools;
using namespace SkeletonMaker;


namespace GraySkeletonCPP {

    /*
     *
     */

    const unsigned char CURVE_TYPE_XPOS = 1;
    const unsigned char CURVE_TYPE_YPOS = 2;
    const unsigned char CURVE_TYPE_ZPOS = 4;
    const unsigned char CURVE_TYPES[2][2][2] = {{{0,CURVE_TYPE_ZPOS}, {CURVE_TYPE_YPOS,0}},
                                                {{CURVE_TYPE_XPOS,0}, {0,0}}
    };
    const unsigned char CURVE_TYPES_3[3]     = {CURVE_TYPE_XPOS, CURVE_TYPE_YPOS, CURVE_TYPE_ZPOS};

    const int VOLUME_NEIGHBOR_CURVES_6[6][7] = {{0,0,0,CURVE_TYPE_XPOS,1,0,0}, {-1,0,0,CURVE_TYPE_XPOS,-1,0,0},
                                                {0,0,0,CURVE_TYPE_YPOS,0,1,0}, {0,-1,0,CURVE_TYPE_YPOS,0,-1,0},
                                                {0,0,0,CURVE_TYPE_ZPOS,0,0,1}, {0,0,-1,CURVE_TYPE_ZPOS,0,0,-1}};

    const unsigned char SURFACE_TYPE_XY = 1;
    const unsigned char SURFACE_TYPE_XZ = 2;
    const unsigned char SURFACE_TYPE_YZ = 4;
    const unsigned char SURFACE_TYPES[2][2][2] = {{{0,0},{0,SURFACE_TYPE_YZ}},{{0,SURFACE_TYPE_XZ},{SURFACE_TYPE_XY,0}}};
    const unsigned char SURFACE_TYPES_3[3]     = {SURFACE_TYPE_YZ, SURFACE_TYPE_XZ, SURFACE_TYPE_XY};

    const int VOLUME_NEIGHBOR_SURFACES_12[12][4] = {{0,0,0,SURFACE_TYPE_XY},  {-1,0,0,SURFACE_TYPE_XY}, {-1,-1,0,SURFACE_TYPE_XY}, {0,-1,0,SURFACE_TYPE_XY},
                                                    {0,0,0,SURFACE_TYPE_YZ},  {0,0,0,SURFACE_TYPE_XZ},  {0,-1,0,SURFACE_TYPE_YZ},  {-1,0,0,SURFACE_TYPE_XZ},
                                                    {0,0,-1,SURFACE_TYPE_YZ}, {0,0,-1,SURFACE_TYPE_XZ}, {0,-1,-1,SURFACE_TYPE_YZ}, {-1,0,-1,SURFACE_TYPE_XZ}};

    class DiscreteMesh : public Volume {
        public:
            DiscreteMesh(int sX, int sY, int sZ);
            DiscreteMesh(const Volume & vol);

            void addVoxel              (int x, int y, int z);
            bool isPointPresent        (int x, int y, int z);
            bool isCurvePresent        (int x, int y, int z, unsigned char direction);
            bool isCurvePresent(Vec3I p1, Vec3I p2);
            bool isSurfacePresent      (int x, int y, int z, unsigned char direction);
            bool followCurve           (int & x, int & y, int & z);
            int getCurveNeighbors      (int x, int y, int z, vector<Vec3I> & neighbors);
            int getCurveNeighborsCount (int x, int y, int z);
            int getSurfaceNeighbors    (int x, int y, int z, vector<int> & neighbors);
            int getSurfaceNeighbors    (int x1, int y1, int z1, int x2, int y2, int z2, vector<int> & neighbors);
            int getSurfaceNeighborCount(int x1, int y1, int z1, int x2, int y2, int z2);
            void getSurfacePoints      (int x, int y, int z, unsigned char direction, vector<Vec3I> & points);

            static int getC6 (vector<Vec3I> &neighbors, int neighborCount, Vec3I currPoint);
            static int getC26(vector<Vec3I> &neighbors, int neighborCount, Vec3I currPoint);
            static int getN6   (vector<Vec3I> &   n6, const Volume & src, int x, int y, int z);
            static int getN6_2 (vector<Vec3I> & n6_2, const Volume & src, int x, int y, int z);
            static int getN18  (vector<Vec3I> &  n18, const Volume & src, int x, int y, int z);
            static int getN26  (vector<Vec3I> &  n26, const Volume & src, int x, int y, int z);
            static int getN6Count                    (const Volume & src, int x, int y, int z);
            static int getN6_2Count                  (const Volume & src, int x, int y, int z);
            static int getN18Count                   (const Volume & src, int x, int y, int z);
            static int getN26Count                   (const Volume & src, int x, int y, int z);
            static int getMCount                     (const Volume & src, int x1, int y1, int z1,
                                                                          int x2, int y2, int z2);
            static int getImmersionN6Count       (Volume & skel, Vec3I point);
            static int getImmersionSkeletalValue (Volume & skel, Vec3I point);
            static bool isImmersionBoundary(const Volume & skel, Vec3I point);
            static bool isPoint                      (const Volume & src, int x, int y, int z);
            static bool isCurveEnd                   (const Volume & src, int x, int y, int z);
            static bool isCurveBody                  (const Volume & src, int x, int y, int z);
            static bool isSurfaceBorder              (const Volume & src, int x, int y, int z);
            static bool isSurfaceBody                (const Volume & src, int x, int y, int z, bool doDependantChecks);
            static bool isVolumeBorder               (const Volume & src, int x, int y, int z, bool doDependantChecks);
            static bool isVolumeBody                 (const Volume & src, int x, int y, int z);
            static bool isSimple                     (      Volume & src, int x, int y, int z);
            static bool isValidSurface               (const Volume & src,
                                                      Vec3D p0, Vec3D p1, Vec3D p2, Vec3D p3);

            template <class T>
            static void findCurveBase  (T &p1, T &p2);
            template <class T>
            static void findSurfaceBase(T &p1, T &p2, T &p3, T &p4);

            void addPoint     (Vec3I point);
            void addCurve     (Vec3I p1, Vec3I p2);
            void addSurface   (Vec3I p1, Vec3I p2, Vec3I p3, Vec3I p4);
            void removePoint  (Vec3I point);
            void removeCurve  (Vec3I p1, Vec3I p2);
            void removeSurface(Vec3I p1, Vec3I p2, Vec3I p3, Vec3I p4);
            void removeSurface(int x, int y, int z, unsigned char surfaceType);

        private:
            vector<bool>          points;
            vector<unsigned char> curves;
            vector<unsigned char> surfaces;
        };

} /* namespace GraySkeletonCPP */

#endif /* SRC_TOOLKIT_VISUALIZATION_DISCRETEMESH_H_ */
