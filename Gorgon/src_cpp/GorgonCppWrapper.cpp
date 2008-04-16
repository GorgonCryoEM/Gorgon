#ifndef GORGON_GORGON_CPP_WRAPPER_CPP
#define GORGON_GORGON_CPP_WRAPPER_CPP

#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1

#include <GraphMatch/BackEndInterface.h>
#include <Gorgon/VolumeRenderer.h>
#include <Gorgon/MeshRenderer.h>
#include <Gorgon/SSERenderer.h>
#include <Gorgon/Renderer.h>
#include <Gorgon/InteractiveSkeletonEngine.h>
#include <Gorgon/CAlphaRenderer.h>

#include <boost/python.hpp>

using namespace boost::python;
using namespace wustl_mm::Visualization;
using namespace wustl_mm::GraphMatch;

BOOST_PYTHON_MODULE(gorgon_cpp_wrapper)
{
	class_<Volume>("Volume", init<int,int,int>())
	;

	class_<NonManifoldMesh_Annotated>("NonManifoldMesh_Annotated", init<>())
	;

	class_<Renderer>("Renderer", init<>())
		.def("draw", &Renderer::Draw)
		.def("drawBoundingBox", &Renderer::DrawBoundingBox)
		.def("loadFile", &Renderer::LoadFile)
		.def("saveFile", &Renderer::SaveFile)
		.def("unload", &Renderer::Unload)
		.def("select", &Renderer::Select)
		.def("getSupportedLoadFileFormats", &Renderer::GetSupportedLoadFileFormats)
		.def("getSupportedSaveFileFormats", &Renderer::GetSupportedSaveFileFormats)
		.def("getMin", &Renderer::GetMin)
		.def("getMax", &Renderer::GetMax)
		.def("setCuttingPlane", &Renderer::SetCuttingPlane)
		
	;

	class_<VolumeRenderer, bases<Renderer>>("VolumeRenderer", init<>())
		.def("draw", &VolumeRenderer::Draw)
		.def("drawBoundingBox", &VolumeRenderer::DrawBoundingBox)
		.def("loadFile", &VolumeRenderer::LoadFile)
		.def("saveFile", &VolumeRenderer::SaveFile)
		.def("unload", &VolumeRenderer::Unload)
		.def("select", &VolumeRenderer::Select)
		.def("getSupportedLoadFileFormats", &VolumeRenderer::GetSupportedLoadFileFormats)
		.def("getSupportedSaveFileFormats", &VolumeRenderer::GetSupportedSaveFileFormats)
		.def("getMin", &VolumeRenderer::GetMin)
		.def("getMax", &VolumeRenderer::GetMax)
		.def("getMinDensity", &VolumeRenderer::GetMinDensity)
		.def("getMaxDensity", &VolumeRenderer::GetMaxDensity)
		.def("getVolume", &VolumeRenderer::GetVolume, return_value_policy<manage_new_object>())		
		.def("setSurfaceValue", &VolumeRenderer::SetSurfaceValue)
		.def("setSampleInterval", &VolumeRenderer::SetSampleInterval)
		.def("setCuttingPlane", &VolumeRenderer::SetCuttingPlane)
		.def("setViewingType", &VolumeRenderer::SetViewingType)
		.def("performBinarySkeletonizationJu2007", &VolumeRenderer::PerformBinarySkeletonizationJu2007, return_value_policy<manage_new_object>())		
		.def("performGrayscaleSkeletonizationAbeysinghe2008", &VolumeRenderer::PerformGrayscaleSkeletonizationAbeysinghe2008, return_value_policy<manage_new_object>())		
	;

	class_<MeshRenderer, bases<Renderer>>("MeshRenderer", init<>())
		.def("draw", &MeshRenderer::Draw)
		.def("drawBoundingBox", &MeshRenderer::DrawBoundingBox)
		.def("loadFile", &MeshRenderer::LoadFile)
		.def("loadVolume", &MeshRenderer::LoadVolume)
		.def("saveFile", &MeshRenderer::SaveFile)
		.def("unload", &MeshRenderer::Unload)
		.def("select", &MeshRenderer::Select)		
		.def("performSmoothLaplacian", &MeshRenderer::PerformSmoothLaplacian)
		.def("getSupportedLoadFileFormats", &MeshRenderer::GetSupportedLoadFileFormats)
		.def("getSupportedSaveFileFormats", &MeshRenderer::GetSupportedSaveFileFormats)
		.def("getMin", &MeshRenderer::GetMin)
		.def("getMax", &MeshRenderer::GetMax)
		.def("getMesh", &MeshRenderer::GetMesh, return_value_policy<manage_new_object>())
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

	class_<CAlphaRenderer, bases<Renderer>>("CAlphaRenderer", init<>())
		.def("draw", &CAlphaRenderer::Draw)
		.def("drawBoundingBox", &CAlphaRenderer::DrawBoundingBox)
		.def("loadFile", &CAlphaRenderer::LoadFile)
		.def("saveFile", &CAlphaRenderer::SaveFile)
		.def("unload", &CAlphaRenderer::Unload)
		.def("select", &CAlphaRenderer::Select)		
		.def("getSupportedLoadFileFormats", &CAlphaRenderer::GetSupportedLoadFileFormats)
		.def("getSupportedSaveFileFormats", &CAlphaRenderer::GetSupportedSaveFileFormats)
		.def("getMin", &CAlphaRenderer::GetMin)
		.def("getMax", &CAlphaRenderer::GetMax)
	;

	class_<InteractiveSkeletonEngine>("InteractiveSkeletonEngine", init<Volume *, NonManifoldMesh_Annotated *, float, float, float, int, int, int>())
		.def("selectSeed", &InteractiveSkeletonEngine::SelectSeed)
		.def("analyzePath", &InteractiveSkeletonEngine::AnalyzePath)		
	;

	/* Dont use this... will end up changing this
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
	*/

}

#endif