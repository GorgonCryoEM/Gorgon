#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1

#include <GraphMatch/BackEndInterface.h>
#include <boost/python.hpp>

using namespace boost::python;
using namespace wustl_mm::GraphMatch;

BOOST_PYTHON_MODULE(TopologyHunter)
{
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