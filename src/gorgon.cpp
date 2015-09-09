/*
 * gorgon.cpp
 *
 *  Created on: Aug 13, 2015
 *      Author: shadow_walker
 */

#include <boost/python.hpp>

#include <MathTools/Vector3D.h>
#include <VolumeRenderer.h>

using namespace boost::python;

using namespace wustl_mm::Visualization;
using namespace wustl_mm::MathTools;

BOOST_PYTHON_MODULE(libpygorgon)
{
	class_<Volume>("Volume", init<int,int,int>())
//			.def("getOriginX", &Volume::getOriginX)
//			.def("getOriginY", &Volume::getOriginY)
//			.def("getOriginZ", &Volume::getOriginZ)
//			.def("getSizeX", &Volume::getSizeX)
//			.def("getSizeY", &Volume::getSizeY)
//			.def("getSizeZ", &Volume::getSizeZ)
//			.def("getSpacingX", &Volume::getSpacingX)
//			.def("getSpacingY", &Volume::getSpacingY)
//			.def("getSpacingZ", &Volume::getSpacingZ)
			.def("getMin", &Volume::getMin)
			.def("getMax", &Volume::getMax)
//			.def("getDataAt", getDataAt1)
			.def("buildHistogram", &Volume::buildHistogram)
			.def("getHistogramBinValue", &Volume::getHistogramBinValue)

		;
	
	class_<Renderer>("Renderer", init<>())
//			.def("draw", &Renderer::Draw)
//			.def("drawBoundingBox", &Renderer::DrawBoundingBox)
//			.def("loadFile", &Renderer::LoadFile)
//			.def("saveFile", &Renderer::SaveFile)
			.def("unload", &Renderer::Unload)
//			.def("selectionRotate", &Renderer::SelectionRotate)
//			.def("selectionObjectCount", &Renderer::SelectionObjectCount)
//			.def("selectionCenterOfMass", &Renderer::SelectionCenterOfMass)
//			.def("selectionMove", &Renderer::SelectionMove)
//			.def("selectionClear", &Renderer::SelectionClear)		
//			.def("selectionToggle", &Renderer::SelectionToggle)	
//			.def("getSupportedLoadFileFormats", &Renderer::GetSupportedLoadFileFormats)
//			.def("getSupportedSaveFileFormats", &Renderer::GetSupportedSaveFileFormats)
//			.def("getMin", &Renderer::GetMin)
//			.def("getMax", &Renderer::GetMax)
//			.def("get3DCoordinates", &Renderer::Get3DCoordinates)
//			.def("setCuttingPlane", &Renderer::SetCuttingPlane)	
//			.def("setSpacing", &Renderer::SetSpacing)
//			.def("getSpacingX", &Renderer::GetSpacingX)
//			.def("getSpacingY", &Renderer::GetSpacingY)
//			.def("getSpacingZ", &Renderer::GetSpacingZ)
//			.def("setOrigin", &Renderer::SetOrigin)
//			.def("getOriginX", &Renderer::GetOriginX)
//			.def("getOriginY", &Renderer::GetOriginY)
//			.def("getOriginZ", &Renderer::GetOriginZ)
//			.def("setDisplayStyle", &Renderer::SetDisplayStyle)
//			.def("setObjectSpecificColoring", &Renderer::SetObjectSpecificColoring)
//			.def("updateBoundingBox", &Renderer::UpdateBoundingBox)
			.def("clearOtherHighlights", &Renderer::ClearOtherHighlights)
		;
	
	class_< VolumeRenderer, bases<Renderer> >("VolumeRenderer", init<>())		
			.def("draw", &VolumeRenderer::Draw)
			.def("drawBoundingBox", &VolumeRenderer::DrawBoundingBox)
			.def("enableDraw", &VolumeRenderer::EnableDraw)
			.def("loadFile", &VolumeRenderer::LoadFile)
//			.def("loadFileRAW", &VolumeRenderer::LoadFileRAW)
			.def("saveFile", &VolumeRenderer::SaveFile)
//			.def("unload", &VolumeRenderer::Unload)
//			.def("selectionRotate", &VolumeRenderer::SelectionRotate)
//			.def("selectionObjectCount", &VolumeRenderer::SelectionObjectCount)
//			.def("selectionCenterOfMass", &VolumeRenderer::SelectionCenterOfMass)
//			.def("selectionMove", &VolumeRenderer::SelectionMove)
			.def("selectionClear", &VolumeRenderer::SelectionClear)		
//			.def("selectionToggle", &VolumeRenderer::SelectionToggle)	
			.def("getSupportedLoadFileFormats", &VolumeRenderer::GetSupportedLoadFileFormats)
			.def("getSupportedSaveFileFormats", &VolumeRenderer::GetSupportedSaveFileFormats)
			.def("getMin", &VolumeRenderer::GetMin)
			.def("getMax", &VolumeRenderer::GetMax)
			.def("getMinDensity", &VolumeRenderer::GetMinDensity)
			.def("getMaxDensity", &VolumeRenderer::GetMaxDensity)
			.def("getVolume", &VolumeRenderer::GetVolume, return_value_policy<reference_existing_object>())		
//			.def("getSurfaceValue", &VolumeRenderer::GetSurfaceValue)
//			.def("get3DCoordinates", &VolumeRenderer::Get3DCoordinates)
			.def("setSurfaceValue", &VolumeRenderer::SetSurfaceValue)		
//			.def("setMaxSurfaceValue", &VolumeRenderer::SetMaxSurfaceValue)		
			.def("setSampleInterval", &VolumeRenderer::SetSampleInterval)
			.def("setDisplayRadius", &VolumeRenderer::SetDisplayRadius)
			.def("setDisplayRadiusOrigin", &VolumeRenderer::SetDisplayRadiusOrigin)		
			.def("setCuttingPlane", &VolumeRenderer::SetCuttingPlane)
			.def("setViewingType", &VolumeRenderer::SetViewingType)
//			.def("normalizeVolume", &VolumeRenderer::NormalizeVolume)
//			.def("downsampleVolume", &VolumeRenderer::DownsampleVolume)
//			.def("cropVolume", &VolumeRenderer::CropVolume)
//			.def("performBinarySkeletonizationJu2007", &VolumeRenderer::PerformBinarySkeletonizationJu2007, return_value_policy<reference_existing_object>())		
//			.def("performGrayscaleSkeletonizationAbeysinghe2008", &VolumeRenderer::PerformGrayscaleSkeletonizationAbeysinghe2008, return_value_policy<reference_existing_object>())		
//			.def("performPreservingGrayscaleSkeletonizationAbeysinghe2008", &VolumeRenderer::PerformPreservingGrayscaleSkeletonizationAbeysinghe2008, return_value_policy<reference_existing_object>())		
			.def("setSpacing", &VolumeRenderer::SetSpacing)
			.def("getSpacingX", &VolumeRenderer::GetSpacingX)
			.def("getSpacingY", &VolumeRenderer::GetSpacingY)
			.def("getSpacingZ", &VolumeRenderer::GetSpacingZ)
			.def("setOrigin", &VolumeRenderer::SetOrigin)
			.def("getOriginX", &VolumeRenderer::GetOriginX)
			.def("getOriginY", &VolumeRenderer::GetOriginY)
			.def("getOriginZ", &VolumeRenderer::GetOriginZ)
//			.def("useDisplayRadius", &VolumeRenderer::UseDisplayRadius)
//			.def("performSmoothLaplacian", &VolumeRenderer::PerformSmoothLaplacian)		
			.def("setDisplayStyle", &VolumeRenderer::SetDisplayStyle)
//			.def("setObjectSpecificColoring", &VolumeRenderer::SetObjectSpecificColoring)
//			.def("updateBoundingBox", &VolumeRenderer::UpdateBoundingBox)
		;
}
