/*
 * Operations.h
 *
 *  Created on: Feb 5, 2016
 *      Author: shadow_walker
 */

#ifndef SRC_CORE_OPERATIONS_H_
#define SRC_CORE_OPERATIONS_H_

#include "Volume.h"
#include <boost/python.hpp>

using namespace boost::python;

using namespace Core;

class Operation {
    public:
        Operation(Volume &vol);
//        virtual void operator()(Volume &vol) =0;
    protected:
        Volume &volume;
};


#endif /* SRC_CORE_OPERATIONS_H_ */
