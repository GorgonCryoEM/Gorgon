/*
 * Vox.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_CORE_VOX_H_
#define SRC_TOOLKIT_CORE_VOX_H_

#include <vector>

#include "volume.h"

using namespace std;
using namespace SkeletonMaker;

namespace Core {

    /*
     *
     */
    class Vox {
        public:
            Vox(const Volume & v);
            virtual ~Vox();

        private:
            vector<vector<vector<double> > > vox;
            const Volume & vol;
    };

} /* namespace Core */

#endif /* SRC_TOOLKIT_CORE_VOX_H_ */
