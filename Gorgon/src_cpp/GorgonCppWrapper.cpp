#ifndef GORGON_GORGON_CPP_WRAPPER_CPP
#define GORGON_GORGON_CPP_WRAPPER_CPP

#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1

#include <Gorgon/MarchingCubes.h>
#include <GraphMatch/BackEndInterface.h>
#include <Gorgon/MeshRenderer.h>

#include <boost/python.hpp>

using namespace boost::python;
using namespace wustl_mm::Visualization;
using namespace wustl_mm::GraphMatch;

BOOST_PYTHON_MODULE(gorgon_cpp_wrapper)
{
	class_<MeshRenderer>("MeshRenderer", init<>())
		.def("center", &MeshRenderer::Center)
		.def("draw", &MeshRenderer::Draw)
		.def("focus", &MeshRenderer::Focus)
		.def("loadFile", &MeshRenderer::LoadFile)
		.def("setDrawBoundingBox", &MeshRenderer::SetDrawBoundingBox)
		.def("getDrawBoundingBox", &MeshRenderer::GetDrawBoundingBox)
		.def("getSupportedFileFormats", &MeshRenderer::GetSupportedFileFormats)
	;

	class_<MarchingCubes>("MarchingCubes", init<>())
        .def("drawMesh", &MarchingCubes::drawMesh)
		.def("setSurfaceValue", &MarchingCubes::setSurfaceValue)
		.def("getSurfaceValue", &MarchingCubes::getSurfaceValue)
		.def("setSampleDensity", &MarchingCubes::setSampleDensity)
		.def("getSampleDensity", &MarchingCubes::getSampleDensity)
		//.def("loadMRC", &MarchingCubes::loadMRC)
		.def("loadMRC", &MarchingCubes::loadOFF)
    ;

	class_<BackEndInterface>("BackEndInterface", init<>())
        .def("SetConstantsFromFile", &BackEndInterface::SetConstantsFromFile)
		.def("LoadSkeletonGraph", &BackEndInterface::LoadSkeletonGraph)
		.def("LoadSequenceGraph", &BackEndInterface::LoadSequenceGraph)
		.def("ExecuteQuery", &BackEndInterface::ExecuteQuery)
		.def("DrawSkeleton", &BackEndInterface::DrawSkeleton)
		.def("DrawSequence", &BackEndInterface::DrawSequence)
		.def("CleanupMemory", &BackEndInterface::CleanupMemory)
		.def("DrawResult", &BackEndInterface::DrawResult)
    ;

}

#endif