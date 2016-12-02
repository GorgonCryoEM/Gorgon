// This file was named WongMatch15ConstrainedNoFuture.h

#ifndef TOOLKIT_GRAPHMATCH_WONGMATCH_H
#define TOOLKIT_GRAPHMATCH_WONGMATCH_H

//#include "Graph.h"
#include "Foundation/Node.h"
//#include "NodeList.h"
//#include "Core/GlobalConstants.h"
#include "PathGenerator.h"

#include "Foundation/Pair.h"
//#include "Structures.h"
#include <ctime>
////#include <SkeletonMaker/PriorityQueue.h>
#include "ProteinMorph/SSEResult.h"

using namespace Foundation;

namespace GraphMatch {

    class WongMatch {
        public:
            Graph & patternGraph;
            Graph & baseGraph;
        public:
            WongMatch(Graph & pGraph, Graph & bGraph,
                      int missingHelixCount =0, int missingSheetCount =0
                      );
            ~WongMatch();
            int run(clock_t startTime);
            SSEResult getResult(int rank);
            void saveResults();

            void set_MISSING_HELIX_COUNT(int n);
            void set_MISSING_SHEET_COUNT(int n);

        protected:
            vector<SSEResult> solutions;

        private:
#ifdef VERBOSE
            clock_t timeInGetA;
            clock_t timeInGetB;
            clock_t timeInQueue;
#endif
            Node * currentNode;

            typedef Pair<double, Node *> Elem;
            typedef priority_queue<Elem, std::vector<Elem>, std::greater<Elem> > PriorityQueue;
            PriorityQueue q;

            vector<NodeStub*> usedNodes;
            int nMissHelix;
            int nMissSheet;
            int nExpand;
            int foundCount;
            int longestMatch;
            PathGenerator * pathGenerator;

        protected:
            void init(Graph & pGraph, Graph & bGraph);

        private:
            double getC(int p, int qp);
            double getC(int j, int p, int qj, int qp);
            double getCost(int d, int m, int qj, int qp, bool debugMsg);
            double getPenaltyCost(int d, int m, bool debugMsg);
            double getCPrime(int a, int b, int c, int d);
            double getK(int p, int qp);
            double getKPrime(int i, int q);
            double getF();
            void popBestNode(); // Gets the best (first) node from the active nodes list.
            bool expandNode(NodeStub * currentStub); // Expands all the children of the current node.
            void normalizeGraphs();
            void normalizeSheets();
            void ComputeSolutionCost(int solution[], bool extraMessages);
            void printNodeConcise(Node *node, int rank, bool endOfLine, bool printCostBreakdown);
            int bestMatches[RESULT_COUNT][MAX_NODES];
    };

    inline void WongMatch::set_MISSING_HELIX_COUNT(int n) {
        nMissHelix = n;
    }

    inline void WongMatch::set_MISSING_SHEET_COUNT(int n) {
        nMissSheet = n;
    }

    inline WongMatch::WongMatch(Graph & pGraph, Graph & bGraph,
                         int missingHelixCount,
                         int missingSheetCount)
                : patternGraph(pGraph), baseGraph(bGraph)
    {
//        init(patternGraph, baseGraph);
        nMissHelix = missingHelixCount;
        nMissSheet = missingSheetCount;
    }

    inline WongMatch::~WongMatch() {
        for(unsigned int i = 0; i < usedNodes.size(); i++)
            delete usedNodes[i];

        while(!q.empty()) {
            Elem res = q.top();
            Node * tempNode = res.second;
            delete tempNode;
            q.pop();
        }

        delete pathGenerator;
    }

    inline void WongMatch::init(Graph & pGraph, Graph & bGraph)
    {
#ifdef VERBOSE
        cout << "Initializing search" << endl;
        cout << "Base graph has " << baseGraph.getHelixCount()
             << " helices and " << baseGraph.getSheetCount() << " sheets."
             << endl;
        cout << "Pattern graph has " << patternGraph.getHelixCount()
             << " helices and " << patternGraph.getSheetCount() << " sheets."
             << endl;
#endif
        usedNodes.clear();
#ifdef VERBOSE
        cout << "Creating priority queue" << endl;
#endif
        q = PriorityQueue();
#ifdef VERBOSE
        cout << "Loading pattern graph" << endl;
#endif
        patternGraph = pGraph;
#ifdef VERBOSE
        cout << "Loading base graph" << endl;
#endif
        baseGraph = bGraph;
        nExpand = 0;

#ifdef VERBOSE
        cout << "Finding the number of missing helices and sheets" << endl;
#endif
        nMissHelix = (patternGraph.getNodeCount() - baseGraph.getNodeCount()) / 2;
        if(nMissHelix < 0) {
            nMissHelix = 0;
        }
        if(nMissSheet < 0) {
            nMissSheet = 0;
        }

        // new method of counting missing sheets and helices
        // count helix nodes in base graph
        int baseHelixNodes = 0;
        int baseSheetNodes = 0;
        int patternHelixNodes = 0;
        int patternSheetNodes = 0;

        for(int i = 0; i < baseGraph.getNodeCount(); i++) {
            switch((int) (baseGraph.adjacencyMatrix[i][i][0] + 0.01)) {
                case (GRAPHNODE_HELIX):
                    baseHelixNodes++;
                    break;
                case (GRAPHNODE_SHEET):
                    baseSheetNodes++;
                    break;
                default:
                    break;
            }
        }
#ifdef VERBOSE
        cout << "base graph has " << baseHelixNodes << " helix nodes and "
             << baseSheetNodes << " sheet nodes." << endl;
#endif

        for(int i = 0; i < patternGraph.getNodeCount(); i++) {
            switch((int) (patternGraph.adjacencyMatrix[i][i][0] + 0.01)) {
                case (GRAPHNODE_HELIX):
                    patternHelixNodes++;
                    break;
                case (GRAPHNODE_SHEET):
                    patternSheetNodes++;
                    break;
                default:
                    break;
            }
        }
#ifdef VERBOSE
        cout << "pattern graph has " << patternHelixNodes << " helix nodes and "
             << patternSheetNodes << " sheet nodes." << endl;
#endif

        nMissHelix = (patternHelixNodes - baseHelixNodes) / 2;

        // allow all strands to be missing
        nMissSheet = patternSheetNodes;

#ifdef VERBOSE
        cout << "missing helix count is " << nMissHelix
             << ", missing sheet count is " << nMissSheet << endl;
        cout << "missing helix penalty is " << MISSING_HELIX_PENALTY
             << ", missing sheet penalty is " << MISSING_SHEET_PENALTY << endl;
#endif

        if(!PERFORMANCE_COMPARISON_MODE) {
            normalizeGraphs();
            normalizeSheets();
        }
        foundCount = 0;
        longestMatch = 0;
#ifdef VERBOSE
        timeInGetA = 0;
        timeInGetB = 0;
        timeInQueue = 0;
#endif

        // create and set up a new node to start the search
        currentNode = new Node();
        for(int j = 1; j <= patternGraph.nodeCount; j++) {
            Node::AddNodeToBitmap(currentNode->m1Bitmap, j);
        }
        for(int j = 1; j <= baseGraph.nodeCount; j++) {
            Node::AddNodeToBitmap(currentNode->m2Bitmap, j);
        }
        q.push(Elem(currentNode->cost, currentNode));
        pathGenerator = new PathGenerator(&baseGraph);
    }

