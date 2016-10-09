#ifndef TOOLKIT_GRAPHMATCH_PATHGENERATOR_H
#define TOOLKIT_GRAPHMATCH_PATHGENERATOR_H

#include "Graph.h"
//#include "Node.h"
//#include "NodeStub.h"
//#include <Core/volume.h>
//#include "VectorMath.h"
#include <vector>
#include "GraphNode.h"

using namespace std;
//using namespace SkeletonMaker;

namespace GraphMatch {
    class PathGenerator {
    public:
        PathGenerator(Graph * graph);
        void GenerateGraph(GraphNode * node, char * outFileName);
        void GenerateGraph(NodeStub * node, char * outFileName);
        bool MarkPath(int startHelix, int startCorner, int endHelix, int endCorner, const Volume & skeletonVol, Volume & newVol);

    private:
        Graph * graph;
    };

    inline PathGenerator::PathGenerator(Graph * graph) {
        this->graph = graph;
    }

    inline void PathGenerator::GenerateGraph(GraphNode * node, char * outFileName)  {
        Volume * skeletonVol = graph->skeletonVolume;
        Volume newVol(*skeletonVol);
        int startNode = 0;
        int endNode = 1;
        while (endNode < node->n2Top) {
            while((startNode < node->n2Top) && (node->n2[startNode] >= (int)graph->skeletonHelixes.size() * 2)) {
                startNode++;
            }
            while((endNode < node->n2Top) && (node->n2[endNode] >= (int)graph->skeletonHelixes.size() * 2)) {
                endNode++;
            }

            if((startNode < node->n2Top) && (endNode < node->n2Top)) {
                int startHelix = node->n2[startNode]/2;
                int startCorner = node->n2[startNode]%2 + 1;
                int endHelix = node->n2[endNode]/2;
                int endCorner = node->n2[endNode]%2 + 1;
                bool marked = MarkPath(startHelix, startCorner, endHelix, endCorner, *skeletonVol, newVol);

                if(!marked) {
                    printf("Path between %d and %d was not marked \n", startHelix + 1, endHelix + 1);
                }


            }
            startNode = endNode;
            endNode++;
        }

        newVol.toMRCFile(outFileName);
    }

    inline void PathGenerator::GenerateGraph(NodeStub * node, char * outFileName)  {
        Volume * skeletonVol = graph->skeletonVolume;
        Volume * newVol = new Volume(*skeletonVol);

        int startHelix = -1, startCorner = -1, endHelix = -1, endCorner = -1;
        bool marked;

        for(NodeStub * currentNode = node;
            currentNode->parentNode != NULL;
            currentNode = currentNode->parentNode
           )
        {
            endHelix = (currentNode->n2Node-1)/2;
            endCorner = ((currentNode->n2Node-1)%2) + 1;

            if((startHelix != -1) && (endHelix != -1)) {
                marked = MarkPath(startHelix, startCorner, endHelix, endCorner, *skeletonVol, *newVol);

    #ifdef VERBOSE
                if(!marked) {
                    printf("\t Path between %d and %d was not marked \n", startHelix + 1, endHelix + 1);
                }
    #endif
            }
            startHelix = endHelix;
            startCorner = endCorner;
        }

        newVol->toMRCFile(outFileName);
        delete newVol;
    }


