// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Tao Ju (taoju@cse.wustl.edu)
// Description:   Reading routines for volumetric data





#ifndef SKELETON_MAKER_READER_H
#define SKELETON_MAKER_READER_H

#include <cstdio>
#include <cstdlib>

#ifdef _MSC_VER
	#include <string>
#else
	#include <cstring>
#endif

#include "volume.h"

namespace wustl_mm {
	namespace SkeletonMaker {
		class VolumeReader
		{
		public:
			VolumeReader ( ){} ;
			virtual ~VolumeReader() {};

			/* Read volume from input */
			virtual Volume* getVolume( ) = 0 ;

			/* Get resolution */
			virtual void getSpacing( float& ax, float& ay, float& az ) = 0 ;


			void flipBits32 ( void *x )
			{
				unsigned char *temp = (unsigned char *)x;
				unsigned char swap;
				
				swap = temp [ 0 ];
				temp [ 0 ] = temp [ 3 ];
				temp [ 3 ] = swap;

				swap = temp [ 1 ];
				temp [ 1 ] = temp [ 2 ];
				temp [ 2 ] = swap;
			}


		};

		class SOFReader : public VolumeReader
		{
		private:

			char soffile[1024] ;

			/* Recursive reader */
			void readSOF( FILE* fin, Volume* vol, int off[3], int len )
			{
				// printf("%d %d %d: %d\n", off[0], off[1], off[2], len) ;
				char type ;
				int noff[3] ;
				int nlen = len / 2 ;
				
				// Get type
				fread( &type, sizeof( char ), 1, fin ) ;

				if ( type == 0 )
				{
					// Internal node
					for ( int i = 0 ; i < 2 ; i ++ )
						for ( int j = 0 ; j < 2 ; j ++ )
							for ( int k = 0 ; k < 2 ; k ++ )
							{
								noff[0] = off[0] + i * nlen ;
								noff[1] = off[1] + j * nlen ;
								noff[2] = off[2] + k * nlen ;
								readSOF( fin, vol, noff, nlen ) ;
							}
				}
				else if ( type == 1 )
				{
					// Empty node
					char sg ;
					fread( &sg, sizeof( char ), 1, fin ) ;

					for ( int i = 0 ; i <= len ; i ++ )
						for ( int j = 0 ; j <= len ; j ++ )
							for ( int k = 0 ; k <= len ; k ++ )
							{
								noff[0] = off[0] + i ;
								noff[1] = off[1] + j ;
								noff[2] = off[2] + k ;
								vol->setDataAt( noff[0], noff[1], noff[2], - sg ) ;
							}
				}
				else if ( type == 2 )
				{
					// Leaf node
					char sg ;
					fread( &sg, sizeof( char ), 1, fin ) ;

					int t = 0 ;
					for ( int i = 0 ; i < 2 ; i ++ )
						for ( int j = 0 ; j < 2 ; j ++ )
							for ( int k = 0 ; k < 2 ; k ++ )
							{
								noff[0] = off[0] + i ;
								noff[1] = off[1] + j ;
								noff[2] = off[2] + k ;
								vol->setDataAt( noff[0], noff[1], noff[2], - (( sg >> t ) & 1) ) ;
								t ++ ;
							}
				}
				else
				{
					printf("Wrong!\n");
				}


			}

		public:
			/* Initializer */
			SOFReader( char* fname )
			{
				sprintf( soffile, "%s", fname ) ;
				FILE* fin = fopen( fname, "rb" ) ;


				if ( fin == NULL )
				{
					printf("Can not open file %s.\n", fname) ;
				}

				fclose( fin ) ;
			}

			/* Read volume */
			Volume* getVolume( )
			{
				int sx, sy, sz ;

				FILE* fin = fopen( soffile, "rb" ) ;

				// Process header
				fread( &sx, sizeof( int ), 1, fin ) ;
				sy = sx ;
				sz = sx ;
				printf("\tDimensions: %d %d %d\n", sx, sy, sz ) ;

				Volume* rvalue = new Volume( sx + 1, sy + 1, sz + 1 ) ;

				// Recursive reader
				int off[3] = { 0, 0, 0 } ;
				readSOF( fin, rvalue, off, sx ) ;

				printf("\tDone reading.\n") ;
				fclose( fin ) ;
				return rvalue ;
			}

			/* Get resolution */
			void getSpacing( float& ax, float& ay, float& az )
			{
				ax = 1 ;
				ay = 1 ;
				az = 1 ;
			}

		};

		class PQRReader : public VolumeReader
		{
		private:

			char pqrfile[1024] ;
			int padding;
			float spc ;
			float minx, miny, minz ;

		public:
			/* Initializer */
			PQRReader( char* fname, float arrspc, int pad )
			{
				sprintf( pqrfile, "%s", fname ) ;
				FILE* fin = fopen( fname, "r" ) ;


				if ( fin == NULL )
				{
					printf("Can not open file %s.\n", fname) ;
				}

				fclose( fin ) ;
				padding = pad ;
				spc = arrspc ;
			}

			float getSpacing( )
			{
				return spc ;
			}
			float getMinx( )
			{
				return minx ;
			}
			float getMiny( )
			{
				return miny ;
			}
			float getMinz( )
			{
				return minz ;
			}