    // searches for correspondences between the pattern graph and base graph.
    inline int WongMatch::run(clock_t startTime) {
#ifdef VERBOSE
        cout << "Starting to search for correspondences." << endl;
        DisplayConstants();
#endif
        bool continueLoop = true;
        clock_t finishTime;
        // repeat the following loop until all results are found
        while(continueLoop) {
            popBestNode();
            if(currentNode == NULL) {
                break;
            }

            // if currentNode contains a complete sequence match, add it to the solutions list
            if(currentNode->depth == patternGraph.nodeCount) {
                finishTime = clock();
                foundCount++;
                printNodeConcise(currentNode, foundCount, false, false);
                printf(": (%d expanded) (%f seconds) (%d parent size)\n",
                                        nExpand,
                                        (double) (finishTime - startTime) / (double) CLOCKS_PER_SEC,
                                        (int)usedNodes.size());
#ifdef _WIN32
                flushall();
#endif
                int numHelices = baseGraph.getHelixCount();
                solutions.push_back(SSEResult(currentNode, numHelices));

#ifdef MAKE_FINAL_MRC
                char fileName[80];
                sprintf(fileName, "Solution%d.mrc", foundCount);
                pathGenerator->GenerateGraph(currentNode, fileName);
#endif
                // otherwise, expand currentNode and adds its children to usedNodes
            }
            else {
                NodeStub * currentStub = new NodeStub(*currentNode);
                if(expandNode(currentStub)) {
                    usedNodes.push_back(currentStub);
                }
                else {
                    delete currentStub;
                }
                delete currentNode;
            }
            // continue until desired number of results are found
            continueLoop = (foundCount < RESULT_COUNT) && (!q.empty());
        }

        //Cleaning up memory
        for(unsigned int i = 0; i < usedNodes.size(); i++) {
            delete usedNodes[i];
        }
        usedNodes.clear();

        while(!q.empty()) {
            Elem res = q.top();
            Node * tempNode = res.second;
            delete tempNode;
            q.pop();
        }

#ifdef VERBOSE
        cout << "Finished the correspondence search. Found " << foundCount
             << " results." << endl;
#endif

        return foundCount;
    }

    // returns one of the results of a correspondence search
    inline SSEResult WongMatch::getResult(int rank) {
        return solutions[rank - 1];
    }

    // prints correspondence search results
    inline void WongMatch::saveResults() {
#ifdef VERBOSE
        printf("Time taken in GetA %f\n", timeInGetA / (double)CLOCKS_PER_SEC);
        printf("Time taken in GetB %f\n", timeInGetB / (double)CLOCKS_PER_SEC);
        printf("Time taken in Queue %f\n", timeInQueue / (double)CLOCKS_PER_SEC);
#endif

    }

    // returns the cost of matching node p in the pattern graph to node qp in the base graph
    // this method does not include any cost for matching strands to sheets.
    inline double WongMatch::getC(int p, int qp) {
        double cost = getC(p, p, qp, qp);

        // if sheet-to-strand match, compute the cost of the match based on the unused sheet capacity and the strand length
        if(   (int) (patternGraph.adjacencyMatrix[ p-1][ p-1][0] + 0.01) == GRAPHNODE_SHEET
           && (int) (   baseGraph.adjacencyMatrix[qp-1][qp-1][0] + 0.01)== GRAPHNODE_SHEET
          )
            cost = abs(patternGraph.nodeWeights[p-1] - baseGraph.nodeWeights[qp-1]);

        return cost;
    }

    // returns the cost of matching edge j,p in the pattern graph to edge qj,qp in the base graph.
    // when j=p and qj=qp, returns the cost of matching node j to node qj.
    // three possible cases:
    //   j == p and qj == qp -- node match cost
    //   j != p and qj == qp -- edge match cost, special case where same sheet revisited by two consecutive nodes
    //   j != p and qj != qp -- edge match cost
    // note: only the first case is ever used, as all calls to this method have j=p and qj=qp.
    inline double WongMatch::getC(int j, int p, int qj, int qp) {

        double jpCost;
        double qjqpCost;
        double typeCost = 0;

        // if no edge exists between j and p
        if(patternGraph.adjacencyMatrix[j - 1][p - 1][1] == MAXINTT)
            jpCost = 1000;
        else {
            // if edge exists or if j == p
            jpCost = patternGraph.adjacencyMatrix[j - 1][p - 1][1];
        }

        // if no edge exists between qj and qp
        if(baseGraph.adjacencyMatrix[qj - 1][qp - 1][1] == MAXINTT)
            qjqpCost = 1000;
        else {
            // if edge exists or if qj == qp
            qjqpCost = baseGraph.adjacencyMatrix[qj - 1][qp - 1][1];
        }

        // if edge types or node types do not match. cost set here may be recomputed in next block.
        if(patternGraph.adjacencyMatrix[j-1][p-1][0] != baseGraph.adjacencyMatrix[qj-1][qp-1][0])
            typeCost = 1000;

        // case where a sheet is revisited by two consecutive strands
        if(    (int) (patternGraph.adjacencyMatrix[ j-1][ p-1][0] + 0.01) == GRAPHEDGE_LOOP
            && (int) (   baseGraph.adjacencyMatrix[qj-1][qp-1][0] + 0.01) == GRAPHNODE_SHEET
          )
            typeCost = 0;

        return fabs(jpCost - qjqpCost) + typeCost;
    }

