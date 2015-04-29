// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Tao Ju (taoju@cse.wustl.edu)
// Description:   A dummy CPP file for the SKELETONMAKER module



#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1

#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "volume.h"
#include "reader.h"
#include <string>

using namespace wustl_mm::SkeletonMaker;

/* Definition of global constants */

/* Erosion parameters */
#define ERODE_THICKNESS 3 // Number of rounds of erosion
#define TOPOLOGY_CHECK 4 // 0 for no topology, 1 for simple topology, 2 for topology plus feature
						// 3 for eroding to atoms, 4 for convergent eroding
						// 5 for Palagy's surface skeleton
						// 6 for bertrand's surface skeleton
						// 7 for my skeleton


/* Scoring parameters */
#define SCORE_RANGE 4 // Half-width of the scoring range
#define VAL_HELIX 1 // Importance factor of a helix
#define VAL_SHEET 1 // Importance factor of a sheet

/* Output parameters */
#define WRITE_CELLS
//#define WRITE_DISTANCE_MAP
//#define WRITE_SEGMENTATION
//#define WRITE_ORIGINALS
#define WRITE_PQR
//#define WRITE_DX

/* Input specific */
#define PQR_PADDING 4
#define DX_MAG 4
#define PQR_COMPONENT 20
//#define VERBOSE

//#define HAS_SKELETON
//#define WRITE_DENSITY

/* End of global definitions */




float weights[ 2 * SCORE_RANGE + 1 ][ 2 * SCORE_RANGE + 1 ][ 2 * SCORE_RANGE + 1 ] ;
float totWeights ;

void prepareWeights() ;
void computeScore( Volume* hvol, Volume* svol, int x, int y, int z, float score[3] ) ;

