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
//#include <GraphMatch/PDBReader.h>
//#include <GraphMatch/PDBAtom.h>
//#include <GraphMatch/PDBBond.h>
//#include "Renderer.h"
//#include <map>
//#include <list>
//#include <GraphMatch/VectorMath.h>
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

    /**
    Begin Hermite Curve code, to be moved into another file after testing
    -this code based on molscript's hermite_curve.c file, and produced with the help
    of wikipedia's article on the cubic hermite spline
    */
    class HermiteCurve{
    public:
        Vec3F p0, p1, m0, m1;

        void setCurve(Vec3F pstart, Vec3F pend, Vec3F tstart, Vec3F tend);
        Vec3F getPos(double t);
        Vec3F getTangent(double t);
    };


    class CAlphaRenderer : public Display {
        public:
            struct Secel{
                vector<unsigned long long> atomHashes;
                bool selected;
            };

            CAlphaRenderer();
            virtual ~CAlphaRenderer();

            void Draw(int subSceneIndex, bool selectEnabled);
            void LoadFile(string fileName);
            void LoadSSEHunterFile(string fileName);
            bool SaveSSEHunterFile(string fileName);
//          void GetSSEHunterAtoms(Volume * vol, NonManifoldMesh_Annotated * skeleton, float resolution, float threshold, float correlationCoeff, float skeletonCoeff, float geometryCoeff);
            void UpdateTotalScoreSSEHunterAtoms(float correlationCoeff, float skeletonCoeff, float geometryCoeff);
            void ColorSSEHunterAtoms();
            int SelectionObjectCount();
            int SelectionAtomCount();
            Vec3F SelectionCenterOfMass();
            bool SelectionRotate(Vec3F centerOfMass, Vec3F rotationAxis, float angle);
            bool SelectionMove(Vec3F moveDirection);
            bool SelectionClear();
            void SelectionToggle(int subsceneIndex, bool forceTrue, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
            void Unload();
            string GetSupportedLoadFileFormats();
            string GetSupportedSaveFileFormats();
            Vec3F Get3DCoordinates(int subsceneIndex, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
            void TransformAllAtomLocations(MatrixFloat transform);
            void UpdateBoundingBox();

            // Controlling the atom vector
            PDBAtom * AddAtom(PDBAtom atom);
            PDBAtom * GetAtom(unsigned long long index);
            PDBAtom * GetAtomFromHitStack(int subsceneIndex, bool forceTrue, int ix0, int ix1, int ix2, int ix3, int ix4);
            PDBAtom * GetSelectedAtom(unsigned int selectionId);
            void DeleteAtom(unsigned long long index);
            int GetAtomCount();
            vector<unsigned long long> GetAtomHashes();

            //Controlling the bond vector
            void AddBond(PDBBond bond);
            PDBBond * GetBond(int index);
            int GetBondIndex(unsigned long long atom0, unsigned long long atom1);
            void DeleteBond(int index);
            int GetBondCount();

            //Controlling the bond vector
            void AddSideChainBond(PDBBond bond);
            PDBBond * GetSideChainBond(int index);
            int GetSideChainBondIndex(unsigned long long atom0, unsigned long long atom1);
            void DeleteSideChainBond(int index);
            int GetSideChainBondCount();

            void SetNumSegments(int segments);
            void SetNumSlices(int slices);

            int StartHelix(); //StartHelix creates a new helix element in aHelices and returns its index
            void AddHelixElement(int, unsigned long long); //adds a helix element to the helix indexed at param 1

            int StartStrand(); //StartStrand creates a new strand element in bStrands and returns its index
            void AddStrandElement(int, unsigned long long); //adds a strand element to the strand indexed at param 1

            int StartLoop(); //StartLoop creates a new strand element in loops and returns its index
            void AddLoopElement(int, unsigned long long); //adds a loop element to the loop indexed at param 1

            //void DrawHelices();
            //void DrawStrands();
            //void DrawLoops();

            bool CleanSecondaryStructures(); //empties the aHelices, bStrands and loops variables
            //what should really happen is that the code should check if it is
            //trying to reload the same one, and then if it did return false
            vector<Vec3F> CreatePointVector(PDBAtom first, PDBAtom last); // functionality mirrored in previously implemented method,
            // will try to refactor
            vector<Vec3F> LaplacianSmoothing(vector<Vec3F> points, int steps); // applies Laplacian smoothing to a vector of
            // Vector3DFloats
            vector<Vec3F> CreateStrandNormals(vector<Vec3F> points, Vec3F previous, Vec3F next); // create line segment normals to be used in drawing Beta
            // strands
            void CreateHelixAxesTangentsAndPoints(vector<Vec3F>& axes, vector<Vec3F>& tangents, vector<Vec3F>& interpPoints, vector<Vec3F> points,
                Vec3F previous, Vec3F next, double HELIX_ALPHA, double HELIX_BETA, double HELIX_HERMITE_FACTOR);
            void DrawOpenBox(vector<Vec3F> points, vector<Vec3F> normals); // takes a vector of 8 points and draws a rectangular prism with two of its six sides not
            // filled in; the first 4 points are from the beggining edge of the box, with the second four
            // forming the end
            void DrawTube(vector<Vec3F> points, vector<Vec3F> normals, int stacks, int slices);
            vector<Vec3F> InterpolateLoopPoints(vector<Vec3F> points, Vec3F previous, Vec3F next, int NUM_SECTIONS); // creates interpolated points for loops
            //vector<Vector3DFloat> InterpolateStrandPoints(vector<Vector3DFloat> points, Vector3DFloat previous, Vector3DFloat next, int NUM_SECTIONS);
            //vector<Vector3DFloat> InterpolateHelixPoints(vector<Vector3DFloat> points, Vector3DFloat previous, Vector3DFloat next, int NUM_SECTIONS);

            // for testing purposes only; allow changing of highlight color values
            void SetHltRValue(int col);
            void SetHltGValue(int col);
            void SetHltBValue(int col);
            void SetHltAValue(int col);


            vector<int> GetSelectedHelixIndices();
            void SetHelixCorrs( vector < int > flatCorrespondences);
            void SetSelectedSSEHelices(vector<int>);
            void ClearOtherHighlights();
            void SetFeatureVecs(vector<Vec3F> flatFeatureVecs);
            void SetHelixColor(int helixNum, float r, float g, float b);
        private:
            void DrawBackboneModel(int subSceneIndex, bool selectEnabled);
            void DrawRibbonModel(int subSceneIndex, bool selectEnabled);
            void DrawSideChainModel(int subSceneIndex, bool selectEnabled);
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
            vector < boost::tuple<int, int> > corrs;
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
    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_VISUALIZATION_CALPHARENDERER_H_ */
