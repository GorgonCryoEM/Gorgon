#include "VolumeSkeletonizer.h"
#include <string> 
#include "..\MatlabInterface\Vector3D.h"

using namespace wustl_mm::GraySkeletonCPP;
using namespace wustl_mm::MatlabInterface;
using namespace std;

int main( int args, char * argv[] ) {
	
	Volume * vol = new Volume(30, 30, 30);		// The volume containing the data
	Volume * maskVol = new Volume(30,30,30);	// This contains a mask of the points we want to get the vectors of)
	int radius = 5;								// The radius of the gaussian mask to be used.

	// just put some random data points into the volume
	// Remember to keep an empty border the width of the radius.
	for(int x = radius; x < 30-radius; x++) {
		for(int y = radius; y < 30-radius; y++) {
			for(int z = radius; z < 30-radius; z++) {
				vol->setDataAt(x, y, z, rand()/RAND_MAX);
				maskVol->setDataAt(x, y, z, 1.0);
			}
		}
	}

	VolumeSkeletonizer * skeletonizer = new VolumeSkeletonizer(0,0,0);
	Vector3D * volumeGradient = skeletonizer->GetVolumeGradient2(vol);  // This contains the vectors containing the partial derivative information

	ProbabilityDistribution3D gaussianFilter;
	gaussianFilter.radius = radius;
	skeletonizer->math->GetBinomialDistribution(gaussianFilter);		// This gives you a 3D gaussian mask.
	
	
	EigenResults3D * eigens = skeletonizer->GetEigenResults2(maskVol, volumeGradient, gaussianFilter, radius, true);  // This contains the eigen vectors.

	// eigens[vol->getIndex[10,10,10]]   can be used the access the eigen vectors and values at location {10,10,10}

	skeletonizer->WriteEigenResultsToVRMLFile(vol, maskVol, maskVol, eigens, "temp.wrl", false);	// Writes the eigen values into a vrml file so you can visualize it.

	delete [] eigens;
	delete [] volumeGradient;
	delete skeletonizer;
	delete vol;

	

}