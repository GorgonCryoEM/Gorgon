/*
 * Operations.h
 *
 *  Created on: Feb 5, 2016
 *      Author: shadow_walker
 */

#ifndef SRC_CORE_OPERATIONS_H_
#define SRC_CORE_OPERATIONS_H_

#include <boost/python.hpp>
#include "VolumeData.h"

using namespace boost::python;

namespace Core {
    class Operation {
        public:
            Operation(VolumeData &vol);
            virtual ~Operation(){};
            void fill(double val);
            void applyMask(double maskValue, bool keepMaskValue);
        protected:
            VolumeData &volume;
    };
}


#endif /* SRC_CORE_OPERATIONS_H_ */
