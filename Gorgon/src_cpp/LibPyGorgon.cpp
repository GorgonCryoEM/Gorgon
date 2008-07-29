#ifndef GORGON_LIB_PY_GORGON
#define GORGON_LIB_PY_GORGON

#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1

#include <Gorgon/SSECorrespondenceEngine.h>
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

BOOST_PYTHON_MODULE(libpyGORGON)
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
	
	
	class_< VolumeRenderer, bases<Renderer> >("VolumeRenderer", init<>())
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
		.def("getSurfaceValue", &VolumeRenderer::GetSurfaceValue)
		.def("get3DCoordinates", &VolumeRenderer::Get3DCoordinates)
		.def("setSurfaceValue", &VolumeRenderer::SetSurfaceValue)		
		.def("setSampleInterval", &VolumeRenderer::SetSampleInterval)
		.def("setCuttingPlane", &VolumeRenderer::SetCuttingPlane)
		.def("setViewingType", &VolumeRenderer::SetViewingType)
		.def("performBinarySkeletonizationJu2007", &VolumeRenderer::PerformBinarySkeletonizationJu2007, return_value_policy<manage_new_object>())		
		.def("performGrayscaleSkeletonizationAbeysinghe2008", &VolumeRenderer::PerformGrayscaleSkeletonizationAbeysinghe2008, return_value_policy<manage_new_object>())		
	;

	class_< MeshRenderer, bases<Renderer> >("MeshRenderer", init<>())
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

	class_< SSERenderer, bases<Renderer> >("SSERenderer", init<>())
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
		.def("setHelixColor", &SSERenderer::SetHelixColor)		
	;

	class_< CAlphaRenderer, bases<Renderer> >("CAlphaRenderer", init<>())
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
		.def("selectStartSeedRay", &InteractiveSkeletonEngine::SelectStartSeedRay)
		.def("selectEndSeed", &InteractiveSkeletonEngine::SelectEndSeed)
		.def("analyzePathRay", &InteractiveSkeletonEngine::AnalyzePathRay)
		.def("setIsoValue", &InteractiveSkeletonEngine::SetIsoValue)		
		.def("finalizeSkeleton", &InteractiveSkeletonEngine::FinalizeSkeleton)		
		.def("draw", &InteractiveSkeletonEngine::Draw)		
	;


	class_<SSECorrespondenceResult>("SSECorrespondenceResult", init<>())
		.def("getNodeString", &SSECorrespondenceResult::GetNodeString)
		.def("getCost", &SSECorrespondenceResult::GetCost)
		.def("getNodeCount", &SSECorrespondenceResult::GetNodeCount)
		.def("getSkeletonNode", &SSECorrespondenceResult::GetSkeletonNode)
		.def("nodeToHelix", &SSECorrespondenceResult::NodeToHelix)		
	;

	class_<SecondaryStructure>("SecondaryStructure", init<>())
		.def("isHelix", &SecondaryStructure::IsHelix)
		.def("isSheet", &SecondaryStructure::IsSheet)
		.def("getSerialNumber", &SecondaryStructure::GetSerialNumber)
		.def("getStartPosition", &SecondaryStructure::GetStartPosition)
		.def("getEndPosition", &SecondaryStructure::GetEndPosition)
		.def("getSecondaryStructureID", &SecondaryStructure::GetSecondaryStructureID)
	;

	class_<GeometricShape>("GeometricShape", init<>())
		.def("isHelix", &GeometricShape::IsHelix)
		.def("isSheet", &GeometricShape::IsSheet)
		.def("getCornerCell2", &GeometricShape::GetCornerCell2)
		.def("setColor", &GeometricShape::SetColor)
	;

	bool (SSECorrespondenceEngine::*SetConstant1)(char *, char *)	= &SSECorrespondenceEngine::SetConstant;
	bool (SSECorrespondenceEngine::*SetConstant2)(char *, double)	= &SSECorrespondenceEngine::SetConstant;
	bool (SSECorrespondenceEngine::*SetConstant3)(char *, int)		= &SSECorrespondenceEngine::SetConstant;
	bool (SSECorrespondenceEngine::*SetConstant4)(char *, bool)		= &SSECorrespondenceEngine::SetConstant;

	class_<SSECorrespondenceEngine>("SSECorrespondenceEngine", init<>())
		.def("setConstant", SetConstant1)
		.def("setConstant", SetConstant2)
		.def("setConstantInt", SetConstant3)
		.def("setConstantBool", SetConstant4)
        .def("setConstantsFromFile", &SSECorrespondenceEngine::SetConstantsFromFile)		
		.def("saveCorrespondenceToFile", &SSECorrespondenceEngine::SaveCorrespondenceToFile)
		.def("loadSkeletonGraph", &SSECorrespondenceEngine::LoadSkeletonGraph)
		.def("loadSequenceGraph", &SSECorrespondenceEngine::LoadSequenceGraph)
		.def("loadCorrespondenceFromFile", &SSECorrespondenceEngine::LoadCorrespondenceFromFile)
		.def("executeQuery", &SSECorrespondenceEngine::ExecuteQuery)
		.def("cleanupMemory", &SSECorrespondenceEngine::CleanupMemory)
		.def("getResult", &SSECorrespondenceEngine::GetResult)
		.def("getResultCount", &SSECorrespondenceEngine::GetResultCount)
		.def("getSupportedLoadFileFormats", &SSECorrespondenceEngine::GetSupportedLoadFileFormats)
		.def("getSupportedSaveFileFormats", &SSECorrespondenceEngine::GetSupportedSaveFileFormats)
		.def("getSkeletonSSE", &SSECorrespondenceEngine::GetSkeletonSSE, return_value_policy<manage_new_object>())
		.def("getSequenceSSE", &SSECorrespondenceEngine::GetSequenceSSE, return_value_policy<manage_new_object>())
		.def("getSkeletonSSECount", &SSECorrespondenceEngine::GetSkeletonSSECount)
		.def("getSequenceSSECount", &SSECorrespondenceEngine::GetSequenceSSECount)
		;

	
}



#endif
