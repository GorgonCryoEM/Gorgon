#ifndef TOOLKIT_GRAPHMATCH_GRAPH_H
#define TOOLKIT_GRAPHMATCH_GRAPH_H

#include <cassert>
#include <vector>
//#include "Core/GlobalConstants.h"
#include "Matcher2Helix.h"
#include "ProteinMorph/SecStruct.h"
#include "Shape.h"
#include <Core/volume.h>
//#include <MathTools/Vector3.h>
//#include <Core/volume.h>
//using namespace std;
using namespace Core;

namespace GraphMatch {

    class Graph {
    public:
        int nodeCount;

    public:
        // Constructors
        Graph(int nCount=0);
        Graph(char* fname);
        ~Graph();

        int getType(int i, int j); // The type of the edge (index starting from 1)
        double getCost(int i, int j); // The cost based on the graph labels (index starting from 1)
        int getNodeCount(); // Returns the number of nodes
        int getHelixCount(); // Returns the number of helices
        int getSheetCount(); // Returns the number of sheets
        void setType(int i, int j, int type); // The type of the edge (index starting from 1)
        void setCost(int i, int j, double cost); // The cost based on the graph labels (index starting from 1)
        void setCost(int i, double cost); // The cost of a node
        void setNodeCount(int nCount); // Sets the number of nodes
        void print();
        void generateEuclidianMatrix(Volume * vol);
        vector<Matcher2Helix> getHelixLengths();
        bool edgeExists(int n, int m);
        void mergeSheets(double maxDist); // Merge all sheets separated by maxDist or less

    public:
        double adjacencyMatrix[MAX_NODES][MAX_NODES][2]; // 0th dimension edge type... 1st dimension distance
        double nodeWeights[MAX_NODES];
        double euclideanMatrix[MAX_NODES][MAX_NODES];
        vector<Vec3I> paths[MAX_NODES][MAX_NODES];
        vector<SecStruct*> pdbStructures; // indexed by structure number along the sequence.
        Volume * skeletonVolume;
        Volume * skeletonSheetVolume;
        vector<Shape*> skeletonHelixes; // helices first, then sheets.
    };

    inline Graph::~Graph() {
        for(int i = 0; i < (int)pdbStructures.size(); i++) {
            delete pdbStructures[i];
        }

        for(int i = 0; i < (int)skeletonHelixes.size(); i++) {
            delete skeletonHelixes[i];
        }

        if(skeletonVolume != NULL) {
            delete skeletonVolume;
        }

        if(skeletonSheetVolume != NULL) {
            delete skeletonSheetVolume;
        }
    }

    inline Graph::Graph(int nCount){
        skeletonVolume = NULL;
        skeletonSheetVolume = NULL;
        nodeCount = nCount;
        for(int i = 0; i < nodeCount; i++) {
            for(int j = 0; j < nodeCount; j++) {
                adjacencyMatrix[i][j][0] = 3;
                adjacencyMatrix[i][j][1] = MAXINTT;
                euclideanMatrix[i][j] = 0.0;
            }
            nodeWeights[i] = 0.0;
        }
        pdbStructures.clear();
    }

    inline Graph::Graph(char* fname) {
        skeletonVolume = NULL;
        skeletonSheetVolume = NULL;
        FILE* fin = fopen(fname, "rt");
        if (fin == NULL)
        {
            printf("Error reading input file %s.\n", fname) ;
            exit(0) ;
        }

        fscanf(fin, "%d\n", &nodeCount);
        for(int i = 0; i < nodeCount; i++) {
            for(int j = 0; j < nodeCount; j++) {
                fscanf(fin, "%lf", &(adjacencyMatrix[i][j][1]));
            }
            fscanf(fin, "\n");
            nodeWeights[i] = 0.0;
        }
        fclose( fin ) ;
        pdbStructures.clear();
    }

    inline bool Graph::edgeExists(int n, int m) {
        return fabs(adjacencyMatrix[n][m][1] - MAXINTT) > 0.01;
    }

