// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A dummy CPP file for the Protein Morph Module.

// CVS Meta Information: 
//   $Source: /project/mm/cvs/graphics/ssa1/source/ProteinMorph/ProteinMorph.cpp,v $
//   $Revision: 1.22 $
//   $Date: 2010/07/22 21:09:07 $
//   $Author: heiderp $
//   $State: Exp $
//
// History Log: 
//   $Log: ProteinMorph.cpp,v $
//   Revision 1.22  2010/07/22 21:09:07  heiderp
//   Minor updates. Mostly commenting and removing extra material from CurveDeformer.h
//
//   Revision 1.21  2010/07/19 17:38:31  heiderp
//   Flexible fitting.
//
//   Revision 1.20  2010/06/08 22:00:05  ssa1
//   Fixing performance issue where changing color took time.
//
//   Revision 1.19  2010/04/27 21:10:17  ssa1
//   Implementing Cost-matrix based SSE Registration and performance optimizations on graph construction
//
//   Revision 1.18  2010/04/27 17:30:54  ssa1
//   SSE Registration search by first finding all cliques, and then finding the matching.
//
//   Revision 1.17  2010/04/04 19:05:51  ssa1
//   Fixing misc bugs, and redoing sheet visualization mechanism
//
//   Revision 1.16  2010/02/23 21:19:08  ssa1
//   Better correspondence search
//
//   Revision 1.15  2010/02/11 23:20:47  ssa1
//   Flexible fitting algorithm #5 - Better scoring function which scales based on depth of tree and local distance
//
//   Revision 1.14  2009/12/21 22:03:32  ssa1
//   Checking in FFTW windows binaries
//
//   Revision 1.13  2009/11/30 04:23:44  ssa1
//   Triangle based A* search for flexible fitting
//
//   Revision 1.12  2009/11/19 18:19:24  ssa1
//   Improved flexible fitting.. (Split nodes to guarantee direction)
//
//   Revision 1.11  2009/11/04 20:29:38  ssa1
//   Implementing Triangle based clique search and chain based flexible fitting.
//
//   Revision 1.10  2009/10/13 18:09:34  ssa1
//   Refactoring Volume.h
//
//   Revision 1.9  2009/09/02 19:06:13  ssa1
//   Working towards flexible fitting
//
//   Revision 1.8  2009/08/26 14:58:55  ssa1
//   Adding in Flexible fitting clique search
//
//   Revision 1.7  2008/11/13 15:49:00  ssa1
//   Performance improvements for creating a mesh from marching cubes
//
//   Revision 1.6  2008/09/29 16:43:15  ssa1
//   Adding in CVS meta information
//

#ifndef PROTEINMORPH_SWITCHBOARD_CPP
#define PROTEINMORPH_SWITCHBOARD_CPP

#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1

#include <Foundation/TimeManager.h>
//#include <SkeletonMaker/volume.h>
#include <Readers/VolumeReaderATOM.h>
#include <string>
#include <Foundation/StringUtils.h>
#include "SSECorrespondenceFinder.h"
#include "SSECorrespondenceFeature.h"
#include "NonManifoldMesh.h"
#include "NonManifoldMesh2.h"
#include <MathTools/MathLib.h>
#include <Foundation/PriorityQueue.h>
#include "CurveDeformer.h"
#include <math.h>
//#include <Readers/reader.h>

#define PI 3.14159265



using namespace std;
//using namespace GraySkeletonCPP;
using namespace wustl_mm::Protein_Morph;
using namespace Foundation;
using namespace MathTools;
using namespace SkeletonMaker;

namespace wustl_mm {
	namespace Protein_Morph {

		const int DO_NOTHING = -1;
		const int DO_SIMPLIFY = 1;