			/* Read volume */
			Volume* getVolume( )
			{
				int sx = 1, sy = 1, sz = 1 ;

				FILE* fin = fopen( pqrfile, "r" ) ;

				// First, run through and determine grid size
				char str[10][1024] ;
				float t1, t2 ;
				minx = 1000000;
				miny = 1000000; 
				minz = 1000000;
				while ( !feof( fin ) )
				{
					float x, y, z ;
					fscanf( fin, "%s %s %s %s %s %f %f %f %f %f", str[0], str[1], str[2], str[3], str[4],  
						&x, &y, &z, &t1, &t2 );
					
					if ( x < minx )
					{
						minx = x ;
					}
					if ( y < miny )
					{
						miny = y ;
					}
					if ( z < minz )
					{
						minz = z ;
					}
					
					// printf("Value at %f %f %f is %f %f %f\n", ix, iy, iz, val[0], val[1], val[2] ) ;
				}
				minx -= spc ;
				miny -= spc ;
				minz -= spc ;
				printf("\tSpacing: %f, Lower corner: %f %f %f.\n", spc, minx, miny, minz) ;
				fseek( fin, 0, SEEK_SET ) ;

				int ctt = 0 ;
				while ( !feof( fin ) )
				{
					float x, y, z ;
					fscanf( fin, "%s %s %s %s %s %f %f %f %f %f", str[0], str[1], str[2], str[3], str[4],  
						&x, &y, &z, &t1, &t2 );
					
					int ix = (int)(( x - minx ) / spc) ;
					int iy = (int)(( y - miny ) / spc) ;
					int iz = (int)(( z - minz ) / spc) ;

					if ( ix >= sx )
					{
						sx = ix + 1 ;
					}
					if ( iy >= sy )
					{
						sy = iy + 1 ;
					}
					if ( iz >= sz )
					{
						sz = iz + 1 ;
					}
					// printf("Value at %f %f %f is %f %f %f\n", ix, iy, iz, val[0], val[1], val[2] ) ;
					ctt ++ ;
				}
				printf("\t%d points, raw dimension (%d %d %d), padding of %d is added.\n", ctt, sx, sy, sz, padding) ;
				sx += padding * 2 ;
				sy += padding * 2 ;
				sz += padding * 2 ;
				fseek( fin, 0, SEEK_SET ) ;

				// Next, parse data
				Volume* rvalue = new Volume( sx, sy, sz ) ;
				while ( !feof( fin ) )
				{
					float x, y, z ;
					fscanf( fin, "%s %s %s %s %s %f %f %f %f %f", str[0], str[1], str[2], str[3], str[4],  
						&x, &y, &z, &t1, &t2 );
					int ix = (int)(( x - minx ) / spc) ;
					int iy = (int)(( y - miny ) / spc) ;
					int iz = (int)(( z - minz ) / spc) ;

					rvalue->setDataAt( ix + padding, iy + padding, iz + padding, 1 ) ;
					
					// printf("Value at %f %f %f is %f %f %f\n", ix, iy, iz, val[0], val[1], val[2] ) ;
				}

				printf("\tDone reading.\n") ;
				fclose( fin ) ;
				return rvalue ;
			}

			/* Get resolution */
			void getSpacing( float& ax, float& ay, float& az )
			{
				ax = 1 ;
				ay = 1 ;
				az = 1 ;
			}

		};

		class DXReader : public VolumeReader
		{
		private:

			char dxfile[1024] ;
			float spcx, spcy, spcz ;
			float minx, miny, minz ;
			int step, padd ;

		public:
			/* Initializer */
			DXReader( char* fname, int s, int padding )
			{
				sprintf( dxfile, "%s", fname ) ;
				FILE* fin = fopen( fname, "r" ) ;


				if ( fin == NULL )
				{
					printf("Can not open file %s.\n", fname) ;
					exit(0) ;
				}

				fclose( fin ) ;
				step = s ;
				padd = padding ;
			}

			float getSpacingx( )
			{
				return spcx ;
			}
			float getSpacingy( )
			{
				return spcy ;
			}
			float getSpacingz( )
			{
				return spcz ;
			}
			float getMinx( )
			{
				return minx ;
			}
			float getMiny( )
			{
				return miny ;
			}
			float getMinz( )
			{
				return minz ;
			}

			/* Read volume */
			Volume* getVolume( )
			{
				int sx, sy, sz ;
				char str[10][1024] ;
				char line[1024];
				float temp[5] ;
				FILE* fin = fopen( dxfile, "r" ) ;

				// Get header
				int pos = 0 ;
				while (1)
				{
					pos = ftell( fin ) ;
					fgets( line, 1024, fin ) ;
					if ( line[0] != '#' )
					{
						fseek( fin, pos, SEEK_SET ) ;
						break ;
					}
				}

				fgets( line, 1024, fin ) ;
				sscanf( line, "%s %s %s %s %s %d %d %d", str[0], str[1], str[2], str[3], str[4],  
						&sx, &sy, &sz );
				fgets( line, 1024, fin ) ;
				sscanf( line, "%s %f %f %f", str[0], &minx, &miny, &minz );
				fgets( line, 1024, fin ) ;
				sscanf( line, "%s %f %f %f", str[0], &spcx, &temp[0], &temp[1] );
				fgets( line, 1024, fin ) ;
				sscanf( line, "%s %f %f %f", str[0], &temp[0], &spcy, &temp[1] );
				fgets( line, 1024, fin ) ;
				sscanf( line, "%s %f %f %f", str[0], &temp[0], &temp[1], &spcz );
				
				fgets( line, 1024, fin ) ;
				fgets( line, 1024, fin ) ;

				printf("\tDimension (%d, %d, %d), corner at (%f, %f, %f), increments (%f, %f, %f)\n", sx, sy, sz, minx, miny, minz, spcx, spcy, spcz ) ;
				// exit(0) ;

				// Next, parse data
				Volume* rvalue = new Volume( (sx-1)/step+1 + padd * 2, (sy-1)/step+1 + padd * 2, (sz-1)/step+1 + padd * 2, -1) ;
				int items = 0 ;
				char *token = NULL ;
				char seps[]   = " ,\t\n";
				float v;
				int ix, iy, iz ;

				// let's pad
				for ( ix = padd / 2 ; ix <= (sx-1)/step + padd + padd / 2 ; ix ++ )
					for ( iy = padd / 2 ; iy <= (sy-1)/step + padd + padd / 2 ; iy ++ )
						for ( iz = padd / 2 ; iz <= (sz-1)/step + padd + padd / 2 ; iz ++ )
						{
							rvalue->setDataAt( ix, iy, iz, 1 ) ;
						}

				for ( ix = 0 ; ix < sx ; ix ++ )
				{
					if ( ix % 10 == 0 )
					{
						printf("\t%d rows read (%% %f).\n", ix, (float)ix/(float)sx*100 ) ;
					}
					for ( iy = 0 ; iy < sy ; iy ++ )
						for ( iz = 0 ; iz < sz ; iz ++ )
						{
							if ( token == NULL )
							{
								fgets( line, 1024, fin ) ;
								token = strtok( line, seps ) ;
							}

							sscanf( token, "%f", &v ) ;
							rvalue->setDataAt( ix/step + padd, iy/step + padd, iz/step + padd, v ) ;
							items ++ ;
							//printf("%f\n", v);

							token = strtok( NULL, seps );
						}
				}

				sx = (sx-1)/step+1 + padd * 2 ;
				sy = (sy-1)/step+1 + padd * 2 ;
				sz = (sz-1)/step+1 + padd * 2 ;

				printf("\tDone reading. %d items\n", items) ;
				fclose( fin ) ;
				return rvalue ;
			}