    // returns the cost of matching a loop from the pattern graph (with missing helices) to a loop in the base graph.
    // d is the start match in the pattern graph
    // m is the number of missing helices or sheets in the pattern graph
    // qj is the start node in the base graph
    // qp is the end node in the base graph
    inline double WongMatch::getCost(int d, int m, int qj, int qp, bool debugMsg) {
        // TODO: Fix patthernLength and baseLength for sheet-to-sheet case.
        double patternLength = 0;
        double baseLength;

        // Count number of skipped helices and sheets
        int skippedHelices = 0;
        int skippedSheets = 0;

        // Adding the length of the skipped helixes
        for(int i = 1; i < m; i++) {
            patternLength += patternGraph.adjacencyMatrix[d+i-1][d+i-1][1];

            if(patternGraph.adjacencyMatrix[d+i-1][d+i-1][0] == GRAPHNODE_SHEET) {
                patternLength += patternGraph.nodeWeights[d + i - 1];
                skippedSheets++;
#ifdef VERBOSE
                if(debugMsg) {
                    cout << "  -- found strand " << d + i << ", adding "
                         << patternGraph.nodeWeights[d + i - 1]
                         << " to patternLength" << endl;
                }
#endif
            }
            else
                skippedHelices++;
        }

        // Adding the length of the edges
        // Check that first and last edges are both loops, a requirement for all edges in the pattern graph
        bool firstIsLoop = false;
        bool lastIsLoop = false;
        for(int i = 0; i < m; i++) {
            lastIsLoop =((int) (patternGraph.adjacencyMatrix[d+i-1][d+i][0] + 0.01) == GRAPHEDGE_LOOP);
            if(i == 0)
                firstIsLoop = lastIsLoop;

            patternLength += patternGraph.adjacencyMatrix[d+i-1][d+i][1];
        }
        // TODO: Fix, has bug. But getting closer.
        skippedHelices = skippedHelices / 2;

        bool euclideanEstimate = false;
        double weight = 1.0;

        // if edge begins with an unmatched node in the base graph
        if(qj == -1) { // special handling for missing helixes at the ends
            baseLength = 0;
            switch((int) (patternGraph.adjacencyMatrix[d-1][d][0] + 0.01)) {
                case (GRAPHEDGE_HELIX):
                    weight = HELIX_WEIGHT_COEFFICIENT;
                    break;
                case (GRAPHEDGE_LOOP):
                    weight = LOOP_WEIGHT_COEFFICIENT;
                    break;
                case (GRAPHEDGE_SHEET):
                    weight = SHEET_WEIGHT_COEFFICIENT;
                    break;
            }
        }
        else {
            assert(baseGraph.edgeExists(qj - 1, qp - 1));
            baseLength = baseGraph.adjacencyMatrix[qj - 1][qp - 1][1];
            euclideanEstimate = ((int) (baseGraph.adjacencyMatrix[qj-1][qp-1][0] + 0.01) == GRAPHEDGE_LOOP_EUCLIDEAN);
            switch((int) (baseGraph.adjacencyMatrix[qj-1][qp-1][0] + 0.01)) {
                case (GRAPHEDGE_HELIX):
                    weight = HELIX_WEIGHT_COEFFICIENT;
                    break;
                case (GRAPHEDGE_LOOP):
                    weight = LOOP_WEIGHT_COEFFICIENT;
                    break;
                case (GRAPHNODE_SHEET): // two strands in a row match to the same sheet
                    weight = LOOP_WEIGHT_COEFFICIENT;
                    break;
            }
            weight = euclideanEstimate ? weight * EUCLIDEAN_LOOP_PENALTY : weight;
        }

        if(m == 1) { // not a skip edge
            if( (qj != -1) && // first node in pattern graph is matched
                ! ( ((int) (patternGraph.adjacencyMatrix[d-1][d][0] + 0.01) == (int) (baseGraph.adjacencyMatrix[qj-1][qp-1][0] + 0.01)))
               && // types don't match exactly
                 ! (    ((int) (patternGraph.adjacencyMatrix[ d-1]   [d][0] + 0.01) == GRAPHEDGE_LOOP)
                     && ((int) (baseGraph.adjacencyMatrix   [qj-1][qp-1][0] + 0.01) == GRAPHEDGE_LOOP_EUCLIDEAN)
                   )
               && // not a loop-Euclidianloop match
               ! (    ((int) (patternGraph.adjacencyMatrix[ d-1]   [d][0] + 0.01) == GRAPHEDGE_LOOP)
                   && ((int) (baseGraph.adjacencyMatrix   [qj-1][qp-1][0] + 0.01) == GRAPHNODE_SHEET)
                 )
              ) { // not a loop-sheet match
                return -1;
            }
#ifdef VERBOSE
            if(debugMsg) {
                cout << "  -- euclidean dist = "
                     << baseGraph.euclideanMatrix[qj - 1][qp - 1]
                     << ", patternLength = "
                     << patternLength
                     << ", loop fudge factor = "
                     << EUCLIDEAN_VOXEL_TO_PDB_RATIO / LOOP_C_ALPHA_TO_ANGSTROMS
                     << ", helix fudge factor = "
                     << EUCLIDEAN_VOXEL_TO_PDB_RATIO / HELIX_C_ALPHA_TO_ANGSTROMS
                     << endl;
            }
            if(debugMsg) {
                cout << "  -- scalar ratio required = "
                     << baseGraph.euclideanMatrix[qj - 1][qp - 1] / patternLength
                     << ", additive headroom = "
                     << baseGraph.euclideanMatrix[qj - 1][qp - 1] - patternLength
                     << endl;
            }
#endif

            if(    (qj!=-1)
                && ((int) (patternGraph.adjacencyMatrix[d - 1][d][0] + 0.01) == GRAPHEDGE_HELIX)
                && (baseGraph.euclideanMatrix[qj-1][qp-1] > patternLength * EUCLIDEAN_VOXEL_TO_PDB_RATIO / HELIX_C_ALPHA_TO_ANGSTROMS)
               ) {
#ifdef VERBOSE
                if(debugMsg) {
                    cout << "  -- -- -- NOT ALLOWED (HELIX) -- -- -- " << endl;
                }
#endif
                return -1;
            }
            else if( (qj!= -1)
                     && ((int) (patternGraph.adjacencyMatrix[d-1][d][0] + 0.01) == GRAPHEDGE_LOOP)
                   ) {
                       if( (   (int) (patternGraph.adjacencyMatrix[d-1][d-1][0] + 0.01) == GRAPHNODE_SHEET
                            || (int) (patternGraph.adjacencyMatrix[d]    [d][0] + 0.01) == GRAPHNODE_SHEET
                           )
                          && (baseGraph.euclideanMatrix[qj-1][qp-1] > patternLength*1.0* EUCLIDEAN_VOXEL_TO_PDB_RATIO / LOOP_C_ALPHA_TO_ANGSTROMS)
                         ) {
#ifdef VERBOSE
                    if(debugMsg) {
                        cout << "  -- -- -- NOT ALLOWED (LOOP WITH STRAND) -- -- -- " << endl;
                    }
#endif
                            return -1;
                           }
                if( (    (int) (patternGraph.adjacencyMatrix[d-1][d-1][0] + 0.01) != GRAPHNODE_SHEET
                      && (int) (patternGraph.adjacencyMatrix[d]    [d][0] + 0.01) != GRAPHNODE_SHEET
                    )
                   &&
                    (baseGraph.euclideanMatrix[qj-1][qp-1] > patternLength * EUCLIDEAN_VOXEL_TO_PDB_RATIO / LOOP_C_ALPHA_TO_ANGSTROMS)
                  ) {
#ifdef VERBOSE
                    if(debugMsg) {
                        cout << "  -- -- -- NOT ALLOWED (LOOP) -- -- -- " << endl;
                    }
#endif
                        return -1;
                    }
                  }
        }
        else { // a skip edge
            // not sure if these checks really help or if they just waste time
                if(! (firstIsLoop && lastIsLoop)
                   || // pattern graph edge doesn't start and end with loops OR
                     ( ( ((int) (baseGraph.adjacencyMatrix[qj - 1][qp - 1][0] + 0.01) != GRAPHEDGE_LOOP))
                        &&// (base graph edge not a loop AND
                       ( ((int) (baseGraph.adjacencyMatrix[qj - 1][qp - 1][0] + 0.01) != GRAPHEDGE_LOOP_EUCLIDEAN))
                        &&	// base graph edge not a Euclidian loop AND
                       ( ((int) (baseGraph.adjacencyMatrix[qj - 1][qp - 1][0] + 0.01) != GRAPHNODE_SHEET))
                     )
                   ) {// base graph edge not a sheet)
                      return -1;
                      }
                // check here to sum up the parts of the skip edge and compare to the euclidian distance, if it's a euclidian edge in the base graph
                if( (qj != -1)
                   && (baseGraph.euclideanMatrix[qj - 1][qp - 1] > (patternLength * EUCLIDEAN_VOXEL_TO_PDB_RATIO))
                  ) {
                      return -1;
                    }
             }

        switch(COST_FUNCTION) {
            case (1):
                return weight * fabs(patternLength - baseLength);
                break;
            case (2):
                return weight * fabs(patternLength - baseLength) / (patternLength + baseLength);
                break;
            case (3):
                return weight * pow( (patternLength - baseLength), 2);
                break;
        }
        // this line should be unreachable
        return 0;
    }

