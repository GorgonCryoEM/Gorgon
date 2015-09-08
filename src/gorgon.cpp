/*
 * gorgon.cpp
 *
 *  Created on: Aug 13, 2015
 *      Author: shadow_walker
 */

#include <boost/python.hpp>

#include <MathTools/Vector3D.h>

using namespace boost::python;

using namespace wustl_mm::MathTools;

BOOST_PYTHON_MODULE(libpygorgon)
{
	class_<Vector3DFloat>("Vector3DFloat", init<float, float, float>())
			.def("x", &Vector3DFloat::X)
			.def("y", &Vector3DFloat::Y)
			.def("z", &Vector3DFloat::Z)
			.def("length", &Vector3DFloat::Length)
			.def("Transform", &Vector3DFloat::Transform)
			.def("rotate", &Vector3DFloat::Rotate)
			.def("Print", &Vector3DFloat::Print)
			.def(self * double())
			.def(self + self)
			.def(self - self)
			.def(self ^ self)
			.def(self += self)
			.def(self -= self)
			//Vector3DTemplate<T> operator-();
			//Vector3DTemplate<T>& operator=(const Vector3DTemplate<T>& d);

		;
}