			/* Get resolution */
			void getSpacing( float& ax, float& ay, float& az )
			{
				ax = 1 ;
				ay = 1 ;
				az = 1 ;
			}

		};


		class MRCReader : public VolumeReader
		{
		public:
			/* Initializer */
			MRCReader( char* fname )
			{
				sprintf( mrcfile, "%s", fname ) ;

				FILE* fin = fopen( fname, "rb" ) ;

				// Parse header
				fread( &totx, sizeof( int ), 1, fin ) ;
				fread( &toty, sizeof( int ), 1, fin ) ;
				fread( &totz, sizeof( int ), 1, fin ) ;

				fread( &mode, sizeof( int ), 1, fin ) ;

				fread( &offx, sizeof( int ), 1, fin ) ;
				fread( &offy, sizeof( int ), 1, fin ) ;
				fread( &offz, sizeof( int ), 1, fin ) ;
				
				fread( &dimx, sizeof( int ), 1, fin ) ;
				fread( &dimy, sizeof( int ), 1, fin ) ;
				fread( &dimz, sizeof( int ), 1, fin ) ;

				fread( &angsx, sizeof( float ), 1, fin ) ;
				fread( &angsy, sizeof( float ), 1, fin ) ;
				fread( &angsz, sizeof( float ), 1, fin ) ;

				fread( &anglex, sizeof( float ), 1, fin ) ;
				fread( &angley, sizeof( float ), 1, fin ) ;
				fread( &anglez, sizeof( float ), 1, fin ) ;

				fseek( fin, 12, SEEK_CUR ) ;

				fread( &dmin, sizeof( float ), 1, fin ) ;
				fread( &dmax, sizeof( float ), 1, fin ) ;
				fread( &dmean, sizeof( float ), 1, fin ) ;

				fseek (fin, 4 * 27, SEEK_CUR );
				fread( &orgx, sizeof( float), 1, fin );
				fread( &orgy, sizeof( float), 1, fin );
				fread( &orgz, sizeof( float), 1, fin );
				
				fseek (fin, 4 * 2, SEEK_CUR);
				fread( &drms, sizeof( float ), 1, fin ) ;
				fclose( fin ) ;

		#ifdef VERBOSE
				printf("\n\tDimension: %d %d %d\n", totx, toty, totz ) ;
				printf("\tMode: %d\n", mode) ;
				printf("\tDensity: from %f to %f, mean at %f, rms at %f\n", dmin, dmax, dmean, drms ) ;
				printf("\tCell size: %f %f %f\n", angsx / dimx, angsy / dimy, angsz / dimz ) ;
				printf("\tCell angles: %f %f %f\n", anglex, angley, anglez ) ;
				printf("\tOrigin location: %f %f %f\n", orgx, orgy, orgz );
		#endif // VERBOSE

				if ( mode > 2 )
				{
					printf("\tComplex mode not supported.\n") ;
					exit(0) ;
				}

				/* Hacking code 
				fseek( fin, 0, SEEK_END ) ;
				long len = ftell( fin ) ;
				len -= 1024 ;

				dimen = 1 ;
				while ( dimen * dimen * dimen < len / 4 )
				{
					dimen ++ ;
				}
				printf("Size: %d\n", dimen) ;
				*/
			}

			/* Read volume */
			Volume* getVolume( )
			{
				FILE* fin = fopen( mrcfile, "rb" ) ;
				fseek( fin, 1024, SEEK_SET ) ;

				char chard ;
				short shortd ;
				float floatd ;
				double d = 0.0;

				
				Volume* vol = new Volume( totx, toty, totz) ;
				for ( int i = 0 ; i < totz ; i ++ )
					for ( int j = 0 ; j < toty ; j ++ )
						for ( int k = 0 ; k < totx ; k ++ )
						{
							switch ( mode )
							{
							case 0: 
								fread( &chard, sizeof( char ), 1, fin ) ;
								d = (double) chard ;
								break ;
							case 1:
								fread( &shortd, sizeof( short ), 1, fin ) ;
								d = (double) shortd ;
								break ;
							case 2:
								fread( &floatd, sizeof( float ), 1, fin ) ;
								d = (double) floatd ;
								break ;
							}
							
							vol->setDataAt( k, j, i, d ) ;
						}

				float ax, ay, az;				
				getSpacing(ax, ay, az);
				vol->setSpacing(ax, ay, az);

				getOffset(ax, ay, az);
				vol->setOrigin(ax, ay, az);
				fclose( fin ) ;

				return vol ;
			}

			/* Get resolution */
			void getSpacing( float& ax, float& ay, float& az )
			{
				ax = angsx / (float)(dimx);
				ay = angsy / (float)(dimy) ;
				az = angsz / (float)(dimz) ;
			}

			void getOffset(float &x, float &y, float &z) 
			{
				float spx, spy, spz;
				getSpacing(spx, spy, spz);
				x = (orgx - 0.5f * (float)totx) * spx;
				y = (orgy - 0.5f * (float)toty) * spy;
				z = (orgz - 0.5f * (float)totz) * spz;
			}


		private:

			int totx, toty, totz ;
			int offx, offy, offz ;
			int dimx, dimy, dimz ;

			float angsx, angsy, angsz ;
			float anglex, angley, anglez ;
			float dmin, dmax, dmean, drms ;
			float orgx, orgy, orgz;
			
			int mode ;

			char mrcfile[1024] ;
		};

