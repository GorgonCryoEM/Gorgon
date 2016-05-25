#ifndef TOOLKIT_GRAYSKELETONCPP_DISCRETE_MESH_H
#define TOOLKIT_GRAYSKELETONCPP_DISCRETE_MESH_H

//#include "Core/GlobalDefinitions.h"
//#include <Core/volume.h>
//#include <MathTools/DataStructures.h>
//#include <MathTools/Vector3.h>
//#include <string>
//#include <list>
//#include <functional>
#include <vector>

using namespace std;
using namespace MathTools;
using namespace SkeletonMaker;

namespace GraySkeletonCPP {
    const unsigned char CURVE_TYPE_XPOS = 1;
    const unsigned char CURVE_TYPE_YPOS = 2;
    const unsigned char CURVE_TYPE_ZPOS = 4;
    const unsigned char CURVE_TYPES[2][2][2] = {{{0,CURVE_TYPE_ZPOS},{CURVE_TYPE_YPOS,0}}, {{CURVE_TYPE_XPOS,0}, {0,0}}};
    const unsigned char CURVE_TYPES_3[3] = {CURVE_TYPE_XPOS, CURVE_TYPE_YPOS, CURVE_TYPE_ZPOS};
    const int VOLUME_NEIGHBOR_CURVES_6[6][7] = {{0,0,0,CURVE_TYPE_XPOS,1,0,0}, {-1,0,0,CURVE_TYPE_XPOS,-1,0,0},
                                                {0,0,0,CURVE_TYPE_YPOS,0,1,0}, {0,-1,0,CURVE_TYPE_YPOS,0,-1,0},
                                                {0,0,0,CURVE_TYPE_ZPOS,0,0,1}, {0,0,-1,CURVE_TYPE_ZPOS,0,0,-1}};

    const unsigned char SURFACE_TYPE_XY = 1;
    const unsigned char SURFACE_TYPE_XZ = 2;
    const unsigned char SURFACE_TYPE_YZ = 4;
    const unsigned char SURFACE_TYPES[2][2][2] = {{{0,0},{0,SURFACE_TYPE_YZ}},{{0,SURFACE_TYPE_XZ},{SURFACE_TYPE_XY,0}}};
    const unsigned char SURFACE_TYPES_3[3] = {SURFACE_TYPE_YZ, SURFACE_TYPE_XZ, SURFACE_TYPE_XY};

    const int VOLUME_NEIGHBOR_SURFACES_12[12][4] = {{0,0,0,SURFACE_TYPE_XY},  {-1,0,0,SURFACE_TYPE_XY}, {-1,-1,0,SURFACE_TYPE_XY}, {0,-1,0,SURFACE_TYPE_XY},
                                                    {0,0,0,SURFACE_TYPE_YZ},  {0,0,0,SURFACE_TYPE_XZ},  {0,-1,0,SURFACE_TYPE_YZ},  {-1,0,0,SURFACE_TYPE_XZ},
                                                    {0,0,-1,SURFACE_TYPE_YZ}, {0,0,-1,SURFACE_TYPE_XZ}, {0,-1,-1,SURFACE_TYPE_YZ}, {-1,0,-1,SURFACE_TYPE_XZ}};


    class DiscreteMesh : public Volume {
        public:
            DiscreteMesh(int sizeX, int sizeY, int sizeZ);
            DiscreteMesh(const Volume & volume);

            void addVoxel(int x, int y, int z);
            int getIndex(int x, int y, int z);
            bool isPointPresent(int x, int y, int z);
            bool isCurvePresent(int x, int y, int z, unsigned char direction);
            bool isCurvePresent(Vector3Int point1, Vector3Int point2);
            bool isSurfacePresent(int x, int y, int z, unsigned char direction);
            bool FollowCurve(int & x, int & y, int & z);
            int getCurveNeighbors(int x, int y, int z, vector<Vector3Int> & neighbors);
            int getCurveNeighborsCount(int x, int y, int z);
            int getSurfaceNeighbors(int x, int y, int z, vector<int> & neighbors);
            int getSurfaceNeighbors(int x1, int y1, int z1, int x2, int y2, int z2, vector<int> & neighbors);
            int getSurfaceNeighborCount(int x1, int y1, int z1, int x2, int y2, int z2);
            void getSurfacePoints(int x, int y, int z, unsigned char direction, vector<Vector3Int> & points);