int main( int args, char* argv[] )
{
	if ( args != 6 )
	{
		printf("Usage: morph <input mrc> <low threshold> <steps> <min helix size> <min sheet size> \n") ;
		exit(0) ;
	}

	int i ;
	float threshold;
	int steps ;
	int minHelix, minSheet ;
	sscanf( argv[2], "%f", &threshold ) ;
	sscanf( argv[3], "%d", &steps ) ;
	sscanf( argv[4], "%d", &minHelix ) ;
	sscanf( argv[5], "%d", &minSheet ) ;
	char name[1024] ;
	strncpy( name, argv[1], strlen(argv[1]) - 4 ) ;
	name[ strlen(argv[1]) - 4 ] = '\0' ;
	printf("from density map %s\n", name) ;
	
	/****** Read volume ******/
	printf("Initialize volume...") ;
	int modelType ; // 0 for MRC, 1 for SOF
	VolumeReader* reader ;
	Volume * vol;
	
	if ( strstr( argv[1], ".mrc" ) != NULL || strstr( argv[1], ".MRC" ) != NULL )
	{
		printf("Reading MRC file format.\n") ;
		modelType = 0 ;

		reader = MRCReaderPicker::pick( argv[1] ) ;
		vol = reader->getVolume() ;
	} 
	else if ( strstr( argv[1], ".sof" ) != NULL || strstr( argv[1], ".SOF" ) != NULL)
	{
		printf("Reading SOF file format.\n") ;
		modelType = 1 ;

		reader = new SOFReader( argv[1] ) ;
		vol = reader->getVolume() ;
		vol->floodFill( 0 ) ;
		//vol->addNoise( 0, 0.8f ) ;
		//vol->addNoise( 0, 0.8f ) ;
		//vol->toMRCFile( "test.mrc" ) ;
		//vol->toOFFCells( "test.off" ) ;
	}
	else
	{
		printf("Unrecognized file format! Only MRC and SOF files are accepted.\n") ;
		exit(0) ;
	}

#ifdef WRITE_ORIGINALS
	vol->toMRCFile( "test.mrc" ) ;
	vol->toOFFCells( "test.off" ) ;
	exit( 0 ) ; 
#endif	
	
#ifdef WRITE_CELLS_ORIGINAL
	char newname[1024];
	sprintf( newname, "%s.off", name ) ;
	vol->toOFFCells( newname, threshold ) ;
	sprintf( newname, "%s2.off", name ) ;
	vol->toOFFCells2( newname, threshold ) ;
#endif

	/****** Compute skeleton ******/

	/*** Step 1: Get plates ***/

	// First, threshold the input to 0/1
	Volume* bvol = new Volume( vol->getSizeX(), vol->getSizeY(), vol->getSizeZ(), 0, 0, 0, vol ) ;
	bvol->threshold2( threshold, 0, 1 ) ;

	// Next, iteratively thin
	printf("Generating surfaces...\n");
	float stepsize = (float)((vol->getMax() - threshold) / steps) ;
	float g = threshold ;
	for ( i = 0 ; i < steps ; i ++  )
	{
		printf("Step %d of %d...\n", i, steps) ;
		bvol->surfaceSkeleton( vol, g, g + stepsize ) ;
		g+= stepsize ;
	}
	
	// Finally, prune
	printf("Pruning surfaces...\n");
	Volume* svol = new Volume( bvol->getSizeX(), bvol->getSizeY(), bvol->getSizeZ(), 0, 0, 0, bvol ) ;
	int maxDis = svol->erodeSheet( minSheet ) ;


	/*** Step 2: Get tubes ***/

	// First, threshold the input to 0/1
	delete bvol ;
	bvol = new Volume( vol->getSizeX(), vol->getSizeY(), vol->getSizeZ(), 0, 0, 0, vol ) ;
	bvol->threshold2( threshold, 0, 1 ) ;

	// Next, iteratively thin
	printf("Generating curves...\n");
	g = threshold ;
	for ( i = 0 ; i < steps ; i ++  )
	{
		printf("Step %d of %d...\n", i, steps) ;
		bvol->curveSkeleton( vol, g, g + stepsize, svol ) ;
		g+= stepsize ;
	}
	
	// Finally, prune
	printf("Pruning curves...\n");
	Volume* hvol = new Volume( bvol->getSizeX(), bvol->getSizeY(), bvol->getSizeZ(), 0, 0, 0, bvol ) ;
	hvol->erodeHelix( minHelix ) ;
	hvol->subtract( svol ) ;

	/*** Step 3: Get points ***/

	// First, threshold the input to 0/1
	delete bvol ;
	bvol = new Volume( vol->getSizeX(), vol->getSizeY(), vol->getSizeZ(), 0, 0, 0, vol ) ;
	bvol->threshold2( threshold, 0, 1 ) ;

	// Next, iteratively thin
	printf("Generating points...\n");
	g = threshold ;
	for ( i = 0 ; i < steps ; i ++  )
	{
		printf("Step %d of %d...\n", i, steps) ;
		bvol->pointSkeleton( vol, g, g + stepsize, svol, hvol ) ;
		g+= stepsize ;
	}


	/****** Write out ******/
	char nname[1024] ;
	sprintf( nname, "%s_skeleton.mrc", name) ;
	bvol->toMRCFile( nname ) ;
	sprintf( nname, "%s2.off", nname) ;
	bvol->toOFFCells2( nname ) ;
	
	char nname1[1024], nname2[1024] ;
	sprintf( nname1, "%s_helix.mrc", name) ;
	sprintf( nname2, "%s_sheet.mrc", name) ;
	hvol->toMRCFile(nname1) ;
	svol->toMRCFile(nname2) ;
	
	sprintf( nname1, "%s2.off", nname1) ;
	sprintf( nname2, "%s2.off", nname2) ;
	hvol->toOFFCells2(nname1) ;
	svol->toOFFCells2(nname2) ;
	return 0;	

}