		class InvMRCReader : public VolumeReader
		{
		public:
			/* Initializer */
			InvMRCReader( char* fname )
			{
				sprintf( mrcfile, "%s", fname ) ;

				FILE* fin = fopen( fname, "rb" ) ;

				// Parse header
				ifread( &totx, sizeof( int ), 1, fin ) ;
				ifread( &toty, sizeof( int ), 1, fin ) ;
				ifread( &totz, sizeof( int ), 1, fin ) ;


				ifread( &mode, sizeof( int ), 1, fin ) ;

				ifread( &offx, sizeof( int ), 1, fin ) ;
				ifread( &offy, sizeof( int ), 1, fin ) ;
				ifread( &offz, sizeof( int ), 1, fin ) ;
				printf("%d %d %d\n", offx, offy, offz) ;
				
				ifread( &dimx, sizeof( int ), 1, fin ) ;
				ifread( &dimy, sizeof( int ), 1, fin ) ;
				ifread( &dimz, sizeof( int ), 1, fin ) ;

				ifread( &angsx, sizeof( float ), 1, fin ) ;
				ifread( &angsy, sizeof( float ), 1, fin ) ;
				ifread( &angsz, sizeof( float ), 1, fin ) ;

				ifread( &anglex, sizeof( float ), 1, fin ) ;
				ifread( &angley, sizeof( float ), 1, fin ) ;
				ifread( &anglez, sizeof( float ), 1, fin ) ;

				fseek( fin, 12, SEEK_CUR ) ;

				ifread( &dmin, sizeof( float ), 1, fin ) ;
				ifread( &dmax, sizeof( float ), 1, fin ) ;
				ifread( &dmean, sizeof( float ), 1, fin ) ;

				fseek( fin, 4 * 32, SEEK_CUR ) ;

				ifread( &drms, sizeof( float ), 1, fin ) ;
				fclose( fin ) ;

		#ifdef VERBOSE
				printf("\tDimension: %d %d %d\n", totx, toty, totz ) ;
				printf("\tMode: %d\n", mode) ;
				printf("\tDensity: from %f to %f, mean at %f, rms at %f\n", dmin, dmax, dmean, drms ) ;
				printf("\tCell size: %f %f %f\n", angsx / dimx, angsy / dimy, angsz / dimz ) ;
				printf("\tCell angles: %f %f %f\n", anglex, angley, anglez ) ;
		#endif // VERBOSE

				if ( mode > 2 )
				{
					printf("\tComplex mode not supported.\n") ;
					// exit(0) ;
				}

				/* Hacking code 
				fseek( fin, 0, SEEK_END ) ;
				long len = ftell( fin ) ;
				len -= 1024 ;

				dimen = 1 ;
				while ( dimen * dimen * dimen < len / 4 )
				{
					dimen ++ ;
				}
				printf("Size: %d\n", dimen) ;
				*/
			}

			/* Read volume */
			Volume* getVolume( )
			{
				FILE* fin = fopen( mrcfile, "rb" ) ;
				fseek( fin, 1024, SEEK_SET ) ;

				char chard ;
				short shortd ;
				float floatd ;
				double d ;

				
				Volume* vol = new Volume( totx, toty, totz) ;

				for ( int i = 0 ; i < totz ; i ++ )
					for ( int j = 0 ; j < toty ; j ++ )
						for ( int k = 0 ; k < totx ; k ++ )
						{
							switch ( mode )
							{
							case 0: 
								fread( &chard, sizeof( char ), 1, fin ) ;
								d = (double) chard ;
								break ;
							case 1:
								fread( &shortd, sizeof( short ), 1, fin ) ;
								d = (double) shortd ;
								break ;
							default:
								ifread( &floatd, sizeof( float ), 1, fin ) ;
								d = (double) floatd ;
								break ;
							}

		//					printf("%g\n", d) ;exit(0) ;
						
							vol->setDataAt( k, j, i, d ) ;
						}
				fclose( fin ) ;

				float ax, ay, az;				
				getSpacing(ax, ay, az);
				vol->setSpacing(ax, ay, az);
				return vol ;
			}

			void ifread( void * d, size_t s1, size_t s2, FILE* f )
			{
				fread( d, s1, s2, f ) ;
				flipBits32( d ) ;
			}

			/* Get resolution */
			void getSpacing( float& ax, float& ay, float& az )
			{
				ax = angsx / (float)(dimx);
				ay = angsy / (float)(dimy) ;
				az = angsz / (float)(dimz) ;
			}


		private:

			int totx, toty, totz ;
			int offx, offy, offz ;
			int dimx, dimy, dimz ;

			float angsx, angsy, angsz ;
			float anglex, angley, anglez ;
			float dmin, dmax, dmean, drms ;
			
			int mode ;

			char mrcfile[1024] ;
		};

		class MRCReaderPicker
		{
		public:
			MRCReaderPicker(){} ;

			static VolumeReader* pick( char* fname )
			{
				FILE* fin = fopen( fname, "rb" ) ;
				if ( fin == NULL )
				{
					printf("Error reading MRC file %s.\n", fname) ;
					exit(0) ;
				}
				int totx, toty, totz ;

				// Parse header
				fread( &totx, sizeof( int ), 1, fin ) ;
				fread( &toty, sizeof( int ), 1, fin ) ;
				fread( &totz, sizeof( int ), 1, fin ) ;

				fclose( fin ) ;

				if ( totx <= 0 || totx > 1024 )
				{
		#ifdef VERBOSE
					printf("\tCalling inverse MRCreader.\n") ;
		#endif
					return new InvMRCReader( fname ) ;
				}
				else
				{
		#ifdef VERBOSE
					printf("\tCalling MRCreader.\n") ;
		#endif
					return new MRCReader( fname ) ;
				}
			}
		};

		class HackMRCReader : public VolumeReader
		{
		public:
			/* Initializer */
			HackMRCReader( char* fname )
			{
				sprintf( mrcfile, "%s", fname ) ;

				FILE* fin = fopen( fname, "rb" ) ;


				fseek( fin, 0, SEEK_END ) ;
				long len = ftell( fin ) ;
				len -= 1024 ;

				dimen = 1 ;
				while ( dimen * dimen * dimen < len / 4 )
				{
					dimen ++ ;
				}
				printf("\tVolume Size: %d\n", dimen) ;

				fclose( fin ) ;
			}

			/* Read volume */
			Volume* getVolume( )
			{
				FILE* fin = fopen( mrcfile, "rb" ) ;
				fseek( fin, 1024, SEEK_SET ) ;
				
				Volume* vol = new Volume( dimen, dimen, dimen ) ;
				float d ;
				for ( int i = 0 ; i < dimen ; i ++ )
					for ( int j = 0 ; j < dimen ; j ++ )
						for ( int k = 0 ; k < dimen ; k ++ )
						{
							fread( &d, sizeof( float ), 1, fin ) ;
							flipBits32( &d ) ;
							// printf("%g\n", d) ;exit(0) ;
							vol->setDataAt( k, j, i, d ) ;
						}
				fclose( fin ) ;

				return vol ;
			}

			/* Get resolution */
			void getSpacing( float& ax, float& ay, float& az )
			{
				ax = 1 ;
				ay = 1 ;
				az = 1 ;
			}


		private:

			int dimen ;
			char mrcfile[1024] ;
		};


		class MatlabReader : public VolumeReader // Reading what's writen using Matlab's fwrite command

		{

		private:



			char imgfile[1024] ;

			int dimx, dimy, dimz ;

			float spx, spy, spz ;



		public:

			/* Initializer */

