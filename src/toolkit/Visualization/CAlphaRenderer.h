/*
 * CAlphaRenderer.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_CALPHARENDERER_H_
#define SRC_TOOLKIT_VISUALIZATION_CALPHARENDERER_H_

#include "Display.h"
//#include <GorgonGL.h>
//#include <cstdlib>
//#include <cstdio>
//#include <ProteinMorph/NonManifoldMesh.h>
//#include <ProteinMorph/SSEHunter.h>
//#include <SkeletonMaker/volume.h>
#include <GraphMatch/PDBReader.h>
#include <GraphMatch/PDBAtom.h>
#include <GraphMatch/PDBBond.h>
//#include "Renderer.h"
//#include <map>
//#include <list>
#include <MathTools/VectorMath.h>
#include <boost/tuple/tuple.hpp>

//using namespace std;
//using namespace Protein_Morph;
//using namespace GraphMatch;
//using namespace SkeletonMaker;
using namespace boost::tuples;

namespace Visualization {

    typedef map<unsigned long long, PDBAtom> AtomMapType;
    struct SerialAndHashType {
        unsigned int serial;
        unsigned long long hashKey;
    };

    class SerialAndHashTypePredicate {
    public:
        bool operator() (const SerialAndHashType& lhs, const SerialAndHashType& rhs) {
            return lhs.serial < rhs.serial;
        }
    };

    const int CALPHA_DISPLAY_STYLE_BACKBONE = 3;
    const int CALPHA_DISPLAY_STYLE_RIBBON = 4;
    const int CALPHA_DISPLAY_STYLE_SIDE_CHAIN = 5;

    class CAlphaRenderer : public Display {
        public:
            struct Secel{
                vector<unsigned long long> atomHashes;
                bool selected;
            };

            CAlphaRenderer();
            virtual ~CAlphaRenderer();

            void draw(int subSceneIndex, bool selectEnabled);
            void loadFile(string fileName);
            void loadSSEHunterFile(string fileName);
            bool saveSSEHunterFile(string fileName);
//          void GetSSEHunterAtoms(Volume * vol, NonManifoldMesh_Annotated * skeleton, float resolution, float threshold, float correlationCoeff, float skeletonCoeff, float geometryCoeff);
            void updateTotalScoreSSEHunterAtoms(float correlationCoeff, float skeletonCoeff, float geometryCoeff);
            void colorSSEHunterAtoms();
            int selectionObjectCount();
            int selectionAtomCount();
            Vec3F selectionCenterOfMass();
            bool selectionRotate(Vec3F centerOfMass, Vec3F rotationAxis, float angle);
            bool selectionMove(Vec3F moveDirection);
            void selectionToggle(int subsceneIndex, bool forceTrue, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
            void unload();
            string getSupportedLoadFileFormats();
            string getSupportedSaveFileFormats();
            Vec3F get3DCoordinates(int subsceneIndex, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
            void transformAllAtomLocations(MatrixFloat transform);

            // Controlling the atom vector
            PDBAtom * addAtom(PDBAtom atom);
            PDBAtom * getAtom(unsigned long long index);
            PDBAtom * getAtomFromHitStack(int subsceneIndex, bool forceTrue, int ix0, int ix1, int ix2, int ix3, int ix4);
            PDBAtom * getSelectedAtom(unsigned int selectionId);
            void deleteAtom(unsigned long long index);
            int getAtomCount();
            vector<unsigned long long> getAtomHashes();

            //Controlling the bond vector
            void addBond(PDBBond bond);
            PDBBond * getBond(int index);
            int getBondIndex(unsigned long long atom0, unsigned long long atom1);
            void deleteBond(int index);
            int getBondCount();

            //Controlling the bond vector
            void addSideChainBond(PDBBond bond);
            PDBBond * getSideChainBond(int index);
            int getSideChainBondIndex(unsigned long long atom0, unsigned long long atom1);
            void deleteSideChainBond(int index);
            int getSideChainBondCount();

            void setNumSegments(int segments);
            void setNumSlices(int slices);

            int startHelix(); //StartHelix creates a new helix element in aHelices and returns its index
            void addHelixElement(int, unsigned long long); //adds a helix element to the helix indexed at param 1

            int startStrand(); //StartStrand creates a new strand element in bStrands and returns its index
            void addStrandElement(int, unsigned long long); //adds a strand element to the strand indexed at param 1

            int startLoop(); //StartLoop creates a new strand element in loops and returns its index
            void addLoopElement(int, unsigned long long); //adds a loop element to the loop indexed at param 1

            //void DrawHelices();
            //void DrawStrands();
            //void DrawLoops();

            bool cleanSecondaryStructures(); //empties the aHelices, bStrands and loops variables
            //what should really happen is that the code should check if it is
            //trying to reload the same one, and then if it did return false
            vector<Vec3F> createPointVector(PDBAtom first, PDBAtom last); // functionality mirrored in previously implemented method,
            // will try to refactor
            vector<Vec3F> laplacianSmoothing(vector<Vec3F> points, int steps); // applies Laplacian smoothing to a vector of
            // Vector3DFloats
            vector<Vec3F> createStrandNormals(vector<Vec3F> points, Vec3F previous, Vec3F next); // create line segment normals to be used in drawing Beta
            // strands
            void createHelixAxesTangentsAndPoints(vector<Vec3F>& axes, vector<Vec3F>& tangents, vector<Vec3F>& interpPoints, vector<Vec3F> points,
                                                  Vec3F previous, Vec3F next, double HELIX_ALPHA, double HELIX_BETA, double HELIX_HERMITE_FACTOR);
            void drawOpenBox(vector<Vec3F> points, vector<Vec3F> normals); // takes a vector of 8 points and draws a rectangular prism with two of its six sides not
            // filled in; the first 4 points are from the beggining edge of the box, with the second four
            // forming the end
            void drawTube(vector<Vec3F> points, vector<Vec3F> normals, int stacks, int slices);
            vector<Vec3F> interpolateLoopPoints(vector<Vec3F> points, Vec3F previous, Vec3F next, int NUM_SECTIONS); // creates interpolated points for loops
            //vector<Vector3DFloat> InterpolateStrandPoints(vector<Vector3DFloat> points, Vector3DFloat previous, Vector3DFloat next, int NUM_SECTIONS);
            //vector<Vector3DFloat> InterpolateHelixPoints(vector<Vector3DFloat> points, Vector3DFloat previous, Vector3DFloat next, int NUM_SECTIONS);

            vector<int> getSelectedHelixIndices();
            void setHelixCorrs( vector < int > flatCorrespondences);
            void setSelectedSSEHelices(vector<int>);
            void clearOtherHighlights();
            void setFeatureVecs(vector<Vec3F> flatFeatureVecs);
            void setHelixColor(int helixNum, float r, float g, float b);
        private:
            void drawBackboneModel(int subSceneIndex, bool selectEnabled);
            void drawRibbonModel(int subSceneIndex, bool selectEnabled);
            void drawSideChainModel(int subSceneIndex, bool selectEnabled);
        private:
            AtomMapType atoms;

            //TODO: possibly implement mouse picking using ray intersection
            vector<unsigned long long> atomHashKeys; //glLoadName(index of this vector)... used for selection

            vector<PDBBond> bonds;
            vector<PDBBond> sidechainBonds;

            vector<Secel> aHelices;
            vector<Secel> bStrands;
            vector<Secel> loops;

            vector<int> selectedHelixIndices;
            //vector<int> selectedSecelIndices; //unsure if I can just keep track of secels as one structure or not
            vector<int> selectedStrandIndices;
            vector<int> selectedLoopIndices;
            vector < pair<int, int> > corrs;
            vector<int> selectedSSEHelices;
            vector< boost::tuple<Vec3F, Vec3F> > featureVecs;

            map<int,boost::tuple<float, float, float> > helixColors;

            int renderingType;
            float thinRibbThickness;

            int NUM_SEGMENTS;
            int NUM_SLICES;

            /* These three constants used in rendering alpha helices */
            float HELIX_ALPHA;
            float HELIX_BETA;
            float HELIX_HERMITE_FACTOR;
            float HELIX_WIDTH;

            float STRAND_HERMITE_FACTOR;

            float LOOP_RADIUS;

            float hlt_r, hlt_g, hlt_b, hlt_a;

            int displayStyle;
    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_VISUALIZATION_CALPHARENDERER_H_ */