    inline double WongMatch::getF() {
        return currentNode->costGStar;
    }

    inline void WongMatch::popBestNode() {
#ifdef VERBOSE
        clock_t start = clock();
#endif
        double cost;
        Elem res = q.top();
        currentNode = res.second;
        q.pop();
#ifdef VERBOSE
        timeInQueue += clock() - start;
#endif
    }

    // add in penalties for skipped helices and sheets
    // m is the number of nodes involved in the match. m=1 is no skipped helices or sheets.
    inline double WongMatch::getPenaltyCost(int d, int m, bool debugMsg) {
        double cost = 0.0;
        int lastPatternNode = patternGraph.getNodeCount() - 1;
        bool startAtBeginning = (d == 0);
        bool finishAtEnd = (d + m - 1 == lastPatternNode);
        bool pastFirst = true;
        bool firstHelixFound = false;
        for(int k = d; k < d + m - 1; k++) {
            // add penalties for all skipped helices
            if((int) (patternGraph.adjacencyMatrix[k][k+1][0] + 0.01) == GRAPHEDGE_HELIX) {
                cost += MISSING_HELIX_PENALTY;
                cost += patternGraph.adjacencyMatrix[k][k + 1][1] * MISSING_HELIX_PENALTY_SCALED;
#ifdef VERBOSE
                if(debugMsg) {
                    cout << "  -- adding missing helix penalties: fixed="
                         << MISSING_HELIX_PENALTY
                         << ", scaled="
                         << patternGraph.nodeWeights[k] * MISSING_HELIX_PENALTY_SCALED
                         << endl;
                }
#endif

                if(startAtBeginning && !firstHelixFound) {
                    cost += START_END_MISSING_HELIX_PENALTY;
#ifdef VERBOSE
                    if(debugMsg) {
                        cout << "  -- adding start_end_miss_helix_pen" << endl;
                    }
#endif
                }

                if(finishAtEnd && !firstHelixFound) {
                    cost += START_END_MISSING_HELIX_PENALTY;
#ifdef VERBOSE
                    if(debugMsg) {
                        cout << "  -- adding start_end_miss_helix_pen" << endl;
                    }
#endif
                }
                firstHelixFound = true;
            }
            // add penalties for skipped strands, unless the strand falls at the beginning of the sequence and is not the first node
            else if(  (startAtBeginning || pastFirst)
                     &&
                      ((int) (patternGraph.adjacencyMatrix[k][k][0] + 0.01) == GRAPHNODE_SHEET)
                    ) {
                        cost += MISSING_SHEET_PENALTY;
                        cost += patternGraph.nodeWeights[k] * MISSING_SHEET_PENALTY_SCALED;
#ifdef VERBOSE
                if(debugMsg) {
                    cout << "  -- adding missing sheet penalties: fixed="
                         << MISSING_SHEET_PENALTY
                         << ", scaled="
                         << patternGraph.nodeWeights[k] * MISSING_SHEET_PENALTY_SCALED
                         << endl;
                }
#endif
                        }
            pastFirst = true;
        }

        if(   finishAtEnd
           && patternGraph.adjacencyMatrix[lastPatternNode-1][lastPatternNode-1][0] + 0.01 == GRAPHNODE_SHEET
          ) {
                cost += MISSING_SHEET_PENALTY;
                cost += patternGraph.nodeWeights[lastPatternNode-1]*MISSING_SHEET_PENALTY_SCALED;
            }
        return cost;
    }