            static int getC6 (Vector3Int * neighbors, int neighborCount, Vector3Int currPoint);
            static int getC26(Vector3Int * neighbors, int neighborCount, Vector3Int currPoint);
            static int getN6  (vector<Vector3Int> &   n6, Volume * sourceVolume, int x, int y, int z);
            static int getN6_2(vector<Vector3Int> & n6_2, Volume * sourceVolume, int x, int y, int z);
            static int getN18  (vector<Vector3Int> & n18, Volume * sourceVolume, int x, int y, int z);
            static int getN26        (Vector3Int * & n26, Volume * sourceVolume, int x, int y, int z);
            static int getN6Count  (const Volume & sourceVolume, int x, int y, int z);
            static int getN6_2Count(Volume * sourceVolume, int x, int y, int z);
            static int getN18Count (Volume * sourceVolume, int x, int y, int z);
            static int getN26Count (Volume * sourceVolume, int x, int y, int z);
            static int getMCount   (Volume * sourceVolume, int x1, int y1, int z1,
                                 int x2, int y2, int z2);
            static int getImmersionN6Count      (Volume * skeleton, Vector3Int point);
            static int getImmersionSkeletalValue(Volume * skeleton, Vector3Int point);
            static bool isImmersionBoundary(Volume * skeleton, Vector3Int point);
            static bool isPoint         (Volume * sourceVolume, int x, int y, int z);
            static bool isCurveEnd      (Volume * sourceVolume, int x, int y, int z);
            static bool isCurveBody     (Volume * sourceVolume, int x, int y, int z);
            static bool isSurfaceBorder (Volume * sourceVolume, int x, int y, int z);
            static bool isSurfaceBody   (Volume * sourceVolume, int x, int y, int z, bool doDependantChecks);
            static bool isVolumeBorder  (Volume * sourceVolume, int x, int y, int z, bool doDependantChecks);
            static bool isVolumeBody    (Volume * sourceVolume, int x, int y, int z);
            static bool isSimple        (Volume * sourceVolume, int x, int y, int z);
            static bool isValidSurface(const Volume & sourceVolume, Vector3Double p0,
                                       Vector3Double p1, Vector3Double p2, Vector3Double p3);

            static void findCurveBase  (Vector3Double &p1, Vector3Double &p2);
            static void findCurveBase  (Vector3Int    &p1, Vector3Int    &p2);
            static void findSurfaceBase(Vector3Double &p1, Vector3Double &p2,
                                        Vector3Double &p3, Vector3Double &p4);
            static void findSurfaceBase(Vector3Int &p1, Vector3Int &p2,
                                        Vector3Int &p3, Vector3Int &p4);

            void addPoint  (Vector3Int point);
            void addCurve  (Vector3Int p1, Vector3Int p2);
            void addSurface(Vector3Int p1, Vector3Int p2, Vector3Int p3, Vector3Int p4);
            void removePoint   (Vector3Int point);
            void removeCurve   (Vector3Int p1, Vector3Int p2);
            void removeSurface(Vector3Int p1, Vector3Int p2, Vector3Int p3, Vector3Int p4);
            void removeSurface(int x, int y, int z, unsigned char surfaceType);

        private:
            vector<bool> points;
            vector<unsigned char> curves;
            vector<unsigned char> surfaces;
    };

    DiscreteMesh::DiscreteMesh(int sizeX, int sizeY, int sizeZ)
            : Volume(sizeX, sizeY, sizeZ),
              points   (sizeX * sizeY * sizeZ, false),
              curves   (sizeX * sizeY * sizeZ, 0),
              surfaces (sizeX * sizeY * sizeZ, 0)
    {}

    DiscreteMesh::DiscreteMesh(const Volume & volume)
        :  Volume(volume),
           points   (size.X() * size.Y() * size.Z(), false),
           curves   (size.X() * size.Y() * size.Z(), 0),
           surfaces (size.X() * size.Y() * size.Z(), 0)
    {
        int sizeX = getSizeX();
        int sizeY = getSizeY();
        int sizeZ = getSizeZ();

        for(int x = 0; x < sizeX; x++) {
            for(int y = 0; y < sizeY; y++) {
                for(int z = 0; z < sizeZ; z++) {
                    if(getDataAt(x, y, z) > 0) {
                        addVoxel(x, y, z);
                    }
                }
            }
        }
    }

    void DiscreteMesh::addVoxel(int x, int y, int z) {
        setDataAt(x, y, z, 1);
        Vector3Int p(x, y, z);
        int neighborCount = 0;

        if(isPoint(this, x, y, z)) {
            addPoint(p);
        } else if (isCurveEnd(this, x, y, z) || isCurveBody(this, x, y, z)) {
            Vector3Int neighbors[6];
            for(int n = 0; n < 6; n++) {
                if(getDataAt(x + VOLUME_NEIGHBORS_6[n][0], y + VOLUME_NEIGHBORS_6[n][1], z + VOLUME_NEIGHBORS_6[n][2]) > 0) {
                    neighbors[neighborCount][0] = x + VOLUME_NEIGHBORS_6[n][0];
                    neighbors[neighborCount][1] = y + VOLUME_NEIGHBORS_6[n][1];
                    neighbors[neighborCount][2] = z + VOLUME_NEIGHBORS_6[n][2];
                    neighborCount++;
                }
            }
            for(int n = 0; n < neighborCount; n++) {
                addCurve(p, neighbors[n]);
            }
        } else if (isSurfaceBorder(this, x, y, z) || isSurfaceBody(this, x, y, z, true)) {
            Vector3Int faces[12][3];
            int faceCount = 0;
            bool allPoints;
            for(int n = 0; n < 12; n++) {
                allPoints = true;
                for(int p = 0; p < 3; p++) {
                    faces[faceCount][p][0] = x + VOLUME_NEIGHBOR_FACES[n][p][0];
                    faces[faceCount][p][1] = y + VOLUME_NEIGHBOR_FACES[n][p][1];
                    faces[faceCount][p][2] = z + VOLUME_NEIGHBOR_FACES[n][p][2];
                    allPoints = allPoints && (getDataAt(faces[faceCount][p][0], faces[faceCount][p][1], faces[faceCount][p][2]) > 0);
                }
                if(allPoints)
                    faceCount++;
            }

            for(int n = 0; n < faceCount; n++)
                addSurface(p, faces[n][0], faces[n][1], faces[n][2]);
        }
    }