int oldmain( int args, char* argv[] )
{
	if ( args != 9 && args != 6 && args != 5 )
	{
		printf("Usage: morph 1 <input mrc> <apix> <threshold> <output mrc> \n") ;
		printf("       morph 2 <eroded mrc> <min sheet size> <output sheet mrc> \n") ;
		printf("       morph 3 <eroded mrc> <min helix size> <output helix mrc> \n") ;
		printf("       morph 4[5|6|7|8|9] <input mrc> <input atoms> <apix> <threshold> <min helix size> <min sheet size> <output pdb>\n") ;
		exit(0) ;
	}

	clock_t start, finish ;
	int i ;
	
	// Read volume
	printf("Initialize volume...") ;
	int modelType ; // 0 for MRC, 1 for SOF
	VolumeReader* reader ;
	Volume * vol;
	
	if ( strstr( argv[2], ".mrc" ) != NULL || strstr( argv[2], ".MRC" ) != NULL )
	{
		printf("Reading MRC file format.\n") ;
		modelType = 0 ;

		reader = MRCReaderPicker::pick( argv[2] ) ;
		vol = reader->getVolume() ;
	} 
	else if ( strstr( argv[2], ".sof" ) != NULL || strstr( argv[2], ".SOF" ) != NULL)
	{
		printf("Reading SOF file format.\n") ;
		modelType = 1 ;

		reader = new SOFReader( argv[2] ) ;
		vol = reader->getVolume() ;
		vol->floodFill( 0 ) ;
		//vol->addNoise( 0, 0.8f ) ;
		//vol->addNoise( 0, 0.8f ) ;
		//vol->toMRCFile( "test.mrc" ) ;
		//vol->toOFFCells( "test.off" ) ;
	}
	else if ( strstr( argv[2], ".pqr" ) != NULL || strstr( argv[2], ".PQR" ) != NULL)
	{
		printf("Reading PQR file format.\n") ;
		float arrspc ;
		sscanf( argv[4], "%f", &arrspc ) ;
		modelType = 2 ;

		reader = new PQRReader( argv[2], arrspc, PQR_PADDING ) ;
// 		rawvol = reader->getVolume(  ) ;
		vol = reader->getVolume( ) ;
		vol->floodFillPQR ( PQR_PADDING / 2 ) ;
	}
	else if ( strstr( argv[2], ".dx" ) != NULL || strstr( argv[2], ".DX" ) != NULL)
	{
		printf("Reading open-DX file format.\n") ;
		modelType = 2 ;

		reader = new DXReader( argv[2], DX_MAG, PQR_PADDING * 2 ) ;
		vol = reader->getVolume( ) ;
//		vol->floodFillPQR ( 1 ) ;
		char tname[1024];
		sprintf( tname, "test_%d.mrc", DX_MAG ) ;
		vol->toMRCFile( tname ) ;
	}
	else
	{
		printf("Unrecognized file format! Only MRC and SOF files are accepted.\n") ;
		reader = new SphereVolumeGenerator( 128, 1, 1, 1 ) ;
		//reader = new CylinderVolumeGenerator( 128, 6 ) ;
		vol = reader->getVolume() ;
//		vol->addNoise( 0, 0.5f ) ;
//		vol->addNoise( 0, 0.5f ) ;
//		vol->addNoise( 0, 0.5f ) ; 
//		vol->addNoise( 0, 1 ) ;
		//vol->toMRCFile( "test.mrc" ) ;
		//vol->toOFFCells( "test.off" ) ;
		modelType = 1 ;
	}

#ifdef WRITE_ORIGINALS
	vol->toMRCFile( "test.mrc" ) ;
	vol->toOFFCells( "test.off" ) ;
	exit( 0 ) ; 
#endif

	

	char name[1024] ;
	strncpy( name, argv[2], strlen(argv[2]) - 4 ) ;
	name[ strlen(argv[2]) - 4 ] = '\0' ;
	printf("from density map %s\n", name) ;

	int flag ;
	sscanf( argv[1], "%d", &flag ) ;

	if ( flag == 1 )
	{
		/* Morphological erosion */
		// Process volume
		float apix, threshold ;
		sscanf( argv[3], "%f", &apix ) ;
		sscanf( argv[4], "%f", &threshold ) ;
		printf("Angstroms per pixel: %f Threshold: %f\n", apix, threshold) ;
		
		switch( TOPOLOGY_CHECK )
		{
		case 0 : vol->erodeNoTopo( threshold, (int)( ERODE_THICKNESS ) ) ;
			break ;
		case 1 : vol->erodeTopo( threshold, (int)( ERODE_THICKNESS ) ) ;
			break ;
		case 2 : vol->erodeShapeTopo( threshold, (int)( ERODE_THICKNESS ) ) ;
			break ;
		}
		

		// Output volume
		printf("Writing volume to %s...\n", argv[5]) ;
		vol->toMRCFile( argv[5] ) ;
	}
	else if ( flag == 2 )
	{
		/* Sheet clustering */
		int minSize ;
		sscanf( argv[3], "%d", &minSize ) ;

		Volume* svol = vol->getSheets( minSize ) ;
		svol->toMRCFile( argv[4] ) ;
	}
	else if ( flag == 3 )
	{
		/* Helix clustering */
		int minSize ;
		sscanf( argv[3], "%d", &minSize ) ;

		Volume* svol = vol->getHelices( minSize ) ;
		svol->toMRCFile( argv[4] ) ;
	}
	else if ( flag > 6 && flag < 10 )
	{
		/* Compute scoring function */
		float apix, threshold ;
		int minHelix, minSheet ;
		sscanf( argv[4], "%f", &apix ) ;
		sscanf( argv[5], "%f", &threshold ) ;
		sscanf( argv[6], "%d", &minHelix ) ;
		sscanf( argv[7], "%d", &minSheet ) ;

		// First, erode volume
		printf("\n***********Eroding input volume**********\n");
		start = clock() ;
		switch( TOPOLOGY_CHECK )
		{
		case 0 : vol->erodeNoTopo( threshold, (int)( ERODE_THICKNESS ) ) ;
			break ;
		case 1 : vol->erodeTopo( threshold, (int)( ERODE_THICKNESS ) ) ;
			break ;
		case 2 : vol->erodeShapeTopo( threshold, (int)( ERODE_THICKNESS ) ) ;
			break ;
		}
		finish = clock() ;
		printf("Took %f seconds.\n", (double) (finish - start) / (double) CLOCKS_PER_SEC ) ;
		if ( flag == 7 || flag == 9 )
		{
			char nname[1024] ;
			sprintf( nname, "%s_skeleton.mrc", name) ;
			vol->toMRCFile(nname) ;
		}

		// Next, compute helix and sheet volumes
		printf("\n***********Segmenting structure**********\n");
		Volume* hvol = vol->getHelices( minHelix ) ;
		Volume* svol = vol->getSheets( minSheet ) ;
		if ( flag == 7 || flag == 9 )
		{
			char nname1[1024], nname2[1024] ;
			sprintf( nname1, "%s_helix.mrc", name) ;
			sprintf( nname2, "%s_sheet.mrc", name) ;
			hvol->toMRCFile(nname1) ;
			svol->toMRCFile(nname2) ;
		}		
		
		if ( flag != 7 && modelType == 0 )
		{
			// Finally, evaluate scores
			printf("\n***********Evaluating scores*************\n");
			prepareWeights() ;
			
			FILE* fin = fopen( argv[3], "r" ) ;
			FILE* fout1 = fopen( argv[8], "w" ) ;
			if ( fin == NULL )
			{
				printf("Fail to open atoms PDB file %s.\n", argv[3]) ;
				exit(0) ;
			}
			
			char str[10][1024] ;
			float ax, ay, az ;
			ax = apix ;
			ay = apix ;
			az = apix ;
			int ofx = 16, ofy = 16, ofz = 16 ;
			float scrs[10000] ;
			float maxscr = -1, minscr = 1 ;
			
			float val[3] ;
			int ctt = 0 ;
			while ( !feof( fin ) )
			{
				float x, y, z ;
				int ix, iy, iz ;
				fscanf( fin, "%s %s %s %s %s %s %f %f %f %s %s %s %s", str[0], str[1], str[2], str[3], str[4], str[5], 
					&x, &y, &z, str[6], str[7], str[8], str[9] );
				
				
				ix = (int)(x / apix) ;
				iy = (int)(y / apix) ;
				iz = (int)(z / apix) ;
				
				
				//float x = ( ix - ofx ) * ax;
				//float y = ( iy - ofy ) * ay;
				//float z = ( iz - ofz ) * az;
				
				computeScore( hvol, svol, ix, iy, iz, val ) ;
				scrs[ ctt ] = val[2] ; 
				if ( scrs[ ctt ] < minscr )
				{
					minscr = scrs[ ctt ] ;
				}
				if ( scrs[ ctt ] > maxscr )
				{
					maxscr = scrs[ ctt ] ;
				}
				
				// printf("Value at %f %f %f is %f %f %f\n", ix, iy, iz, val[0], val[1], val[2] ) ;
				
				ctt ++ ;
			}
			
			// Normalization
			for ( i = 0 ; i < ctt ; i ++ )
			{
				if ( scrs[ i ] > 0 )
				{
					scrs[ i ] /= maxscr ;
				}
				else
				{
					scrs[ i ] /= ( - minscr ) ;
				}
			}
			
			fseek( fin, 0, SEEK_SET ) ;
			ctt = 0 ;
			while ( !feof( fin ) )
			{
				float x, y, z ;
				fscanf( fin, "%s %s %s %s %s %s %f %f %f %s %s %s %s", str[0], str[1], str[2], str[3], str[4], str[5], 
					&x, &y, &z, str[6], str[7], str[8], str[9] );
				fprintf( fout1, "ATOM  %5d  C   GLY A%4d    %8.3f%8.3f%8.3f  1.00%6.2f      S_00  0 \n", 
					ctt + 1, ctt + 1, x, y, z, scrs[ ctt ]   );
				
				ctt ++ ;
			}
			
			fclose( fin ) ;
			fclose( fout1 ) ;
			//fclose( fout2 ) ;
			//fclose( fout3 ) ;
		}

		delete hvol ;
		delete svol ;
		delete vol ;
	}
	else if ( flag > 3 && flag < 7 )
	{
		
		/* Compute scoring function */
		float apix, threshold ;
		int minHelix, minSheet ;
		sscanf( argv[4], "%f", &apix ) ;
		sscanf( argv[5], "%f", &threshold ) ;
		sscanf( argv[6], "%d", &minHelix ) ;
		sscanf( argv[7], "%d", &minSheet ) ;

#ifdef WRITE_CELLS_ORIGINAL
		char newname[1024];
		sprintf( newname, "%s.off", name ) ;
		vol->toOFFCells( newname, threshold ) ;
		sprintf( newname, "%s2.off", name ) ;
		vol->toOFFCells2( newname, threshold ) ;
#endif
		// First, read PDB atoms location
		Volume* atomvol = new Volume( vol->getSizeX(), vol->getSizeY(), vol->getSizeZ(), -1 ) ;
		FILE* fin ;
		char str[10][1024] ;
		int ctt = 0 ;

		if ( modelType == 0 )
		{
			fin = fopen( argv[3], "r" ) ;
			if ( fin == NULL )
			{
				printf("Fail to open atoms PDB file %s.\n", argv[3]) ;
				modelType = 1 ;
			}
			else
			{
				while ( !feof( fin ) )
				{
					float x, y, z ;
					int ix, iy, iz ;
					fscanf( fin, "%s %s %s %s %s %s %f %f %f %s %s %s %s", str[0], str[1], str[2], str[3], str[4], str[5], 
						&x, &y, &z, str[6], str[7], str[8], str[9] );
					ix = (int)(x / apix);
					iy = (int)(y / apix);
					iz = (int)(z / apix);

					/* Disabled for test */
					atomvol->setDataAt( ix, iy, iz, 1 ) ;
					
					ctt ++ ;
				}
			}
		}


		// First, erode volume
		Volume * hvol, * svol, * ovol;
		// printf("\n***********Eroding input volume**********\n");
		start = clock() ;
		switch( TOPOLOGY_CHECK )
		{
		case 0 : vol->erodeNoTopo( threshold, (int)( ERODE_THICKNESS ) ) ;
			break ;
		case 1 : vol->erodeTopo( threshold, (int)( ERODE_THICKNESS ) ) ;
			break ;
		case 2 : vol->erodeShapeTopo( threshold, (int)( ERODE_THICKNESS ) ) ;
			break ;
		case 3 : vol->erodeAtom( threshold, (int)( ERODE_THICKNESS ), atomvol ) ;
			break ;
		case 4 : 
			printf("\n***********Generating medial surfaces**********\n");			
			//vol->dumbsurfaceSkeleton( threshold ) ;
#ifndef HAS_SKELETON
			ovol = new Volume( vol->getSizeX(), vol->getSizeY(), vol->getSizeZ(), 0, 0, 0, vol ) ;
#ifdef WRITE_DX
			vol->skeleton( threshold, PQR_PADDING ) ;
#else
			vol->surfaceSkeleton( threshold ) ;
#endif
#endif
			//vol->bertrandSurfaceSkeleton( threshold ) ;
			break;
		case 5 : vol->palagyiSurfaceSkeleton( threshold ) ;
			break ;
		case 6 : vol->bertrandSurfaceSkeleton( threshold ) ;
			break ;
		case 7 : vol->sequentialSkeleton( threshold, 1, 0 ) ;
			break ;
		}
		finish = clock() ;
		printf("Took %f seconds.\n", (double) (finish - start) / (double) CLOCKS_PER_SEC ) ;
#ifndef HAS_SKELETON
		if ( ( flag == 4 && TOPOLOGY_CHECK != 4 ) || flag == 6 )
		{
			char nname[1024] ;
			sprintf( nname, "%s_skeleton.mrc", name) ;
			vol->toMRCFile(nname) ;  

#ifdef WRITE_CELLS
			if ( TOPOLOGY_CHECK == 6 || TOPOLOGY_CHECK == 7 )
			{
				char newname[1024] ;
				sprintf( newname, "%s2.off", nname) ;
				vol->toOFFCells2(newname) ;
			}
			sprintf( nname, "%s.off", nname) ;
			vol->toOFFCells(nname) ;
#endif
		}
		else if ( flag == 4 )
		{
			char nname[1024] ;
			sprintf( nname, "%s_skeleton_test.mrc", name) ;
			vol->toMRCFile(nname) ;  

#ifdef WRITE_CELLS
			char nname2[1024] ;
			sprintf( nname2, "%s.off", nname) ;
//			vol->toOFFCells(nname2) ;
			sprintf( nname2, "%s2.off", nname) ;
			vol->toOFFCells2(nname2) ;
#endif
		}
#endif
		// return 0 ;

		// Next, compute helix and sheet volumes
		if ( TOPOLOGY_CHECK != 4 )
		{
			printf("\n***********Segmenting structure**********\n");
			hvol = vol->getHelices( minHelix ) ;
			svol = vol->getSheets( minSheet ) ;
		}
		else
		{
			printf("Pruning medial surfaces...\n");
			svol = new Volume( vol->getSizeX(), vol->getSizeY(), vol->getSizeZ(), 0, 0, 0, vol ) ;
			int maxDis = svol->erodeSheet( minSheet ) ;

#ifdef WRITE_DX
			char nname4[1024] ;
			printf("Pruning medial curves...\n");
			vol->subtract( svol ) ;
			
			sprintf( nname4, "%s_path_s.off", name) ;
			svol->toOFFCells2(nname4) ;
			sprintf( nname4, "%s_path_h.off", name) ;
			vol->toOFFCells2(nname4) ;


			vol->erodeHelix( minHelix ) ;
			vol->reduceComponent2( 1 ) ;

			printf("Write out path...\n");
			sprintf( nname4, "%s_path.pqr", name) ;
			/*
			hvol->toPQRFile(nname4, ((PQRReader *)reader)->getSpacing(), 
				((PQRReader *)reader)->getMinx(),
				((PQRReader *)reader)->getMiny(),
				((PQRReader *)reader)->getMinz(),
				PQR_PADDING) ;
			*/
			vol->toPQRFile(nname4, 0.25f * DX_MAG, 
				-4.014000f, 2.071000f, -2.898000f,
				PQR_PADDING * 2) ;
			sprintf( nname4, "%s_path.mrc", name) ;
			vol->toMRCFile(nname4) ;
			sprintf( nname4, "%s_path.off", name) ;
			vol->toOFFCells2(nname4) ;
			exit(0) ;
#endif
			hvol = new Volume( ovol->getSizeX(), ovol->getSizeY(), ovol->getSizeZ(), 0, 0, 0, ovol ) ;
			printf("\n***********Generating medial curves**********\n");
			hvol->curveSkeleton( threshold, svol ) ;
			printf("Pruning medial curves...\n");
			hvol->erodeHelix( minHelix ) ;
			hvol->subtract( svol ) ;

			Volume* finvol = new Volume( ovol->getSizeX(), ovol->getSizeY(), ovol->getSizeZ(), 0, 0, 0, ovol ) ;
			printf("\n***********Generating complete skeleton**********\n");
			finvol->skeleton( threshold, svol, hvol ) ;

			char nname[1024] ;
			sprintf( nname, "%s_skeleton.mrc", name) ;
			finvol->toMRCFile( nname ) ;

#ifdef WRITE_DENSITY
			char denname[1024] ;
			sprintf( denname, "%s_skeleton_den.mrc", name) ;
			Volume* denvol = finvol->getPseudoDensity();
			denvol->toMRCFile( denname ) ;
#endif


#ifdef WRITE_CELLS
			char nname2[1024] ;
			sprintf( nname2, "%s.off", nname) ;
			finvol->toOFFCells(nname2) ;
			sprintf( nname2, "%s2.off", nname) ;
			finvol->toOFFCells2(nname2) ;
#endif

#ifdef WRITE_SEGMENTATION
			char nname5[1024] ;
			sprintf( nname5, "%s_colors.mrc", name) ;
			finvol->subtract( svol ) ;
			ovol->segment( threshold, finvol, svol, nname5 ) ;
#endif

#ifdef WRITE_DISTANCE_MAP
			char nname6[1024] ;
			sprintf( nname6, "%s_colors.mrc", name) ;
			ovol->segment( threshold, vol, maxDis, nname6 ) ;
#endif

		}

		if ( flag == 4 || flag == 6 )
		{
			char nname1[1024], nname2[1024] ;
			sprintf( nname1, "%s_helix.mrc", name) ;
			sprintf( nname2, "%s_sheet.mrc", name) ;

#ifdef WRITE_PQR

			hvol->reduceComponent( minHelix ) ;

			char nname4[1024] ;
			sprintf( nname4, "%s_path.pqr", name) ;
			hvol->toPQRFile(nname4, ((PQRReader *)reader)->getSpacing(), 
				((PQRReader *)reader)->getMinx(),
				((PQRReader *)reader)->getMiny(),
				((PQRReader *)reader)->getMinz(),
				PQR_PADDING) ;
#else
			hvol->toMRCFile(nname1) ;
			svol->toMRCFile(nname2) ;
#endif
			
#ifdef WRITE_CELLS
			char nname3[1024] ;
			sprintf( nname3, "%s2.off", nname1) ;
			hvol->toOFFCells2(nname3) ;
			sprintf( nname3, "%s2.off", nname2) ;
			svol->toOFFCells2(nname3) ;
#endif


		}		

		// return 0 ;
		
		if ( flag != 4 && modelType == 0 )
		{
			// Finally, evaluate scores
			printf("\n***********Evaluating scores*************\n");
			prepareWeights() ;
			
			FILE* fout1 = fopen( argv[8], "w" ) ;
			fseek( fin, 0, SEEK_SET ) ;
			
			float ax, ay, az ;
			ax = apix ;
			ay = apix ;
			az = apix ;
			int ofx = 16, ofy = 16, ofz = 16 ;
			float scrs[10000] ;
			float maxscr = -1, minscr = 1 ;
			
			float val[3] ;
			ctt = 0 ;
			while ( !feof( fin ) )
			{
				float x, y, z ;
				int ix, iy, iz ;
				fscanf( fin, "%s %s %s %s %s %s %f %f %f %s %s %s %s", str[0], str[1], str[2], str[3], str[4], str[5], 
					&x, &y, &z, str[6], str[7], str[8], str[9] );
				
				
				ix = (int)(x / apix);
				iy = (int)(y / apix);
				iz = (int)(z / apix);
				
				
				//float x = ( ix - ofx ) * ax;
				//float y = ( iy - ofy ) * ay;
				//float z = ( iz - ofz ) * az;
				
				computeScore( hvol, svol, ix, iy, iz, val ) ;
				scrs[ ctt ] = val[2] ; 
				if ( scrs[ ctt ] < minscr )
				{
					minscr = scrs[ ctt ] ;
				}
				if ( scrs[ ctt ] > maxscr )
				{
					maxscr = scrs[ ctt ] ;
				}
				
				// printf("Value at %f %f %f is %f %f %f\n", ix, iy, iz, val[0], val[1], val[2] ) ;
				
				ctt ++ ;
			}
			
			// Normalization
			for ( i = 0 ; i < ctt ; i ++ )
			{
				if ( scrs[ i ] > 0 )
				{
					scrs[ i ] /= maxscr ;
				}
				else
				{
					scrs[ i ] /= ( - minscr ) ;
				}
			}
			
			fseek( fin, 0, SEEK_SET ) ;
			ctt = 0 ;
			while ( !feof( fin ) )
			{
				float x, y, z ;
				fscanf( fin, "%s %s %s %s %s %s %f %f %f %s %s %s %s", str[0], str[1], str[2], str[3], str[4], str[5], 
					&x, &y, &z, str[6], str[7], str[8], str[9] );
				float ix = x / apix ;
				float iy = y / apix ;
				float iz = z / apix ;
				fprintf( fout1, "ATOM  %5d  C   GLY A%4d    %8.3f%8.3f%8.3f  1.00%6.2f      S_00  0 \n", 
					ctt + 1, ctt + 1, x, y, z , scrs[ ctt ]   );
				
				ctt ++ ;
			}
			
			fclose( fin ) ;
			fclose( fout1 ) ;
			//fclose( fout2 ) ;
			//fclose( fout3 ) ;
		}

		delete hvol ;
		delete svol ;
		delete vol ;
	}

	return 0 ;
}


