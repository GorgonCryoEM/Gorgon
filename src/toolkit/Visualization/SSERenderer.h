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
#include "NonManifoldMesh.h"
#include "Core/Point.h"

//#include "Renderer.h"
//#include "MeshRenderer.h"
//#include <ProteinMorph/SheetGenerator.h>
//#include <GraphMatch/StandardGraph.h>
#include "Readers/SkeletonReader.h"
#include <GraphMatch/Shape.h>
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

    class SSERenderer {
        public:
            SSERenderer();
            virtual ~SSERenderer();

            vector<Shape*> * GetHelices();

            void AddHelix(Vec3F p1, Vec3F p2);
            void StartNewSSE();
            void AddSSEPoint(Vec3F p);
            void FinalizeHelix();
            void Draw(int subSceneIndex, bool selectEnabled);
            void LoadHelixFile(string fileName);
            void LoadSheetFile(string fileName);
            void Unload();
            void UnloadGraphSSEs();
            void SetHelixColor(int index, float r, float g, float b, float a);
            void SetSheetColor(int index, float r, float g, float b, float a);
            void SetSSEColor(int index, float r, float g, float b, float a);
            void SetSSEOrientationFlips(vector<bool>);
            bool SelectionRotate(Vec3F centerOfMass, Vec3F rotationAxis, float angle);
            void SaveHelixFile(string fileName);
            string GetSupportedHelixLoadFileFormats();
            string GetSupportedHelixSaveFileFormats();
            string GetSupportedSheetLoadFileFormats();
            string GetSupportedSheetSaveFileFormats();
            void RemoveHelices();
            void RemoveSheets();
            int GetHelixCount();
            Vec3F GetHelixCorner(int helixIx, int cornerIx);

            vector<int> GetSelectedHelixIndices();
            void SetHelixCorrs(  vector < int > flatCorrespondences);
            void SetSelectedPDBHelices(vector<int> indices);
            void ClearOtherHighlights();
        private:
            void SheetListToMesh(vector<Shape*> & sheets);
            void LoadHelixFileSSE(string fileName);
            void LoadHelixFileVRML(string fileName);
            void SaveHelixFileSSE(FILE* fout);
            void SaveHelixFileVRML(FILE* fout);
            vector<Shape*> helices;
            vector<Shape*> sheets;
            NonManifoldMesh sheetMesh;
            NonManifoldMesh graphSheetMesh;
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
