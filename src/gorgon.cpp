/*
 * gorgon.cpp
 *
 *  Created on: Aug 13, 2015
 *      Author: shadow_walker
 */

#include <boost/python.hpp>
#include "hello.h"

using namespace boost::python;

BOOST_PYTHON_MODULE(libpygorgon)
{
    def("hello", hello);
}
