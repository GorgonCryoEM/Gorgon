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

    void CAlphaRenderer::setHelixCorrs(  vector < int > flatCorrespondences){
        if(flatCorrespondences.size() %2 != 0)
            return;
        else
            corrs.clear();
        for(int i=0; i < flatCorrespondences.size(); i = i+2){
            corrs.push_back(make_pair(flatCorrespondences[i], flatCorrespondences[i+1]));
        }
    }

} /* namespace Visualization */