		void DisplayInputFormat(int function) {
			switch(function) {
				case DO_NOTHING:
					printf("ProteinMorph.exe [function] {[args]}*\n\n");
					printf("\t[function = %i]    - Simplify the skeleton\n", DO_SIMPLIFY);
					break;
				case DO_SIMPLIFY:
					printf("ProteinMorph.exe %i [skeleton] [helices] [sheets] [fixedPoints] [outFile]\n\n", DO_SIMPLIFY);
					printf("\t[skeleton]   -The ATOM file containing the skeletal voxels\n");
					printf("\t[helices]    -The ATOM file containing the helix voxels\n");
					printf("\t[sheets]     -The ATOM file containing the sheet voxels\n");
					printf("\t[fixedPoints]-The FIX file containing the fixed voxel indices\n");
					printf("\t[outFile]    -The name of the output file\n");
					break;
				default:
					DisplayInputFormat(DO_NOTHING);
			}
		}

		void DoSimplify(string skeleton, string helix, string sheet, string fix, string outFile) {
			string outPath = outFile.substr(0, outFile.rfind("."));	
			Volume * skeletonVol = VolumeReaderATOM::LoadVolume(skeleton, fix, helix);
			//Volume * helixVol = VolumeReaderATOM::LoadVolume(helix, "", skeletonVol->getSizeX(), skeletonVol->getSizeY(), skeletonVol->getSizeZ());
			Volume * sheetVol = VolumeReaderATOM::LoadVolume(sheet, "", "", skeletonVol->getSizeX(), skeletonVol->getSizeY(), skeletonVol->getSizeZ());
			//NonManifoldMesh * mesh = new ProteinMesh(skeletonVol, helixVol, sheetVol);
			NonManifoldMesh_NoTags * mesh = new NonManifoldMesh_NoTags(skeletonVol);
			NonManifoldMesh_NoTags * smoothedMesh = mesh->SmoothLaplacian(0.5);
			NonManifoldMesh_NoTags * smoothedMesh2 = smoothedMesh->SmoothLaplacian(0.5);
			NonManifoldMesh_NoTags * smoothedMesh3 = smoothedMesh2->SmoothLaplacian(0.5);
			mesh->ToOffCells(outPath + "-Skeleton.off");
			smoothedMesh->ToOffCells(outPath + "-Skeleton-Smoothed.off");
			smoothedMesh2->ToOffCells(outPath + "-Skeleton-Smoothed2.off");
			smoothedMesh3->ToOffCells(outPath + "-Skeleton-Smoothed3.off");
			smoothedMesh3->ToMathematicaFile(outPath + "-Skeleton-Smoothed.nb");
			delete mesh;
			delete smoothedMesh;
			delete smoothedMesh2;
			delete smoothedMesh3;
			delete skeletonVol;
			//delete helixVol;
			delete sheetVol;
		}
	}
}

vector<Vec3F> genSpring(int numPoints, int numPeriods, int magnitude)
{
	vector<Vec3F> points(numPoints);
	for(int i = 1; i <= numPoints; ++i)
	{
		float x = magnitude*sin(2*PI*((float)i/((float)numPoints/(float)numPeriods)));
		float y = magnitude*cos(2*PI*((float)i/((float)numPoints/(float)numPeriods)));
		points[i-1] = Vec3F(x, y, i);
	}
	return points;
}

// Trim the space out of head and tail
std::string trimString(std::string const& str)
{
    std::size_t first = str.find_first_not_of(' ');    

    if( first == std::string::npos )
    {
        std::cout<<"Try to trim a space only string...";
        // Return an empty string
        return "";
    }

    std::size_t last  = str.find_last_not_of(' ');

    return str.substr(first, last-first+1);
}

// Parse an input string by given character. Put the parsed result into a given vector
void ParseByCharater(std::string input, std::vector<std::string> & tokens, char flag)
{
    std::istringstream ss(input);
    std::string token;

    while(std::getline(ss, token, flag)) 
    {
        if(token.size() > 0)
        {
            tokens.push_back( trimString(token) );
        }        
    }
}

