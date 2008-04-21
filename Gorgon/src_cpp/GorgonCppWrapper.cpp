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
#include <MathTools/Vector3D.h>
#include <GraphMatch/PDBAtom.h>
#include <GraphMatch/LinkedNode.h>

#include <boost/python.hpp>

using namespace boost::python;
using namespace wustl_mm::Visualization;
using namespace wustl_mm::GraphMatch;

BOOST_PYTHON_MODULE(gorgon_cpp_wrapper)
{
	class_<Vector3DFloat>("Vector3DFloat", init<float, float, float>())
		.def("x", &Vector3DFloat::X)
		.def("y", &Vector3DFloat::Y)
		.def("z", &Vector3DFloat::Z)
	;

	class_<PDBAtom>("PDBAtom", init<>())
		.def("getSerial", &PDBAtom::GetSerial)
		.def("getName", &PDBAtom::GetName)
		.def("getAltLoc", &PDBAtom::GetAltLoc)
		.def("getResName", &PDBAtom::GetResName)
		.def("getChainId", &PDBAtom::GetChainId)
		.def("getResSeq", &PDBAtom::GetResSeq)
		.def("getICode", &PDBAtom::GetICode)
		.def("getPosition", &PDBAtom::GetPosition)
		.def("getOccupancy", &PDBAtom::GetOccupancy)
		.def("getTempFactor", &PDBAtom::GetTempFactor)
		.def("getElement", &PDBAtom::GetElement)
		.def("getCharge", &PDBAtom::GetCharge)
		.def("setSerial", &PDBAtom::SetSerial)
		.def("setName", &PDBAtom::SetName)
		.def("setAltLoc", &PDBAtom::SetAltLoc)
		.def("setResName", &PDBAtom::SetResName)
		.def("setChainId", &PDBAtom::SetChainId)
		.def("setResSeq", &PDBAtom::SetResSeq)
		.def("setICode", &PDBAtom::SetICode)
		.def("setPosition", &PDBAtom::SetPosition)
		.def("setOccupancy", &PDBAtom::SetOccupancy)
		.def("setTempFactor", &PDBAtom::SetTempFactor)
		.def("setElement", &PDBAtom::SetElement)
		.def("setCharge", &PDBAtom::SetCharge)
	;

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
		.def("get3DCoordinates", &Renderer::Get3DCoordinates)
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
		.def("get3DCoordinates", &VolumeRenderer::Get3DCoordinates)
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
		.def("get3DCoordinates", &MeshRenderer::Get3DCoordinates)
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
		.def("get3DCoordinates", &SSERenderer::Get3DCoordinates)
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
		.def("addAtom", &CAlphaRenderer::AddAtom)
		.def("get3DCoordinates", &CAlphaRenderer::Get3DCoordinates)
	;

	class_<InteractiveSkeletonEngine>("InteractiveSkeletonEngine", init<Volume *, NonManifoldMesh_Annotated *, float, float, float, int, int, int>())
		.def("selectSeed", &InteractiveSkeletonEngine::SelectSeed)
		.def("analyzePath", &InteractiveSkeletonEngine::AnalyzePath)		
	;


	bool (BackEndInterface::*SetConstant1)(char *, char *)	= &BackEndInterface::SetConstant;
	bool (BackEndInterface::*SetConstant2)(char *, double)	= &BackEndInterface::SetConstant;
	bool (BackEndInterface::*SetConstant3)(char *, int)		= &BackEndInterface::SetConstant;
	bool (BackEndInterface::*SetConstant4)(char *, bool)	= &BackEndInterface::SetConstant;


	class_<LinkedNode>("HelixCorrespondenceResult", init<>())
		.def("getNodeString", &LinkedNode::GetNodeString)
		.def("getCost", &LinkedNode::GetCost)
	;

	class_<BackEndInterface>("HelixCorrespondenceEngine", init<>())
		.def("setConstant", SetConstant1)
		.def("setConstant", SetConstant2)
		.def("setConstant", SetConstant3)
		.def("setConstant", SetConstant4)
        .def("setConstantsFromFile", &BackEndInterface::SetConstantsFromFile)		
		.def("loadSkeletonGraph", &BackEndInterface::LoadSkeletonGraph)
		.def("loadSequenceGraph", &BackEndInterface::LoadSequenceGraph)
		.def("executeQuery", &BackEndInterface::ExecuteQuery)
		.def("cleanupMemory", &BackEndInterface::CleanupMemory)
		.def("getResult", &BackEndInterface::GetResult, return_value_policy<manage_new_object>())
    ;

}

#endif