			MatlabReader( char* fname, int sx, int sy, int sz )

			{

				dimx = sz ;

				dimy = sy ;

				dimz = sx ;

				spx = 1 ;

				spy = 1 ;

				spz = 1 ;



				FILE *fp;

				if((fp=fopen(fname,"r"))==NULL)

				{

					fprintf(stderr,"Can't open:<%s>\n", fname);

					exit(0);

				}

				fclose( fp ) ;

				strcpy(imgfile, fname) ;



				printf("Dimensions: %d %d %d, ImgFile: %s \n",dimx, dimy, dimz, imgfile) ;

			}



			/* Read volume */

			Volume* getVolume( )

			{

				FILE* fin = fopen( imgfile, "rb" ) ;

				

				Volume* vol = new Volume( dimx, dimy, dimz ) ;

				int dim[3] = {dimx, dimy, dimz};		


				// Read in data

				int size = dim[ 0 ] * dim[ 1 ] * dim[ 2 ] ;

				unsigned short * tempdata = new unsigned short [ size ] ;

				fread( tempdata, sizeof( unsigned short ), size, fin ) ;

				int vmin = 100000, vmax = - 100000 ;

				int ct = 0 ;

				for ( int i = 0 ; i < dim[0] ; i ++ )

					for ( int j = 0 ; j < dim[1] ; j ++ )

						for ( int k = 0 ; k < dim[2] ; k ++ )

						{

							if ( tempdata[ ct ] < vmin )

							{

								vmin = tempdata[ ct ] ;

							}

							if ( tempdata[ ct ] > vmax )

							{

								vmax = tempdata[ ct ] ;

							}

							

							vol->setDataAt( i,j,k, (double) ( tempdata[ ct ] ) );

							ct ++ ;

						}

						printf("Min: %d Max: %d...", vmin, vmax) ;

						

						fclose( fin ) ;



				delete tempdata ;

				vol->setSpacing( spx, spy, spz ) ;



				return vol ;

			}



			/* Get resolution */

			void getSpacing( float& ax, float& ay, float& az )

			{

				ax = spx ;

				ay = spy ;

				az = spz ;

			}





		private:



			int dimen ;

			char mrcfile[1024] ;

		};



		class RawReader : public VolumeReader

		{

		private:



			char imgfile[1024] ;

			int dimx, dimy, dimz ;

			float spx, spy, spz ;



		public:

			/* Initializer */

			RawReader( char* fname, int sx, int sy, int sz )

			{

				dimx = sz ;

				dimy = sy ;

				dimz = sx ;

				spx = 1.4f ;

				spy = 1 ;

				spz = 1 ;



				FILE *fp;

				if((fp=fopen(fname,"r"))==NULL)

				{

					fprintf(stderr,"Can't open:<%s>\n", fname);

					exit(0);

				}

				fclose( fp ) ;

				strcpy(imgfile, fname) ;



				printf("Dimensions: %d %d %d, ImgFile: %s \n",dimx, dimy, dimz, imgfile) ;

			}



			/* Read volume */

			Volume* getVolume( )

			{

				FILE* fin = fopen( imgfile, "rb" ) ;

				

				Volume* vol = new Volume( dimx, dimy, dimz ) ;

				int dim[3] = {dimx, dimy, dimz};


				

				// Read in data

				int size = dim[ 0 ] * dim[ 1 ] * dim[ 2 ] ;

				unsigned short * tempdata = new unsigned short [ size ] ;

				fread( tempdata, sizeof( unsigned short ), size, fin ) ;

				int vmin = 100000, vmax = - 100000 ;

				int ct = 0 ;

				for ( int i = 0 ; i < dim[0] ; i ++ )

					for ( int j = 0 ; j < dim[1] ; j ++ )

						for ( int k = 0 ; k < dim[2] ; k ++ )

						{

							if ( tempdata[ ct ] < vmin )

							{

								vmin = tempdata[ ct ] ;

							}

							if ( tempdata[ ct ] > vmax )

							{

								vmax = tempdata[ ct ] ;

							}

							

							vol->setDataAt( i,j,k, (double) ( tempdata[ ct ] ) );

							ct ++ ;

						}

						printf("Min: %d Max: %d...", vmin, vmax) ;

						

						fclose( fin ) ;



				delete tempdata ;

				vol->setSpacing( spx, spy, spz ) ;



				return vol ;

			}



			/* Get resolution */

			void getSpacing( float& ax, float& ay, float& az )

			{

				ax = spx ;

				ay = spy ;

				az = spz ;

			}





		private:



			int dimen ;

			char mrcfile[1024] ;

		};





		class FlatListReader : public VolumeReader

		{

		private:



			char imgfile[1024] ;

			int dimx, dimy, dimz ;

			int datatype ;

			float spx, spy, spz ;



		public:

			/* Initializer */

			FlatListReader( char* fname )

			{

				FILE *fp;

				if((fp=fopen(fname,"r"))==NULL)

				{

					fprintf(stderr,"Can't open:<%s>\n", fname);

					exit(0);

				}

				

				fscanf( fp, "%d\n", &dimx ) ;

				fscanf( fp, "%d\n", &dimy ) ;

				fscanf( fp, "%d\n", &dimz ) ;

				strcpy(imgfile, fname) ;



				printf("Dimensions: %d %d %d\n",dimx, dimy, dimz) ;

			}



			/* Read volume */

			Volume* getVolume( )

			{

				FILE* fin = fopen( imgfile, "r" ) ;

				char line[1024];

				fgets( line, 1024, fin ) ;

				fgets( line, 1024, fin ) ;

				fgets( line, 1024, fin ) ;

				

				Volume* vol = new Volume( dimx, dimy, dimz ) ;

				float d ;

				for ( int i = 0 ; i < dimx ; i ++ )

					for ( int j = 0 ; j < dimy ; j ++ )

						for ( int k = 0 ; k < dimz ; k ++ )

						{

							fscanf( fin, "%f\n", &d ) ;

							// printf("%g\n", d) ;exit(0) ;

							vol->setDataAt( i,j,k, d ) ;

						}

				fclose( fin ) ;



				vol->setSpacing( 1,1,1 ) ;



				return vol ;

			}



			/* Get resolution */

			void getSpacing( float& ax, float& ay, float& az )

			{

				ax = 1 ;

				ay = 1 ;

				az = 1 ;

			}





		private:



			int dimen ;

			char mrcfile[1024] ;

		};





		class TimDoseReader : public VolumeReader

		{

		private:



			char imgfile[1024] ;

