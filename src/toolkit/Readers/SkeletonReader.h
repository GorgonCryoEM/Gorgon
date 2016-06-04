#ifndef SRC_TOOLKIT_READERS_SKELETONREADER_H
#define SRC_TOOLKIT_READERS_SKELETONREADER_H

#include <Readers/reader.h>
#include <Core/volume.h>
//#include <MathTools/BasicDefines.h>
#include "GraphMatch/Shape.h"
#include <vector>
#include <queue>
#include <list>
//#include "Core/GlobalConstants.h"
#include "GraphMatch/Graph.h"

using namespace std;
using namespace SkeletonMaker;

namespace GraphMatch {

    class SkeletonReader {
        public:
            static int GetGraphIndex(vector<Shape*> & helixes, int helixNum,
                                     int cornerNum);
            static int GetGraphIndex(vector<Shape*> & helixes, int helixNum,
                                     Point3Pair * point);
            static Graph * ReadFile(string volumeFile, string helixFile,
                                    string sseFile, string sheetFile);
            static Volume* getSheetsNoThreshold(Volume * vol, int minSize);
            static void ReadSheetFile(string sheetFile,
                                      vector<Shape*> & helixes);
            static void ReadHelixFile(string helixFile, string sseFile,
                                      vector<Shape*> & helixes);
            static void FindSizes(int startHelix, int startCell,
                                  vector<Shape*> & helixList, Volume * vol,
                                  Volume * coloredVol, Graph * graph);
            static void FindPaths(Graph * graph);
            static void FindPath(int startIx, int endIx,
                                 vector<vector<Vec3I> > nodes,
                                 Volume * maskVol, Graph * graph,
                                 bool eraseMask);
            static void FindCornerCellsInSheet(Volume * vol,
                                               Volume * paintedVol,
                                               vector<Shape*> & helixes,
                                               int sheetId);
            static int isSkeletonSheet(const Volume &vol, int ox, int oy,
                                       int oz);

    };

    // returns the graph node index for corner cornerNum of helix/sheet helixNum
    inline int SkeletonReader::GetGraphIndex(vector<Shape*> & helixes, int helixNum, int cornerNum) {
        int numH = 0;
        for (unsigned int i = 0; i < (int)helixes.size(); i++) {
            if(helixes[i]->type == GRAPHEDGE_HELIX) {
                numH++;
            }
        }
        if (helixNum < numH) {
            // if this is a helix, index is 2*helixNum + cornerNum
            return helixNum*2 + helixes[helixNum]->cornerCells[cornerNum].node;
        } else {
            // if this is a sheet, index is # helices + helixNum
            return helixNum + numH + 1;
        }
    }

    // returns the graph node index for the corner of helix/sheet helixNum nearest to point point.
    inline int SkeletonReader::GetGraphIndex(vector<Shape*> & helixes, int helixNum, Point3Pair * point) {
        int node = 1;
        double minDistance = MAXINT;
        double dist;
        // find nearest corner to the input point. node = 1 or 2 for a helix.
        for(int i = 0; i < (int)helixes[helixNum]->cornerCells.size(); i++) {
            dist = Point3Pair::EuclideanDistance(helixes[helixNum]->cornerCells[i], *point);
            if(dist < minDistance) {
                node = helixes[helixNum]->cornerCells[i].node;
                minDistance = dist;
            }
        }

        // count the helices
        int numH = 0;
        for (unsigned int i = 0; i < (int)helixes.size(); i++) {
            if(helixes[i]->type == GRAPHEDGE_HELIX) {
                numH++;
            }
        }

        // if the point is near the corner, return the corner number.
        if(minDistance <= BORDER_MARGIN_THRESHOLD) {
            if (helixNum < numH) {
                return helixNum*2 + node;
            } else {
                return helixNum + numH + 1;
            }
        } else {
            return -1;
        }
    }