    int DiscreteMesh::getIndex(int x, int y, int z) {
        return getIndex(x, y, z);
    }

    bool DiscreteMesh::isPointPresent(int x, int y, int z) {
        return points[getIndex(x, y, z)];
    }

    bool DiscreteMesh::isCurvePresent(int x, int y, int z, unsigned char direction) {
        return ((curves[getIndex(x, y, z)] & direction) == direction);
    }

    bool DiscreteMesh::isCurvePresent(Vector3Int point1, Vector3Int point2) {
        Vector3Int p1, p2;
        p1 = point1;
        p2 = point2;
        findCurveBase(p1, p2);

        unsigned char curveType = CURVE_TYPES[p2[0] - p1[0]][p2[1] - p1[1]][p2[2] - p1[2]];
        return isCurvePresent(p1[0], p1[1], p1[2], curveType);
    }


    bool DiscreteMesh::isSurfacePresent(int x, int y, int z, unsigned char direction) {
        return ((surfaces[getIndex(x, y, z)] & direction) == direction);
    }

    bool DiscreteMesh::FollowCurve(int & x, int & y, int & z) {
        vector<Vector3Int> neighbors(6);
        int count = getCurveNeighbors(x, y, z, neighbors);
        if(count==1) {
            x = neighbors[0][0];
            y = neighbors[0][1];
            z = neighbors[0][2];
        }
        return (count==1);
    }

    int DiscreteMesh::getCurveNeighbors(int x, int y, int z, vector<Vector3Int> & neighbors) {
        int count = 0;

        for(int i = 0; i < 6; i++) {
            if(isCurvePresent(x+VOLUME_NEIGHBOR_CURVES_6[i][0], y+VOLUME_NEIGHBOR_CURVES_6[i][1], z+VOLUME_NEIGHBOR_CURVES_6[i][2], VOLUME_NEIGHBOR_CURVES_6[i][3])) {
                neighbors[count][0] = x + VOLUME_NEIGHBOR_CURVES_6[i][4];
                neighbors[count][1] = y + VOLUME_NEIGHBOR_CURVES_6[i][5];
                neighbors[count][2] = z + VOLUME_NEIGHBOR_CURVES_6[i][6];
                count++;
            }
        }
        return count;
    }

    int DiscreteMesh::getCurveNeighborsCount(int x, int y, int z) {
        int count = 0;
        for(int i = 0; i < 6; i++) {
            if(isCurvePresent(x+VOLUME_NEIGHBOR_CURVES_6[i][0], y+VOLUME_NEIGHBOR_CURVES_6[i][1], z+VOLUME_NEIGHBOR_CURVES_6[i][2], VOLUME_NEIGHBOR_CURVES_6[i][3])) {
                count++;
            }
        }
        return count;
    }

    int DiscreteMesh::getSurfaceNeighbors(int x, int y, int z, vector<int> & neighbors) {
        int count = 0;
        neighbors.resize(12*4);

        for(int i = 0; i < 12; i++) {
            if(isSurfacePresent(x+VOLUME_NEIGHBOR_SURFACES_12[i][0], y+VOLUME_NEIGHBOR_SURFACES_12[i][1], z+VOLUME_NEIGHBOR_SURFACES_12[i][2], VOLUME_NEIGHBOR_SURFACES_12[i][3])) {
                neighbors[count*4 + 0] = x + VOLUME_NEIGHBOR_SURFACES_12[i][0];
                neighbors[count*4 + 1] = y + VOLUME_NEIGHBOR_SURFACES_12[i][1];
                neighbors[count*4 + 2] = z + VOLUME_NEIGHBOR_SURFACES_12[i][2];
                neighbors[count*4 + 3] = VOLUME_NEIGHBOR_SURFACES_12[i][3];
                count++;
            }
        }
        return count;
    }

    int DiscreteMesh::getSurfaceNeighbors(int x1, int y1, int z1, int x2, int y2, int z2, vector<int> & neighbors) {
        vector<int> p1Neighbors, p2Neighbors;
        int p1Count, p2Count;
        p1Count = getSurfaceNeighbors(x1, y1, z1, p1Neighbors);
        p2Count = getSurfaceNeighbors(x2, y2, z2, p2Neighbors);

        int nCount = 0;
        neighbors.resize(4*4);
        bool allFound;

        for(int i = 0; i < p1Count; i++) {
            for(int j = 0; j < p2Count; j++) {
                allFound = true;
                for(int k = 0; k < 4; k++) {
                    allFound = allFound && (p1Neighbors[i*4 +k] == p2Neighbors[j*4 +k]);
                }
                if(allFound) {
                    for(int k = 0; k < 4; k++) {
                        neighbors[nCount*4 +k] = p1Neighbors[i*4 +k];
                    }
                    nCount++;
                }
            }
        }

        return nCount;
    }