			int dimx, dimy, dimz ;

			float spx, spy, spz ;



		public:

			/* Initializer */

			TimDoseReader( char* fname )

			{

				strcpy(imgfile, fname) ;



				/*

				FILE *fid;

				if((fid=fopen(fname,"r"))==NULL)

				{

					fprintf(stderr,"Can't open:<%s>\n", fname);

					exit(0);

				}

				char line[1024];

				

				// Read dose Header

				fscanf( fid, "%d %d %d", &dimx, &dimy, &dimz ) ;



				// Read dose coords

				spx = 0 ;

				spy = 0 ;

				spz = 0 ;

				float lx=0,hx=0,ly=0,hy=0,lz=0,hz=0;

				for ( int i = 0 ; i < dimx ; i ++ )

				{

					lx = hx ;

					fscanf( fid, "%f ", &hx);

					if ( i )

					{

						spx = hx - lx ;

					}

				}

				for ( i = 0 ; i < dimy ; i ++ )

				{

					ly = hy ;

					fscanf( fid, "%f ", &hy);

					if ( i )

					{

						spy = hy - ly ;

					}

				}

				for ( i = 0 ; i < dimz ; i ++ )

				{

					lz = hz ;

					fscanf( fid, "%f ", &hz);

					if ( i )

					{

						spz = hz - lz ;

					}

				}



				fclose( fid ) ;

				*/



				FILE *fin;

				if((fin=fopen(fname,"r"))==NULL)

				{

					fprintf(stderr,"Can't open:<%s>\n", fname);

					exit(0);

				}



				// Scan

		#ifdef CT_STRUCTURE

				{

					// Scan Header

					fscanf(fin, "%d %d %d \n", &dimx, &dimy, &dimz);



					readSpacing( fin ) ;

				}

		#else

				{

					// Scan Header

					int xSize, ySize, zSize;

					fscanf(fin, "%d %d %d \n", &xSize, &ySize, &zSize);



					printf("skipping...");

					skipFloats( fin, xSize ) ;

					skipFloats( fin, ySize ) ;

					skipFloats( fin, zSize ) ;

					skipFloats(fin, (xSize * ySize * zSize));

					printf("skipped.");

				}

		#endif

				// Structures

				{

					// Structures Header

					int xSize, ySize, zSize, numStructures;

					fscanf(fin, "%d %d %d %d \n", &xSize, &ySize, &zSize, &numStructures);



					skipFloats( fin, xSize ) ;

					skipFloats( fin, ySize ) ;

					skipFloats( fin, zSize ) ;

					skipFloats(fin, (xSize * ySize * zSize));

				}

			

		#ifndef CT_STRUCTURE

				// Dose

				{

					// Dose Header

					fscanf(fin, "%d %d %d \n", &dimx, &dimy, &dimz);

					readSpacing( fin ) ;

				}

		#endif

				fclose(fin);



				printf("Dimensions: %d %d %d, ImgFile: %s Spacing: %f %f %f\n",dimx, dimy, dimz, imgfile, spx, spy, spz) ;

			}



			void readSpacing( FILE* fid )

			{
					int i ;

					// Read dose coords

					spx = 0 ;

					spy = 0 ;

					spz = 0 ;

					float lx=0,hx=0,ly=0,hy=0,lz=0,hz=0;

					for ( i = 0 ; i < dimx ; i ++ )

					{

						lx = hx ;

						fscanf( fid, "%f ", &hx);

						if ( i )

						{

							spx = hx - lx ;

						}

					}

					for ( i = 0 ; i < dimy ; i ++ )

					{

						ly = hy ;

						fscanf( fid, "%f ", &hy);

						if ( i )

						{

							spy = hy - ly ;

						}

					}

					for ( i = 0 ; i < dimz ; i ++ )

					{

						lz = hz ;

						fscanf( fid, "%f ", &hz);

						if ( i )

						{

							spz = hz - lz ;

						}

					}



			}



			void skipInts(FILE* fin, int n)

			{

				skipFloats( fin, n ) ;

				/*

				int temp;

				for(int i = 0; i < n; i++)

				{

					fscanf(fin, "%d ", &temp);

				}

				*/

			}



			void skipFloats(FILE* fin, int n)

			{

				char* line = new char[ n * 10 ] ;

				fgets( line, n * 10, fin ) ;

				delete line ;

				/*

				float temp;

				for(int i = 0; i < n; i++)

				{

					fscanf(fin, "%f ", &temp);

				}

				*/

			}



			void readVolume( Volume* vol, FILE* fid )

			{

				float maxd = -100000, mind = 100000;

				for ( int x = 0 ; x < dimx ; x ++ )

					for ( int y = 0 ; y < dimy ; y ++ )

						for ( int z = 0 ; z < dimz ; z ++ )

						{

							float data ;

							fscanf( fid, "%f ", &data ) ;

							vol->setDataAt( x, y, z, data ) ;



							if ( data < mind )

							{

								mind = data ;

							}

							if ( data > maxd )

							{

								maxd = data ;

							}

						}



				fclose( fid ) ;

			}





			/* Read volume */

			Volume* getVolume( )

