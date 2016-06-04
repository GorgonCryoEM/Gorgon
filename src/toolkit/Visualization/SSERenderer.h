/*
 * SSERenderer.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_SSERENDERER_H_
#define SRC_TOOLKIT_VISUALIZATION_SSERENDERER_H_

#include "RendererBase.h"
#include "MathTools/Vector3.h"
#include "SkeletonMesh.h"
#include "Core/Point.h"

//#include "Renderer.h"
//#include "SkeletonRenderer.h"
//#include <ProteinMorph/SheetGenerator.h>
//#include <GraphMatch/StandardGraph.h>
#include "Readers/SkeletonReader.h"
#include "GraphMatch/Shape.h"
//#include <GraphMatch/VectorMath.h>
//#include <MathTools/LinearSolver.h>
//#include <ProteinMorph/SSEFlexibleFitter.h>
//#include <ProteinMorph/SSECorrespondenceNode.h>
//#include <vector>
//#include <map>

//using namespace Protein_Morph;
//using namespace GraySkeletonCPP;
using namespace GraphMatch;
//using namespace MathTools;
//using namespace SkeletonMaker;
//using namespace std;
using namespace Core;

namespace Visualization {

    const float HELIX_LENGTH_TO_RESIDUE_RATIO = 1.54;

    class SSERenderer : public RendererBase {
        public:
            SSERenderer();
            virtual ~SSERenderer();

            vector<Shape*> * getHelices();

            void addHelix(Vec3F p1, Vec3F p2);
            void startNewSSE();
            void addSSEPoint(Vec3F p);
            void finalizeHelix();
            void draw(int subSceneIndex, bool selectEnabled);
            void loadHelixFile(string fileName);
            void loadSheetFile(string fileName);
            void unload();
            void unloadGraphSSEs();
            void setHelixColor(int index, float r, float g, float b, float a);
            void setSheetColor(int index, float r, float g, float b, float a);
            void setSSEColor  (int index, float r, float g, float b, float a);
            void setSSEOrientationFlips(vector<bool>);
            bool selectionRotate(Vec3F centerOfMass, Vec3F rotationAxis, float angle);
            void saveHelixFile(string fileName);
            string getSupportedHelixLoadFileFormats();
            string getSupportedHelixSaveFileFormats();
            string getSupportedSheetLoadFileFormats();
            string getSupportedSheetSaveFileFormats();
            void removeHelices();
            void removeSheets();
            int getHelixCount();
            Vec3F getHelixCorner(int helixIx, int cornerIx);

            vector<int> getSelectedHelixIndices();
            void setHelixCorrs(vector<int> flatCorrespondences);
            void setSelectedPDBHelices(vector<int> indices);
            void clearOtherHighlights();

        private:
            void sheetListToMesh(vector<Shape*> & sheets);
            void loadHelixFileSSE(string fileName);
            void loadHelixFileVRML(string fileName);
            void saveHelixFileSSE(FILE* fout);
            void saveHelixFileVRML(FILE* fout);

        private:
            vector<Shape*> helices;
            vector<Shape*> sheets;
            SkeletonMesh sheetMesh;
            SkeletonMesh graphSheetMesh;
            int sheetCount;
            int graphSheetCount;
            bool selectedSheets[256];
            bool selectedGraphSheets[256];
            vector<Vec3F> tempSSEPoints;

            vector<bool> helixFlips;
            vector<int> selectedHelices;
            vector < pair<int,int> > corrs;
            vector<int> selectedPDBHelices;
    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_VISUALIZATION_SSERENDERER_H_ */