    // expand a node.
    // checks the base graph for edges between this node and every other.
    // if an edge is found, match the pattern graph to that edge and add the match to the queue.
    // also match edges that include skip edges in the pattern graph
    // costs of matches are determined by the GetC method
    inline bool WongMatch::expandNode(NodeStub * currentStub) {
        bool expanded = false;
        nExpand++;

        Node * temp;
        double edgeCost;
#ifdef VERBOSE
        if(longestMatch < currentNode->depth) {
            longestMatch = currentNode->depth;
            printf(" %d elements matched!\n", longestMatch);
        }
#endif

        int currentM1Top = patternGraph.nodeCount - currentNode->depth; // remaining unmatched nodes in sequence
        bool notConstrained;

        // Expanding nodes with a real terminal node
        // for every node i in baseGraph
        for(int i = 1; i <= baseGraph.nodeCount; i++) {
            // if:
            //   currentNode is at level 0 of tree
            //   or
            //   i is in the currentNode bitmap, and there is an edge in baseGraph between currentNode and node i
            if(    (currentNode->depth == 0)
                || (Node::IsNodeInBitmap(currentNode->m2Bitmap, i)
                     && baseGraph.edgeExists(currentNode->n2Node - 1, i - 1)
                   )
                ) {
                    int skippedHelixNodes = 0;
                    int skippedSheetNodes = 0;
                for(int j = 0;
                         j <= currentM1Top
                      && skippedHelixNodes + currentNode->missingHelixNodesUsed <= nMissHelix * 2
                      && skippedSheetNodes + currentNode->missingSheetNodesUsed <= nMissSheet;
                    ) {
                        // i is the node from baseGraph being matched to currentNode
                        // j is the number of missing helix or sheet nodes from patternGraph to be skipped for this match
                    notConstrained = true;

                    for(int k  = currentNode->n1Node + 1;
                            k <= currentNode->n1Node + j;
                            k++)
                      {
                        notConstrained = notConstrained && IsNodeAssignmentAllowed(k, -1);
                      }
                    notConstrained = notConstrained && IsNodeAssignmentAllowed(currentNode->n1Node + j+1, i);

                    // used later for case where first helix is missing
                    bool firstMissing = false;

                    if(notConstrained) {
                        // store the current node as temp
                        temp = currentNode;
                        // create new current node. i is the index of the new node(?), j is the number of skipped nodes.

                        // check whether i is a revisitable node (a sheet)
                        bool revisitable = ((int) (baseGraph.adjacencyMatrix[i-1][i-1][0] + 0.01) == GRAPHNODE_SHEET);

                        if(   ( (temp->depth == 0)
                                && (j > 0)
                              )
                           || ( (patternGraph.nodeCount - currentNode->depth == 0)
                                && (currentNode->n2Node == -1)
                              )
                          ) {
                                if(    skippedHelixNodes == 0
                                    && patternGraph.adjacencyMatrix[0][0][0] == GRAPHNODE_HELIX
                                  ) {
                                        skippedHelixNodes = 1;
#ifdef VERBOSE
                                cout << "node skipped. adding one to skippedHelixNodes. result is "
                                     << skippedHelixNodes << endl;
#endif
                                        }
                            }

                        // generate a current node, marking it as revisitable or not depending on result from test
                        // the constructor marches forward along the sequence, skipping j nodes
                        currentNode = new Node(currentNode, currentStub,
                                                i, skippedHelixNodes,
                                                skippedSheetNodes, revisitable);

                        currentNode->costGStar = 0;

                        // if previous node was at top of tree and it was skipped
                        if(   (    (temp->depth == 0)
                                && (j > 0)
                              )
                           || (    (patternGraph.nodeCount - currentNode->depth == 0)
                                && (currentNode->n2Node == -1)
                              )
                          ) {
                                if(skippedHelixNodes > 0)
                                        firstMissing = true;
                        }

                        // if previous node was at top of tree
                        if(temp->depth == 0)
                            edgeCost = 0;
                        else
                            edgeCost = getCost(temp->n1Node, j+1, temp->n2Node, currentNode->n2Node, false);

                        // if this is an allowed match:
                        if(edgeCost >= 0) {
                            //worked! currentNode->costGStar += temp->costGStar + edgeCost + GetC(currentNode->n1Node, currentNode->n2Node, currentNode);
                            currentNode->costGStar +=  temp->costGStar
                                                     + edgeCost
                                                     + getC(currentNode->n1Node, currentNode->n2Node);

                            // add costs for skipped helices and sheets
                            currentNode->costGStar += getPenaltyCost(temp->n1Node, j+1, false);

                            currentNode->cost = getF();
                            q.push(Elem(currentNode->cost, currentNode));
                            expanded = true;
                        }
                        else { // not an allowed match
                            delete currentNode;
                        }
                        // set currentNode pointer back to the current node
                        currentNode = temp;
                    }
                    // if this node is a helix, increment j by one more to prepare for the next iteration
                    switch((int) (patternGraph.adjacencyMatrix[currentNode->n1Node+j][currentNode->n1Node + j][0] + 0.01)) {
                        case GRAPHNODE_HELIX:
                            skippedHelixNodes += 2;
                            j += 2;
                            break;
                        default:
                            skippedSheetNodes += 1;
                            j++;
                            break;
                    }
                }
            }
        }

        // Expanding nodes with a dummy terminal node:
        // Count the number of sheets and helices remaining in the helix, and then try to make
        // a long skip edge to pass over all of them.

        // count the number of helix and sheet nodes that are not yet matched
        int remainingSheetNodes = 0;
        int remainingHelixNodes = 0;
        for(int l  = currentNode->n1Node + 1;
                l <= patternGraph.nodeCount;
                l++)
        {
            if(patternGraph.adjacencyMatrix[l-1][l-1][0] == GRAPHNODE_HELIX)
                remainingHelixNodes++;
            else if(patternGraph.adjacencyMatrix[l-1][l-1][0] == GRAPHNODE_SHEET)
                remainingSheetNodes++;
        }

        // if possible, create an edge to jump to the end of the sequence
        if(    2 * nMissHelix - currentNode->missingHelixNodesUsed >= remainingHelixNodes
            && 2 * nMissSheet - currentNode->missingSheetNodesUsed >= remainingSheetNodes
           )
        {
            notConstrained = true;
            for(int k = currentNode->n1Node + 1;
                    k <= patternGraph.nodeCount;
                    k++)
            {
                notConstrained = notConstrained && IsNodeAssignmentAllowed(k,-1);
            }

            if(notConstrained) {
                temp = currentNode;
                currentNode = new Node(*temp);
                currentNode->depth = (char)patternGraph.nodeCount;
                currentNode->costGStar  = temp->costGStar;
                currentNode->costGStar += getPenaltyCost(temp->n1Node, remainingHelixNodes + remainingSheetNodes, false);
                currentNode->cost = currentNode->costGStar;
                q.push(Elem(currentNode->cost, currentNode));
                currentNode = temp;
            }
        }
        return expanded;
    }

