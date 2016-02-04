#ifndef CORE_CALPHA_RENDERER_H
#define CORE_CALPHA_RENDERER_H


//#include <GorgonGL.h>
//#include <cstdlib>
//#include <cstdio>
//#include <ProteinMorph/NonManifoldMesh.h>
//#include <ProteinMorph/SSEHunter.h>
#include <SkeletonMaker/volume.h>
#include <GraphMatch/PDBReader.h>
//#include <GraphMatch/PDBAtom.h>
#include <GraphMatch/PDBBond.h>
//#include <map>
//#include <list>
#include <GraphMatch/VectorMath.h>
//#include <boost/tuple/tuple.hpp>

//using namespace std;
//using namespace wustl_mm::Protein_Morph;
//using namespace wustl_mm::GraphMatch;
//using namespace wustl_mm::SkeletonMaker;
//using namespace boost::tuples;

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
        Vector3DFloat p0, p1, m0, m1;

        void setCurve(Vector3DFloat pstart, Vector3DFloat pend, Vector3DFloat tstart, Vector3DFloat tend);
        Vector3DFloat getPos(double t);
        Vector3DFloat getTangent(double t);
    };

    void HermiteCurve::setCurve(Vector3DFloat pstart, Vector3DFloat pend, Vector3DFloat tstart, Vector3DFloat tend){
        p0 = pstart;
        p1 = pend;
        m0 = tstart;
        m1 = tend;
    }

    Vector3DFloat HermiteCurve::getPos(double t){
        double tsquared = t*t;
        double tcubed = tsquared * t;

        double cp0 = 2*tcubed - 3*tsquared + 1;
        double cm0 = tcubed - 2*tsquared + t;
        double cp1 = (cp0 - 1)*(-1);
        double cm1 = tcubed - tsquared;

        double xt = cp0*p0.X() + cm0*m0.X() + cp1*p1.X() + cm1*m1.X();
        double yt = cp0*p0.Y() + cm0*m0.Y() + cp1*p1.Y() + cm1*m1.Y();
        double zt = cp0*p0.Z() + cm0*m0.Z() + cp1*p1.Z() + cm1*m1.Z();

        return Vector3DFloat(xt, yt, zt);
    }

    // I don't know how this method works, but it is a part of the entirely functional
    // molscript code - BC
    Vector3DFloat HermiteCurve::getTangent(double t){
        double t2 = t * t;
        double cp0 = 6.0 * (t2 - t);
        double cp1 = 6.0 * (-t2 + t);
        double cm0 = 3.0 * t2 - 4.0 * t + 1.0;
        double cm1 = 3.0 * t2 - 2.0 * t;
        double vxt = p0.X()*cp0 + p1.X() * cp1 + m0.X() * cm0 + m1.X() * cm1;
        double vyt = p0.Y()*cp0 + p1.Y() * cp1 + m0.Y() * cm0 + m1.Y() * cm1;
        double vzt = p0.Z()*cp0 + p1.Z() * cp1 + m0.Z() * cm0 + m1.Z() * cm1;

        return Vector3DFloat(vxt, vyt, vzt);
    }
    /**
    End Hermite Curve code
    */

    class CAlphaRenderer : public Volume{
    public:
        struct Secel{
            vector<unsigned long long> atomHashes;
            bool selected;
        };

        CAlphaRenderer();

        void LoadFile(string fileName);
        void LoadSSEHunterFile(string fileName);
        bool SaveSSEHunterFile(string fileName);
//			void GetSSEHunterAtoms(Volume * vol, NonManifoldMesh_Annotated * skeleton, float resolution, float threshold, float correlationCoeff, float skeletonCoeff, float geometryCoeff);
        void UpdateTotalScoreSSEHunterAtoms(float correlationCoeff, float skeletonCoeff, float geometryCoeff);
        void Unload();
        string GetSupportedLoadFileFormats();
        string GetSupportedSaveFileFormats();
        Vector3DFloat Get3DCoordinates(int subsceneIndex, int ix0, int ix1 = -1, int ix2 = -1, int ix3 = -1, int ix4 = -1);
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
        vector<Vector3DFloat> CreatePointVector(PDBAtom first, PDBAtom last); // functionality mirrored in previously implemented method,
        // will try to refactor
        vector<Vector3DFloat> LaplacianSmoothing(vector<Vector3DFloat> points, int steps); // applies Laplacian smoothing to a vector of
        // Vector3DFloats
        vector<Vector3DFloat> CreateStrandNormals(vector<Vector3DFloat> points, Vector3DFloat previous, Vector3DFloat next); // create line segment normals to be used in drawing Beta
        // strands
        void CreateHelixAxesTangentsAndPoints(vector<Vector3DFloat>& axes, vector<Vector3DFloat>& tangents, vector<Vector3DFloat>& interpPoints, vector<Vector3DFloat> points,
            Vector3DFloat previous, Vector3DFloat next, double HELIX_ALPHA, double HELIX_BETA, double HELIX_HERMITE_FACTOR);
        vector<Vector3DFloat> InterpolateLoopPoints(vector<Vector3DFloat> points, Vector3DFloat previous, Vector3DFloat next, int NUM_SECTIONS); // creates interpolated points for loops
        //vector<Vector3DFloat> InterpolateStrandPoints(vector<Vector3DFloat> points, Vector3DFloat previous, Vector3DFloat next, int NUM_SECTIONS);
        //vector<Vector3DFloat> InterpolateHelixPoints(vector<Vector3DFloat> points, Vector3DFloat previous, Vector3DFloat next, int NUM_SECTIONS);

        void SetHelixCorrs( vector < int > flatCorrespondences);
        void SetFeatureVecs(vector<Vector3DFloat> flatFeatureVecs);
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
        vector< boost::tuple<Vector3DFloat, Vector3DFloat> > featureVecs;

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


    CAlphaRenderer::CAlphaRenderer() {
        NUM_SEGMENTS = 10;
        NUM_SLICES = 10;
        HELIX_HERMITE_FACTOR = 4.7;
        HELIX_ALPHA = 32.0 * PI/180.0;
        HELIX_BETA = -11.0 * PI/180.0; // these three values taken from molscript code
        HELIX_WIDTH = 4.0;

        STRAND_HERMITE_FACTOR = .5;
        LOOP_RADIUS = .25;

        thinRibbThickness = .05;
    }

    PDBAtom * CAlphaRenderer::AddAtom(PDBAtom atom) {
        atoms[atom.GetHashKey()] = atom;
        return &atoms[atom.GetHashKey()];
    }

    void CAlphaRenderer::AddBond(PDBBond bond) {
        bonds.push_back(bond);
    }

    void CAlphaRenderer::AddSideChainBond(PDBBond bond) {
        sidechainBonds.push_back(bond);
    }

    void CAlphaRenderer::LoadFile(string fileName) {
        atoms.clear();
        bonds.clear();
        atoms = PDBReader::ReadAtomPositions(fileName);

        // Keeping only C-Alpha atoms
        vector<unsigned long long> eraseKeys;
        eraseKeys.clear();

        for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
            if(i->second.GetName().compare("CA") != 0) {
                eraseKeys.push_back(i->first);
            }
        }

        for(unsigned int i = 0; i < eraseKeys.size(); i++) {
            atoms.erase(atoms.find(eraseKeys[i]));
        }

        eraseKeys.clear();

        list<SerialAndHashType> sortedSerials;
        SerialAndHashType elem;
        for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
            elem.hashKey = i->first;
            elem.serial = i->second.GetSerial();

            sortedSerials.push_back(elem);
        }
        sortedSerials.sort(SerialAndHashTypePredicate());


        list<SerialAndHashType>::iterator oldAtom = sortedSerials.begin();
        list<SerialAndHashType>::iterator startAtom = sortedSerials.begin();

        startAtom++;
        for(list<SerialAndHashType>::iterator i = startAtom; i != sortedSerials.end(); i++) {
            bonds.push_back(PDBBond(oldAtom->hashKey, i->hashKey, false));
            oldAtom = i;
        }
        sortedSerials.clear();

    }

    void CAlphaRenderer::LoadSSEHunterFile(string fileName) {
        atoms.clear();
        bonds.clear();
        atoms = PDBReader::ReadAtomPositions(fileName);

        float maxTempFactor = -10000.0f, minTempFactor = 10000.0f;
        float tempFactor;

        for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
            tempFactor = i->second.GetTempFactor();
            if(tempFactor > maxTempFactor) {
                maxTempFactor = tempFactor;
            }
            if(tempFactor < minTempFactor) {
                minTempFactor = tempFactor;
            }
        }
        float r, g, b;

        for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
            i->second.SetAtomRadius(3.0);
            tempFactor = i->second.GetTempFactor();
            if(tempFactor < 0) {
                tempFactor = (tempFactor / minTempFactor);
                r = 1.0f - tempFactor;
                g = 1.0f - tempFactor;
                b = 1.0f;
            } else {
                tempFactor = (tempFactor / maxTempFactor);
                r = 1.0f;
                g = 1.0f - tempFactor;
                b = 1.0f - tempFactor;
            }

            i->second.SetColor(r, g, b, 1.0f);
        }

    }
    bool CAlphaRenderer::SaveSSEHunterFile(string fileName) {
        return PDBReader::WriteAtomPositions(atoms, fileName);
    }

    void CAlphaRenderer::UpdateTotalScoreSSEHunterAtoms(float correlationCoeff, float skeletonCoeff, float geometryCoeff) {
        for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
            i->second.SetTempFactor( i->second.GetTotalScore(correlationCoeff, skeletonCoeff, geometryCoeff) );
        }
    }


    void CAlphaRenderer::Unload() {
        atoms.clear();
        bonds.clear();
        sidechainBonds.clear();
    }

    string CAlphaRenderer::GetSupportedLoadFileFormats() {
        return "Atom Positions (*.pdb)";
    }

    string CAlphaRenderer::GetSupportedSaveFileFormats() {
        return "Atom Positions (*.atom)";
    }
    PDBAtom * CAlphaRenderer::GetAtom(unsigned long long index) {
        return &atoms[index];
    }

    PDBBond * CAlphaRenderer::GetBond(int index) {
        return &bonds[index];
    }

    PDBBond * CAlphaRenderer::GetSideChainBond(int index) {
        return &sidechainBonds[index];
    }

    vector<unsigned long long> CAlphaRenderer::GetAtomHashes() {
        vector<unsigned long long> atomHashes;
        for (AtomMapType::iterator it = atoms.begin(); it != atoms.end(); it++) {
            atomHashes.push_back(it->first);
        }
        return atomHashes;
    }

    int CAlphaRenderer::GetBondIndex(unsigned long long atom0, unsigned long long atom1) {
        for(unsigned int i = 0; i < bonds.size(); i++) {
            if(((bonds[i].GetAtom0Ix() == atom0) && (bonds[i].GetAtom1Ix() == atom1)) ||
                ((bonds[i].GetAtom0Ix() == atom1) && (bonds[i].GetAtom1Ix() == atom0))) {
                    return i;
            }
        }
        return -1;
    }

    int CAlphaRenderer::GetSideChainBondIndex(unsigned long long atom0, unsigned long long atom1) {
        for(unsigned int i = 0; i < sidechainBonds.size(); i++) {
            if(((sidechainBonds[i].GetAtom0Ix() == atom0) && (sidechainBonds[i].GetAtom1Ix() == atom1)) ||
                ((sidechainBonds[i].GetAtom0Ix() == atom1) && (sidechainBonds[i].GetAtom1Ix() == atom0))) {
                    return i;
            }
        }
        return -1;
    }

    int CAlphaRenderer::GetAtomCount() {
        return atoms.size();
    }

    int CAlphaRenderer::GetBondCount() {
        return bonds.size();
    }

    int CAlphaRenderer::GetSideChainBondCount() {
        return sidechainBonds.size();
    }

    void CAlphaRenderer::DeleteAtom(unsigned long long index) {
        atoms.erase(atoms.find(index));
    }

    void CAlphaRenderer::DeleteBond(int index) {
        bonds.erase(bonds.begin() + index);
    }

    void CAlphaRenderer::DeleteSideChainBond(int index) {
        sidechainBonds.erase(sidechainBonds.begin() + index);
    }

    Vector3DFloat CAlphaRenderer::Get3DCoordinates(int subsceneIndex, int ix0, int ix1, int ix2, int ix3, int ix4) {
        Vector3DFloat position;
        switch(subsceneIndex) {
            case(0):
                if((ix0 >= 0) && (ix0 <= (int)atoms.size())) {
                    PDBAtom * a =  & (atoms [ix0]);
                    position = a->GetPosition();
                }
                break;
            case(1):
                if((ix0 >= 0) && (ix0 <= (int)bonds.size())) {
                    position = (atoms[bonds[ix0].GetAtom0Ix()].GetPosition() + atoms[bonds[ix0].GetAtom1Ix()].GetPosition()) * 0.5;
                }
                break;
            default:
                position = Vector3DFloat(0,0,0);
                break;
        }
        return position;
    }

    void CAlphaRenderer::TransformAllAtomLocations(MatrixFloat transform) {
        for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
            i->second.Transform(transform);
        }
    }

    int CAlphaRenderer::StartHelix() {
        aHelices.push_back(Secel());
        return aHelices.size() - 1;
    }

    void CAlphaRenderer::AddHelixElement(int index, unsigned long long hashKey){
        aHelices[index].atomHashes.push_back(hashKey);
    }

    int CAlphaRenderer::StartStrand() {
        bStrands.push_back(Secel());
        return bStrands.size() - 1;
    }

    void CAlphaRenderer::AddStrandElement(int index, unsigned long long hashKey){
        bStrands[index].atomHashes.push_back(hashKey);
    }

    int CAlphaRenderer::StartLoop() {
        loops.push_back(Secel());
        return loops.size() - 1;
    }

    void CAlphaRenderer::AddLoopElement(int index, unsigned long long hashKey){
        loops[index].atomHashes.push_back(hashKey);
    }

    bool CAlphaRenderer::CleanSecondaryStructures(){
        aHelices.clear();
        bStrands.clear();
        loops.clear();
        return true;
    }

    void CAlphaRenderer::SetHelixCorrs(  vector < int > flatCorrespondences){
        if(flatCorrespondences.size() %2 != 0)
            return;
        else
            corrs.clear();
        for(int i=0; i < flatCorrespondences.size(); i = i+2){
            corrs.push_back(boost::tuple<int, int>(flatCorrespondences[i], flatCorrespondences[i+1]));
        }
    }

    void CAlphaRenderer::SetFeatureVecs(vector<Vector3DFloat> flatFeatureVecs){
        if(flatFeatureVecs.size() %2 != 0)
            return;
        else
            featureVecs.clear();
        for(int i=0; i < flatFeatureVecs.size(); i = i+2){
            featureVecs.push_back(boost::tuple<Vector3DFloat, Vector3DFloat>(flatFeatureVecs[i], flatFeatureVecs[i+1]));
        }

    }

    // creates a vector of Vector3DFloats that represents the locations of all the PDBAtoms
    // starting with start and ending with end; it does not error check, so incorrectly
    // ordered points will break this method.  there are more efficient ways to handle this
    // functionality, but this seems simple and flexible enough
    vector<Vector3DFloat> CAlphaRenderer::CreatePointVector(PDBAtom start, PDBAtom end){
        vector<Vector3DFloat> points;

        PDBAtom current = start;
        while(current.GetHashKey() != end.GetHashKey()){
            points.push_back(current.GetPosition());
            if(current.GetHashKey() == current.GetNextCAHash()){
                break;
            }
            current = atoms.find(current.GetNextCAHash())->second;
        }

        points.push_back(end.GetPosition());
        return points;
    }

    // implementation of Laplacian smoothing for a vector of Vector3DFloats (treats them like points)
    // creating copies of "points" twice seems unnecessary, but I am unsure about the performance cost,
    // so I am leaving it for simplicity of implementation
    vector<Vector3DFloat> CAlphaRenderer::LaplacianSmoothing(vector<Vector3DFloat> points, int steps){
        vector<Vector3DFloat> pointsTemp(points);
        vector<Vector3DFloat> smoothedPoints(points);

        for(int i = 0; i < steps; ++i){
            for(int j = 1; j < points.size()-1; ++j){
                smoothedPoints[j] = (pointsTemp[j-1] + pointsTemp[j+1])*.5;
                smoothedPoints[j] = (smoothedPoints[j] + pointsTemp[j])*.5;
            }
            pointsTemp = smoothedPoints;
        }
        return pointsTemp;
    }

    // unsure of what behavior should be if points.size() < 3; in molscript the strand is skipped in this case
    vector<Vector3DFloat> CAlphaRenderer::CreateStrandNormals(vector<Vector3DFloat> points, Vector3DFloat previous, Vector3DFloat next){
        vector<Vector3DFloat> normals(points);
        int ptsSize = points.size();

        for(int i = 1, length = ptsSize - 1; i < length; ++i){
            Vector3DFloat newPos = (points[i-1] + points[i+1])*.5;
            normals[i] = points[i] - newPos;
            normals[i].Normalize();
        }

        normals[0] = (points[1] + previous)*.5 - points[0];
        if ((points[0] - previous).Length() < .0001){
            normals[0] = normals[1];
        }

        normals[ptsSize - 1] = (points[ptsSize - 2] + next)*.5 - points[ptsSize - 1];
        if ((points[ptsSize - 2] - next).Length() < .0001){
            normals[ptsSize - 1] = normals[ptsSize - 2];
        }

        // "normals must point the same way" - molscript/graphics.c
        for(int j = 0, size = ptsSize - 1; j < size; ++j){
            if(normals[j]*normals[j+1] < 0){
                normals[j+1] = normals[j+1]*-1;
            }
        }

        // "smooth normals, one iteration" - molscript/graphics.c
        vector<Vector3DFloat> smoothedNormals(normals);

        for(int k = 1, size = ptsSize - 1; k < size; ++k){
            smoothedNormals[k] = normals[k-1] + normals[k] + normals[k+1];
            smoothedNormals[k].Normalize();
        }

        // "normals exactly perpendicular to strand" - molscript/graphics.c
        Vector3DFloat direction = points[1] - points[0];
        Vector3DFloat side = direction^smoothedNormals[0];
        smoothedNormals[0] = side ^ direction;
        smoothedNormals[0].Normalize();

        for(int i = 1, size = ptsSize - 1; i < size; ++i){
            direction = points[i+1] - points[i-1];
            side = direction^smoothedNormals[i];
            smoothedNormals[i] = side^direction;
            smoothedNormals[i].Normalize();
        }

        direction = points[ptsSize - 1] - points[ptsSize - 2];
        side = direction^smoothedNormals[ptsSize - 1];
        smoothedNormals[ptsSize - 1] = side^direction;
        smoothedNormals[ptsSize - 1].Normalize();
        return smoothedNormals;
    }

    void CAlphaRenderer::CreateHelixAxesTangentsAndPoints(vector<Vector3DFloat>& axes, vector<Vector3DFloat>& tangents, vector<Vector3DFloat>& interpPoints, std::vector<Vector3DFloat> points, Vector3DFloat previous, Vector3DFloat next, double HELIX_ALPHA, double HELIX_BETA, double HELIX_HERMITE_FACTOR){
        if(points.size() > 2){

            for(int i = 0; i < points.size() - 1; ++i){

                if(i > 0){
                    Vector3DFloat cvec = points[i+1] - points[i-1];
                    cvec.Normalize();

                    Vector3DFloat rvec = (points[i]-points[i-1])^(points[i+1]-points[i]);
                    rvec.Normalize();

                    axes[i] = rvec*sin(HELIX_ALPHA) + cvec*cos(HELIX_ALPHA);
                    tangents[i] = rvec*sin(HELIX_BETA) + cvec*cos(HELIX_BETA);
                    tangents[i] = tangents[i]*HELIX_HERMITE_FACTOR;
                }
            }
            axes[0] = axes[1];
            axes[axes.size()-1] = axes[axes.size()-2];

            tangents[0] = previous - points[1];
            tangents[0].Normalize();
            tangents[0] = tangents[0]*HELIX_HERMITE_FACTOR;
            tangents[tangents.size()-1] = next - points[points.size()-2];
            tangents[tangents.size()-1].Normalize();
            tangents[tangents.size()-1] = tangents[tangents.size()-1]*HELIX_HERMITE_FACTOR;
        }
    }

    vector<Vector3DFloat> CAlphaRenderer::InterpolateLoopPoints(std::vector<Vector3DFloat> points, Vector3DFloat previous, Vector3DFloat next, int NUM_SECTIONS){
        HermiteCurve curve;
        Vector3DFloat m0, m1;
        vector<Vector3DFloat> pointstemp(points);
        bool LAPLACIAN_SMOOTHING = true;
        int SMOOTHING_STEPS = 1;
        double HERMITE_FACTOR = 0.5;
        int LOOP_SLICES = 10;
        if(LAPLACIAN_SMOOTHING){
            pointstemp = LaplacianSmoothing(points, SMOOTHING_STEPS);
        }

        vector<Vector3DFloat> interpolatedPoints((pointstemp.size()-1)*(NUM_SEGMENTS));

        for(unsigned int i = 0; i < points.size()-1; ++i){
            if(i == 0){
                m0 = pointstemp[i+1] - previous;
            } else {
                m0 = pointstemp[i+1] - pointstemp[i-1];
                m0 = m0*HERMITE_FACTOR;
            }

            if(i + 2 > pointstemp.size() - 1){
                m1 = next - pointstemp[i];
            } else {
                m1 = pointstemp[i+2] - pointstemp[i];
                m1 = m1*HERMITE_FACTOR;
            }

            curve.setCurve(pointstemp[i], pointstemp[i+1], m0, m1);
            interpolatedPoints[i*(NUM_SEGMENTS)] = pointstemp[i];
            for (int sect = 1; sect < NUM_SEGMENTS; ++sect){
                double tsect = ((double)sect)/((double)NUM_SEGMENTS);
                interpolatedPoints[i*(NUM_SEGMENTS) + sect] = curve.getPos(tsect);
            }
        }
        interpolatedPoints[interpolatedPoints.size()-1] = points[points.size() -1];
        return interpolatedPoints;
    }

    void CAlphaRenderer::SetNumSegments(int segments){
        NUM_SEGMENTS = segments;
    }

    void CAlphaRenderer::SetNumSlices(int slices){
        NUM_SLICES = slices;
    }
}

#endif
