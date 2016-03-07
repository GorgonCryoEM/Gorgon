/*
 * Vox.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_CORE_VOX_H_
#define SRC_TOOLKIT_CORE_VOX_H_

#include <vector>

using namespace std;

namespace Core {

    /*
     *
     */
    class Vox {
        public:
            Vox();
            virtual ~Vox();

        private:
            vector<vector<vector<double> > > vox;
    };

} /* namespace Core */

#endif /* SRC_TOOLKIT_CORE_VOX_H_ */