/* Compute score of a point in space given sheet and helix volumes */
void prepareWeights()
{
	int i, j, k ;
	totWeights = 0 ;
	float maxw = 3 * SCORE_RANGE * SCORE_RANGE;
	float w ;

	for ( i = - SCORE_RANGE ; i <= SCORE_RANGE ; i ++ )
		for ( j = - SCORE_RANGE ; j <= SCORE_RANGE ; j ++ )
			for ( k = - SCORE_RANGE ; k <= SCORE_RANGE ; k ++ )
			{
				/*
				if ( i && j && k )
				{
					totWeights += ( weights[ i + SCORE_RANGE ][ j + SCORE_RANGE ][ k + SCORE_RANGE ] = sqrt(1 / (float) ( i*i + j*j + k*k )) );
				}
				*/
				w = (float)(i*i + j*j + k*k) ;
				totWeights += ( weights[ i + SCORE_RANGE ][ j + SCORE_RANGE ][ k + SCORE_RANGE ] = 1 - w / maxw  );
			}
	printf("Total weights: %f\n", totWeights) ;
}


void computeScore( Volume* hvol, Volume* svol, int x, int y, int z, float score[3] )
{
	/*
	double val ;
	if ( (val = hvol->getDataAt( x, y, z )) > 0 )
	{
		score[0] = val ;
		score[1] = 0 ;
		score[2] = val ;
		return ;
	}
	if ( (val = svol->getDataAt( x, y, z )) > 0 )
	{
		score[0] = 0 ;
		score[1] = val ;
		score[2] = -val ;
		return ;
	}
	*/

	int i, j, k ;
	score[0] = 0 ;
	score[1] = 0 ;
	score[2] = 0 ;
	float tot = 0 ;
	int ct = 0 ;

	int lx = ( x - SCORE_RANGE >= 0 ? x - SCORE_RANGE : 0 ) ;
	int ly = ( y - SCORE_RANGE >= 0 ? y - SCORE_RANGE : 0 ) ;
	int lz = ( z - SCORE_RANGE >= 0 ? z - SCORE_RANGE : 0 ) ;

	int hx = ( x + SCORE_RANGE < hvol->getSizeX() ? x + SCORE_RANGE : hvol->getSizeX() - 1 ) ;
	int hy = ( y + SCORE_RANGE < hvol->getSizeY() ? y + SCORE_RANGE : hvol->getSizeY() - 1 ) ;
	int hz = ( z + SCORE_RANGE < hvol->getSizeZ() ? z + SCORE_RANGE : hvol->getSizeZ() - 1 ) ;

	int ix, iy, iz ;
	for ( i = lx ; i <= hx ; i ++ )
	{
		ix = i - x + SCORE_RANGE ;
		for ( j = ly ; j <= hy ; j ++ )
		{
			iy = j - y + SCORE_RANGE ;
			for ( k = lz ; k <= hz ; k ++ )
			{
				iz = k - z + SCORE_RANGE ;
				double vh = hvol->getDataAt( i, j, k ) ;
				double vs = svol->getDataAt( i, j, k ) ;

				if ( vh > 0 )
				{
					score[0] += VAL_HELIX * weights[ ix ][ iy ][ iz ] ;
					tot += weights[ ix ][ iy ][ iz ] ;
					ct ++ ;
				}
				if ( vs > 0 )
				{
					score[1] += VAL_SHEET * weights[ ix ][ iy ][ iz ] ;
					tot += weights[ ix ][ iy ][ iz ] ;
					ct ++ ;
				}

			}
		}
	}

	if ( ct != 0 )
	{
		score[0] /= ct ;
		score[1] /= ct ;
		score[2] = score[0] - score[1] ;
	}
}