    int DiscreteMesh::getSurfaceNeighborCount(int x1, int y1, int z1,
                                              int x2, int y2, int z2)
    {
        vector<int> p1Neighbors, p2Neighbors;
        int p1Count, p2Count;
        p1Count = getSurfaceNeighbors(x1, y1, z1, p1Neighbors);
        p2Count = getSurfaceNeighbors(x2, y2, z2, p2Neighbors);

        int nCount = 0;
        bool allFound;

        for(int i = 0; i < p1Count; i++) {
            for(int j = 0; j < p2Count; j++) {
                allFound = true;
                for(int k = 0; k < 4; k++) {
                    allFound = allFound && (p1Neighbors[i*4 +k] == p2Neighbors[j*4 +k]);
                }
                if(allFound) {
                    nCount++;
                }
            }
        }

        return nCount;
    }

    void DiscreteMesh::getSurfacePoints(int x, int y, int z,
                                        unsigned char direction,
                                        vector<Vector3Int> & points)
    {
        points.resize(4);
        switch(direction) {
            case SURFACE_TYPE_XY:
                points[0] = Vector3Int(x, y, z);
                points[1] = Vector3Int(x+1, y, z);
                points[2] = Vector3Int(x+1, y+1, z);
                points[3] = Vector3Int(x, y+1, z);
                break;
            case SURFACE_TYPE_XZ:
                points[0] = Vector3Int(x, y, z);
                points[1] = Vector3Int(x+1, y, z);
                points[2] = Vector3Int(x+1, y, z+1);
                points[3] = Vector3Int(x, y, z+1);
                break;
            case SURFACE_TYPE_YZ:
                points[0] = Vector3Int(x, y, z);
                points[1] = Vector3Int(x, y+1, z);
                points[2] = Vector3Int(x, y+1, z+1);
                points[3] = Vector3Int(x, y, z+1);
                break;
        }
    }

    void DiscreteMesh::addPoint(Vector3Int point){
        //printf("Adding Point: {%i %i %i}\n", point[0], point[1], point[2]);
        points[getIndex(point[0], point[1], point[2])] = true;
    }

    void DiscreteMesh::addCurve(Vector3Int p1, Vector3Int p2){
        removePoint(p1);
        removePoint(p2);
        Vector3Int p11 = p1, p22 = p2;
        findCurveBase(p11, p22);

        unsigned char curveType = CURVE_TYPES[p22[0] - p11[0]][p22[1] - p11[1]][p22[2] - p11[2]];
        int index = getIndex(p11[0], p11[1], p11[2]);

        //printf("Adding Curve: {%i %i %i} - {%i %i %i} %i\n",
        //	p11[0], p11[1], p11[2],
        //	p22[0], p22[1], p22[2], curveType);

        curves[index] = curves[index] | curveType;
    }

    void DiscreteMesh::addSurface(Vector3Int p1, Vector3Int p2, Vector3Int p3, Vector3Int p4)
    {
        removePoint(p1);
        removePoint(p2);
        removePoint(p3);
        removePoint(p4);

        Vector3Int p11 = p1, p22 = p2, p33 = p3, p44 = p4;
        findSurfaceBase(p11, p22, p33, p44);
        removeCurve(p11, p22);
        removeCurve(p22, p33);
        removeCurve(p44, p33);
        removeCurve(p11, p44);
        unsigned char surfaceType = SURFACE_TYPES[p33[0] - p11[0]][p33[1] - p11[1]][p33[2] - p11[2]];
        int index = getIndex(p11[0], p11[1], p11[2]);

        surfaces[index] = surfaces[index] | surfaceType;
        //printf("Adding Surface: {%i %i %i} - {%i %i %i} - {%i %i %i} - {%i %i %i} - %i\n",
        //	p11[0], p11[1], p11[2],
        //	p22[0], p22[1], p22[2],
        //	p33[0], p33[1], p33[2],
        //	p44[0], p44[1], p44[2],
        //	surfaceType);
    }

    void DiscreteMesh::removePoint(Vector3Int point){
        //printf("Removing Point: {%i %i %i}\n", point[0], point[1], point[2]);
        points[getIndex(point[0], point[1], point[2])] = false;
    }

    void DiscreteMesh::removeCurve(Vector3Int p1, Vector3Int p2) {
        Vector3Int p11 = p1, p22 = p2;
        findCurveBase(p11, p22);

        unsigned char curveType = CURVE_TYPES[p22[0] - p11[0]][p22[1] - p11[1]][p22[2] - p11[2]];
        int index = getIndex(p11[0], p11[1], p11[2]);

        //printf("Removing Curve: {%i %i %i} - {%i %i %i} %i\n",
        //	p11[0], p11[1], p11[2],
        //	p22[0], p22[1], p22[2], curveType);

        curves[index] = curves[index] & ~curveType;
    }