			{

				Volume* vol = new Volume( dimx, dimy, dimz ) ;

				vol->setSpacing( spx, spy, spz ) ;

		/*

				FILE *fid;

				if((fid=fopen(imgfile,"r"))==NULL)

				{

					fprintf(stderr,"Can't open:<%s>\n", imgfile);

					exit(0);

				}



				

				

				// By pass Header

				fgets( line, 10000, fid ) ;

				fgets( line, 10000, fid ) ;

				fgets( line, 10000, fid ) ;

				fgets( line, 10000, fid ) ;

				fgets( line, 10000, fid ) ;



		#ifdef CT_STRUCTURE 

				for ( int skip = 0 ; skip < CT_STRUCTURE + 4; skip ++ )

				{

					fgets( line, 10000, fid ) ;

				}

		#endif



				// Read dose values

				float maxd = -100000, mind = 100000;

				for ( int x = 0 ; x < dimx ; x ++ )

					for ( int y = 0 ; y < dimy ; y ++ )

						for ( int z = 0 ; z < dimz ; z ++ )

						{

							float data ;

							fscanf( fid, "%f ", &data ) ;

							vol->setDataAt( x, y, z, data ) ;



							if ( data < mind )

							{

								mind = data ;

							}

							if ( data > maxd )

							{

								maxd = data ;

							}

						}



				fclose( fid ) ;



				printf("Max: %f Min: %f\n", maxd, mind) ;

				*/



				FILE *fin;

				if((fin=fopen(imgfile,"r"))==NULL)

				{

					fprintf(stderr,"Can't open:<%s>\n", imgfile);

					exit(0);

				}



					// Scan Header

					int xSize, ySize, zSize;

					fscanf(fin, "%d %d %d \n", &xSize, &ySize, &zSize);



					skipFloats( fin, xSize ) ;

					skipFloats( fin, ySize ) ;

					skipFloats( fin, zSize ) ;

					skipFloats(fin, (xSize * ySize * zSize));



				// Structures

				{

					// Structures Header

					int xSize, ySize, zSize, numStructures;

					fscanf(fin, "%d %d %d %d \n", &xSize, &ySize, &zSize, &numStructures);



					skipFloats( fin, xSize ) ;

					skipFloats( fin, ySize ) ;

					skipFloats( fin, zSize ) ;

		#ifdef CT_STRUCTURE

					readVolume( vol, fin );

		#else

					skipFloats(fin, (xSize * ySize * zSize));

		#endif

				}

			

		#ifndef CT_STRUCTURE

				// Dose

				{

					// Dose Header

					int xSize, ySize, zSize;

					fscanf(fin, "%d %d %d \n", &xSize, &ySize, &zSize);

					skipFloats( fin, xSize ) ;

					skipFloats( fin, ySize ) ;

					skipFloats( fin, zSize ) ;

					readVolume( vol, fin ) ;

				}

		#endif

				fclose(fin);





				return vol ;

			}



			/* Get resolution */

			void getSpacing( float& ax, float& ay, float& az )

			{

				ax = spx ;

				ay = spy ;

				az = spz ;

			}





		private:



			int dimen ;

			char mrcfile[1024] ;

		};



		class StructureReader : public VolumeReader

		{

		private:



			char imgfile[1024] ;

			int dimx, dimy, dimz ;

			float spx, spy, spz ;

			int numstructs ;



		public:

			/* Initializer */

			StructureReader( char* fname )

			{

				strcpy(imgfile, fname) ;

			

				FILE *fid;

				if((fid=fopen(fname,"rb"))==NULL)

				{

					fprintf(stderr,"Can't open:<%s>\n", fname);

					exit(0);

				}

				

				// Read structure Header

				fread(&dimx, sizeof(int), 1, fid ) ;

				fread(&dimy, sizeof(int), 1, fid ) ;

				fread(&dimz, sizeof(int), 1, fid ) ;

				fread(&numstructs, sizeof(int), 1, fid ) ;

				// Read structure coords

				float* data = new float[ dimx ] ;

				fread( data, sizeof(float), dimx, fid ) ;

				spx= (data[dimx-1]- data[0]) / (float)(dimx-1) ;

				delete data ;

				data = new float[ dimy ] ;

				fread( data, sizeof(float), dimy, fid ) ;

				spy= (data[dimy-1]- data[0]) / (float)(dimy-1) ;

				delete data ;

				data = new float[ dimz ] ;

				fread( data, sizeof(float), dimz, fid ) ;

				spz= (data[dimz-1]- data[0]) / (float)(dimz-1) ;

				delete data ;



				fclose( fid ) ;



				printf("Dimensions: %d %d %d, Spacing: %f %f %f, structures: %d\n",dimx, dimy, dimz, spx, spy, spz, numstructs) ;

			}



			void skipInts(FILE* fin, int n)

			{

				int* data = new int[ n ] ;

				fread( data, sizeof( int ), n, fin ) ;

				delete data ;

			}



			void skipFloats(FILE* fin, int n)

			{

				float* data = new float[ n ] ;

				fread( data, sizeof( float ), n, fin ) ;

				delete data ;

			}



			void readVolume( Volume* vol, FILE* fid, int ind )

			{

				int mind = 100000, maxd = -100000 ;

				int* data = new int[ dimx * dimy * dimz ] ;

				fread( data, sizeof( int ), dimx * dimy * dimz, fid ) ;

				int ct = 0 ;

				for ( int x = 0 ; x < dimx ; x ++ )

					for ( int y = 0 ; y < dimy ; y ++ )

						for ( int z = 0 ; z < dimz ; z ++ )

						{

							int d = (( data[ct] >> ind ) & 1 ) ;

							vol->setDataAt( x, y, z, (double) d ) ;

							ct ++ ;



							if ( d < mind )

							{

								mind = d ;

							}

							if ( d > maxd )

							{

								maxd = d ;

							}

						}

				printf( "Min: %d, max: %d\n", mind, maxd ) ;

				delete data ;

			}



			int getNumStructures( )

			{

				return this->numstructs ;

			}



			/* Read volume */

			Volume* getVolume( )

			{

				return NULL ;

			}



			Volume* getVolume( int ind )

			{

				Volume* vol = new Volume( dimx, dimy, dimz ) ;

				vol->setSpacing( spx, spy, spz ) ;



				FILE *fid;

				if((fid=fopen(imgfile,"rb"))==NULL)

				{

					fprintf(stderr,"Can't open:<%s>\n", imgfile);

					exit(0);

				}



				// By pass Header

				skipFloats( fid, 4 + dimx + dimy + dimz ) ;

				

				readVolume( vol, fid, ind + 1 ) ;



				fclose(fid);





				return vol ;

			}



			/* Get resolution */

			void getSpacing( float& ax, float& ay, float& az )

			{

				ax = spx ;

				ay = spy ;

				az = spz ;

			}





		private:



			int dimen ;

			char mrcfile[1024] ;

		};



		class DoseReader : public VolumeReader

		{

		private:



			char imgfile[1024] ;

			int dimx, dimy, dimz ;

			float spx, spy, spz ;



		public:

			/* Initializer */

			DoseReader( char* fname )

			{

				strcpy(imgfile, fname) ;

			

				FILE *fid;

				if((fid=fopen(fname,"rb"))==NULL)

				{

					fprintf(stderr,"Can't open:<%s>\n", fname);

					exit(0);

				}

				

				// Read dose Header

				fread(&dimx, sizeof(int), 1, fid ) ;

				fread(&dimy, sizeof(int), 1, fid ) ;

				fread(&dimz, sizeof(int), 1, fid ) ;



				// Read dose coords

				float* data = new float[ dimx ] ;

				fread( data, sizeof(float), dimx, fid ) ;

				printf("%f %f\n", data[dimx-2], data[dimx-1]) ;

				spx= data[dimx-1]- data[dimx-2] ;

				delete data ;

				data = new float[ dimy ] ;

				fread( data, sizeof(float), dimy, fid ) ;

				spy= data[dimy-1]- data[dimy-2] ;

				delete data ;

				data = new float[ dimz ] ;

				fread( data, sizeof(float), dimz, fid ) ;

				spz= data[dimz-1]- data[dimz-2] ;

				delete data ;



				fclose( fid ) ;



				printf("Dimensions: %d %d %d, ImgFile: %s Spacing: %f %f %f\n",dimx, dimy, dimz, imgfile, spx, spy, spz) ;

			}