    // Compute the cost of the ground truth solution which is submitted by the user.
    inline void WongMatch::ComputeSolutionCost(int solution[], bool extraMessages) {
        if(extraMessages) {
            cout << "starting ComputeSolutionCost" << endl;
        }
        int n1 = 0, n2 = 1;
        double edgeCost = 0.0;

        double helixCost = 0.0;
        double helixPenaltyCost = 0.0;
        double loopCost = 0.0;
        double loopPenaltyCost = 0.0;
        double sheetCost = 0.0;
        double sheetPenaltyCost = 0.0;
        double skipPenaltyCost = 0.0;

        double edgePenaltyCost = 0.0;
        double nodeCost = 0.0;
        int skippedHelixNodes = 0;
        int skippedSheetNodes = 0;

        int numNodes = patternGraph.getNodeCount();

        // iterate over all correspondences, adding each to the previous solution
        while(n2 < numNodes) {
            // check if first node is skipped helix or sheet
            if(solution[n1] == -1) {
                if(extraMessages) {
                    cout << "skipped node found at " << n1 + 1
                         << " with adj matrix value "
                         << patternGraph.adjacencyMatrix[n1][n1][0] << endl;
                }
                if(patternGraph.adjacencyMatrix[n1][n1][0] == GRAPHNODE_HELIX)
                    skippedHelixNodes++;
                if(patternGraph.adjacencyMatrix[n1][n1][0] == GRAPHNODE_SHEET)
                    skippedSheetNodes++;
            }

            // find the end of the current correspondence
            while(solution[n2] == -1 && n2 < numNodes - 1) {
                if(patternGraph.adjacencyMatrix[n2][n2][0] == GRAPHNODE_HELIX)
                    skippedHelixNodes++;
                if(patternGraph.adjacencyMatrix[n2][n2][0] == GRAPHNODE_SHEET)
                    skippedSheetNodes++;
                n2++;
            }

            // add edge cost
            if(extraMessages) {
                cout << "adding (" << n1 + 1 << "," << n2 + 1 << ","
                     << solution[n1] << "," << solution[n2] << ")" << endl;
            }
            double singleEdgeCost = 1000;
            double singleEdgePenaltyCost = 0;

            // if edge exists in base graph, find the cost of this correspondence.
            if(solution[n1] == -1) {
                singleEdgeCost = 0;
                singleEdgePenaltyCost = getPenaltyCost(n1, n2 - n1 + 1, extraMessages);

                if(extraMessages) {
                    cout << "  GetPenaltyCost(" << n1 << "," << n2 - n1 + 1
                         << ")=" << singleEdgePenaltyCost << endl;
                }

                skipPenaltyCost += singleEdgePenaltyCost;
            }
            else if(baseGraph.edgeExists(solution[n1]-1, solution[n2]-1)) {
                if(solution[n2] == -1 && n2 == numNodes - 1) {
                    // last edge is skip edge
                    singleEdgeCost = 0;
                }
                else
                    singleEdgeCost = getCost(n1+1, n2-n1, solution[n1], solution[n2], extraMessages);

                singleEdgePenaltyCost = getPenaltyCost(n1+1, n2-n1, extraMessages);

                if(baseGraph.adjacencyMatrix[solution[n1]-1][solution[n2]-1][0] == GRAPHEDGE_HELIX) {
                    helixCost        += singleEdgeCost;
                    helixPenaltyCost += singleEdgePenaltyCost;
                    skipPenaltyCost  += singleEdgePenaltyCost;
                }
                else {
                    loopCost        += singleEdgeCost;
                    skipPenaltyCost += singleEdgePenaltyCost;
                }
                if(extraMessages) {
                    cout << "  GetCost(" << n1 + 1 << "," << n2 - n1 << ","
                         << solution[n1] << "," << solution[n2] << ")="
                         << singleEdgeCost << endl;
                    cout << "  GetPenaltyCost(" << n1 + 1 << "," << n2 - n1
                         << ")=" << singleEdgePenaltyCost << endl;
                }
                if(singleEdgeCost == -1) {
                    cout << "  MATCH FROM NODE " << solution[n1] << " TO NODE "
                         << solution[n2]
                         << " IS NOT ALLOWED (CUTOFF OR TYPE MISMATCH?)"
                         << endl;
                }
            }
            else {
                cout << "  BASE GRAPH DOES NOT HAVE AN EDGE FROM NODE "
                     << solution[n1] << " TO NODE " << solution[n2]
                     << ". THIS SOLUTION NOT POSSIBLE!" << endl;
            }

            // check if first or last helix is unmatched
            if(   (n1 == 0 && singleEdgeCost == -1)
               || (n2 == numNodes && singleEdgeCost == -1)
              )
            {
                if(extraMessages)
                    cout << "  first helix or sheet is unmatched. adding penalty of " << singleEdgeCost << endl;
            }
            else {
                if(extraMessages)
                    cout << "  cost of this addition is " << singleEdgeCost << endl;
            }

            // add node cost
            double singleNodeCost = getC(n2+1, solution[n2]);
            // if at beginning of sequence, check first node
            if(patternGraph.adjacencyMatrix[n2][n2][0] == GRAPHNODE_SHEET)
                sheetCost += singleNodeCost;
            if(n1 == 0 && patternGraph.adjacencyMatrix[n1][n1][0] == GRAPHNODE_SHEET) {
                double firstNodeCost = getC(n1 + 1, solution[n1]);
                sheetCost      += firstNodeCost;
                singleNodeCost += firstNodeCost;
            }
            if(extraMessages) {
                cout << "  node cost for nodes " << n2 + 1 << " and "
                     << solution[n2] << " is " << singleNodeCost << endl;
            }

            // add the costs from this iteration to the running totals
            edgeCost        += singleEdgeCost;
            edgePenaltyCost += singleEdgePenaltyCost;
            nodeCost        += singleNodeCost;

            // prepare for next iteration
            n1 = n2;
            n2++;
        }

        // Check if all helices or all sheets were correctly matched
        bool sheetsCorrect = true;
        bool helicesCorrect = true;
        bool helicesCorrectFlipped = true;
        double ratioCorrectHelices = 0;
        double ratioCorrectHelicesFlipped = 0;
        double ratioCorrectSheets = 0;
        int ns = 0, nh = 0;
        for(int i = 0; i < numNodes; i++) {
            if(patternGraph.adjacencyMatrix[i][i][0] == GRAPHNODE_HELIX) {
                nh++;
                if(solution[i] == SOLUTION[i])
                    ratioCorrectHelices++;
                else
                    helicesCorrect = false;

                if(   solution[i] == SOLUTION[i]
                   || solution[i] == SOLUTION[max(0, i - 1)]
                   || solution[i] == SOLUTION[min(numNodes - 1, i + 1)]
                  )
                {
                    ratioCorrectHelicesFlipped++;
                }
                else
                    helicesCorrectFlipped = false;
            }

            if(patternGraph.adjacencyMatrix[i][i][0] == GRAPHNODE_SHEET) {
                ns++;
                if(solution[i] == SOLUTION[i])
                    ratioCorrectSheets++;
                else
                    sheetsCorrect = false;
            }
        }
        int incorrectHelixCount = (nh - (int) (ratioCorrectHelices + 0.1)) / 2;
        int incorrectStrandCount = ns - (int) (ratioCorrectSheets + 0.1);
        if(ns > 0)
            ratioCorrectSheets /= ns;

        if(nh > 0)
            ratioCorrectHelices /= nh;

        if(nh > 0)
            ratioCorrectHelicesFlipped /= nh;

        char sheetChar;
        if(sheetsCorrect) sheetChar = 'S';
        else              sheetChar = '-';

        char helixChar;
        if(helicesCorrect)             helixChar = 'H';
        else if(helicesCorrectFlipped) helixChar = 'h';
        else                           helixChar = '-';

        if(extraMessages) {
            cout << "total edge cost is " << edgeCost << endl;
            cout << "total edge penalty cost is " << edgePenaltyCost << endl;
            cout << "total node cost is " << nodeCost << endl;
        }

        double helixPenalty = skippedHelixNodes / 2 * MISSING_HELIX_PENALTY;
        double sheetPenalty = skippedSheetNodes * MISSING_SHEET_PENALTY;

        if(extraMessages) {
            cout << "missing helices: " << skippedHelixNodes / 2
                 << " contribute cost of " << helixPenalty << endl;
            cout << "missing sheets:  " << skippedSheetNodes
                 << " contribute cost of " << sheetPenalty << endl;
            cout << "together, missing helices and sheets contribute cost of "
                 << sheetPenalty + helixPenalty << endl;
            cout << "algorithm thinks missing helices and sheets should contribute cost of "
                 << edgePenaltyCost << endl;
        }

        double cost = edgeCost + nodeCost + helixPenalty + sheetPenalty;

        if(extraMessages) {
            cout << "total cost is " << cost << endl;
        }
        cout << "C=" << loopCost + helixCost + sheetCost + skipPenaltyCost
             << "(" << helixChar << sheetChar << ")(h" << incorrectHelixCount
             << ",s" << incorrectStrandCount << ")(" << ratioCorrectHelices
             << "," << ratioCorrectHelicesFlipped << "," << ratioCorrectSheets
             << ")(L=" << loopCost << ",H=" << helixCost << ",S=" << sheetCost
             << ",P=" << skipPenaltyCost << ")";
        if(extraMessages) {
            cout << endl;
        }
    }