    void DiscreteMesh::removeSurface(Vector3Int p1, Vector3Int p2,
                                     Vector3Int p3, Vector3Int p4)
    {
        Vector3Int p11 = p1, p22 = p2, p33 = p3, p44 = p4;
        findSurfaceBase(p11, p22, p33, p44);

        unsigned char surfaceType = SURFACE_TYPES[p33[0] - p11[0]][p33[1] - p11[1]][p33[2] - p11[2]];
        removeSurface(p11[0], p11[1], p11[2], surfaceType);

        //printf("Removing Surface: {%i %i %i} - {%i %i %i} - {%i %i %i} - {%i %i %i} - %i\n",
        //	p11[0], p11[1], p11[2],
        //	p22[0], p22[1], p22[2],
        //	p33[0], p33[1], p33[2],
        //	p44[0], p44[1], p44[2],
        //	surfaceType);

    }


    void DiscreteMesh::removeSurface(int x, int y, int z, unsigned char surfaceType){
        int index = getIndex(x, y, z);
        surfaces[index] = surfaces[index] & ~surfaceType;
    }


    void DiscreteMesh::findCurveBase(Vector3Double &p1, Vector3Double &p2) {
        Vector3Double temp;
        if ((p1[0] > p2[0]) ||
            (p1[1] > p2[1]) ||
            (p1[2] > p2[2])) {
            temp = p1;
            p1 = p2;
            p2 = temp;
        }
    }

    void DiscreteMesh::findCurveBase(Vector3Int &p1, Vector3Int &p2) {
        Vector3Int temp;
        if ((p1[0] > p2[0]) ||
            (p1[1] > p2[1]) ||
            (p1[2] > p2[2])) {
            temp = p1;
            p1 = p2;
            p2 = temp;
        }
    }

    void DiscreteMesh::findSurfaceBase(Vector3Double &p1, Vector3Double &p2,
                                       Vector3Double &p3, Vector3Double &p4)
    {
        Vector3Double points[4] = {p1, p2, p3, p4};
        Vector3Double temp;
        int jVal, minVal, minIndex;

        for(int i = 0; i < 3; i++) {
            minVal = points[i][0] + points[i][1] + points[i][2];
            minIndex = i;
            for(int j = i+1; j < 4; j++) {
                jVal = points[j][0] + points[j][1] + points[j][2];
                if(jVal < minVal) {
                    minVal = jVal;
                    minIndex = j;
                }
            }
            temp = points[i];
            points[i] = points[minIndex];
            points[minIndex] = temp;
        }


        p1 = points[0];
        p2 = points[1];
        p3 = points[3];
        p4 = points[2];
    }

    void DiscreteMesh::findSurfaceBase(Vector3Int &p1, Vector3Int &p2,
                                       Vector3Int &p3, Vector3Int &p4)
    {
        Vector3Int points[4] = {p1, p2, p3, p4};
        Vector3Int temp;
        int jVal, minVal, minIndex;

        for(int i = 0; i < 3; i++) {
            minVal = points[i][0] + points[i][1] + points[i][2];
            minIndex = i;
            for(int j = i+1; j < 4; j++) {
                jVal = points[j][0] + points[j][1] + points[j][2];
                if(jVal < minVal) {
                    minVal = jVal;
                    minIndex = j;
                }
            }
            temp = points[i];
            points[i] = points[minIndex];
            points[minIndex] = temp;
        }


        p1 = points[0];
        p2 = points[1];
        p3 = points[3];
        p4 = points[2];
    }

    int DiscreteMesh::getC6(Vector3Int * neighbors, int neighborCount,
                            Vector3Int currPoint)
    {
        Volume * vol = new Volume(5, 5, 5);
        for(int i = 0; i < neighborCount; i++) {
            vol->setDataAt(neighbors[i][0] - currPoint[0] + 2,
                    neighbors[i][1] - currPoint[1] + 2,
                    neighbors[i][2] - currPoint[2] + 2, 1);
        }

        int c6Count = 0;
        vector<Vector3Int> n6;
        int n6Count;
        Vector3Int * queue = new Vector3Int[26];
        Vector3Int temp;
        int queueSize = 0;

        for(int x = 2; x < 4; x++) {
            for(int y = 2; y < 4; y++) {
                for(int z = 2; z < 4; z++) {
                    if(vol->getDataAt(x, y, z) > 0) {
                        c6Count++;
                        queue[0] = Vector3Int(x, y, z);
                        queueSize = 1;
                        while (queueSize > 0) {
                            temp = queue[queueSize-1];
                            queueSize--;
                            vol->setDataAt(temp[0], temp[1], temp[2], 0);
                            n6Count = getN6(n6, vol, temp[0], temp[1], temp[2]);
                            for(int i = 0; i < n6Count; i++) {
                                queue[queueSize] = n6[i];
                                queueSize++;
                            }
                        }
                    }
                }
            }
        }

        delete [] queue;
        delete vol;
        return c6Count;
    }