			void skipInts(FILE* fin, int n)

			{

				int* data = new int[ n ] ;

				fread( data, sizeof( int ), n, fin ) ;

				delete data ;

			}



			void skipFloats(FILE* fin, int n)

			{

				float* data = new float[ n ] ;

				fread( data, sizeof( float ), n, fin ) ;

				delete data ;

			}



			void readVolume( Volume* vol, FILE* fid )

			{

				float maxd = -100000, mind = 100000;

				float* data = new float[ dimx * dimy * dimz ] ;

				fread( data, sizeof( float ), dimx * dimy * dimz, fid ) ;

				int ct = 0 ;

				for ( int x = 0 ; x < dimx ; x ++ )

					for ( int y = 0 ; y < dimy ; y ++ )

						for ( int z = 0 ; z < dimz ; z ++ )

						{

							float d = data[ct] ;

							vol->setDataAt( x, y, z, d ) ;

							ct ++ ;



							if ( d < mind )

							{

								mind = d ;

							}

							if ( d > maxd )

							{

								maxd = d ;

							}

						}

				delete data ;

			}





			/* Read volume */

			Volume* getVolume( )

			{

				Volume* vol = new Volume( dimx, dimy, dimz ) ;

				vol->setSpacing( spx, spy, spz ) ;



				FILE *fid;

				if((fid=fopen(imgfile,"rb"))==NULL)

				{

					fprintf(stderr,"Can't open:<%s>\n", imgfile);

					exit(0);

				}



				// By pass Header

				skipFloats( fid, 3 + dimx + dimy + dimz ) ;

				

				readVolume( vol, fid ) ;



				fclose(fid);





				return vol ;

			}



			/* Get resolution */

			void getSpacing( float& ax, float& ay, float& az )

			{

				ax = spx ;

				ay = spy ;

				az = spz ;

			}





		private:



			int dimen ;

			char mrcfile[1024] ;

		};







		class CylinderVolumeGenerator : public VolumeReader
		{
		public:
			/* Initializer */
			CylinderVolumeGenerator( int dim, double rad )
			{
				size = dim ;
				radius = rad ;
			};

			Volume* getVolume()
			{
				Volume* vol = new Volume( size, size, size ) ;
				int thick = size / 6 ;

				double cent = (size - 1) / 2.0 ;
				for ( int x = 0 ; x < size ; x ++ )
					for ( int y = 0 ; y < size ; y ++ )
						for ( int z = 0 ; z < size ; z ++ )
						{
							double dis = sqrt( ( z - cent ) * ( z - cent ) + ( x - cent ) * ( x - cent ) );
							
							if ( fabs( y - cent ) > thick )
							{
								vol->setDataAt( x, y, z, - ( fabs( y - cent ) - thick ) ) ;
							}
							else
							{
								vol->setDataAt( x, y, z, radius - dis ) ;
							}

							// double dis = ( z - cent ) * ( z - cent ) + ( x - cent ) * ( x - cent );
							// vol->setDataAt( x, y, z, 10 - 10 * dis / r2 ) ;
						};

				return vol ;
			};

			/* Get resolution */
			void getSpacing( float& ax, float& ay, float& az )
			{
				ax = 1 ;
				ay = 1 ;
				az = 1 ;
			}


		private:

			int size ;
			double radius ;
		};


		class SheetVolumeGenerator : public VolumeReader
		{
		public:
			/* Initializer */
			SheetVolumeGenerator( int dim, double hit )
			{
				size = dim ;
				height = hit ;
			};

			Volume* getVolume()
			{
				Volume* vol = new Volume( size, size, size ) ;

				double cent = (size - 1) / 2.0 ;
				for ( int x = 0 ; x < size ; x ++ )
					for ( int y = 0 ; y < size ; y ++ )
						for ( int z = 0 ; z < size ; z ++ )
						{
							double dis = z - cent;
							if(dis < 0) {
								dis = -dis;
							}
							vol->setDataAt( x, y, z, 10 - 10 * dis / height ) ;

							// double dis = ( z - cent ) * ( z - cent ) + ( x - cent ) * ( x - cent );
							// vol->setDataAt( x, y, z, 10 - 10 * dis / r2 ) ;
						};

				return vol ;
			};

			/* Get resolution */
			void getSpacing( float& ax, float& ay, float& az )
			{
				ax = 1 ;
				ay = 1 ;
				az = 1 ;
			}


		private:

			int size ;
			double height ;
		};




		class SphereVolumeGenerator : public VolumeReader
		{
		public:
			/* Initializer */
			SphereVolumeGenerator( int dim, double ratiox, double ratioy, double ratioz )
			{
				size = dim ;
				rx = (int)ratiox ;
				ry = (int)ratioy ;
				rz = (int)ratioz ;
			};

			Volume* getVolume()
			{
				Volume* vol = new Volume( size, size, size ) ;

				double dis = ( size - 1 ) * ( size - 1 ) / 100 ;

				for ( int x = 0 ; x < size ; x ++ )
					for ( int y = 0 ; y < size ; y ++ )
						for ( int z = 0 ; z < size ; z ++ )
						{
							double d = rx * ( x - ( size - 1 ) / 2) * ( x - ( size - 1 ) / 2) + 
								ry * ( y - ( size - 1 ) / 2) * ( y - ( size - 1 ) / 2) + 
								rz * ( z - ( size - 1 ) / 2) * ( z - ( size - 1 ) / 2) ;
							vol->setDataAt( x, y, z, 10 - 10 * sqrt(d) / sqrt(dis) ) ;
							// vol->setDataAt( x, y, z, 10 - 10 * (x + y) / size ) ;
						};

				return vol ;
			};

			/* Get resolution */
			void getSpacing( float& ax, float& ay, float& az )
			{
				ax = 1 ;
				ay = 1 ;
				az = 1 ;
			}


		private:

			int size, rx, ry, rz ;
		};
	}
}

#endif