    inline Graph * SkeletonReader::ReadFile(string volumeFile, string helixFile, string sseFile, string sheetFile) {

        // Read the volume file and load volume data structure
        Volume * vol = (MRCReaderPicker::pick(volumeFile.c_str()))->getVolume();
        //vol->toMathematicaFile("myVolume.nb");
#ifdef VERBOSE
        printf("\033[34mConstructing 'paintedVol'...\n\033[0m");
#endif

        Volume paintedVol(vol->getSizeX(), vol->getSizeY(), vol->getSizeZ());

#ifdef VERBOSE
        printf("\033[34mFinished reading volume file, now moving on to helixes...\n\033[0m");
#endif

        // Read the helix file
        vector<Shape*> helixes;
        ReadHelixFile(helixFile, sseFile, helixes);

#ifdef INCLUDE_SHEETS

#ifdef VERBOSE
        printf("\033[34mFinished reading helix file, now moving on to sheets...\n\033[0m");
#endif // VERBOSE

        vector<Shape*> sheets;

        //ReadSheetFile(sheetFile, helixes);
        ReadSheetFile(sheetFile, sheets);

#ifdef VERBOSE
        printf("\033[34mFinished reading sheet file.\n\033[0m");
#endif // VERBOSE

#endif // INCLUDE_SHEETS

        Vec3D point, pointScaled;

        // Finding all points inside of the helixes.
        for(int x = 0; x < vol->getSizeX(); x++) {
            point[0] = vol->getOriginX() + x * vol->getSpacingX();
            for(int y = 0; y < vol->getSizeY(); y++) {
                point[1] = vol->getOriginY() + y * vol->getSpacingY();
                for(int z = 0; z < vol->getSizeZ(); z++) {
                    point[2] = vol->getOriginZ() + z * vol->getSpacingZ();
                    // if this voxel nonzero
                    if((*vol)(x, y, z) > 0) {
                        // check all helices to see if it's inside one
                        bool inHelix = false;
                        for(int i = 0; i < (int)helixes.size(); i++) {
                            // if i is a helix and if point is inside helix i
                            if(helixes[i]->type == GRAPHEDGE_HELIX && helixes[i]->isInsideShape(point)) {
                                // store helix number for this point in the volume
                                paintedVol(x, y, z) = (float)i+1;
                                // add this point as as internal cell of the helix
                                inHelix = true;
                                helixes[i]->addInternalCell(Point3Pair(x, y, z, 0));
                            }
                        }
                    }
                }
            }
        }
        #ifdef GORGON_DEBUG
              cout<<"\033[32mDEBUG: File:   SkeletonReader.h"<<endl;
              cout<<"DEBUG: Method: SkeletonReader::ReadFile(char*, char*, char*, char*)\033[0m"<<endl;
              cout<<"vol: "<<endl;
              cout<<*vol<<endl;
        #endif


        Volume* sheetClusters = getSheetsNoThreshold(vol, MINIMUM_SHEET_SIZE);

        // make the offset and scale of sheetClusters volume match the vol volume
        sheetClusters->setOrigin(vol->getOriginX(), vol->getOriginY(), vol->getOriginZ() );
        sheetClusters->setSpacing(vol->getSpacingX(), vol->getSpacingY(), vol->getSpacingZ() );

        int numSkeletonSheets = (int) sheetClusters->getMax();

        #ifdef GORGON_DEBUG
              cout<<"sheetClusters: "<<endl;
              cout<<*sheetClusters<<endl;
              cout << "min sheet size = " << MINIMUM_SHEET_SIZE << ", num skeleton sheets = " << numSkeletonSheets << ", num SSEs = " << (int)helixes.size() + (int)sheets.size() << endl;
        #endif

        vector<vector<double> > sheetDistance(numSkeletonSheets+1, vector<double> ((int)sheets.size()) );

        // for each sheet
        for (int i = 1; i <= numSkeletonSheets; i++) {
            // for each point (x,y,z)
            int count = 0;
            for(int x = 0; x < sheetClusters->getSizeX(); x++) {
                point[0] = sheetClusters->getOriginX() + x * sheetClusters->getSpacingX();
                for(int y = 0; y < sheetClusters->getSizeY(); y++) {
                    point[1] = sheetClusters->getOriginY() + y * sheetClusters->getSpacingY();
                    for(int z = 0; z < sheetClusters->getSizeZ(); z++) {
                        point[2] = sheetClusters->getOriginZ() + z * sheetClusters->getSpacingZ();
                        // if this voxel nonzero
                        if((*sheetClusters)(x, y, z) == i) {
                            count++;

                            // measure distance to every SSE sheet, add to running total
                            for(int j = 0; j < (int)sheets.size(); j++) {
                                // find plates on skeleton that are associated with sheets
                                if(sheets[j]->type == GRAPHEDGE_SHEET) {
                                    sheetDistance[i][j] += sheets[j]->minimumDistanceToPoint(point);
                                }
                            }
                        }
                    }
                }
            }
            // divide running total by number of sheet voxels to give average shortest distance
            for(int j = 0; j < (int)sheets.size(); j++) {
                sheetDistance[i][j] /= (double) count;
            }
        }

#ifdef VERBOSE
        cout << "\033[34mmin distance matrix: \033[0m" << endl;
        for (int i = 1; i <= numSkeletonSheets; i++) {
            cout << "skeleton sheet " << i << ": ";
            for(int j = 0; j < (int)sheets.size(); j++) {
                cout << sheetDistance[i][j] << " - ";
            }
            cout << endl;
        }
#endif // VERBOSE

        vector<int> sseSheetMapping(numSkeletonSheets+1, -1);
        vector<int> helixesMapping(numSkeletonSheets+1, -1);
        for (int i = 1; i <= numSkeletonSheets; i++) {
            double minDist = MAXIMUM_DISTANCE_SHEET_SKELETON;
            for (int j = 0; j < (int)sheets.size(); j++) {
                if (sheets[j]->type == GRAPHEDGE_SHEET && sheetDistance[i][j] < minDist) {
                    minDist = sheetDistance[i][j];
                    sseSheetMapping[i] = j;
                }
            }
#ifdef VERBOSE
            cout << "\033[34mskeleton sheet " << i << " maps to SSEHunter sheet " << sseSheetMapping[i] << endl;
            cout<<"\033[0m";
#endif // VERBOSE
        }

        // Add all sheets from sheet data structures to helixes list
        for (int i = 1; i <= numSkeletonSheets; i++) {
            //cout << "checking sheet " << i << " which maps to " << sseSheetMapping[i] << endl;
            if (sseSheetMapping[i] != -1) {
                helixes.push_back(sheets[sseSheetMapping[i]]);
                //cout << "added sheet " << i << " as element " << helixes.size()-1 << " of helixes vector" << endl;
                helixesMapping[i] = helixes.size() - 1;
            }
        }

        // Add all points in each sheet to the helixes data structure
        // for each point (x,y,z)
        for(int x = 0; x < sheetClusters->getSizeX(); x++) {
            point[0] = sheetClusters->getOriginX() + x * sheetClusters->getSpacingX();
            for(int y = 0; y < sheetClusters->getSizeY(); y++) {
                point[1] = sheetClusters->getOriginY() + y * sheetClusters->getSpacingY();
                for(int z = 0; z < sheetClusters->getSizeZ(); z++) {
                    point[2] = sheetClusters->getOriginZ() + z * sheetClusters->getSpacingZ();
                    // check which sheet is associated with this voxel
                    int skeletonSheetNum = (int)(*sheetClusters)(x, y, z);
                    // for voxels that are assigned to some sheet
                    if (skeletonSheetNum > 0) {
                        int sseSheetNum = helixesMapping[skeletonSheetNum];
                        if (sseSheetNum != -1) {
                            // associate this voxel with this sheet
                            paintedVol(x, y, z) = sseSheetNum+1;
                            // add this point as as internal cell of the helix
                            helixes[sseSheetNum]->addInternalCell(Point3Pair(x, y, z, 0));
                        }
                    }
                }
            }
        }

        // Save separate sheets in helixes data structure
        int numSheets = (int) sheetClusters->getMax();

        // vector to hold all the sheet volumes
        vector<Volume*> skeletonSheets;
        skeletonSheets.push_back(sheetClusters);

        // create an individual volume for each sheet from the sheetClusters volume
        for (int i = 1; i <= numSheets; i++) {
            Volume* singleSheet = new Volume(sheetClusters->getSizeX(), sheetClusters->getSizeY(), sheetClusters->getSizeZ(), 1.0); // trying something new
            singleSheet->applyMask(*sheetClusters,i,true);
            singleSheet->threshold( 0.1, 0, 1 ) ;
            skeletonSheets.push_back(singleSheet);
        }

#ifdef VERBOSE
        printf("\033[34mFinished finding points inside helices and sheets.\n\033[0m");
#endif // VERBOSE

        int numH = 0;
        int numS = 0;
        for (unsigned int i = 0; i < (int)helixes.size(); i++) {
            if(helixes[i]->type == GRAPHEDGE_HELIX) {
                numH++;
            }
            if(helixes[i]->type == GRAPHEDGE_SHEET) {
                numS++;
            }
        }

        Graph * graph = new Graph(2 * numH + numS);

        // create a graph with one node per helix end point and with edges connecting nodes that
        // are connected along the volume.
        //cout << "adding " << (int)helixes.size() << " helices and sheets to adjacency matrix" << endl;
        for(unsigned int i = 0; i < (int)helixes.size(); i++) {
            if(helixes[i]->type == GRAPHEDGE_HELIX) {
                // assign node numbers for helix ends
                int node1 = (i*2)+1;
                int node2 = (i*2)+2;

                // find the two corner cells in this helix
                helixes[i]->findCornerCellsInHelix();
                //cout << "helix " << i << " has " << helixes[i]->cornerCells.size() << " corners." << endl;
                for (unsigned int j = 0; j < helixes[i]->cornerCells.size(); j++) {
                    //cout << "corner " << j << " is associated with node " << helixes[i]->cornerCells[j].node << endl;
                }

                // length of this helix
                float length = helixes[i]->length;

                // populate adjacency matrix
                // no cost to go from a helix end back to itself
                graph->setCost(node1, node1, 0);
                graph->setType(node1, node1, GRAPHNODE_HELIX);
                graph->setCost(node2, node2, 0);
                graph->setType(node2, node2, GRAPHNODE_HELIX);
                // cost of traversing a helix is the helix length
                graph->setCost(node1, node2, length);
                // mark this edge type as helix or sheet, determined by helixes graph
                graph->setType(node1, node2, helixes[i]->type);
                // same for reverse direction
                graph->setCost(node2, node1, length);
                graph->setType(node2, node1, helixes[i]->type);
            } else if (helixes[i]->type == GRAPHEDGE_SHEET) {
                // assign node number this sheet
                int sheetNode = numH + i + 1; // each helix takes two nodes

                // find all the corner cells in this sheet
                FindCornerCellsInSheet(vol, &paintedVol, helixes, i);

                // cost is length of self-loops
                graph->setCost(sheetNode, sheetNode, SHEET_SELF_LOOP_LENGTH); // nonzero so it shows up as edge in StandardGraph::EdgeExists
                graph->setType(sheetNode, sheetNode, GRAPHNODE_SHEET);
            }

        }
        //cout << "adding sheet sizes as sheet node costs" << endl;
        for (unsigned int s = 0; s < skeletonSheets.size(); s++) {
            int sseSheetNum = helixesMapping[s];
            //cout << "node " << s << " corresponds to sheet " << helixesMapping[s] << endl;
            if (sseSheetNum != -1) {
                int sheetSize = skeletonSheets[s]->getNonZeroVoxelCount();
                int sheetNode = numH + sseSheetNum + 1; // each helix takes two nodes
                // TODO: Scale the sheet size by the geometric scale factor to make units match
                graph->setCost(sheetNode, sheetSize); // nonzero so it shows up as edge in StandardGraph::EdgeExists
                //cout << "adding sheet " << sseSheetNum << "(s=" << s << ") with size " << sheetSize << " as node " << sheetNode << endl;
            }
        }
#ifdef VERBOSE
        for (int i = 0; i < graph->getNodeCount(); i++) {
            cout << "cost of node " << i << " is " << graph->nodeWeights[i] << endl;
        }
#endif // VERBOSE

#ifdef VERBOSE
        printf("\033[34mFinished creating connectivity graph.\n\033[0m");
#endif // VERBOSE

        // populate graph->skeletonHelixes with list of helices and sheets
        for(int i = 0; i < (int)helixes.size(); i++) {
            graph->skeletonHelixes.push_back(helixes[i]);
        }

        // find the costs of all other paths along the volume, from any helix end to any other helix end
        // store the results as edges in the graph
        for(int i = 0; i < (int)helixes.size(); i++) {
            // for every entry and exit point of that helix/sheet
            for(int j = 0; j < (int)helixes[i]->cornerCells.size(); j++) {
                // find all the paths from the entry/exit point to every other helix.
                // results are stored in vol and paintedVol and as graph edges.
                FindSizes(i, j, helixes, vol, &paintedVol, graph);
            }
        }

#ifdef VERBOSE
        printf("\033[34mFinished running FindSizes2.\n\033[0m");
#endif // VERBOSE

#ifdef VERBOSE
        printf("\033[34mFinished creating a list of helices and sheets.\n\033[0m");
#endif // VERBOSE

        // save results to graph->skeletonVolume
        graph->skeletonVolume = vol;

        // save skeleton sheet volume to graph->skeletonSheetVolume
        graph->skeletonSheetVolume = sheetClusters;

        // erase extra volumes
        while ((int)skeletonSheets.size() > 1) {
            delete skeletonSheets[1];
            skeletonSheets.erase(skeletonSheets.begin() + 1);
        }

#ifdef VERBOSE
        printf("\033[34mGraph saved to object.\n\033[0m");
#endif // VERBOSE

        // measure Euclidian distance between all pairs of nodes and add edges between those nodes that are
        // closer than EUCLIDIAN_DISTANCE_THRESHOLD
        graph->generateEuclidianMatrix(vol);

#ifdef VERBOSE
        printf("\033[34mEuclidian matrix generated.\n\033[0m");
#endif // VERBOSE

#ifdef VERBOSE
        printf("\033[34mMerging pairs of sheets that are close to each other.\n\033[0m");
#endif // VERBOSE

        graph->mergeSheets(SHEET_MERGE_THRESHOLD);

#ifdef VERBOSE
        printf("\033[34mDone merging pairs of sheets.\n\033[0m");
#endif // VERBOSE


        return graph;
    }