    inline int Graph::getType(int i, int j) {
        return (int)adjacencyMatrix[i-1][j-1][0];
    }

    inline double Graph::getCost(int i, int j) {
        if(adjacencyMatrix[i-1][j-1][1] == MAXINTT) {
            return 1000;
        } else {
            return adjacencyMatrix[i-1][j-1][1];
        }
    }

    inline int Graph::getNodeCount() {
        return nodeCount;
    }

    inline int Graph::getHelixCount() {
        int count = 0;
        for (int i = 0; i < nodeCount; i++) {
            if (adjacencyMatrix[i][i][0] == GRAPHNODE_HELIX) {
                count++;
            }
        }
        return count/2;
    }

    inline int Graph::getSheetCount() {
        int count = 0;
        for (int i = 0; i < nodeCount; i++) {
            if (adjacencyMatrix[i][i][0] == GRAPHNODE_SHEET) {
                count++;
            }
        }
        return count;
    }

    inline void Graph::setType(int i, int j, int type) {
        assert(((i >= 1) && (i <= nodeCount) && (j >= 1) && (j <= nodeCount)));
        adjacencyMatrix[i-1][j-1][0] = type;
    }

    inline void Graph::setCost(int i, int j, double cost) {
        assert(((i >= 1) && (i <= nodeCount) && (j >= 1) && (j <= nodeCount)));
        adjacencyMatrix[i-1][j-1][1] = cost;
    }

    inline void Graph::setCost(int i, double cost) {
        assert((i >= 1) && (i <= nodeCount));
        nodeWeights[i-1] = cost;
    }

    inline void Graph::setNodeCount(int nCount) {
        nodeCount = nCount;
    }

