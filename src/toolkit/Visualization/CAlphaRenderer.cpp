/*
 * CAlphaRenderer.cpp
 *
 *      Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "CAlphaRenderer.h"
#include "GraphMatch/PDBReader.h"
#include "HermiteCurve.h"

namespace Visualization {

    void CAlphaRenderer::addAtom(PDBAtom atom) {
        atoms[atom.GetHashKey()] = atom;
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
    }

    string CAlphaRenderer::getSupportedLoadFileFormats() {
        return "Atom Positions (*.pdb)";
    }

    string CAlphaRenderer::getSupportedSaveFileFormats() {
        return "Atom Positions (*.atom)";
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
            default:
                position = Vec3F(0,0,0);
                break;
        }
        return position;
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

} /* namespace Visualization */