    // Returns a collection of individual sheets with minimum size minSize, created from input volume vol
    // This is a copy of getSheets from volume.h, with the final thresholding step removed.
    inline Volume* SkeletonReader::getSheetsNoThreshold( Volume * vol, int minSize ) {
        //Initialize volume
#ifdef VERBOSE
        printf("\033[34mInitialize volume at %d %d %d\n\033[0m",  vol->getSizeX(), vol->getSizeY(), vol->getSizeZ() ) ;
#endif // VERBOSE

        Volume* svol = new Volume( vol->getSizeX(), vol->getSizeY(), vol->getSizeZ() ) ;

#ifdef GORGON_DEBUG
        cout<<"\033[32mDEBUG: File:   SkeletonReader.h"<<endl;
        cout<<"DEBUG: Method: SkeletonReader::getSheetsNoThreshold(Volume*, int)\033[0m"<<endl;
        cout<<"vol in getSheetNoThreshold()"<<endl;
        cout<<*vol;
        cout<<"svol in getSheetNoThreshold()"<<endl;
        cout<<*svol;
#endif

        //Initialize cluster counters
        int sheets[MAX_SHEETS] ;
        for (int i = 0 ; i < MAX_SHEETS ; i ++ )
        {
            sheets[ i ] = 0 ;
        }
        int totSheets = 1 ;

        //Start clustering
#ifdef VERBOSE
        printf("\033[34mStart clustering...\n\033[0m" ) ;
#endif // VERBOSE

        int ox, oy, oz ;
        for ( int i = 0 ; i < vol->getSizeX() ; i ++ )
            for ( int j = 0 ; j < vol->getSizeY() ; j ++ )
                for ( int k = 0 ; k < vol->getSizeZ() ; k ++ )
                {
                    if ( (*vol)(i,j,k) <= 0 || (*svol)(i,j,k) != 0 )
                    {
                        // Not a data point or has been visited
                        continue ;
                    }
                    if ( ! isSkeletonSheet( *vol, i, j, k ) )
                    {
                        // Not a sheet point
                        continue ;
                    }

                    //Initialize queue
                    int numNodes = 1 ;
                    (*svol)( i, j, k) = totSheets;
                    queue<QueueNode> q;
                    q.push(QueueNode( i, j, k ) );
                    while ( !q.empty() )
                    {
                        QueueNode res = q.front();
                        q.pop();
                        ox  = res.x;
                        oy  = res.y;
                        oz  = res.y;

                        // Test if neighbors satisfy sheet condition
                        if ( isSkeletonSheet(*vol, ox, oy, oz ) )
                        {
                            for ( int m = 0 ; m < 6 ; m ++ )
                            {
                                int nx = ox + neighbor6[m][0] ;
                                int ny = oy + neighbor6[m][1] ;
                                int nz = oz + neighbor6[m][2] ;

                                //if ( vol->(*this)(nx,ny,nz) > 0 && svol->(*this)(nx,ny,nz) == 0 )
                                if ( (*vol)(nx,ny,nz) > 0 && (*svol)(nx,ny,nz) == 0 && isSkeletonSheet(*vol,nx,ny,nz) )
                                {
                                    (*svol)(nx,ny,nz) =totSheets;
                                    q.push(QueueNode(nx,ny,nz));
                                    numNodes ++ ;
                                }
                            }
                        }
                    }

                    if ( numNodes > 0 )
                    {
                    	printf("Sheet %d contain %d nodes.\n", totSheets, numNodes) ;
                        sheets[ totSheets ] = numNodes ;
                        totSheets ++ ;
                    }
                }

        // Removing clusters less than minSize
#ifdef VERBOSE
        printf("\033[34mRemoving small clusters.\n\033[0m") ;
#endif // VERBOSE
        for (int i = 0 ; i < vol->getSizeX() ; i ++ )
            for (int j = 0 ; j < vol->getSizeY() ; j ++ )
                for (int k = 0 ; k < vol->getSizeZ() ; k ++ )
                {
                    int cnt = (int) (*svol)(i,j,k) ;
                    if ( cnt > 0 && sheets[ cnt ] < minSize )
                    {
                        (*svol)(i,j,k) = -1;
                    }
                }
        return svol ;
    }