    inline void Graph::print() {
        char temp;
        int seqNode = 1;
        for(int i = 0; i < (int)pdbStructures.size(); i++) {
            if(pdbStructures[i]->sseType == GRAPHEDGE_HELIX) {
                printf("\tHelix %d \t\t(%2d,%2d)\t Length: %d \t Start Pos: %d \t End Pos: %d\n", pdbStructures[i]->serialNumber, seqNode, seqNode+1, pdbStructures[i]->GetLengthResidues(), pdbStructures[i]->startPosition, pdbStructures[i]->endPosition);
                seqNode += 2;
            } else {
                printf("\tSheet Strand %s-%d \t(%2d)   \t Length: %d \t Start Pos: %d \t End Pos: %d\n", pdbStructures[i]->ID.c_str(), pdbStructures[i]->serialNumber, seqNode, pdbStructures[i]->GetLengthResidues(), pdbStructures[i]->startPosition, pdbStructures[i]->endPosition);
                seqNode += 1;
            }
        }

        int skelNode = 1;
        for(int i = 0; i < (int)skeletonHelixes.size(); i++) {
            if(skeletonHelixes[i]->type == GRAPHEDGE_HELIX) {
                printf("\tHelix #%d \t(%2d,%2d)\t Length: %f\n", i+1, skelNode, skelNode+1, skeletonHelixes[i]->length);
                skelNode += 2;
            } else if(skeletonHelixes[i]->type == GRAPHEDGE_SHEET) {
                printf("\tSheet #%d \t(%2d)   \t Length: %f\n", i+1, skelNode, skeletonHelixes[i]->length);
                skelNode += 1;
            }
        }

        printf("\t");
        for(int i = 0; i < nodeCount; i++) {
            printf("      %d    \t|", i+1);
        }
        printf("\n");
        double used = 0.0;
        double euclideanUsed = 0.0;


        for(int i = 0; i < nodeCount; i++) {
            printf("   %d\t", i+1);
            for(int j = 0; j < nodeCount; j++) {
                if(adjacencyMatrix[i][j][0] == GRAPHEDGE_HELIX) {
                    temp = 'H';
                    used++;
                } else if (adjacencyMatrix[i][j][0] == GRAPHEDGE_LOOP) {
                    temp = 'L';
                    used++;
                } else if (adjacencyMatrix[i][j][0] == GRAPHEDGE_SHEET) {
                    temp = 'S';
                    used++;
                } else if (adjacencyMatrix[i][j][0] == GRAPHEDGE_LOOP_EUCLIDEAN) {
                    temp = 'E';
                    euclideanUsed++;

                } else {
                    temp = ' ';
                }

                if(adjacencyMatrix[i][j][1] == MAXINTT) {
                    printf(" %c         \t|", temp);
                } else {
                    //printf(" %c %d\t|", temp, (int)(adjacencyMatrix[i][j][1] + 0.5));
                    printf(" %c %f\t|", temp, adjacencyMatrix[i][j][1]);
                }
            }
            printf("\n");
        }

#ifdef VERBOSE
        // print out the euclidean matrix
        printf("\n  Euclidean distances:\n");
        for(int i = 0; i < nodeCount; i++) {
            printf("   %d\t", i+1);
            for(int j = 0; j < nodeCount; j++) {
                temp = 'E';
                if(euclideanMatrix[i][j] == MAXINTT) {
                    printf(" %c         \t|", temp);
                } else {
                    printf(" %c %f\t|", temp, euclideanMatrix[i][j]);
                }
            }
            printf("\n");
        }
#endif

#ifdef VERBOSE
        // print out the node weights
        bool hasWeightedNodes = false;
        for(int i = 0; i < nodeCount; i++) {
            hasWeightedNodes = hasWeightedNodes || (nodeWeights[i] > 0);
        }
        if(hasWeightedNodes) {
            printf("\n  Node weights:\n");
            for(int i = 0; i < nodeCount; i++) {
                printf("   %d\t", i+1);
                if(adjacencyMatrix[i][i][0] == GRAPHNODE_SHEET) {
                    temp = 'S';
                    used++;
                } else {
                    temp = ' ';
                }
                if(nodeWeights[i] == MAXINTT) {
                    printf(" %c         \t|", temp);
                } else {
                    printf(" %c %f\t|", temp, nodeWeights[i]);
                }
                printf("\n");
            }
        }
#endif
        printf("Graph Density %f%%,   (With Euclidean edges %f%%)\n", used * 100 / (nodeCount * nodeCount), (used + euclideanUsed) * 100 / (nodeCount * nodeCount));
    }

    inline vector<Matcher2Helix> Graph::getHelixLengths() {
        vector<Matcher2Helix> helixes;

        for(int i = 0 ; i < (int)pdbStructures.size(); i++) {
            helixes.push_back(Matcher2Helix(pdbStructures[i]->GetLengthAngstroms(), i*2, i*2+1));
        }
        for(int i = 0; i < (int)skeletonHelixes.size(); i++) {
            helixes.push_back(Matcher2Helix(skeletonHelixes[i]->length, i*2, i*2+1));
        }

        return helixes;
    }

