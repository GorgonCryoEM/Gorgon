/*
 * CAlphaRenderer.h
 *
 *      Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_CALPHARENDERER_H_
#define SRC_TOOLKIT_VISUALIZATION_CALPHARENDERER_H_

#include "GraphMatch/PDBAtom.h"

#include <string>
#include <map>

namespace Visualization {

    typedef map<unsigned long long, PDBAtom> AtomMapType;

    class CAlphaRenderer {
        public:
            bool saveSSEHunterFile(string fileName);
            void updateTotalScoreSSEHunterAtoms(float correlationCoeff, float skeletonCoeff, float geometryCoeff);

            void addAtom(PDBAtom atom);
            void setHelixCorrs( vector < int > flatCorrespondences);

        private:
            AtomMapType atoms;
            vector < pair<int, int> > corrs;
    };

} /* namespace Visualization */

#endif /* SRC_TOOLKIT_VISUALIZATION_CALPHARENDERER_H_ */