    // Returns a collection of individual sheets with minimum size minSize, created from input volume vol
    // This is a copy of isSheet from volume.h, with the minimum number of corner nodes changed from 3 to 1.
    inline int SkeletonReader::isSkeletonSheet(const Volume &vol, int ox, int oy, int oz )
    {
        int cn = 12 ;
        int nx, ny, nz ;

        for ( int i = 0 ; i < 12 ; i ++ )
        {
            for ( int j = 0 ; j < 4 ; j ++ )
            {
                nx = ox + sheetNeighbor[i][j][0] ;
                ny = oy + sheetNeighbor[i][j][1] ;
                nz = oz + sheetNeighbor[i][j][2] ;

                if ( vol( nx, ny, nz ) <= 0 )
                {
                    cn -- ;
                    break ;
                }
            }
        }

        return ( cn >= 1 ) ; // isSheet has (ch >= 3) here
    }

    // finds all the corner cells in a sheet
    // corner cells are cells that are inside the sheet but have more than one neighbor on the skeleton that lies outside the sheet
    inline void SkeletonReader::FindCornerCellsInSheet(Volume * vol, Volume * paintedVol, vector<Shape*> & helixes, int sheetId) {

        // helper function for iterating over 6 neighbor voxels
        int d[6][3];
        d[0][0] = 0;		d[0][1] = 0;		d[0][2] = -1;
        d[1][0] = 0;		d[1][1] = 0;		d[1][2] = 1;
        d[2][0] = 0;		d[2][1] = -1;		d[2][2] = 0;
        d[3][0] = 0;		d[3][1] = 1;		d[3][2] = 0;
        d[4][0] = -1;		d[4][1] = 0;		d[4][2] = 0;
        d[5][0] = 1;		d[5][1] = 0;		d[5][2] = 0;

        int outsideCounter; // count of neighbors that lie outside the sheet

        // for each voxel that is inside the sheet
        for(int i = 0; i < (int)helixes[sheetId]->internalCells.size(); i++) {
            outsideCounter = 0;

            // iterate over 6 neighbor voxels, counting how many lie outside the sheet
            for(int j = 0; j < 6; j++) {
                if(((*vol)(helixes[sheetId]->internalCells[i].x + d[j][0], helixes[sheetId]->internalCells[i].y + d[j][1], helixes[sheetId]->internalCells[i].z + d[j][2]) > 0) &&
                    ((*paintedVol)(helixes[sheetId]->internalCells[i].x + d[j][0], helixes[sheetId]->internalCells[i].y + d[j][1], helixes[sheetId]->internalCells[i].z + d[j][2]) == 0)) {
                    outsideCounter++;
                }
            }

            // if some neighbor voxels are outside the sheet
            if(outsideCounter > 0) {
                bool found = false;
                // iterate over all corner cells of this sheet, checking to see if this voxel has been labeled as a corner cell yet
                for(int k = 0; k < (int)helixes[sheetId]->cornerCells.size(); k++) {
                    found = found || ((helixes[sheetId]->cornerCells[k].x == helixes[sheetId]->internalCells[i].x) &&
                        (helixes[sheetId]->cornerCells[k].y == helixes[sheetId]->internalCells[i].y) &&
                        (helixes[sheetId]->cornerCells[k].z == helixes[sheetId]->internalCells[i].z));
                }
                // if this voxel has not already been labeled as a corner cell, do so now
                if(!found) {
                    helixes[sheetId]->cornerCells.push_back(helixes[sheetId]->internalCells[i]);
                    //helixes[sheetId]->cornerCells[helixes[sheetId]->cornerCells.size()-1].node = 1;
                    helixes[sheetId]->cornerCells[helixes[sheetId]->cornerCells.size()-1].node = helixes[sheetId]->cornerCells.size();
#ifdef VERBOSE
                    cout << "\033[34mSheet corner cell found at sheet " << sheetId << " node " << i << ", corner " << helixes[sheetId]->cornerCells.size() << ", coordinates (" << helixes[sheetId]->internalCells[i].x << "," << helixes[sheetId]->internalCells[i].y << "," << helixes[sheetId]->internalCells[i].z << "), outsideCounter = " << outsideCounter << endl;
                    cout<<"\033[0m";
#endif // VERBOSE
                }
            }
        }
#ifdef VERBOSE
        cout << "\033[34mDone finding corner cells for sheet " << sheetId << ". " << helixes[sheetId]->cornerCells.size() << " corner cells.  " << helixes[sheetId]->internalCells.size() << " internal cells." << endl;
        cout<<"\033[0m";
#endif // VERBOSE
        //assert(helixes[sheetId]->cornerCells.size() >= 2);
        helixes[sheetId]->length = (float)helixes[sheetId]->internalCells.size();
    }