    // measures Euclidian distance between all pairs of nodes, stores the distances in euclidianMatrix, and adds
    // graph edges (stored in adjacencyMatrix) where the Euclidian distance is below EUCLIDEAN_DISTANCE_THRESHOLD
    inline void Graph::generateEuclidianMatrix(Volume * vol) {
        double xSpacing = vol->getSpacingX();
        double ySpacing = vol->getSpacingY();
        double zSpacing = vol->getSpacingZ();
        Shape * iSse;
        Shape * jSse;
        Point3Pair iLoc;
        Point3Pair iBestLoc;
        int iCorners;
        Point3Pair jLoc;
        Point3Pair jBestLoc;
        int jCorners;

        // count number of helices
        int numH = 0;
        for (unsigned int i = 0; i < (int)skeletonHelixes.size(); i++) {
            if(skeletonHelixes[i]->type == GRAPHEDGE_HELIX) {
                numH++;
            }
        }

        // for each graph node i
        for(int i = 0; i < nodeCount; i++) {
            // if i is a helix node
            if (i < 2 * numH) {
                iSse = skeletonHelixes[i/2];
                iCorners = 1;
            } else { // if i is a sheet node
                iSse = skeletonHelixes[i - numH];
                iCorners = iSse->cornerCells.size();
            }

            // for every other graph node j
            for(int j = 0; j < nodeCount; j++) {
                // if j is a helix node
                if (j < 2 * numH) {
                    jSse = skeletonHelixes[j/2];
                    //jCorners = 1;
                    jCorners = skeletonHelixes[j/2]->cornerCells.size();
                } else { // if j is a sheet node
                    jSse = skeletonHelixes[j - numH];
                    jCorners = jSse->cornerCells.size();
                }

                double minDist = MAXDOUBLE;
                double dist;

                // for every corner s of node i
                for (int s = 1; s <= iCorners; s++) {
                    if (i < 2 * numH) {		// i is a helix node
                        // TODO: looks like the following line just returns a single corner for the helix
                        iLoc = iSse->getCornerCell(i%2 + 1);
                    } else {				 // i is a sheet node
                        iLoc = iSse->cornerCells[s-1];
                    }

                    // for every corner t of node j
                    for (int t = 1; t <= jCorners; t++) {
                        if (j < 2 * numH) {		// j is a helix node
                            // TODO: looks like the following line just returns a single corner for the helix
                            jLoc = jSse->getCornerCell(j%2 + 1);
                        } else {				// j is a sheet node
                            jLoc = jSse->cornerCells[t-1];
                        }

                        // measure distance between corner s of node i and corner t of node j
                        dist = sqrt(pow(xSpacing * (double)(iLoc.x - jLoc.x), 2) + pow(ySpacing *(double)(iLoc.y - jLoc.y), 2) + pow(zSpacing *(double)(iLoc.z - jLoc.z), 2));

                        // if this distance is closer than the smallest found previously, save it
                        if (dist < minDist) {
                            minDist = dist;
                            iBestLoc = iLoc;
                            jBestLoc = jLoc;
                        }
                    }
                }
                // store the distance between the points corresponding to nodes i and j in euclidianMatrix
                euclideanMatrix[i][j] = minDist;

                // if the distance is less than EUCLIDEAN_DISTANCE_THRESHOLD, store this in the graph as a Euclidian loop edge
                if((adjacencyMatrix[i][j][1] == MAXINTT) && (euclideanMatrix[i][j] <= EUCLIDEAN_DISTANCE_THRESHOLD)) {
                    adjacencyMatrix[i][j][1] = euclideanMatrix[i][j];
                    adjacencyMatrix[i][j][0] = GRAPHEDGE_LOOP_EUCLIDEAN;
                    adjacencyMatrix[j][i][1] = euclideanMatrix[i][j];
                    adjacencyMatrix[j][i][0] = GRAPHEDGE_LOOP_EUCLIDEAN;

                    Vec3I iVec(iBestLoc.x, iBestLoc.y, iBestLoc.z);
                    Vec3I jVec(jBestLoc.x, jBestLoc.y, jBestLoc.z);
                    paths[i][j].clear();
                    paths[j][i].clear();
                    paths[j][i].push_back(jVec);
                    paths[j][i].push_back(iVec);
                    // add reverse direction path
                    paths[i][j].push_back(iVec);
                    paths[i][j].push_back(jVec);
                }
            }
        }
    }

