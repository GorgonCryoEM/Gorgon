#ifndef GLOBAL_DEFINITIONS_H
#define GLOBAL_DEFINITIONS_H

#define round(f) ((f >= 0)?(int)(f + .5):(int)(f - .5))

namespace wustl_mm {
	namespace GraySkeletonCPP {
		const int PRUNE_AMOUNT = 4;
		const int PARTIAL_DERIVATIVE_MASK_SIZE = 5;
		const unsigned char PIXEL_CLASS_BACKGROUND = 0;
		const unsigned char PIXEL_CLASS_POINT = 254;
		const unsigned char PIXEL_CLASS_CURVE_END = 223;
		const unsigned char PIXEL_CLASS_CURVE_BODY = 191;
		const unsigned char PIXEL_CLASS_SURFACE_BORDER = 159;
		const unsigned char PIXEL_CLASS_SURFACE_BODY = 127;
		const unsigned char PIXEL_BINARY_FALSE = 0;
		const unsigned char PIXEL_BINARY_TRUE = 255;

		const unsigned char VOXEL_CLASS_CURVE_BACKGROUND = 0;
		const unsigned char VOXEL_CLASS_CURVE_POINT = 1;
		const unsigned char VOXEL_CLASS_CURVE_END = 2;
		const unsigned char VOXEL_CLASS_CURVE_BODY = 3;
		const unsigned char VOXEL_CLASS_SURFACE_BORDER = 4;
		const unsigned char VOXEL_CLASS_SURFACE_BODY = 5;
		const unsigned char VOXEL_CLASS_VOLUME_BORDER = 6;
		const unsigned char VOXEL_CLASS_VOLUME_BODY = 7;
		const unsigned char VOXEL_BINARY_FALSE = 0;
		const unsigned char VOXEL_BINARY_TRUE = 255;



		const int IMAGE_NEIGHBORS_4[4][2]      ={{0,1},{1,0},{0,-1},{-1,0}};
		const int IMAGE_NEIGHBORS_8[8][2]      ={{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1}};
		const int VOLUME_NEIGHBORS_6[6][3]     ={{0,0,1},{0,0,-1},{0,1,0},{0,-1,0},{1,0,0},{-1,0,0}};
		const int VOLUME_NEIGHBORS_26[26][3]   ={
			{-1,-1,-1}, {-1,-1,0}, {-1,-1,1}, {-1,0,-1}, {-1,0,0}, {-1,0,1}, {-1,1,-1},	{-1,1,0}, {-1,1,1}, 
			{0,-1,-1},  {0,-1,0},  {0,-1,1},  {0,0,-1},            {0,0,1},  {0,1,-1},  {0,1,0},  {0,1,1}, 
			{1,-1,-1},  {1,-1,0},  {1,-1,1},  {1,0,-1},  {1,0,0},  {1,0,1},  {1,1,-1},  {1,1,0},  {1,1,1}};
		const int VOLUME_NEIGHBOR_CUBES[8][7][3] = {
			{{1,0,0},  {1,-1,0},  {0,-1,0}, {0,0,-1}, {1,0,-1},  {1,-1,-1},  {0,-1,-1}},	// Bottom right back
			{{1,0,0},  {1,-1,0},  {0,-1,0}, {0,0,1},  {1,0,1},   {1,-1,1},   {0,-1,1}},		// Bottom right front
			{{-1,0,0}, {-1,-1,0}, {0,-1,0}, {0,0,-1}, {-1,0,-1}, {-1,-1,-1}, {0,-1,-1}},	// Bottom left back
			{{-1,0,0}, {-1,-1,0}, {0,-1,0}, {0,0,1},  {-1,0,1},  {-1,-1,1},  {0,-1,1}},		// Bottom left front
			{{1,0,0},  {1,1,0},   {0,1,0},  {0,0,-1}, {1,0,-1},  {1,1,-1},   {0,1,-1}},		// Top right back
			{{1,0,0},  {1,1,0},   {0,1,0},  {0,0,1},  {1,0,1},   {1,1,1},    {0,1,1}},		// Top right front
			{{-1,0,0}, {-1,1,0},  {0,1,0},  {0,0,-1}, {-1,0,-1}, {-1,1,-1},  {0,1,-1}},		// Top left back
			{{-1,0,0}, {-1,1,0},  {0,1,0},  {0,0,1},  {-1,0,1},  {-1,1,1},   {0,1,1}}};		// Top left front					
	}
}

#endif