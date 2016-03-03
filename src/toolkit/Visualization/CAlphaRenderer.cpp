/*
 * CAlphaRenderer.cpp
 *
 *      Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "CAlphaRenderer.h"
#include "GraphMatch/PDBReader.h"

namespace Visualization {

    void HermiteCurve::setCurve(Vec3F pstart, Vec3F pend, Vec3F tstart, Vec3F tend){
        p0 = pstart;
        p1 = pend;
        m0 = tstart;
        m1 = tend;
    }

    Vec3F HermiteCurve::getPos(double t){
        double tsquared = t*t;
        double tcubed = tsquared * t;

        double cp0 = 2*tcubed - 3*tsquared + 1;
        double cm0 = tcubed - 2*tsquared + t;
        double cp1 = (cp0 - 1)*(-1);
        double cm1 = tcubed - tsquared;

        double xt = cp0*p0.X() + cm0*m0.X() + cp1*p1.X() + cm1*m1.X();
        double yt = cp0*p0.Y() + cm0*m0.Y() + cp1*p1.Y() + cm1*m1.Y();
        double zt = cp0*p0.Z() + cm0*m0.Z() + cp1*p1.Z() + cm1*m1.Z();

        return Vec3F(xt, yt, zt);
    }

    // I don't know how this method works, but it is a part of the entirely functional
    // molscript code - BC
    Vec3F HermiteCurve::getTangent(double t){
        double t2 = t * t;
        double cp0 = 6.0 * (t2 - t);
        double cp1 = 6.0 * (-t2 + t);
        double cm0 = 3.0 * t2 - 4.0 * t + 1.0;
        double cm1 = 3.0 * t2 - 2.0 * t;
        double vxt = p0.X()*cp0 + p1.X() * cp1 + m0.X() * cm0 + m1.X() * cm1;
        double vyt = p0.Y()*cp0 + p1.Y() * cp1 + m0.Y() * cm0 + m1.Y() * cm1;
        double vzt = p0.Z()*cp0 + p1.Z() * cp1 + m0.Z() * cm0 + m1.Z() * cm1;

        return Vec3F(vxt, vyt, vzt);
    }
    /**
    End Hermite Curve code
    */

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

    PDBAtom * CAlphaRenderer::addAtom(PDBAtom atom) {
        atoms[atom.GetHashKey()] = atom;
        return &atoms[atom.GetHashKey()];
    }

    void CAlphaRenderer::addBond(PDBBond bond) {
        bonds.push_back(bond);
    }

    void CAlphaRenderer::addSideChainBond(PDBBond bond) {
        sidechainBonds.push_back(bond);
    }

    bool CAlphaRenderer::saveSSEHunterFile(string fileName) {
        return PDBReader::WriteAtomPositions(atoms, fileName);
    }

    void CAlphaRenderer::updateTotalScoreSSEHunterAtoms(float correlationCoeff, float skeletonCoeff, float geometryCoeff) {
        for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
            i->second.SetTempFactor( i->second.GetTotalScore(correlationCoeff, skeletonCoeff, geometryCoeff) );
        }
    }


    void CAlphaRenderer::unload() {
        atoms.clear();
        bonds.clear();
        sidechainBonds.clear();
    }

    string CAlphaRenderer::getSupportedLoadFileFormats() {
        return "Atom Positions (*.pdb)";
    }

    string CAlphaRenderer::getSupportedSaveFileFormats() {
        return "Atom Positions (*.atom)";
    }
    PDBAtom * CAlphaRenderer::getAtom(unsigned long long index) {
        return &atoms[index];
    }

    PDBBond * CAlphaRenderer::getBond(int index) {
        return &bonds[index];
    }

    PDBBond * CAlphaRenderer::getSideChainBond(int index) {
        return &sidechainBonds[index];
    }

    vector<unsigned long long> CAlphaRenderer::GetAtomHashes() {
        vector<unsigned long long> atomHashes;
        for (AtomMapType::iterator it = atoms.begin(); it != atoms.end(); it++) {
            atomHashes.push_back(it->first);
        }
        return atomHashes;
    }

    int CAlphaRenderer::getBondIndex(unsigned long long atom0, unsigned long long atom1) {
        for(unsigned int i = 0; i < bonds.size(); i++) {
            if(((bonds[i].GetAtom0Ix() == atom0) && (bonds[i].GetAtom1Ix() == atom1)) ||
                ((bonds[i].GetAtom0Ix() == atom1) && (bonds[i].GetAtom1Ix() == atom0))) {
                    return i;
            }
        }
        return -1;
    }

    int CAlphaRenderer::getSideChainBondIndex(unsigned long long atom0, unsigned long long atom1) {
        for(unsigned int i = 0; i < sidechainBonds.size(); i++) {
            if(((sidechainBonds[i].GetAtom0Ix() == atom0) && (sidechainBonds[i].GetAtom1Ix() == atom1)) ||
                ((sidechainBonds[i].GetAtom0Ix() == atom1) && (sidechainBonds[i].GetAtom1Ix() == atom0))) {
                    return i;
            }
        }
        return -1;
    }

    int CAlphaRenderer::getAtomCount() {
        return atoms.size();
    }

    int CAlphaRenderer::getBondCount() {
        return bonds.size();
    }

    int CAlphaRenderer::getSideChainBondCount() {
        return sidechainBonds.size();
    }

    void CAlphaRenderer::deleteAtom(unsigned long long index) {
        atoms.erase(atoms.find(index));
    }

    void CAlphaRenderer::deleteBond(int index) {
        bonds.erase(bonds.begin() + index);
    }

    void CAlphaRenderer::deleteSideChainBond(int index) {
        sidechainBonds.erase(sidechainBonds.begin() + index);
    }

    Vec3F CAlphaRenderer::get3DCoordinates(int subsceneIndex, int ix0, int ix1, int ix2, int ix3, int ix4) {
        Vec3F position;
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
                position = Vec3F(0,0,0);
                break;
        }
        return position;
    }

    void CAlphaRenderer::transformAllAtomLocations(MatrixFloat transform) {
        for(AtomMapType::iterator i = atoms.begin(); i != atoms.end(); i++) {
            i->second.Transform(transform);
        }
    }

    int CAlphaRenderer::startHelix() {
        aHelices.push_back(Secel());
        return aHelices.size() - 1;
    }

    void CAlphaRenderer::addHelixElement(int index, unsigned long long hashKey){
        aHelices[index].atomHashes.push_back(hashKey);
    }

    int CAlphaRenderer::startStrand() {
        bStrands.push_back(Secel());
        return bStrands.size() - 1;
    }

    void CAlphaRenderer::addStrandElement(int index, unsigned long long hashKey){
        bStrands[index].atomHashes.push_back(hashKey);
    }

    int CAlphaRenderer::startLoop() {
        loops.push_back(Secel());
        return loops.size() - 1;
    }

    void CAlphaRenderer::addLoopElement(int index, unsigned long long hashKey){
        loops[index].atomHashes.push_back(hashKey);
    }

    bool CAlphaRenderer::cleanSecondaryStructures(){
        aHelices.clear();
        bStrands.clear();
        loops.clear();
        return true;
    }

    void CAlphaRenderer::setHelixCorrs(  vector < int > flatCorrespondences){
        if(flatCorrespondences.size() %2 != 0)
            return;
        else
            corrs.clear();
        for(int i=0; i < flatCorrespondences.size(); i = i+2){
            corrs.push_back(boost::tuple<int, int>(flatCorrespondences[i], flatCorrespondences[i+1]));
        }
    }

    void CAlphaRenderer::setFeatureVecs(vector<Vec3F> flatFeatureVecs){
        if(flatFeatureVecs.size() %2 != 0)
            return;
        else
            featureVecs.clear();
        for(int i=0; i < flatFeatureVecs.size(); i = i+2){
            featureVecs.push_back(boost::tuple<Vec3F, Vec3F>(flatFeatureVecs[i], flatFeatureVecs[i+1]));
        }

    }

    // creates a vector of Vector3Floats that represents the locations of all the PDBAtoms
    // starting with start and ending with end; it does not error check, so incorrectly
    // ordered points will break this method.  there are more efficient ways to handle this
    // functionality, but this seems simple and flexible enough
    vector<Vec3F> CAlphaRenderer::createPointVector(PDBAtom start, PDBAtom end){
        vector<Vec3F> points;

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

    // implementation of Laplacian smoothing for a vector of Vector3Floats (treats them like points)
    // creating copies of "points" twice seems unnecessary, but I am unsure about the performance cost,
    // so I am leaving it for simplicity of implementation
    vector<Vec3F> CAlphaRenderer::laplacianSmoothing(vector<Vec3F> points, int steps){
        vector<Vec3F> pointsTemp(points);
        vector<Vec3F> smoothedPoints(points);

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
    vector<Vec3F> CAlphaRenderer::createStrandNormals(vector<Vec3F> points, Vec3F previous, Vec3F next){
        vector<Vec3F> normals(points);
        int ptsSize = points.size();

        for(int i = 1, length = ptsSize - 1; i < length; ++i){
            Vec3F newPos = (points[i-1] + points[i+1])*.5;
            normals[i] = points[i] - newPos;
            normals[i].normalize();
        }

        normals[0] = (points[1] + previous)*.5 - points[0];
        if ((points[0] - previous).length() < .0001){
            normals[0] = normals[1];
        }

        normals[ptsSize - 1] = (points[ptsSize - 2] + next)*.5 - points[ptsSize - 1];
        if ((points[ptsSize - 2] - next).length() < .0001){
            normals[ptsSize - 1] = normals[ptsSize - 2];
        }

        // "normals must point the same way" - molscript/graphics.c
        for(int j = 0, size = ptsSize - 1; j < size; ++j){
            if(normals[j]*normals[j+1] < 0){
                normals[j+1] = normals[j+1]*-1;
            }
        }

        // "smooth normals, one iteration" - molscript/graphics.c
        vector<Vec3F> smoothedNormals(normals);

        for(int k = 1, size = ptsSize - 1; k < size; ++k){
            smoothedNormals[k] = normals[k-1] + normals[k] + normals[k+1];
            smoothedNormals[k].normalize();
        }

        // "normals exactly perpendicular to strand" - molscript/graphics.c
        Vec3F direction = points[1] - points[0];
        Vec3F side = direction^smoothedNormals[0];
        smoothedNormals[0] = side ^ direction;
        smoothedNormals[0].normalize();

        for(int i = 1, size = ptsSize - 1; i < size; ++i){
            direction = points[i+1] - points[i-1];
            side = direction^smoothedNormals[i];
            smoothedNormals[i] = side^direction;
            smoothedNormals[i].normalize();
        }

        direction = points[ptsSize - 1] - points[ptsSize - 2];
        side = direction^smoothedNormals[ptsSize - 1];
        smoothedNormals[ptsSize - 1] = side^direction;
        smoothedNormals[ptsSize - 1].normalize();
        return smoothedNormals;
    }

    void CAlphaRenderer::createHelixAxesTangentsAndPoints(vector<Vec3F>& axes, vector<Vec3F>& tangents, vector<Vec3F>& interpPoints, std::vector<Vec3F> points, Vec3F previous, Vec3F next, double HELIX_ALPHA, double HELIX_BETA, double HELIX_HERMITE_FACTOR){
        if(points.size() > 2){

            for(int i = 0; i < points.size() - 1; ++i){

                if(i > 0){
                    Vec3F cvec = points[i+1] - points[i-1];
                    cvec.normalize();

                    Vec3F rvec = (points[i]-points[i-1])^(points[i+1]-points[i]);
                    rvec.normalize();

                    axes[i] = rvec*sin(HELIX_ALPHA) + cvec*cos(HELIX_ALPHA);
                    tangents[i] = rvec*sin(HELIX_BETA) + cvec*cos(HELIX_BETA);
                    tangents[i] = tangents[i]*HELIX_HERMITE_FACTOR;
                }
            }
            axes[0] = axes[1];
            axes[axes.size()-1] = axes[axes.size()-2];

            tangents[0] = previous - points[1];
            tangents[0].normalize();
            tangents[0] = tangents[0]*HELIX_HERMITE_FACTOR;
            tangents[tangents.size()-1] = next - points[points.size()-2];
            tangents[tangents.size()-1].normalize();
            tangents[tangents.size()-1] = tangents[tangents.size()-1]*HELIX_HERMITE_FACTOR;
        }
    }

    vector<Vec3F> CAlphaRenderer::interpolateLoopPoints(std::vector<Vec3F> points, Vec3F previous, Vec3F next, int NUM_SECTIONS){
        HermiteCurve curve;
        Vec3F m0, m1;
        vector<Vec3F> pointstemp(points);
        bool LAPLACIAN_SMOOTHING = true;
        int SMOOTHING_STEPS = 1;
        double HERMITE_FACTOR = 0.5;
        int LOOP_SLICES = 10;
        if(LAPLACIAN_SMOOTHING){
            pointstemp = laplacianSmoothing(points, SMOOTHING_STEPS);
        }

        vector<Vec3F> interpolatedPoints((pointstemp.size()-1)*(NUM_SEGMENTS));

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

    void CAlphaRenderer::setNumSegments(int segments){
        NUM_SEGMENTS = segments;
    }

    void CAlphaRenderer::setNumSlices(int slices){
        NUM_SLICES = slices;
    }

} /* namespace Visualization */