    // Merge all sheets separated by maxDist or less
    inline void Graph::mergeSheets(double maxDist) {
#ifdef VERBOSE
        cout << "=== graph before merging sheets ===" << endl;
        print();
        cout << "=== ===" << endl;
#endif
        int firstSheet = getHelixCount();
        int numSheets = getSheetCount();
#ifdef VERBOSE
        cout << "beginning merge with " << firstSheet << " helices and " << numSheets << " sheets. maxDist = " << maxDist << ". size of helixes vector is " << skeletonHelixes.size() << endl;
#endif
        bool repeat = true;
        while (repeat) {
            repeat = false;
            for (unsigned int i = firstSheet; i < skeletonHelixes.size(); i++) {
                for (unsigned int j = skeletonHelixes.size()-1; j > i; j--) {
                    int nodei = i + firstSheet;
                    int nodej = j + firstSheet;
#ifdef VERBOSE
                    cout << "starting iteration with " << numSheets << " sheets." << endl;
                    cout << "dist between sheets " << i << " and " << j << " is " << adjacencyMatrix[nodei][nodej][1] << endl;
#endif
                    if ( (adjacencyMatrix[nodei][nodej][1] < maxDist) || (skeletonHelixes[i] == skeletonHelixes[j]) ) {
#ifdef VERBOSE
                        cout << "merging sheet " << j << " (node " << nodej << ") into sheet " << i << " (node " << nodei << ")" << endl;
#endif

                        // modify skeletonHelixes vector
                        if (skeletonHelixes[i] != skeletonHelixes[j]) {
                            // internal cells
                            int internalCellCount = (int)skeletonHelixes[j]->internalCells.size();
                            for (int k = 0; k < internalCellCount; k++) {
                                skeletonHelixes[i]->internalCells.push_back(skeletonHelixes[j]->internalCells[k]);
                            }

                            for (unsigned int k = 0; k < skeletonHelixes[j]->cornerCells.size(); k++) {
                                skeletonHelixes[i]->cornerCells.push_back(skeletonHelixes[j]->cornerCells[k]);
                            }

                            // polygonPoints
                            for (unsigned int k = 0; k < skeletonHelixes[j]->polygonPoints.size(); k++) {
                                skeletonHelixes[i]->polygonPoints.push_back(skeletonHelixes[j]->polygonPoints[k]);
                            }

                            // polygons
                            for (unsigned int k = 0; k < skeletonHelixes[j]->polygons.size(); k++) {
                                skeletonHelixes[i]->polygons.push_back(skeletonHelixes[j]->polygons[k]);
                            }

                            // length
                            skeletonHelixes[i]->length += skeletonHelixes[j]->length;

                            // cleanup
                            delete skeletonHelixes[j];
                        }

                        for (int k = j; k < firstSheet + numSheets - 1; k++) {
                            skeletonHelixes[k] = skeletonHelixes[k+1];
                        }
                        skeletonHelixes.pop_back();


                        // adjacency matrix

                        // merge rows and columns i and j:
                        int lastRowColumn = firstSheet * 2 + numSheets - 1;

                        //cout << "merging rows " << nodei << " and " << nodej << endl;
                        for (int k = 0; k <= lastRowColumn; k++){
                            int itype = (int)adjacencyMatrix[k][nodei][0];
                            int jtype = (int)adjacencyMatrix[k][nodej][0];
                            int ilength = adjacencyMatrix[k][nodei][1];
                            int jlength = adjacencyMatrix[k][nodej][1];
                            // i same type and j shorter than i, overwrite i
                            if ( (itype == jtype) && (jlength < ilength) ) {
                                adjacencyMatrix[k][nodei][1] = adjacencyMatrix[k][nodej][1];
                                adjacencyMatrix[nodei][k][1] = adjacencyMatrix[k][nodej][1];
                                // modify paths vectors
                                paths[k][nodei] = paths[k][nodej];
                                paths[nodei][k] = paths[nodej][k];
                            }
                            // if no edge at i and loop or euclidean loop at j, overwrite i
                            if ( (itype == GRAPHEDGE_OTHER) && ( ( jtype == GRAPHEDGE_LOOP) || (jtype == GRAPHEDGE_LOOP_EUCLIDEAN) ) ) {
                                adjacencyMatrix[k][nodei][0] = adjacencyMatrix[k][nodej][0];
                                adjacencyMatrix[nodei][k][0] = adjacencyMatrix[k][nodej][0];
                                adjacencyMatrix[k][nodei][1] = adjacencyMatrix[k][nodej][1];
                                adjacencyMatrix[nodei][k][1] = adjacencyMatrix[k][nodej][1];
                                paths[k][nodei] = paths[k][nodej];
                                paths[nodei][k] = paths[nodej][k];
                            }
                            // if euclidean loop at i and loop at j, overwrite i
                            if ( (itype == GRAPHEDGE_LOOP_EUCLIDEAN) && (jtype == GRAPHEDGE_LOOP) ) {
                                adjacencyMatrix[k][nodei][0] = adjacencyMatrix[k][nodej][0];
                                adjacencyMatrix[nodei][k][0] = adjacencyMatrix[k][nodej][0];
                                adjacencyMatrix[k][nodei][1] = adjacencyMatrix[k][nodej][1];
                                adjacencyMatrix[nodei][k][1] = adjacencyMatrix[k][nodej][1];
                                paths[k][nodei] = paths[k][nodej];
                                paths[nodei][k] = paths[nodej][k];
                            }
                        }

                        // merge rows in Euclidean matrix
                        //cout << "Euclidean matrix: merging rows " << nodei << " and " << nodej << endl;
                        for (int k = 0; k <= lastRowColumn; k++) {
                            int ilength = euclideanMatrix[k][nodei];
                            int jlength = euclideanMatrix[k][nodej];
                            // i same type and j shorter than i, overwrite i
                            if (jlength < ilength) {
                                euclideanMatrix[k][nodei] = euclideanMatrix[k][nodej];
                                euclideanMatrix[nodei][k] = euclideanMatrix[k][nodej];
                            }
                        }


                        // shift all rows > j up, then shift all columns > nodej to the left
                        for (int k = nodej; k <= lastRowColumn; k++) {
                            for (int m = 0; m <= lastRowColumn; m++) {
                                adjacencyMatrix[k][m][0] = adjacencyMatrix[k+1][m][0];
                                adjacencyMatrix[k][m][1] = adjacencyMatrix[k+1][m][1];
                                euclideanMatrix[k][m] = euclideanMatrix[k+1][m];
                                paths[k][m] = paths[k+1][m];
                            }
                        }

                        for (int k = nodej; k <= lastRowColumn; k++) {
                            for (int m = 0; m < firstSheet*2 + numSheets + 1; m++) {
                                adjacencyMatrix[m][k][0] = adjacencyMatrix[m][k+1][0];
                                adjacencyMatrix[m][k][1] = adjacencyMatrix[m][k+1][1];
                                euclideanMatrix[m][k] = euclideanMatrix[m][k+1];
                                paths[m][k] = paths[m][k+1];
                            }
                        }
                        // remove last row
                        for (int m = 0; m <= lastRowColumn; m++) {
                            adjacencyMatrix[lastRowColumn][m][0] = 3;
                            adjacencyMatrix[lastRowColumn][m][1] = MAXINTT;
                            euclideanMatrix[lastRowColumn][m] = 0;
                        }
                        // remove last column
                        for (int m = 0; m <= lastRowColumn; m++) {
                            adjacencyMatrix[m][lastRowColumn][0] = 3;
                            adjacencyMatrix[m][lastRowColumn][1] = MAXINTT;
                            euclideanMatrix[m][lastRowColumn] = 0;
                        }

                        // modify nodeWeights vector
                        nodeWeights[nodei] = nodeWeights[nodei] + nodeWeights[nodej];
                        for (int k = nodej; k <= lastRowColumn; k++) {
                            nodeWeights[k] = nodeWeights[k+1];
                        }
                        nodeWeights[lastRowColumn] = 0;

                        numSheets--; // local to this loop
                        nodeCount--; // for the class
                    }
                }
            }
        }
#ifdef VERBOSE
        cout << "=== graph after merging sheets ===" << endl;
        print();
        cout << "=== ===" << endl;
#endif
    }
}
#endif

