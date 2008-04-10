#ifndef GORGON_GORGON_CPP_WRAPPER_CPP
#define GORGON_GORGON_CPP_WRAPPER_CPP

#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1

#include <GraphMatch/BackEndInterface.h>
#include <Gorgon/VolumeRenderer.h>
#include <Gorgon/MeshRenderer.h>
#include <Gorgon/SSERenderer.h>
#include <Gorgon/Renderer.h>

#include <boost/python.hpp>

using namespace boost::python;
using namespace wustl_mm::Visualization;
using namespace wustl_mm::GraphMatch;

BOOST_PYTHON_MODULE(gorgon_cpp_wrapper)
{
	class_<Volume>("Volume", init<int,int,int>())
	;

	class_<Renderer>("Renderer", init<>())
		.def("draw", &Renderer::Draw)
		.def("drawBoundingBox", &Renderer::DrawBoundingBox)
		.def("loadFile", &Renderer::LoadFile)
		.def("unload", &Renderer::Unload)
		.def("select", &Renderer::Select)
		.def("getSupportedLoadFileFormats", &Renderer::GetSupportedLoadFileFormats)
		.def("getSupportedSaveFileFormats", &Renderer::GetSupportedSaveFileFormats)
		.def("getMin", &Renderer::GetMin)
		.def("getMax", &Renderer::GetMax)
	;

	class_<VolumeRenderer, bases<Renderer>>("VolumeRenderer", init<>())
		.def("draw", &VolumeRenderer::Draw)
		.def("drawBoundingBox", &VolumeRenderer::DrawBoundingBox)
		.def("loadFile", &VolumeRenderer::LoadFile)
		.def("unload", &VolumeRenderer::Unload)
		.def("select", &VolumeRenderer::Select)
		.def("getSupportedLoadFileFormats", &VolumeRenderer::GetSupportedLoadFileFormats)
		.def("getSupportedSaveFileFormats", &VolumeRenderer::GetSupportedSaveFileFormats)
		.def("getMin", &VolumeRenderer::GetMin)
		.def("getMax", &VolumeRenderer::GetMax)
		.def("getMinDensity", &VolumeRenderer::GetMinDensity)
		.def("getMaxDensity", &VolumeRenderer::GetMaxDensity)
		.def("setSurfaceValue", &VolumeRenderer::SetSurfaceValue)
		.def("setSampleInterval", &VolumeRenderer::SetSampleInterval)
		.def("performBinarySkeletonizationJu2007", &VolumeRenderer::PerformBinarySkeletonizationJu2007, return_value_policy<manage_new_object>())		
		.def("performGrayscaleSkeletonizationAbeysinghe2008", &VolumeRenderer::PerformGrayscaleSkeletonizationAbeysinghe2008, return_value_policy<manage_new_object>())		
	;

	class_<MeshRenderer, bases<Renderer>>("MeshRenderer", init<>())
		.def("draw", &MeshRenderer::Draw)
		.def("drawBoundingBox", &MeshRenderer::DrawBoundingBox)
		.def("loadFile", &MeshRenderer::LoadFile)
		.def("loadVolume", &MeshRenderer::LoadVolume)
		.def("unload", &MeshRenderer::Unload)
		.def("select", &MeshRenderer::Select)
		.def("performSmoothLaplacian", &MeshRenderer::PerformSmoothLaplacian)
		.def("getSupportedLoadFileFormats", &MeshRenderer::GetSupportedLoadFileFormats)
		.def("getSupportedSaveFileFormats", &MeshRenderer::GetSupportedSaveFileFormats)
		.def("getMin", &MeshRenderer::GetMin)
		.def("getMax", &MeshRenderer::GetMax)
	;

	class_<SSERenderer, bases<Renderer>>("SSERenderer", init<>())
		.def("draw", &SSERenderer::Draw)
		.def("drawBoundingBox", &SSERenderer::DrawBoundingBox)
		.def("loadHelixFile", &SSERenderer::LoadHelixFile)
		.def("loadSheetFile", &SSERenderer::LoadSheetFile)
		.def("unload", &SSERenderer::Unload)
		.def("select", &SSERenderer::Select)
		.def("getSupportedLoadFileFormats", &SSERenderer::GetSupportedLoadFileFormats)
		.def("getSupportedSaveFileFormats", &SSERenderer::GetSupportedSaveFileFormats)
		.def("getMin", &SSERenderer::GetMin)
		.def("getMax", &SSERenderer::GetMax)
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