    int DiscreteMesh::getC26(Vector3Int * neighbors, int neighborCount,
                             Vector3Int currPoint)
    {
        Volume * vol = new Volume(5, 5, 5);
        for(int i = 0; i < neighborCount; i++) {
            vol->setDataAt(neighbors[i][0] - currPoint[0] + 2,
                    neighbors[i][1] - currPoint[1] + 2,
                    neighbors[i][2] - currPoint[2] + 2, 1);
        }

        int c26Count = 0;
        Vector3Int * n26;
        int n26Count;
        Vector3Int * queue = new Vector3Int[26];
        Vector3Int temp;
        int queueSize = 0;

        for(int x = 2; x < 4; x++) {
            for(int y = 2; y < 4; y++) {
                for(int z = 2; z < 4; z++) {
                    if(vol->getDataAt(x, y, z) > 0) {
                        c26Count++;
                        queue[0] = Vector3Int(x, y, z);
                        queueSize = 1;
                        while (queueSize > 0) {
                            temp = queue[queueSize-1];
                            queueSize--;
                            vol->setDataAt(temp[0], temp[1], temp[2], 0);
                            n26Count = getN26(n26, vol, temp[0], temp[1], temp[2]);
                            for(int i = 0; i < n26Count; i++) {
                                queue[queueSize] = n26[i];
                                queueSize++;
                            }
                            delete [] n26;
                        }
                    }
                }
            }
        }

        delete [] queue;
        delete vol;
        return c26Count;
    }


    int DiscreteMesh::getN6(vector<Vector3Int> & n6, Volume * sourceVolume, int x, int y, int z){
        int n6Count = 0;
        n6.resize(6);
        for(int i = 0; i < 6; i++) {
            n6[n6Count][0] = x + VOLUME_NEIGHBORS_6[i][0];
            n6[n6Count][1] = y + VOLUME_NEIGHBORS_6[i][1];
            n6[n6Count][2] = z + VOLUME_NEIGHBORS_6[i][2];
            if(sourceVolume->getDataAt(n6[n6Count][0], n6[n6Count][1], n6[n6Count][2]) > 0) {
                n6Count++;
            }
        }
        return n6Count;
    }

    int DiscreteMesh::getN6_2(vector<Vector3Int> & n6_2, Volume * sourceVolume, int x,
                              int y, int z)
    {
        n6_2.resize(18);
        vector<Vector3Int> n18;
        vector<Vector3Int> n6X, n6Y;
        vector<Vector3Int> n62List(36);
        int n62Count = 0;

        int n18Count = getN18(n18, sourceVolume, x, y, z);
        int n6CountX = getN6(n6X, sourceVolume, x, y, z);
        int n6CountY;

        for(int i = 0; i < n6CountX; i++) {
            n6CountY = getN6(n6Y, sourceVolume, n6X[i][0], n6X[i][1], n6X[i][2]);
            for(int j = 0; j < n6CountY; j++) {
                n62List[n62Count] = n6Y[j];
                n62Count++;
            }
        }

        int retVal = 0;
        bool found;
        for(int i = 0; i < n18Count; i++) {
            found = false;
            for(int j = 0; j < n62Count; j++) {
                found = found || ((n18[i][0] == n62List[j][0]) && (n18[i][1] == n62List[j][1]) && (n18[i][2] == n62List[j][2]));
            }
            if(found) {
                n6_2[retVal] = n18[i];
                retVal++;
            }
        }
        return retVal;
    }

    int DiscreteMesh::getN18(vector<Vector3Int> & n18, Volume * sourceVolume, int x,
                             int y, int z)
    {
        n18.resize(18);
        int n18Count = 0;
        for(int i = 0; i < 18; i++) {
            n18[n18Count][0] = x + VOLUME_NEIGHBORS_18[i][0];
            n18[n18Count][1] = y + VOLUME_NEIGHBORS_18[i][1];
            n18[n18Count][2] = z + VOLUME_NEIGHBORS_18[i][2];
            if(sourceVolume->getDataAt(n18[n18Count][0], n18[n18Count][1], n18[n18Count][2]) > 0) {
                n18Count++;
            }
        }
        return n18Count;
    }

    int DiscreteMesh::getN26(Vector3Int * & n26, Volume * sourceVolume, int x,
                             int y, int z)
    {
        int n26Count = 0;
        n26 = new Vector3Int[26];
        for(int i = 0; i < 26; i++) {
            n26[n26Count][0] = x + VOLUME_NEIGHBORS_26[i][0];
            n26[n26Count][1] = y + VOLUME_NEIGHBORS_26[i][1];
            n26[n26Count][2] = z + VOLUME_NEIGHBORS_26[i][2];
            if(sourceVolume->getDataAt(n26[n26Count][0], n26[n26Count][1], n26[n26Count][2]) > 0) {
                n26Count++;
            }
        }
        return n26Count;
    }

    int DiscreteMesh::getN6Count(const Volume & sourceVolume, int x, int y, int z) {
        int n6Count = 0;
        for(int i = 0; i < 6; i++) {
            if(sourceVolume.getDataAt(x + VOLUME_NEIGHBORS_6[i][0], y + VOLUME_NEIGHBORS_6[i][1], z + VOLUME_NEIGHBORS_6[i][2]) > 0)  {
                n6Count++;
            }
        }
        return n6Count;
    }