    inline void WongMatch::normalizeGraphs() {
#ifdef VERBOSE
        printf("Normalizing Graphs\n");
        printf("\tNormalizing the base graph from Angstroms to amino acids\nNormalized Graph:\n");
#endif
        for(int i = 0; i < baseGraph.nodeCount; i++) {
            for(int j = 0; j < baseGraph.nodeCount; j++) {
                // base graph
                if(   baseGraph.adjacencyMatrix[i][j][1] != MAXINTT
                   && baseGraph.adjacencyMatrix[i][j][0] == GRAPHEDGE_HELIX
                  )
                {
                    baseGraph.setCost(i+1, j+1, baseGraph.adjacencyMatrix[i][j][1] / HELIX_C_ALPHA_TO_ANGSTROMS);
                }
                else if(baseGraph.adjacencyMatrix[i][j][1] != MAXINTT)
                    baseGraph.setCost(i + 1, j + 1, baseGraph.adjacencyMatrix[i][j][1] / LOOP_C_ALPHA_TO_ANGSTROMS);

                // euclidean distance matrix
                if(baseGraph.adjacencyMatrix[i][j][0] == GRAPHEDGE_HELIX)
                    baseGraph.euclideanMatrix[i][j] = baseGraph.euclideanMatrix[i][j] / HELIX_C_ALPHA_TO_ANGSTROMS;
                else
                    baseGraph.euclideanMatrix[i][j] = baseGraph.euclideanMatrix[i][j] / LOOP_C_ALPHA_TO_ANGSTROMS;
            }
        }

#ifdef VERBOSE
        baseGraph.print();
#endif
    }