int main( int args, char * argv[] ) {
	//NonManifoldMesh_NoTags * m = NonManifoldMesh_NoTags::LoadOffFile("C:\\_WUSTL\\sasakthi\\research\\source\\Gorgon\\src_py\\temp\\Apple.off");
	//int function = DO_NOTHING;
	//bool error = true;

	//if(args >= 2) {
	//	function = StringUtils::StringToInt(argv[1]);
	//	switch(function) {
	//		case DO_SIMPLIFY:
	//			if(args == 7) {
	//				DoSimplify(argv[2], argv[3], argv[4], argv[5], argv[6]);
	//				error = false;
	//			}
	//			break;
	//	}
	//} 
	//
	//if(error) {
	//	DisplayInputFormat(function);
	//}

	/*vector<SSECorrespondenceFeature> fl1, fl2;


	fl1.push_back(SSECorrespondenceFeature(Vec3F(1,0,0), Vec3F(4,0,0)));
	fl1.push_back(SSECorrespondenceFeature(Vec3F(0,1,0), Vec3F(0,4,0)));
	fl1.push_back(SSECorrespondenceFeature(Vec3F(1,5,0), Vec3F(4,5,0)));
	fl1.push_back(SSECorrespondenceFeature(Vec3F(5,1,0), Vec3F(5,4,0)));

		
	fl2.push_back(SSECorrespondenceFeature(Vec3F(1,0,1), Vec3F(4,0,1)));
	fl2.push_back(SSECorrespondenceFeature(Vec3F(0,1,1), Vec3F(0,4,1)));
	fl2.push_back(SSECorrespondenceFeature(Vec3F(1,5,1), Vec3F(4,5,1)));
	fl2.push_back(SSECorrespondenceFeature(Vec3F(5,1,1), Vec3F(5,4,1)));

	fl1.push_back(SSECorrespondenceFeature(Vec3F(-2.36, -4.82, -14.82), Vec3F(8.02, 5.66, -13.65)));
	fl1.push_back(SSECorrespondenceFeature(Vec3F(8.14, -2.94, -7.86), Vec3F(-4.29, -6.07, 0.87)));
	fl1.push_back(SSECorrespondenceFeature(Vec3F(-0.47, 9.74, -4.92), Vec3F(11.38, 4.39, 5.46)));
	fl1.push_back(SSECorrespondenceFeature(Vec3F(0.56, -8.16, 6.86), Vec3F(2.45, -13.67, 4.68)));

	fl2.push_back(SSECorrespondenceFeature(Vec3F(-2.15, -4.40, -14.25), Vec3F(8.23, 6.08, -13.08)));
	fl2.push_back(SSECorrespondenceFeature(Vec3F(3.26, -5.05, -12.20), Vec3F(-4.07, -5.65, 1.44)));
	fl2.push_back(SSECorrespondenceFeature(Vec3F(-0.26, 10.16, -4.35), Vec3F(11.59, 4.81, 6.03)));
	fl2.push_back(SSECorrespondenceFeature(Vec3F(0.77, -7.74, 7.43), Vec3F(2.67, -13.25, 5.25)));

	SSECorrespondenceFinder finder;
	finder.InitializeFeatures(fl1, fl2);
	finder.InitializeConstants(
		StringUtils::StringToDouble(argv[6]), 
		StringUtils::StringToDouble(argv[7]), 
		StringUtils::StringToDouble(argv[8]), 
		StringUtils::StringToDouble(argv[9]), 
		StringUtils::StringToDouble(argv[10]),
		StringUtils::StringToDouble(argv[11]), 
		StringUtils::StringToDouble(argv[12]), 
		StringUtils::StringToDouble(argv[13]), 
		StringUtils::StringToDouble(argv[14]), 
		StringUtils::StringToDouble(argv[15]), 
		StringUtils::StringToInt(argv[16]));
	finder.GetAStarTriangleBasedFeatureCorrespondence(true, true);
	return 0;*/

	SSECorrespondenceFinder finder;

    //bool useDirection = (StringUtils::StringToInt(argv[2]) == 1);
    //bool multipleSearch = (StringUtils::StringToInt(argv[3]) == 1);
    //int smallestAStarCliqueSize = StringUtils::StringToInt(argv[18]);
    //printf("Smallest clique %d\n", smallestAStarCliqueSize);


    cout<<"Begin to process in Gorgon...\n";

    //string testFile1 = "./Data/HD-PDB-Selective/3tgl_A.pdb";
    //string testFile2 = "./Data/HD-PDB-Selective/4tgl_A.pdb";

    //std::string inputFile = "./Data/TimingBatch.txt";
    std::string dataDir   = "./Test_Data/";
    std::string fileExt   = ".pdb";

    //std::vector< std::vector<std::string> > fileList;

    // Read in all the batch data from AccuracyBatch.txt
    // The information in the file has to be line by line, in the order above.
    // The angles in the AccuracyBatch file are measured by degree

    //std::ifstream file(inputFile);
    //std::string line;

    //if( !file.is_open() )
    //    std::cout<<"Unable to open "<<inputFile<<"...\n";
    //else
    //{
    //    std::cout<<"Read in the file...\n";

    //    while( std::getline(file, line) )
    //    {
    //        // Protect from empty lines
    //        if( "" == line )
    //            continue;

    //        std::vector<std::string> tokens;
    //        ParseByCharater(line, tokens, ':');

    //        if("fileName" == tokens[0])
    //        {
    //            // Parse each parameter out by space
    //            std::vector<std::string> content;
    //            ParseByCharater(tokens[1], content, ' ');

    //            std::vector<std::string> temp;
    //            for(int i = 0; i < (int)content.size(); ++i)
    //            {
    //                temp.push_back( content[i] );
    //            }
    //            fileList.push_back(temp);
    //        }

    //    }

    //    std::cout<<"Finish reading in the file!\n";
    //}

    //file.close();   

    //fileList.push_back(string(argv[1]));

    //int i = 0;
    //while(i < fileList.size())
    //{

    //cout<<argv[1]<<" "<<argv[3]<<"\n";

//        std::cout<<"\n(+) Begin to process "<<argv[1]<<" & "<<argv[3]<<".\n";

        //std::string file1FullPath = dataDir + string(argv[1]) + fileExt;
        //std::string file2FullPath = dataDir + string(argv[3]) + fileExt;

    std::string file1FullPath = dataDir + "4ake_A" + fileExt; // 1j4p_A
    std::string file2FullPath = dataDir + "1ake_A" + fileExt; // 1k3q_A

    finder.InitializeFeaturesFromPDBFiles(file1FullPath, file2FullPath, false);        


        //std::string file1FullPath3 = "./Test_Data_TPS/3tgl_A_3.wrl";
        //SSERenderer tempRender;
        //tempRender.LoadHelixFile(file1FullPath3);
        //(tempRender.GetHelices())[0][0].GetLocationInVector();

        //std::string inputListFile  = "./Test_Data_TPS/fileList.txt";
        //std::string dataDir        = "./Test_Data_TPS/3tgl/";

        //// Source model
        //std::vector< std::string > inputFileList;
        //// Deformed models by bending energy (each energy level has 50 bended models)
        //std::vector< std::string > outputFileList;

        //// Only one input source
        //inputFileList.push_back("inputLineSegs.txt");

        //std::ifstream file(inputListFile);
        //std::string   line;

        //// Read the deformed model list by energy level
        //if( !file.is_open() )
        //    std::cout<<"Unable to open "<<inputListFile<<"...\n";
        //else
        //{
        //    std::cout<<"Read in the file...\n";

        //    while( std::getline(file, line) )
        //    {
        //        // Protect from empty lines
        //        if( "" == line )
        //            continue;

        //        // Attention: no protection for space character
        //        outputFileList.push_back(line);
        //    }

        //    std::cout<<"Finish reading in all the file names!\n";
        //}

        //file.close();

        ///////////  Process all the files /////////

        //// Read in input line source
        //vector<SSECorrespondenceFeature> fl1, fl2;

        //std::string inputFile = dataDir + inputFileList[0];
        //file.open(inputFile, std::ifstream::in);

        //if( !file.is_open() )
        //    std::cout<<"Unable to open "<<inputFile<<"...\n";
        //else
        //{
        //    std::cout<<"Read in the file...\n";

        //    while( std::getline(file, line) )
        //    {
        //        // Protect from empty lines
        //        if( "" == line )
        //            continue;

        //        std::vector<std::string> tokens;
        //        ParseByCharater(line, tokens, '\t');

        //        // Each line has 6 coordinates (2 points)
        //        if("e" != tokens[0])
        //        {
        //            // Read in the two points
        //            fl1.push_back(SSECorrespondenceFeature(Vec3F(std::stof(tokens[0]),std::stof(tokens[1]),std::stof(tokens[2])), Vec3F(std::stof(tokens[3]),std::stof(tokens[4]),std::stof(tokens[5]))));
        //        }

        //    }

        //    std::cout<<"Finish reading in the inputPoints file!\n";
        //}

        //file.close();

        //unsigned int energyIndex = 0;
        //while(energyIndex < outputFileList.size())
        //{
        //    double accuracy = 0.0;

        //    // Read in the data and model from the file
        //    // ATTENTION!! "inputPoints" are source and "outputPoints" are deformed points
        //    std::string outputFile = dataDir + outputFileList[energyIndex];

        //    // The list holding all the deformed models
        //    fl2.clear();

        //    ////////////////////// Read output points //////////////////////
        //    file.open(outputFile, std::ifstream::in);

        //    if( !file.is_open() )
        //        std::cout<<"Unable to open "<<outputFile<<"...\n";
        //    else
        //    {
        //        std::cout<<"Read in the file...\n";

        //        bool initFlag = true;

        //        //int tempI = 0;

        //        while( std::getline(file, line) )
        //        {
        //            

        //            // Protect from empty lines
        //            if( "" == line )
        //                continue;

        //            std::vector<std::string> tokens;
        //            ParseByCharater(line, tokens, '\t');

        //            if("e" == tokens[0])
        //            {
        //                //++tempI;
        //                //std::cout<<tempI<<"\n";

        //                // We create new input and outputs
        //                if( !initFlag )
        //                {
        //                    // Compute
        //                    SSECorrespondenceFinder finder;
        //                    finder.InitializeFeatures(fl1, fl2);
        //                    //std::cout<<"AAAAAAAAA\n";
        //                    finder.InitializeConstants(0, 10.0f, 0, 0, 0, 0, 0, 30.0f*PI/180.0f, 30.0f*PI/180.0f, 5.0f, 1, 10);
        //                    //std::cout<<"BBBBBBBBBB\n";
        //                    auto corr = finder.GetAStarTriangleBasedCliqueDistanceFeatureCorrespondence(false, true, 4);
        //                    //std::cout<<"CCCCCCCCCC\n";
        //                    double correctCount = 0.0;
        //                    if(!corr.empty())
        //                    {
        //                        if(!corr[0].empty())
        //                        {
        //                            for(auto it = corr[0][0].begin(); it != corr[0][0].end(); ++it)
        //                            {
        //                                if(it->GetPIndex() == it->GetQIndex())
        //                                    correctCount += 1.0;
        //                            }
        //                        }
        //                    }

        //                    //std::cout<<"DDDDDDDDDDDDD\n";
        //                    accuracy += correctCount/10.0;

        //                    fl2.clear();
        //                }
        //                // Avoid the first e
        //                initFlag = false;
        //            }
        //            else
        //            {
        //                // Read in the new features
        //                fl2.push_back(SSECorrespondenceFeature(Vec3F(std::stof(tokens[0]),std::stof(tokens[1]),std::stof(tokens[2])), Vec3F(std::stof(tokens[3]),std::stof(tokens[4]),std::stof(tokens[5]))));
        //            }

        //        }

        //        // Compute the last set in the file
        //        SSECorrespondenceFinder finder;
        //        finder.InitializeFeatures(fl1, fl2);
        //        finder.InitializeConstants(0, 10.0f, 0, 0, 0, 0, 0, 30.0f*PI/180.0f, 30.0f*PI/180.0f, 5.0f, 1, 10);
        //        auto corr = finder.GetAStarTriangleBasedCliqueDistanceFeatureCorrespondence(false, true, 4);

        //        double correctCount = 0.0;
        //        if(!corr.empty())
        //        {
        //            if(!corr[0].empty())
        //            {
        //                for(auto it = corr[0][0].begin(); it != corr[0][0].end(); ++it)
        //                {
        //                    if(it->GetPIndex() == it->GetQIndex())
        //                        correctCount += 1.0;
        //                }
        //            }
        //        }
        //        accuracy += correctCount/10.0;

        //        fl2.clear();

        //        //std::cout<<"Finish reading in the deformed models file!\n";
        //    }

        //    file.close();  

        //    accuracy /= 50.0;
        //    std::cout<<"Accuracy for "<<energyIndex+1<<"th bending energy: "<<accuracy<<"\n";

        //    ++energyIndex;
        //}


        //finder.InitializeFeatures(fl1, fl2);

        //// Parameter set 1
        //cout<<"10,5,30,30"<<"\n";
        //finder.InitializeConstants(0, 10.0f, 0, 0, 0, 0, 0, 30.0f*PI/180.0f, 30.0f*PI/180.0f, 5.0f, 1, 10);
        //auto corr = finder.GetAStarTriangleBasedCliqueDistanceFeatureCorrespondence(true, true, 4);

        //if(!corr.empty())
        //{
        //    if(!corr[0].empty())
        //    {            
        //        for(auto it = corr[0][0].begin(); it != corr[0][0].end(); ++it)
        //        {
        //            std::cout<<"{"<<it->GetPIndex()<<", "<<it->GetQIndex()<<"} ; ";
        //        }
        //    }
        //}


        //finder.PrintTimes();

        //// Parameter set 2
        //cout<<"10,10,45,45"<<"\n";
        //finder.InitializeConstants(0, 10.0f, 0, 0, 0, 0, 0, 45.0f*PI/180.0f, 45.0f*PI/180.0f, 10.0f, 5, 10);
        //finder.GetAStarTriangleBasedCliqueDistanceFeatureCorrespondence(false, true, 4);
        //finder.PrintTimes();

//        std::cout<<"(-) Done with to process "<<argv[1]<<" & "<<argv[3]<<".\n\n";

    //    ++i;
    //}


    //finder.InitializeFeaturesFromPDBFiles(testFile1, testFile2, false);
    //finder.InitializeConstants(0, 10.0f, 0, 0, 0, 0, 0, 45.0f*PI/180.0f, 45.0f*PI/180.0f, 5.0f, 1, 10);

    ////finder.PrintFeatureListsMathematica();
    //vector< vector < vector<SSECorrespondenceNode> > > corr;
    ////vector < vector<SSECorrespondenceNode> > corr2;

    //corr = finder.GetAStarTriangleBasedCliqueDistanceFeatureCorrespondence(false, true, 4);
    //finder.PrintTimes();

    cout<<"Done to process in Gorgon...\n";

    //getchar();

	//float j1, j2, d;
	//f.GetFeatureAngles(j1, j2, d, f1, f2);
	//printf("%f %f %f\n", j1*180.0/PI, j2*180.0/PI, d*180.0/PI);*/

	//CurveDeformer cd = CurveDeformer();
	//vector<Vec3F> points = genSpring(6, 1, 3);
	//vector<Vec3F> handles(points.size());
	//for(int i = 0; i < handles.size(); ++i)
	//{
	//	if(i == 0 || i == 1)
	//		handles[i] = points[i];
	//	else
	//		handles[i] = Vec3F();
	//	if(i == handles.size()-2 || i == handles.size() -1)
	//		handles[i] = Vec3F(points[i][0], points[i][1], points[i][2]);
	//}
	//vector<int> v1;
	//v1.push_back(3);
	//vector<int> v2;
	//v2.push_back(5);

	//cd.Deform(points, handles, handles, 2);
	
	return 0;
}



#endif