    int DiscreteMesh::getN6_2Count(Volume * sourceVolume, int x, int y, int z) {
        vector<Vector3Int> n6_2;
        int count = getN6_2(n6_2, sourceVolume, x, y, z);

        return count;
    }

    int DiscreteMesh::getN18Count(Volume * sourceVolume, int x, int y, int z) {
        int n18Count = 0;
        for(int i = 0; i < 18; i++) {
            if(sourceVolume->getDataAt(x + VOLUME_NEIGHBORS_18[i][0],
                       y + VOLUME_NEIGHBORS_18[i][1],
                       z + VOLUME_NEIGHBORS_18[i][2])
               > 0) {
                n18Count++;
            }
        }
        return n18Count;
    }

    int DiscreteMesh::getN26Count(Volume * sourceVolume, int x, int y, int z) {
        int n26Count = 0;
        for(int i = 0; i < 26; i++) {
            if(sourceVolume->getDataAt(x + VOLUME_NEIGHBORS_26[i][0],
                       y + VOLUME_NEIGHBORS_26[i][1],
                       z + VOLUME_NEIGHBORS_26[i][2])
               > 0) {
                n26Count++;
            }
        }
        return n26Count;
    }

    int DiscreteMesh::getMCount(Volume * sourceVolume, int x1, int y1, int z1,
                                int x2, int y2, int z2)
    {
        vector<Vector3Int> n18X;
        int n18XCount = getN18(n18X, sourceVolume, x1, y1, z1);
        vector<Vector3Int> n6Y;
        int n6YCount = getN6(n6Y, sourceVolume, x2, y2, z2);
        int mCount = 0;
        for(int i = 0; i < n18XCount; i++) {
            for(int j = 0; j < n6YCount; j++) {
                if( (n18X[i][0] == n6Y[j][0]) && (n18X[i][1] == n6Y[j][1])
                   && (n18X[i][2] == n6Y[j][2]))
                {
                    mCount++;
                }
            }
        }
        return mCount;
    }

    int DiscreteMesh::getImmersionN6Count(Volume * skeleton, Vector3Int point) {
        Volume * range = skeleton->getDataRange(point[0], point[1], point[2], 1);
        range->threshold(range->getDataAt(1,1,1), 0, 1, 0, false);
        int n6Count = getN6Count(*range,1,1,1);
        delete range;
        return n6Count;
    }

    int DiscreteMesh::getImmersionSkeletalValue(Volume * skeleton, Vector3Int point) {
        Volume * range = skeleton->getDataRange(point[0], point[1], point[2], 2);
        Volume * thresholdedRange = new Volume(*range);
        double value = 0;
        thresholdedRange->threshold(range->getDataAt(2, 2, 2), -1, 1, -1, false);
        /*if(IsSurfaceBorder(thresholdedRange, 2, 2, 2) || IsCurveEnd(thresholdedRange, 2, 2, 2) || IsPoint(thresholdedRange, 2, 2, 2)) {
            delete thresholdedRange;
            value = range->getDataAt(2, 2, 2);
        } else {*/
            delete thresholdedRange;
            list<double> thresholds;
            thresholds.push_back(range->getDataAt(2, 2, 2));

        for(int i = 0; i < 26; i++) {
            value = range->getDataAt(2 + VOLUME_NEIGHBORS_26[i][0],
                    2 + VOLUME_NEIGHBORS_26[i][1],
                    2 + VOLUME_NEIGHBORS_26[i][2]);
            if(value <= range->getDataAt(2, 2, 2))
                thresholds.push_back(value);
        }
            thresholds.sort(greater<double>());
            for(unsigned int i = 0; i < thresholds.size(); i++) {
                value = thresholds.front();
                thresholds.pop_front();
                thresholdedRange = new Volume(*range);
                thresholdedRange->threshold(value, -1, 1, -1, false);
                if(!isSimple(thresholdedRange, 2, 2, 2)) {
                    break;
                }
                delete thresholdedRange;
            }

            delete range;
        //}
        return (int)round(value);
    }

    bool DiscreteMesh::isPoint(Volume * sourceVolume, int x, int y, int z) {
        return (getN6Count(*sourceVolume, x, y, z) == 0);
    }

    bool DiscreteMesh::isCurveEnd(Volume * sourceVolume, int x, int y, int z) {
        return (getN6Count(*sourceVolume, x, y, z) == 1);
    }

    bool DiscreteMesh::isCurveBody(Volume * sourceVolume, int x, int y, int z) {
        bool foundFace = false;
        bool allPoints;
        for(int n = 0; n < 12; n++) {
            allPoints = true;
            for(int p = 0; p < 3; p++) {
                allPoints = allPoints
                        && (sourceVolume->getDataAt(
                                    x + VOLUME_NEIGHBOR_FACES[n][p][0],
                                    y + VOLUME_NEIGHBOR_FACES[n][p][1],
                                    z + VOLUME_NEIGHBOR_FACES[n][p][2])
                            > 0);
            }
            foundFace = foundFace || allPoints;
        }

        return (!foundFace && getN6Count(*sourceVolume, x, y, z) >= 2);

    }

