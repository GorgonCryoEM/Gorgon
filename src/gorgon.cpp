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
	
	class_< VolumeRenderer, bases<Renderer> >("VolumeRenderer", init<>())		
//			.def("draw", &VolumeRenderer::Draw)
//			.def("drawBoundingBox", &VolumeRenderer::DrawBoundingBox)
//			.def("enableDraw", &VolumeRenderer::EnableDraw)
//			.def("loadFile", &VolumeRenderer::LoadFile)
//			.def("loadFileRAW", &VolumeRenderer::LoadFileRAW)
//			.def("saveFile", &VolumeRenderer::SaveFile)
//			.def("unload", &VolumeRenderer::Unload)
//			.def("selectionRotate", &VolumeRenderer::SelectionRotate)
//			.def("selectionObjectCount", &VolumeRenderer::SelectionObjectCount)
//			.def("selectionCenterOfMass", &VolumeRenderer::SelectionCenterOfMass)
//			.def("selectionMove", &VolumeRenderer::SelectionMove)
//			.def("selectionClear", &VolumeRenderer::SelectionClear)		
//			.def("selectionToggle", &VolumeRenderer::SelectionToggle)	
//			.def("getSupportedLoadFileFormats", &VolumeRenderer::GetSupportedLoadFileFormats)
//			.def("getSupportedSaveFileFormats", &VolumeRenderer::GetSupportedSaveFileFormats)
//			.def("getMin", &VolumeRenderer::GetMin)
//			.def("getMax", &VolumeRenderer::GetMax)
//			.def("getMinDensity", &VolumeRenderer::GetMinDensity)
//			.def("getMaxDensity", &VolumeRenderer::GetMaxDensity)
//			.def("getVolume", &VolumeRenderer::GetVolume, return_value_policy<reference_existing_object>())		
//			.def("getSurfaceValue", &VolumeRenderer::GetSurfaceValue)
//			.def("get3DCoordinates", &VolumeRenderer::Get3DCoordinates)
//			.def("setSurfaceValue", &VolumeRenderer::SetSurfaceValue)		
//			.def("setMaxSurfaceValue", &VolumeRenderer::SetMaxSurfaceValue)		
//			.def("setSampleInterval", &VolumeRenderer::SetSampleInterval)
//			.def("setDisplayRadius", &VolumeRenderer::SetDisplayRadius)
//			.def("setDisplayRadiusOrigin", &VolumeRenderer::SetDisplayRadiusOrigin)		
//			.def("setCuttingPlane", &VolumeRenderer::SetCuttingPlane)
//			.def("setViewingType", &VolumeRenderer::SetViewingType)
//			.def("normalizeVolume", &VolumeRenderer::NormalizeVolume)
//			.def("downsampleVolume", &VolumeRenderer::DownsampleVolume)
//			.def("cropVolume", &VolumeRenderer::CropVolume)
//			.def("performBinarySkeletonizationJu2007", &VolumeRenderer::PerformBinarySkeletonizationJu2007, return_value_policy<reference_existing_object>())		
//			.def("performGrayscaleSkeletonizationAbeysinghe2008", &VolumeRenderer::PerformGrayscaleSkeletonizationAbeysinghe2008, return_value_policy<reference_existing_object>())		
//			.def("performPreservingGrayscaleSkeletonizationAbeysinghe2008", &VolumeRenderer::PerformPreservingGrayscaleSkeletonizationAbeysinghe2008, return_value_policy<reference_existing_object>())		
//			.def("setSpacing", &VolumeRenderer::SetSpacing)
//			.def("getSpacingX", &VolumeRenderer::GetSpacingX)
//			.def("getSpacingY", &VolumeRenderer::GetSpacingY)
//			.def("getSpacingZ", &VolumeRenderer::GetSpacingZ)
//			.def("setOrigin", &VolumeRenderer::SetOrigin)
//			.def("getOriginX", &VolumeRenderer::GetOriginX)
//			.def("getOriginY", &VolumeRenderer::GetOriginY)
//			.def("getOriginZ", &VolumeRenderer::GetOriginZ)
//			.def("useDisplayRadius", &VolumeRenderer::UseDisplayRadius)
//			.def("performSmoothLaplacian", &VolumeRenderer::PerformSmoothLaplacian)		
//			.def("setDisplayStyle", &VolumeRenderer::SetDisplayStyle)
//			.def("setObjectSpecificColoring", &VolumeRenderer::SetObjectSpecificColoring)
//			.def("updateBoundingBox", &VolumeRenderer::UpdateBoundingBox)
		;
}