    // Parses sheetFile, a .wrl file containing a list of polygons that form a sheet.
    // Creates a Shape object consisting of a collection of polygons (triangles) for each sheet.
    // Adds these sheet objects to helixes.
    inline void SkeletonReader::ReadSheetFile(string sheetFile, vector<Shape*> & helixes){
        ifstream fin(sheetFile.c_str());
        if (!fin) {
            cout<<"Error reading sheet input file "<<sheetFile<<".  Skipping sheets.\n" ;
        } else {
            Shape * shape = NULL;

            string token;
            double x,y,z;
            int a,b,c,d;
            Polygon p;
            bool lastSheet = false;
            while (fin>>token) {
                // add shape from previous iteration to list of SSEs
                if(token == TOKEN_VRML_SHAPE) {
                    if(shape != NULL) {
                        helixes.push_back(shape);
                    }
                    shape = new Shape();
                    shape->type = GRAPHEDGE_SHEET;
                    lastSheet = false;
                // adds new 3d points to polygonPoints
                } else if(token == TOKEN_VRML_POINT) {
                    fin>>token;
                    while (fin>>x>>y>>z) {
                        shape->polygonPoints.push_back(Vec3D(x, y, z));
                    }
                    lastSheet = true;
                // adds new polygons built from list of polygonPoints to shape
                } else if(token == TOKEN_VRML_COORDINDEX) {
                    fin>>token;
                    while (fin>>a>>b>>c>>d) {
                        p.pointIndex1 = a;
                        p.pointIndex2 = b;
                        p.pointIndex3 = c;
                        p.pointIndex4 = d;
                        shape->polygons.push_back(p);
                    }
                    lastSheet = true;
                }
            }
            // add last shape to list of SSEs
            if(lastSheet) {
                helixes.push_back(shape);
            }

            fin.close();
        }
    }

    inline void SkeletonReader::ReadHelixFile(string helixFile, string sseFile, vector<Shape*> & helixes){
//        #ifdef GORGON_DEBUG
              cout<<"\033[32mDEBUG: File:   SkeletonReader.h"<<endl;
              cout<<"DEBUG: Method: SkeletonReader::ReadHelixFile(string, string, vector<Shape*>&)\033[0m"<<endl;
              cout<<"Im in!"<<endl;
//        #endif

        ifstream fin(helixFile.c_str());
        if (!fin) {
            cout<<"Error reading helix input file "<<helixFile<<".  Skipping helices.\n" ;
        } else {
            Shape * shape = new Shape();
            shape->type = GRAPHEDGE_HELIX;

            string token;
            double x,y,z,a;

            // read in helices, one at a time, adding each to helixes
            while (fin>>token) {
                if(token == TOKEN_VRML_TRANSLATION) {
                    fin>>x>>y>>z;
                    //shape->Translate(Vector3(x, y, z));
                    shape->setCenter(Vec3D(x, y, z));
                } else if(token == TOKEN_VRML_ROTATION) {
                    fin>>x>>y>>z>>a;
                    shape->rotate(Vector3<double>(x, y, z), a);
                } else if(token == TOKEN_VRML_HEIGHT) {
                    fin>>a;
                    //shape->Scale(1.0, a, 1.0);
                    shape->setHeight(a);
                } else if(token == TOKEN_VRML_RADIUS) {
                    fin>>a;
                    //shape->Scale(a*2, 1.0, a*2);
                    shape->setRadius(a);

                    // add shape to list of helices
                    helixes.push_back(shape);

                    // reinitialize shape variable
                    shape = new Shape();
                    shape->type = GRAPHEDGE_HELIX;
                }
            }
            delete shape;

            fin.close();

            // if sseFile was provided as an argument, parse it to get lengths of helices.
            // store the helix lengths in this file with the helices in helixes
            // assume that the lengths in this file are provided in the same order as the helices were stored above.
            fin.open(sseFile.c_str());
            if (!fin) {
                cout<<"Error reading helix length file "<<sseFile<<". Skipping helix lengths\n" ;
            } else {

                string t1, t2, t3;
                int length;
                unsigned int count = 0;

                while (fin>>token) {
                    if(token == TOKEN_SSE_ALPHA && count < helixes.size() ) { // size check prevents crash when lengths file has more entries than helices loaded above
                        fin>>t1>>t2>>t3>>length;
                        helixes[count]->length = (float)length * HELIX_C_ALPHA_TO_ANGSTROMS;
                        count++;
                    }
                }
                fin.close();
            }
        }
    }

