/*
 * CAlphaRenderer.h
 *
 *      Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_CALPHARENDERER_H_
#define SRC_TOOLKIT_VISUALIZATION_CALPHARENDERER_H_

#include "GraphMatch/PDBBond.h"
#include "GraphMatch/PDBAtom.h"
#include "Core/volume.h"

#include <string>
#include <map>

#include <boost/tuple/tuple.hpp>

using namespace SkeletonMaker;


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

    class CAlphaRenderer : public Volume {
        public:
            struct Secel{
                vector<unsigned long long> atomHashes;
                bool selected;
            };

            CAlphaRenderer();

            void LoadSSEHunterFile(string fileName);
            bool SaveSSEHunterFile(string fileName);
    //          void GetSSEHunterAtoms(Volume * vol, NonManifoldMesh * skeleton, float resolution, float threshold, float correlationCoeff, float skeletonCoeff, float geometryCoeff);
            void UpdateTotalScoreSSEHunterAtoms(float correlationCoeff, float skeletonCoeff, float geometryCoeff);
            void Unload();
            string GetSupportedLoadFileFormats();
            string GetSupportedSaveFileFormats();
            Vec3F Get3DCoordinates(int subsceneIndex, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
            void TransformAllAtomLocations(MatrixFloat transform);

            // Controlling the atom vector
            PDBAtom * AddAtom(PDBAtom atom);
            PDBAtom * GetAtom(unsigned long long index);
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

            bool CleanSecondaryStructures(); //empties the aHelices, bStrands and loops variables
            //what should really happen is that the code should check if it is
            //trying to reload the same one, and then if it did return false
            vector<Vec3F> CreatePointVector(PDBAtom first, PDBAtom last); // functionality mirrored in previously implemented method,
            // will try to refactor
            vector<Vec3F> LaplacianSmoothing(vector<Vec3F> points, int steps); // applies Laplacian smoothing to a vector of
            // Vector3Floats
            vector<Vec3F> CreateStrandNormals(vector<Vec3F> points, Vec3F previous, Vec3F next); // create line segment normals to be used in drawing Beta
            // strands
            void CreateHelixAxesTangentsAndPoints(vector<Vec3F>& axes, vector<Vec3F>& tangents, vector<Vec3F>& interpPoints, vector<Vec3F> points,
                Vec3F previous, Vec3F next, double HELIX_ALPHA, double HELIX_BETA, double HELIX_HERMITE_FACTOR);
            vector<Vec3F> InterpolateLoopPoints(vector<Vec3F> points, Vec3F previous, Vec3F next, int NUM_SECTIONS); // creates interpolated points for loops
            //vector<Vec3F> InterpolateStrandPoints(vector<Vec3F> points, Vec3F previous, Vec3F next, int NUM_SECTIONS);
            //vector<Vec3F> InterpolateHelixPoints(vector<Vec3F> points, Vec3F previous, Vec3F next, int NUM_SECTIONS);

            void SetHelixCorrs( vector < int > flatCorrespondences);
            void SetFeatureVecs(vector<Vec3F> flatFeatureVecs);
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
    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_VISUALIZATION_CALPHARENDERER_H_ */
