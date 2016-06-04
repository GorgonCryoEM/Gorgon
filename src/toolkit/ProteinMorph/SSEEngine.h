/*
 * SSEEngine.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_PROTEINMORPH_SSEENGINE_H_
#define SRC_TOOLKIT_PROTEINMORPH_SSEENGINE_H_

//#include <GraphMatch/SSEResult.h>
#include "GraphMatch/WongMatch.h"
#include "GraphMatch/PDBReader.h"
#include "Readers/SEQReader.h"
#include "Readers/SkeletonReader.h"
//#include <GraphMatch/Shape.h>
#include <vector>
#include <map>
#include <set>
#include <iomanip>
#include "GorgonGL.h"
//#include <Foundation/StringUtils.h>
#include "Visualization/Shapes.h"
#include "Visualization/SkeletonMesh.h"

//using namespace GraphMatch;
//using namespace Foundation;
//using namespace std;

namespace Visualization {
    class SSEEngine : public WongMatch {
    public:
        SSEEngine();

        int load(string fileName);
        void save(string fileName);
        Shape * getSkeletonSSE(int sseId);
        SecStruct * getSequenceSSE(int sseId);
        int getSkeletonSSECount();
        int getSequenceSSECount();
        void setVisibleCorrespondence(int correspondenceIndex);
        void drawAllPaths(int sceneIndex, bool showPaths, bool showHelixCorners, bool showSheetCorners, bool showSheetColors);

        virtual void loadSequence();
        virtual void loadSkeleton();

        void setSSEColor(int index, float r, float g, float b, float a);

        int run();

    private:
        Graph skeleton;
        Graph sequence;

        int curInd;
    };

    inline SSEEngine::SSEEngine()
                : WongMatch(sequence, skeleton)
    {
        curInd = -1;
    }

    inline void SSEEngine::loadSequence() {
        string type = SEQUENCE_FILE_TYPE; //easier than doing comparison with a char array
        #ifdef VERBOSE
            printf("Pattern sequence \n");
        #endif
        clock_t start = clock();
        if (type == "PDB")
            sequence = *PDBReader::ReadFile(SEQUENCE_FILE_NAME.c_str());
        else if (type == "SEQ")
            sequence = *SEQReader::ReadFile(SEQUENCE_FILE_NAME.c_str());

        clock_t finish = clock();
        #ifdef VERBOSE
            printf("\tReading Pattern file Took %f seconds.\n", (double) (finish - start) / (double) CLOCKS_PER_SEC ) ;
//            sequence.print();
        #endif
    }

    inline void SSEEngine::loadSkeleton() {
        #ifdef VERBOSE
            printf("Base Graph \n");
        #endif
        clock_t start = clock();
        skeleton = *SkeletonReader::readFile(MRC_FILE_NAME, VRML_HELIX_FILE_NAME, SSE_FILE_NAME, VRML_SHEET_FILE_NAME);
        clock_t finish = clock();
        #ifdef VERBOSE
            printf("\033[32m\tReading Base file Took %f seconds.\n\033[0m", (double) (finish - start) / (double) CLOCKS_PER_SEC ) ;
//            skeleton.print();
        #endif
    }

    // set the color of an SSE.
    inline void SSEEngine::setSSEColor(int index, float r, float g, float b, float a) {
        skeleton.skeletonHelixes[index]->setColor(r, g, b, a);
    }

    inline int SSEEngine::run() {
        PERFORMANCE_COMPARISON_MODE = false;

        // Match Graphs
        // Constrained no future
        if(MISSING_HELIX_COUNT == -1) {
            set_MISSING_HELIX_COUNT(0);
            set_MISSING_SHEET_COUNT(0);
        } else {
            set_MISSING_HELIX_COUNT(MISSING_HELIX_COUNT);
            set_MISSING_SHEET_COUNT(MISSING_SHEET_COUNT);
        }
        init(sequence, skeleton);
        clock_t start = clock();
        int matchCount = WongMatch::run(start);
        saveResults();

        return matchCount;
    }

    inline int SSEEngine::load(string fileName) {
        ifstream fin(fileName.c_str());
        if (!fin) {
            cout<<"Error opening input file "<<fileName<<".\n";
            exit(0) ;
        }

        solutions.clear();

        int correspondenceCount;
        fin>>correspondenceCount;

        for(int i = 0; i < correspondenceCount; i++) {
            vector<int> nodes;

            int nodeCount;
            fin>>nodeCount;
            for(int j = 0; j < nodeCount; j++) {
                int skeletonNode;
                fin>>skeletonNode;
                nodes.push_back(skeletonNode);
            }
            double cost;
            fin>>cost;
            // TODO: Fix! 0 not acceptable!
            solutions.push_back(SSEResult(nodes, cost, 0));
        }

        fin.close();

        return correspondenceCount;
    }

    inline void SSEEngine::save(string fileName) {
        ofstream fout(fileName.c_str());
        if (!fout) {
            cout<<"Error opening output file "<<fileName<<".\n";
            exit(0) ;
        }

        fout<<solutions.size()<<endl;
        for(unsigned int i = 0; i < solutions.size(); i++) {
            fout<<solutions[i].getNodeCount()<<" ";
            for(int j = 0; j < solutions[i].getNodeCount(); j++) {
                fout<<solutions[i].getSkeletonNode(j)<<" ";
            }
            fout<<fixed<<setprecision(6)<<solutions[i].getCost()<<endl;
        }

        fout.close();
    }

    inline Shape * SSEEngine::getSkeletonSSE(int sseId) {
        if(sseId < (int)skeleton.skeletonHelixes.size()) {
            return skeleton.skeletonHelixes[sseId];
        } else {
            return NULL;
        }
    }

    inline SecStruct * SSEEngine::getSequenceSSE(int sseId) {
        if(sseId < (int)sequence.pdbStructures.size()) {
            return sequence.pdbStructures[sseId];
        } else {
            return NULL;
        }
    }

    inline int SSEEngine::getSkeletonSSECount() {
        return skeleton.skeletonHelixes.size();
    }

    inline int SSEEngine::getSequenceSSECount() {
        return sequence.pdbStructures.size();
    }

    inline void SSEEngine::setVisibleCorrespondence(int correspondenceIndex) {
        this->curInd = correspondenceIndex;
    }

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_PROTEINMORPH_SSEENGINE_H_ */