    bool DiscreteMesh::isSurfaceBorder(Volume * sourceVolume, int x, int y, int z) {
        vector<Vector3Int> n6_2, n6;
        Vector3Int currPoint = Vector3Int(x, y, z);
        int n6_2Count = getN6_2(n6_2, sourceVolume, x, y, z);
        int n6Count = getN6(n6, sourceVolume, x, y, z);
        int mZigma = 0;
        for(int i = 0; i < n6Count; i++) {
            mZigma += getMCount(sourceVolume, x, y, z, n6[i][0], n6[i][1], n6[i][2]);
        }

        return (mZigma - n6_2Count - n6Count) == 0;
    }

    bool DiscreteMesh::isSurfaceBody(Volume * sourceVolume, int x, int y, int z, bool doDependantChecks) {
        if(sourceVolume->getDataAt(x, y, z) <= 0)
            return false;

        double points[3][3][3];
        for(int xx = 0; xx < 3; xx++){
            for(int yy = 0; yy < 3; yy++){
                for(int zz = 0; zz < 3; zz++) {
                    points[xx][yy][zz] = sourceVolume->getDataAt(x + xx - 1, y + yy - 1, z + zz - 1);
                }
            }
        }

        bool cubeFound;
        for(int i = 0; i < 8; i++) {
            cubeFound = true;
            for(int e = 0; e < 7; e++) {
                cubeFound = cubeFound
                        && (points[VOLUME_NEIGHBOR_CUBES[i][e][0] + 1][VOLUME_NEIGHBOR_CUBES[i][e][1] + 1][VOLUME_NEIGHBOR_CUBES[i][e][2] + 1] > 0);
            }
            if(cubeFound) {
                for(int e = 0; e < 7; e++) {
                    points[VOLUME_NEIGHBOR_CUBES[i][e][0] + 1][VOLUME_NEIGHBOR_CUBES[i][e][1] + 1][VOLUME_NEIGHBOR_CUBES[i][e][2] + 1] = 0;
                }
            }
        }

        bool surfaceFound = true;
        for(int i = 0; i < 12; i++) {
            surfaceFound = true;
            for(int e = 0; e < 3; e++) {
                surfaceFound = surfaceFound
                        && (points[VOLUME_NEIGHBOR_FACES[i][e][0] + 1][VOLUME_NEIGHBOR_FACES[i][e][1] + 1][VOLUME_NEIGHBOR_FACES[i][e][2] + 1] > 0);
            }
            if(surfaceFound) {
                break;
            }
        }

        return surfaceFound && ((doDependantChecks && !isSurfaceBorder(sourceVolume, x, y, z)) || (!doDependantChecks));
    }

    bool DiscreteMesh::isVolumeBorder(Volume * sourceVolume, int x, int y,
                                      int z, bool doDependantChecks)
    {
        if(doDependantChecks && isVolumeBody(sourceVolume, x, y, z))
            return false;

        bool isBorder = false;
        bool allInCube;
        for(int cube = 0; cube < 8; cube++) {
            allInCube = true;
            for(int p = 0; p < 7; p++) {
                allInCube = allInCube
                        && (sourceVolume->getDataAt(
                                    x + VOLUME_NEIGHBOR_CUBES[cube][p][0],
                                    y + VOLUME_NEIGHBOR_CUBES[cube][p][1],
                                    z + VOLUME_NEIGHBOR_CUBES[cube][p][2])
                            > 0);
            }
            isBorder = isBorder || allInCube;
        }
        return isBorder;
    }

    bool DiscreteMesh::isVolumeBody(Volume * sourceVolume, int x, int y, int z) {
        return (getN26Count(sourceVolume, x, y, z) == 26);
    }

    bool DiscreteMesh::isSimple(Volume * sourceVolume, int x, int y, int z) {
        return sourceVolume->isSimple(x, y, z) != 0;
    }

    bool DiscreteMesh::isValidSurface(const Volume & sourceVolume, Vector3Double p0,
                                      Vector3Double p1, Vector3Double p2,
                                      Vector3Double p3)
    {
        Vector3Double surface[4] = {p0, p1, p2, p3};
        Vector3Double pDelta = p2 - p0;
        Vector3Double upperVector, lowerVector;
        if((int)round(pDelta[0]) == 0) {
            upperVector = Vector3Double(1, 0, 0);
            lowerVector = Vector3Double(-1, 0, 0);
        } else if ((int)round(pDelta[1]) == 0) {
            upperVector = Vector3Double(0, 1, 0);
            lowerVector = Vector3Double(0, -1, 0);
        } else {
            upperVector = Vector3Double(0, 0, 1);
            lowerVector = Vector3Double(0, 0, -1);
        }

        bool allFound = true;
        Vector3Double currentPos;
        for(int i = 0; i < 4; i++) {
            currentPos = surface[i] + upperVector;
            allFound = allFound
                    && (sourceVolume.getDataAt(currentPos.XInt(), currentPos.YInt(), currentPos.ZInt()) > 0);
            currentPos = surface[i] + lowerVector;
            allFound = allFound
                    && (sourceVolume.getDataAt(currentPos.XInt(), currentPos.YInt(), currentPos.ZInt()) > 0);
        }
        return !allFound;
    }
}

#endif