    // finds the loops from the helix/sheet corner given by helixList[startHelix]->cornerCells[startCell] to
    // all other helices/sheets by flooding outward along the skeleton volume
    // stores the resulting loops in the graph object using graph->SetCost and graph->SetType
    inline void SkeletonReader::FindSizes(int startHelix, int startCell, vector<Shape*> & helixList, Volume * vol, Volume * coloredVol, Graph * graph) {
        vector<Point3Pair *> oldStack;
        vector<Point3Pair *> newStack;
        int currentHelix;

        // create arrays to store paint color (paintVol) and a path direction indicator (backVol) for each voxel in maskVol
        int * backVol = new int[coloredVol->getSizeX() * coloredVol->getSizeY() * coloredVol->getSizeZ()];
        // initialize to -1
        for(int i = 0; i < coloredVol->getSizeX() * coloredVol->getSizeY() * coloredVol->getSizeZ(); i++) {
            backVol[i] = -1;
        }
        int newIx;

        // calculate starting point, in unscaled voxel coords
        Point3Pair * startPoint = new Point3Pair(helixList[startHelix]->cornerCells[startCell].x, helixList[startHelix]->cornerCells[startCell].y, helixList[startHelix]->cornerCells[startCell].z, 0);

        // add to list of voxels to be explored
        oldStack.push_back(startPoint);

        Point3Pair * currentPoint; //CurrentPoint
        Vec3D cPt, nPt;
        int x, y, z, xx, yy, zz;

        // helper array for finding one of 26 neighbors of a voxel.
        // number on right is index in d that moves in the opposite direction. stored as back[] below and used for retracing paths.
        int d[26][3];
        d[0][0] = 0;		d[0][1] = 0;		d[0][2] = -1; // 1
        d[1][0] = 0;		d[1][1] = 0;		d[1][2] = 1;  // 0
        d[2][0] = 0;		d[2][1] = -1;		d[2][2] = 0;  // 5
        d[3][0] = 0;		d[3][1] = -1;		d[3][2] = -1; // 7
        d[4][0] = 0;		d[4][1] = -1;		d[4][2] = 1;  // 6
        d[5][0] = 0;		d[5][1] = 1;		d[5][2] = 0;  // 2
        d[6][0] = 0;		d[6][1] = 1;		d[6][2] = -1; // 4
        d[7][0] = 0;		d[7][1] = 1;		d[7][2] = 1;  // 3
        d[8][0] = -1;		d[8][1] = 0;		d[8][2] = 0;  // 17
        d[9][0] = -1;		d[9][1] = 0;		d[9][2] = -1; // 19
        d[10][0] = -1;		d[10][1] = 0;		d[10][2] = 1; // 18
        d[11][0] = -1;		d[11][1] = -1;		d[11][2] = 0; // 23
        d[12][0] = -1;		d[12][1] = -1;		d[12][2] = -1;// 25
        d[13][0] = -1;		d[13][1] = -1;		d[13][2] = 1; // 24
        d[14][0] = -1;		d[14][1] = 1;		d[14][2] = 0; // 20
        d[15][0] = -1;		d[15][1] = 1;		d[15][2] = -1;// 22
        d[16][0] = -1;		d[16][1] = 1;		d[16][2] = 1; // 21
        d[17][0] = 1;		d[17][1] = 0;		d[17][2] = 0; // 8
        d[18][0] = 1;		d[18][1] = 0;		d[18][2] = -1;// 10
        d[19][0] = 1;		d[19][1] = 0;		d[19][2] = 1; // 9
        d[20][0] = 1;		d[20][1] = -1;		d[20][2] = 0; // 14
        d[21][0] = 1;		d[21][1] = -1;		d[21][2] = -1;// 16
        d[22][0] = 1;		d[22][1] = -1;		d[22][2] = 1; // 15
        d[23][0] = 1;		d[23][1] = 1;		d[23][2] = 0; // 11
        d[24][0] = 1;		d[24][1] = 1;		d[24][2] = -1;// 13
        d[25][0] = 1;		d[25][1] = 1;		d[25][2] = 1; // 12

        int back[26] = {1, 0, 5, 7, 6, 2, 4, 3, 17, 19, 18, 23, 25, 24, 20, 22, 21, 8, 10, 9, 14, 16, 15, 11, 13, 12};



        // mark all voxels as unvisited
        Volume * visited = new Volume(vol->getSizeX(), vol->getSizeY(), vol->getSizeZ());

        int helixCount = graph->getHelixCount();

        bool expand;

        // while the previous iteration ended with more voxels to expand
        while(oldStack.size() > 0) {
            // oldStack is list of points from previous iteration
            // newStack is list of points found so far in this iteration
            newStack.clear();
            for(int i = 0; i < (int)oldStack.size(); i++) {
                currentPoint = oldStack[i];
                cPt = Vec3D(currentPoint->x * vol->getSpacingX(), currentPoint->y * vol->getSpacingY(), currentPoint->z * vol->getSpacingZ());
                expand = true;
                xx = currentPoint->x;
                yy = currentPoint->y;
                zz = currentPoint->z;
                currentHelix = round((*coloredVol)(xx,yy,zz)) - 1;
                // mark this point as visited
                (*visited)(xx, yy, zz) = 1;



                // if the current point is inside some helix/sheet other than the start helix/sheet,
                // save the cost in the adjacency matrix and expand no further from this point
                if((currentHelix >= 0) && (currentHelix != startHelix)) {
                    int n1, n2;
                    // n1 is the graph index of start helix/sheet in helixList.
                    // n2 is the graph index of currentPoint, which is some other helix/sheet.
                    n1 = GetGraphIndex(helixList, startHelix, startCell);
                    n2 = GetGraphIndex(helixList, currentHelix, currentPoint);
                    bool found = false;
                    if( (n1 >= 0) && (n2 >= 0) && (currentPoint->distance < graph->getCost(n1, n2)) ) { // includes check for previously found shorter path
                        // store the distance to the currentPoint as the cost of going from the start helix/sheet to the currentPoint helix/sheet
                        graph->setCost(n1, n2, currentPoint->distance);
                        // this is a loop type
                        graph->setType(n1, n2, GRAPHEDGE_LOOP);
                        // save the same info for the reverse direction
                        graph->setCost(n2, n1, currentPoint->distance);
                        graph->setType(n2, n1, GRAPHEDGE_LOOP);
                        found = true;
                    }

                    // if the found SSE is a helix, stop expanding. if a sheet, keep expanding. this prevents paths from passing through helices
                    // and allows them to cross sheets.
                    if( (currentHelix < helixCount) ) {
                        // stop expanding, since some other helix has been found
                        expand = false;
                    } else {
                        // keep expanding, since a sheet was found, and paths are allowed to cross sheets
                        expand = true;
                    }

                    // retrace the path
                    bool backFound = false;

                    // if a path was found above, retrace it using info from backVol and store the path in graph->paths
                    Vec3I currentPos = Vec3I(xx, yy, zz); // update the current position
                    if(found) {
                        // store endPosition to path vector
                        graph->paths[n2-1][n1-1].clear();
                        graph->paths[n2-1][n1-1].push_back(currentPos);
                        graph->paths[n1-1][n2-1].clear();
                        // store opposite direction path
                        graph->paths[n1-1][n2-1].insert(graph->paths[n1-1][n2-1].begin(),currentPos);
                        while(!backFound) {
                            newIx = coloredVol->getIndex(currentPos.XInt(), currentPos.YInt(), currentPos.ZInt());
                            backFound = (backVol[newIx] < 0);
                            if(!backFound) {
                                // move in the direction indicated by backVol for this point, to retrace path found above
                                currentPos = currentPos + Vec3I(d[backVol[newIx]][0], d[backVol[newIx]][1], d[backVol[newIx]][2]);
                                // add the next point to the path
                                graph->paths[n2-1][n1-1].push_back(currentPos);
                                // store in opposite direction path as well
                                graph->paths[n1-1][n2-1].insert(graph->paths[n1-1][n2-1].begin(),currentPos);
                            }
                        }
                    }

                }

                // if no other helix/sheet has been found yet, keep expanding
                if(expand) {
                    // for each of 26 neighbors
                    for(int j = 0; j < 26; j++) {
                        // get coords of this neighbor point
                        x = currentPoint->x+d[j][0];
                        y = currentPoint->y+d[j][1];
                        z = currentPoint->z+d[j][2];
                        // scale coords to volume space
                        nPt = Vec3D(x * vol->getSpacingX(), y * vol->getSpacingY(), z * vol->getSpacingZ());

                        // if neighbor point is inside the volume
                        if((x >= 0) && (x < vol->getSizeX()) && (y >=0) && (y < vol->getSizeY()) && (z >= 0) && (z < vol->getSizeZ())) {
                            // if all these conditions met:
                            //    this point not yet visited
                            //    volume at this point has value at least 0.001
                            //    this point is not inside the start helix or on the start sheet
                            //  TODO: Consider case where this voxel may have already been visited but with a longer path
                            if(((*visited)(x, y, z) <= 0.001) && ((*vol)(x, y, z) > 0.001) &&
                                (round((*coloredVol)(x, y, z)) - 1 != startHelix)) {
                                // add this point to newStack with distance = | cPt - nPt |
                                // the distance is the length of the vector from the cPt voxel to this neighbor nPt
                                newStack.push_back(new Point3Pair(x, y, z, currentPoint->distance + (float)(cPt - nPt).length()));
                                // mark this point as visited
                                (*visited)(x, y, z) = 1.0;
                                // Look up array index in backVol
                                newIx = coloredVol->getIndex(x,y,z);
                                backVol[newIx] = back[j];
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
        delete visited;
        delete backVol;
    }


    // Find all paths in a graph
    inline void SkeletonReader::FindPaths(Graph * graph) {
        vector<Vec3I> endPoints;
        vector< vector<Vec3I> > nodes;
        Point3Pair pt;
        vector<Vec3I> node;

#ifdef VERBOSE
        printf("\033[34mStoring helix endpoints.\n\033[0m");
#endif // VERBOSE

        // find all graph nodes
        for(unsigned int i = 0; i < graph->skeletonHelixes.size(); i++) {
            // store all helix endpoints in nodes vector
            // 2 pts per helix: even are helix start, odd are helix end
            if (graph->skeletonHelixes[i]->isHelix()) {
                for(unsigned int j = 1; j <= 2; j++) {
                    pt = graph->skeletonHelixes[i]->getCornerCell(j);
                    node = vector<Vec3I>();
                    endPoints.push_back(Vec3I(pt.x, pt.y, pt.z));
                    node.push_back(Vec3I(pt.x, pt.y, pt.z));
                    nodes.push_back(node);
                }
            }

            // store all sheet corners in nodes vector
            if (graph->skeletonHelixes[i]->isSheet()) {
                node = vector<Vec3I>();
                for(unsigned int j = 1; j <= graph->skeletonHelixes[i]->cornerCells.size(); j++) {
                    pt = graph->skeletonHelixes[i]->getCornerCell(j);
                    node.push_back(Vec3I(pt.x, pt.y, pt.z));
                }
                nodes.push_back(node);
            }
        }

        Volume * maskVol = new Volume(*graph->skeletonVolume);

#ifdef VERBOSE
        printf("\033[34mFinding all paths through helices.\n\033[0m");
#endif // VERBOSE

        // add all paths through helices, from (odd) start index to (even) end index
        for(unsigned int i = 0; i < nodes.size(); i+=2) {
            if (graph->skeletonHelixes[i/2]->isHelix()) {
                FindPath(i, i+1, nodes, maskVol, graph, true); // eraseMask=true means don't render?
            }
        }

#ifdef VERBOSE
        printf("\033[34mFinding all paths between helices.\n\033[0m");
#endif // VERBOSE
        // for every pair of endpoints i!=j, add a path, if this path is not already defined
        for(unsigned int i = 0; i < nodes.size()-1; i++) {
            for(unsigned int j = i+1; j < nodes.size(); j++) {
                cout << "finding path between nodes " << i << " and " << j << endl;
                if(graph->paths[i][j].size() == 0) {
                    FindPath(i, j, nodes, maskVol, graph, false);
                }
            }
        }

        cout << "after adding all paths, maskVol has " << maskVol->getNonZeroVoxelCount() << " nonzero voxels" << endl;

#ifdef VERBOSE
        printf("\033[34mDone finding all paths between helices.\n\033[0m");
#endif // VERBOSE

        delete maskVol;
    }

    // Find path from endpoints[startIx] to endpoints[endIx] in a graph.
    // The path grows outward from start point to end point along voxels in the maskVol with values > 0.5.
    // The path is stored in graph->paths[startIx][endIx] and also painted in maskVol.
    // If eraseMask is set, maskVol voxels inside the startIx and endIx helices are not painted.
    inline void SkeletonReader::FindPath(int startIx, int endIx, vector<vector<Vec3I> > nodes, Volume * maskVol, Graph * graph, bool eraseMask) {
        // erase any old path
        graph->paths[startIx][endIx] = vector<Vec3I>();

        // positions will store the path
        queue<Vec3I> positions;

        int shortestPathLength = MAXINT;
        vector<Vec3I> shortestPath;

        for (unsigned int s = 0; s < nodes[startIx].size(); s++) {
            for (unsigned int t = 0; t < nodes[endIx].size(); t++) {
                // start with empty queue
                positions = queue<Vec3I>();
                Vec3I currentPos = nodes[startIx][s], newPos, endPos = nodes[endIx][t];
                positions.push(currentPos);
                // create arrays to store paint color (paintVol) and a path direction indicator (backVol) for each voxel in maskVol
                int * paintVol = new int[maskVol->getSizeX() * maskVol->getSizeY() * maskVol->getSizeZ()];
                int * backVol = new int[maskVol->getSizeX() * maskVol->getSizeY() * maskVol->getSizeZ()];
                // initialize to -1
                for(int i = 0; i < maskVol->getSizeX() * maskVol->getSizeY() * maskVol->getSizeZ(); i++) {
                    paintVol[i] = -1;
                    backVol[i] = -1;
                }
                // paint start point
                paintVol[maskVol->getIndex(currentPos.X(), currentPos.Y(), currentPos.Z())] = 0;
                // local vars for while loop
                bool found = false;
                int currVal, newIx;
                int currentPathLength = 0;
                vector<Vec3I> currentPath;

                // grows outward repeatedly from the start point to each of 26 neighbors until the end point is found
                // only paints voxels with maskVol > 0.5
                // stops the first time the end point is found, or when all paths have already been explored and no path found
                while(!found && !positions.empty()) {
                    // get current voxel position from queue
                    currentPos = positions.front();
                    positions.pop();
                    // look up paint color of current voxel
                    currVal = paintVol[maskVol->getIndex(currentPos.X(), currentPos.Y(), currentPos.Z())];
                    // check if current voxel is the end voxel
                    found = (currentPos == endPos);

                    // if not already at the end voxel
                    if(!found) {
                        // for each of the 26 neighboring voxels
                        for(int i = 0; i < 26; i++) {
                            // set newPos to the neighbor voxel location
                            newPos = currentPos + Vec3I(D26[i][0], D26[i][1], D26[i][2]);
                            // find the maskVol index number corresponding to this voxel
                            newIx = maskVol->getIndex(newPos.X(), newPos.Y(), newPos.Z());
                            // must satisfy one of two cases:
                            //   new voxel is unvisited, and is an allowed voxel according to maskVol
                            //   voxel has already been visited, but with a longer path from the source
                            if(((*maskVol)(newIx) > 0.5) && ((paintVol[newIx] < 0) || (paintVol[newIx] > currVal + 1))) {
                                // add new voxel to queue
                                positions.push(newPos);
                                // associate a new path length with the new voxel
                                paintVol[newIx] = currVal + 1;
                                // store a number in backVol to describe this path
                                backVol[newIx] = BACK26[i];
                            }
                        }
                    }
                }



                bool backFound = false;

                // if a path was found above, retrace it using info from backVol and store the path in graph->paths
                if(found) {
                    // set currentPos to the end of the path
                    currentPos = Vec3I(endPos.X(), endPos.Y(), endPos.Z());
                    // store endPosition to path vector
                    //graph->paths[startIx][endIx].push_back(currentPos);
                    currentPath.push_back(currentPos);
                    while(!backFound) {
                        newIx = maskVol->getIndex(currentPos.X(), currentPos.Y(), currentPos.Z());
                        backFound = (backVol[newIx] < 0);
                        if(!backFound) {
                            // move in the direction indicated by backVol for this point, to retrace path found above
                            currentPos = currentPos + Vec3I(D26[backVol[newIx]][0], D26[backVol[newIx]][1], D26[backVol[newIx]][2]);
                            // add the next point to the path
                            currentPath.push_back(currentPos);
                            currentPathLength++;
                        }
                    }
                }
                delete [] paintVol;
                delete [] backVol;

                if (found && currentPathLength < shortestPathLength) {
                    shortestPathLength = currentPathLength;
                    shortestPath = currentPath;
                }
            }
        }
        if (shortestPathLength < MAXINT) {
            graph->paths[startIx][endIx] = shortestPath;
        }

        if(eraseMask) {
            // for each voxel along the path found above
            for(int i = 1; i < (int)graph->paths[startIx][endIx].size()-1; i++) {
                Vec3I currentPos = graph->paths[startIx][endIx][i];
                Vec3D pt = Vec3D(currentPos.X(), currentPos.Y(), currentPos.Z());
                // if this voxel is inside either the start helix or the end helix for this path
                if(graph->skeletonHelixes[(int)startIx/2]->isInsideShape(pt) ||
                        graph->skeletonHelixes[(int)endIx/2]->isInsideShape(pt)) {
                    // erase the voxel from maskVol
                    (*maskVol)(currentPos.X(), currentPos.Y(), currentPos.Z()) = 0.0;
                }
            }
        }
    }
}

#endif
