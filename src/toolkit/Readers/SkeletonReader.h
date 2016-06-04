#ifndef SRC_TOOLKIT_READERS_SKELETONREADER_H
#define SRC_TOOLKIT_READERS_SKELETONREADER_H

#include "Core/volume.h"
#include "GraphMatch/Shape.h"
#include "GraphMatch/Graph.h"

namespace GraphMatch {

    int getGraphIndex(vector<Shape*> & helixes, int helixNum, int cornerNum);
    int getGraphIndex(vector<Shape*> & helixes, int helixNum, Point3Pair * point);
    Graph * readFile(string volumeFile, string helixFile, string sseFile, string sheetFile);
    Volume* getSheetsNoThreshold(Volume * vol, int minSize);
    void readSheetFile(string sheetFile, vector<Shape*> & helixes);
    void readHelixFile(string helixFile, string sseFile, vector<Shape*> & helixes);
    void findSizes(int startHelix, int startCell, vector<Shape*> & helixList,
                   Volume * vol, Volume * coloredVol, Graph * graph);
    void findPaths(Graph * graph);
    void findPath(int startIx, int endIx, vector<vector<Vec3I> > nodes,
                  Volume * maskVol, Graph * graph, bool eraseMask);
    void findCornerCellsInSheet(Volume * vol, Volume * paintedVol,
                                vector<Shape*> & helixes, int sheetId);
    int isSkeletonSheet(const Volume &vol, int ox, int oy, int oz);
}

#endif
