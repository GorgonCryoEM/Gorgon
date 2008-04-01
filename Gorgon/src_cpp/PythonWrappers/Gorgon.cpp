#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1

#include <boost/python.hpp>
#include <GraySkeletonCPP/MarchingCubes.h>

using namespace boost::python;
using namespace wustl_mm::GraySkeletonCPP;


BOOST_PYTHON_MODULE(gorgon)
{
	class_<MarchingCubes>("MarchingCubes", init<>())
        .def("drawMesh", &MarchingCubes::drawMesh)
		.def("setSurfaceValue", &MarchingCubes::setSurfaceValue)
		.def("getSurfaceValue", &MarchingCubes::getSurfaceValue)
		.def("setSampleDensity", &MarchingCubes::setSampleDensity)
		.def("getSampleDensity", &MarchingCubes::getSampleDensity)
		//.def("loadMRC", &MarchingCubes::loadMRC)
		.def("loadMRC", &MarchingCubes::loadOFF)
    ;
}