    inline bool PathGenerator::MarkPath(int startHelix, int startCorner, int endHelix,
                                 int endCorner, const Volume & skeletonVol,
                                 Volume & newVol)
    {
        vector<Point3Pair *> oldStack;
        vector<Point3Pair *> newStack;

        Point3Pair tempCell = Point3Pair(1,1,1,0);

        for(unsigned int i = 0; i < graph->skeletonHelixes[startHelix]->cornerCells.size(); i++) {
            tempCell = graph->skeletonHelixes[startHelix]->cornerCells[i];
            if(tempCell.node == startCorner) {
                oldStack.push_back(new Point3Pair(tempCell.x, tempCell.y, tempCell.z, 1));
            }
        }

        Point3Pair * currentPoint; //CurrentPoint
        int x, y, z, xx = 0, yy = 0, zz = 0;
        int d[6][3];
        d[0][0] = 0;		d[0][1] = 0;		d[0][2] = -1;
        d[1][0] = 0;		d[1][1] = 0;		d[1][2] = 1;
        d[2][0] = 0;		d[2][1] = -1;		d[2][2] = 0;
        d[3][0] = 0;		d[3][1] = 1;		d[3][2] = 0;
        d[4][0] = -1;		d[4][1] = 0;		d[4][2] = 0;
        d[5][0] = 1;		d[5][1] = 0;		d[5][2] = 0;
        Volume visited(skeletonVol);

        for(int i = 0; i < (int)graph->skeletonHelixes.size(); i++) {
            if(((i != endHelix) && (i != startHelix))) {
            //if(i != endHelix) {
                for(unsigned int j = 0; j < graph->skeletonHelixes[i]->internalCells.size(); j++) {
                    Point3Pair cell = graph->skeletonHelixes[i]->internalCells[j];
                    visited(cell.x, cell.y, cell.z) = 100000;
                }
            }


            for(unsigned int j = 0; j < graph->skeletonHelixes[i]->cornerCells.size(); j++) {
                Point3Pair cell = graph->skeletonHelixes[i]->cornerCells[j];
                if(((i==startHelix) && (cell.node == startCorner))  || ((i==endHelix) && (cell.node == endCorner))) {
                    visited(cell.x, cell.y, cell.z) = 0;
                }
            }
        }



        bool found = false;
        int foundDistance = -1;

        while((!found) && (oldStack.size() > 0)) {
            newStack.clear();
            for(unsigned int i = 0; (i < oldStack.size() && !found); i++) {
                currentPoint = oldStack[i];
                xx = currentPoint->x;
                yy = currentPoint->y;
                zz = currentPoint->z;

                for(unsigned int i = 0; i < graph->skeletonHelixes[endHelix]->cornerCells.size(); i++) {
                    tempCell = graph->skeletonHelixes[endHelix]->cornerCells[i];
                    if(tempCell.node == endCorner) {
                        found = found || ((xx == tempCell.x) && (yy == tempCell.y) && (zz == tempCell.z));
                        if(found) {
                            foundDistance = currentPoint->distance;
                        }
                    }
                }

                if(!found) {
                    for(int j = 0; j < 6; j++) {
                        x = currentPoint->x+d[j][0];
                        y = currentPoint->y+d[j][1];
                        z = currentPoint->z+d[j][2];
                        if((x >= 0) && (x < skeletonVol.getSizeX()) && (y >=0) && (y < skeletonVol.getSizeY()) && (z >= 0) && (z < skeletonVol.getSizeZ())) {
                            if((visited(x, y, z) <= 0.001) && (skeletonVol(x, y, z) > 0.001)) {
                                Point3Pair * newPoint = new Point3Pair(x, y, z, currentPoint->distance + 1);


                                newStack.push_back(newPoint);
                                visited(x, y, z) = currentPoint->distance;
                            }
                        }
                    }
                }
                delete currentPoint;
            }
            oldStack.clear();
            oldStack = newStack;
            newStack.clear();
        }

        if(!found || (foundDistance == -1)) {
            return false;
        }


        newVol(xx, yy, zz) = 1;
        for(int i = foundDistance - 1; i >= 1; i--) {
            for(int j = 0; j < 6; j++) {
                x = xx+d[j][0];
                y = yy+d[j][1];
                z = zz+d[j][2];
                if((x >= 0) && (x < skeletonVol.getSizeX()) && (y >=0) && (y < skeletonVol.getSizeY()) && (z >= 0) && (z < skeletonVol.getSizeZ())) {
                    if(int(visited(x, y, z) - (double)i + 0.5) == 0) {
                        xx = x;
                        yy = y;
                        zz = z;
                        newVol(xx, yy, zz) = 1;
                        break;
                    }
                }
            }
        }
        return true;
    }
}


#endif
