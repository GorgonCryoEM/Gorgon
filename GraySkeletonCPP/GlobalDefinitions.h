// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   Stores the global definitions for the GRAYSKELETONCPP module


#ifndef GRAYSKELETONCPP_GLOBAL_DEFINITIONS_H
#define GRAYSKELETONCPP_GLOBAL_DEFINITIONS_H

#include <Foundation/TimeManager.h>

//#define SAVE_INTERMEDIATE_RESULTS

double round(double f){
	return (f >= 0?(int)(f + .5):(int)(f - .5));
}

using namespace wustl_mm::Foundation;

namespace wustl_mm {
	namespace GraySkeletonCPP {
		
		TimeManager appTimeManager = TimeManager();

		const bool WRITE_DEBUG_FILES = false;
		const int PRUNE_AMOUNT = 3;
		const int MAX_GAUSSIAN_FILTER_RADIUS = 10;		
		const unsigned char PIXEL_CLASS_BACKGROUND = 0;
		const unsigned char PIXEL_CLASS_POINT = 254;
		const unsigned char PIXEL_CLASS_CURVE_END = 223;
		const unsigned char PIXEL_CLASS_CURVE_BODY = 191;
		const unsigned char PIXEL_CLASS_SURFACE_BORDER = 159;
		const unsigned char PIXEL_CLASS_SURFACE_BODY = 127;
		const unsigned char PIXEL_BINARY_FALSE = 0;
		const unsigned char PIXEL_BINARY_TRUE = 255;

		const unsigned char VOXEL_CLASS_CURVE_BACKGROUND = 0;
		const unsigned char VOXEL_CLASS_POINT = 1;
		const unsigned char VOXEL_CLASS_CURVE_END = 2;
		const unsigned char VOXEL_CLASS_CURVE_BODY = 3;
		const unsigned char VOXEL_CLASS_SURFACE_BORDER = 4;
		const unsigned char VOXEL_CLASS_SURFACE_BODY = 5;
		const unsigned char VOXEL_CLASS_VOLUME_BORDER = 6;
		const unsigned char VOXEL_CLASS_VOLUME_BODY = 7;
		const unsigned char VOXEL_BINARY_FALSE = 0;
		const unsigned char VOXEL_BINARY_TRUE = 255;
		const double BAD_NORMAL = 9999.0;
		const double MIN_EIGEN_VALUE = 0.000001;
		const double DEFAULT_PRUNE_THRESHOLD = 0.57735;

		const int DEFAULT_SKELETON_DIRECTION_RADIUS = 3;

		const int IMAGE_NEIGHBORS_4[4][2]      ={{0,1},{1,0},{0,-1},{-1,0}};
		const int IMAGE_NEIGHBORS_8[8][2]      ={{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1}};
		const int VOLUME_NEIGHBORS_6[6][3]     ={{0,0,1},{0,0,-1},{0,1,0},{0,-1,0},{1,0,0},{-1,0,0}};
		const int VOLUME_NEIGHBORS_18[18][3]   ={
			{-1,-1,0}, {-1,0,-1}, {-1,0,0}, {-1,0,1}, {-1,1,0}, {0,-1,-1},  
			{0,-1,0}, {0,-1,1}, {0,0,-1}, {0,0,1}, {0,1,-1}, {0,1,0},
			{0,1,1}, {1,-1,0}, {1,0,-1}, {1,0,0}, {1,0,1}, {1,1,0}};

		// If changing VOLUME_NEIGHBORS_26, remember to also change VOLUME_NEIGHBORS_26_INVERSE_INDEX accordingly
		const int VOLUME_NEIGHBORS_26[26][3]   ={
			{-1,-1,-1}, {-1,-1,0}, {-1,-1,1}, {-1,0,-1}, {-1,0,0}, {-1,0,1}, {-1,1,-1},	{-1,1,0}, {-1,1,1}, 
			{0,-1,-1},  {0,-1,0},  {0,-1,1},  {0,0,-1},            {0,0,1},  {0,1,-1},  {0,1,0},  {0,1,1}, 
			{1,-1,-1},  {1,-1,0},  {1,-1,1},  {1,0,-1},  {1,0,0},  {1,0,1},  {1,1,-1},  {1,1,0},  {1,1,1}};

		const int VOLUME_NEIGHBORS_26_INVERSE_INDEX[26]   ={25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 
			13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

		const int VOLUME_NEIGHBOR_CUBES[8][7][3] = {
			{{1,0,0},  {1,-1,0},  {0,-1,0}, {0,0,-1}, {1,0,-1},  {1,-1,-1},  {0,-1,-1}},	// Bottom right back
			{{1,0,0},  {1,-1,0},  {0,-1,0}, {0,0,1},  {1,0,1},   {1,-1,1},   {0,-1,1}},		// Bottom right front
			{{-1,0,0}, {-1,-1,0}, {0,-1,0}, {0,0,-1}, {-1,0,-1}, {-1,-1,-1}, {0,-1,-1}},	// Bottom left back
			{{-1,0,0}, {-1,-1,0}, {0,-1,0}, {0,0,1},  {-1,0,1},  {-1,-1,1},  {0,-1,1}},		// Bottom left front
			{{1,0,0},  {1,1,0},   {0,1,0},  {0,0,-1}, {1,0,-1},  {1,1,-1},   {0,1,-1}},		// Top right back
			{{1,0,0},  {1,1,0},   {0,1,0},  {0,0,1},  {1,0,1},   {1,1,1},    {0,1,1}},		// Top right front
			{{-1,0,0}, {-1,1,0},  {0,1,0},  {0,0,-1}, {-1,0,-1}, {-1,1,-1},  {0,1,-1}},		// Top left back
			{{-1,0,0}, {-1,1,0},  {0,1,0},  {0,0,1},  {-1,0,1},  {-1,1,1},   {0,1,1}}};		// Top left front					

		const int VOLUME_NEIGHBOR_FACES[12][3][3] = {
			{{1,0,0},  {1,1,0},   {0,1,0}},		// Top Left Z (1)
			{{0,1,0},  {-1,1,0},  {-1,0,0}},	// Top Right Z (2)
			{{-1,0,0}, {-1,-1,0}, {0,-1,0}},	// Bottom Right Z (3)
			{{0,-1,0}, {1,-1,0},  {1,0,0}},		// Bottom Left Z (4)
			{{0,0,1},  {1,0,1},   {1,0,0}},		// Top Left Y (5)
			{{-1,0,0}, {-1,0,1},  {0,0,1}},		// Top Right Y (6)
			{{0,0,-1}, {-1,0,-1}, {-1,0,0}},	// Bottom Right Y (7)
			{{1,0,0},  {1,0,-1},  {0,0,-1}},	// Bottom Left Y (8)
			{{0,0,1},  {0,1,1},   {0,1,0}},		// Top Left X (9)
			{{0,-1,0}, {0,-1,1},  {0,0,1}},		// Bottom Left X (10)
			{{0,0,-1}, {0,-1,-1}, {0,-1,0}},	// Bottom Right X (11)
			{{0,1,0},  {0,1,-1},  {0,0,-1}}};	// Top Right X (12)
	
		const int VOLUME_NEIGHBOR_NUMBERS[12][2] = {
			{4, 8},
			{8, 5},
			{9, 5},
			{4, 9},
			{8, 0},
			{8, 1},
			{11, 1},
			{11, 0},
			{1, 5},
			{5, 2},
			{6, 2},
			{6, 1}};
	}
}

#undef round
#endif
