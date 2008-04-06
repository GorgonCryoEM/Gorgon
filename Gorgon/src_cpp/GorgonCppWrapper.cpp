#ifndef GORGON_GORGON_CPP_WRAPPER_CPP
#define GORGON_GORGON_CPP_WRAPPER_CPP

#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1

#include <GraphMatch/BackEndInterface.h>
#include <Gorgon/VolumeRenderer.h>
#include <Gorgon/MeshRenderer.h>

#include <boost/python.hpp>

using namespace boost::python;
using namespace wustl_mm::Visualization;
using namespace wustl_mm::GraphMatch;

BOOST_PYTHON_MODULE(gorgon_cpp_wrapper)
{
	class_<MeshRenderer>("MeshRenderer", init<>())
		.def("draw", &MeshRenderer::Draw)
		.def("drawBoundingBox", &MeshRenderer::DrawBoundingBox)
		.def("loadFile", &MeshRenderer::LoadFile)
		.def("unload", &MeshRenderer::Unload)
		.def("smoothLaplacian", &MeshRenderer::SmoothLaplacian)
		.def("getSupportedFileFormats", &MeshRenderer::GetSupportedFileFormats)
		.def("getMin", &MeshRenderer::GetMin)
		.def("getMax", &MeshRenderer::GetMax)
	;

	class_<VolumeRenderer>("VolumeRenderer", init<>())
		.def("draw", &VolumeRenderer::Draw)
		.def("drawBoundingBox", &VolumeRenderer::DrawBoundingBox)
		.def("loadFile", &VolumeRenderer::LoadFile)
		.def("unload", &VolumeRenderer::Unload)
		.def("getSupportedFileFormats", &VolumeRenderer::GetSupportedFileFormats)
		.def("getMin", &VolumeRenderer::GetMin)
		.def("getMax", &VolumeRenderer::GetMax)
		.def("getMinDensity", &VolumeRenderer::GetMinDensity)
		.def("getMaxDensity", &VolumeRenderer::GetMaxDensity)
		.def("setSurfaceValue", &VolumeRenderer::SetSurfaceValue)
		.def("setSampleInterval", &VolumeRenderer::SetSampleInterval)
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