    inline void WongMatch::normalizeSheets() {
#ifdef VERBOSE
        printf("\tNormalizing the sheet nodes in the base graph based on sheet ratio\nNormalized Graph:\n");
#endif
        // TODO: Also normalize the sheet capacity here?
        double totalSheetSize = 0;
        double totalStrandLength = 0;

        for(int i = 0; i < (int)baseGraph.skeletonHelixes.size(); i++) {
            if(baseGraph.skeletonHelixes[i]->type == GRAPHEDGE_SHEET) {
                totalSheetSize += (double)baseGraph.skeletonHelixes[i]->length;
#ifdef VERBOSE
                cout << "after sheet " << i << ", total sheet size is now " << totalSheetSize << endl;
#endif
            }
        }

        for(int i = 0; i < (int)patternGraph.pdbStructures.size(); i++) {
            if(patternGraph.pdbStructures[i]->sseType == GRAPHEDGE_SHEET) {
                totalStrandLength += patternGraph.pdbStructures[i]->GetLengthResidues();
#ifdef VERBOSE
                cout << "After adding strand " << i << " with length "
                     << patternGraph.pdbStructures[i]->GetLengthResidues()
                     << ", total strand length is now " << totalStrandLength
                     << endl;
#endif
            }
        }

        // scale the sheet sizes so that the units are in amino acids
        double ratio = totalStrandLength / totalSheetSize;
#ifdef VERBOSE
        cout << "sheet sizes must be scaled by a factor of " << ratio << endl;
        printf("\tNormalizing the base graph sheets from voxels to scaled voxels\nNormalized Graph:\n");
#endif
        for(int i = 0; i < baseGraph.nodeCount; i++) {
            if(baseGraph.adjacencyMatrix[i][i][1] != MAXINTT && baseGraph.adjacencyMatrix[i][i][0] == GRAPHNODE_SHEET) {
                // scale the sheet weight to the # of amino acids
                baseGraph.setCost(i + 1, baseGraph.nodeWeights[i] * ratio);
                // take sqrt for matching algorithm
                baseGraph.setCost(i + 1, sqrt(baseGraph.nodeWeights[i]));
            }
        }

#ifdef VERBOSE
        baseGraph.print();
#endif
    }

    // code copied from LinkedNode::PrintNodeConcise
    // Adding a breakdown of the cost into loops, nodes, and helices
    inline void WongMatch::printNodeConcise(Node * node, int rank, bool endOfLine, bool printCostBreakdown) {
        bool used[MAX_NODES];
        int n1[MAX_NODES];
        int n2[MAX_NODES];
        int top = 0;
        for(int i = 0; i < MAX_NODES; i++) {
            used[i] = false;
        }

        NodeStub * currentNode = node;
        bool continueLoop = true;
        while(continueLoop) {
            if(currentNode->parentNode == NULL) {
                break;
            }
            n1[top] = currentNode->n1Node;
            n2[top] = currentNode->n2Node;
            used[(int)currentNode->n1Node] = true;
            top++;
            currentNode = currentNode->parentNode;
        }

        for(int i = 1; i <= node->depth; i++) {
            if(!used[i]) {
                n1[top] = i;
                n2[top] = -1;
                top++;
            }
        }

        int minIndex;
        int temp;
        for(int i = 0; i < top - 1; i++) {
            minIndex = i;
            for(int j = i+1; j < top; j++) {
                if(n1[minIndex] > n1[j]) {
                    minIndex = j;
                }
            }
            temp = n1[minIndex];
            n1[minIndex] = n1[i];
            n1[i] = temp;

            temp = n2[minIndex];
            n2[minIndex] = n2[i];
            n2[i] = temp;
        }


        if(rank != -1) {
            printf("%d)", rank);
        }
        printf("\t");
        for(int i = 0; i < top; i++) {
            printf("%2d ", n2[i]);
        }

        // print the cost of the current solution
        if(INCLUDE_STRANDS) {
            ComputeSolutionCost(n2,false);
        }
        for (int i = 0; i < MAX_NODES; i++){
            bestMatches[rank-1][i]=n2[i];
        }

        if(printCostBreakdown) {
            printf(" - %f = %f + %f", node->cost, node->costGStar, node->cost - node->costGStar);
        } else {
            printf(" - %f", node->cost);
        }
        if(endOfLine) {
            printf("\n");
        }
    }
}
#endif
