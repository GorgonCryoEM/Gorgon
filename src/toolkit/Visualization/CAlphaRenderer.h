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

    class CAlphaRenderer : public Volume {
        public:
            struct Secel{
                vector<unsigned long long> atomHashes;
                bool selected;
            };

            CAlphaRenderer();

            bool saveSSEHunterFile(string fileName);
            void updateTotalScoreSSEHunterAtoms(float correlationCoeff, float skeletonCoeff, float geometryCoeff);
            void unload();
            string getSupportedLoadFileFormats();
            string getSupportedSaveFileFormats();
            Vec3F get3DCoordinates(int subsceneIndex, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
            void transformAllAtomLocations(MatrixFloat transform);

            // Controlling the atom vector
            void addAtom(PDBAtom atom);
            PDBAtom getAtom(unsigned long long index);
            void deleteAtom(unsigned long long index);
            int getAtomCount();

            //Controlling the bond vector
            void addBond(PDBBond bond);
            void deleteBond(int index);
            int getBondCount();

            void setNumSegments(int segments);
            void setNumSlices(int slices);

            bool cleanSecondaryStructures(); //empties the aHelices, bStrands and loops variables
            //what should really happen is that the code should check if it is
            //trying to reload the same one, and then if it did return false
            vector<Vec3F> createPointVector(PDBAtom first, PDBAtom last); // functionality mirrored in previously implemented method,
            // will try to refactor
            vector<Vec3F> laplacianSmoothing(vector<Vec3F> points, int steps); // applies Laplacian smoothing to a vector of
            // Vector3Floats
            vector<Vec3F> createStrandNormals(vector<Vec3F> points, Vec3F previous, Vec3F next); // create line segment normals to be used in drawing Beta
            // strands
            void createHelixAxesTangentsAndPoints(vector<Vec3F>& axes,
                                                  vector<Vec3F>& tangents,
                                                  vector<Vec3F>& interpPoints,
                                                  vector<Vec3F> points,
                                                  Vec3F previous, Vec3F next,
                                                  double HELIX_ALPHA,
                                                  double HELIX_BETA, double HELIX_HERMITE_FACTOR);
            vector<Vec3F> interpolateLoopPoints(vector<Vec3F> points, Vec3F previous, Vec3F next, int NUM_SECTIONS); // creates interpolated points for loops

            void setHelixCorrs( vector < int > flatCorrespondences);
            void setFeatureVecs(vector<Vec3F> flatFeatureVecs);
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
