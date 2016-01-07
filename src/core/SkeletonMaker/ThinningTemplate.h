// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Tao Ju, Refactored by Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A template for thinning.


#ifndef SKELETON_MAKER_THINNING_TEMPLATE_H
#define SKELETON_MAKER_THINNING_TEMPLATE_H

namespace wustl_mm {
	namespace SkeletonMaker {
		class ThinningTemplate {
		public:
			ThinningTemplate( int blk[], int nblk, int wht[], int nwht, int oneblk[], int noneblk, int oneblk2[], int noneblk2, int onewht[], int nonewht, int opp[], int nopp  );
			ThinningTemplate( ThinningTemplate* temp, int axis );
			ThinningTemplate( ThinningTemplate* temp, int axis, int dir );
			int isMatch( int vox[3][3][3] );

		public:
			int coords[27][3] ;					// Coordinates
			int black[27], numBlack ;			// Must-blacks
			int white[27], numWhite ;			// Must-whites
			int oneBlack[27], numOneBlack ;		// At-least-one-blacks
			int oneBlack2[27], numOneBlack2 ;	// At-least-one-blacks
			int oneWhite[27], numOneWhite ;		// At-least-one-whites
			int opposite[2], numOpposite ;		// Opposites
		};

		ThinningTemplate::ThinningTemplate( int blk[], int nblk, int wht[], int nwht, int oneblk[], int noneblk, int oneblk2[], int noneblk2, int onewht[], int nonewht, int opp[], int nopp  )	{
			int i, j, k ;
			
			numBlack = nblk ;
			numWhite = nwht ;
			numOneBlack = noneblk ;
			numOneBlack2 = noneblk2 ;
			numOneWhite = nonewht ;
			numOpposite = nopp ;

			for ( i = 0 ; i < numBlack ; i ++ )
			{
				black[i] = blk[i] ;
			}
			for ( i = 0 ; i < numWhite ; i ++ )
			{
				white[i] = wht[i] ;
			}
			for ( i = 0 ; i < numOneBlack ; i ++ )
			{
				oneBlack[i] = oneblk[i] ;
			}
			for ( i = 0 ; i < numOneBlack2 ; i ++ )
			{
				oneBlack2[i] = oneblk2[i] ;
			}
			for ( i = 0 ; i < numOneWhite ; i ++ )
			{
				oneWhite[i] = onewht[i] ;
			}
			for ( i = 0 ; i < numOpposite ; i ++ )
			{
				opposite[i] = opp[i] ;
			}

			int ct = 0 ;
			for ( i = 0 ; i < 3 ; i ++ )
				for ( j = 0 ; j < 3 ; j ++ )
					for ( k = 0 ; k < 3 ; k ++ )
					{
						coords[ ct ][ 0 ] = i ;
						coords[ ct ][ 1 ] = j ;
						coords[ ct ][ 2 ] = k ;
						ct ++ ;
					}
		}

			/* Flipping to generate a new template */
		ThinningTemplate::ThinningTemplate ( ThinningTemplate* temp, int axis )	{
			int i ;
			
			numBlack = temp->numBlack ;
			numWhite = temp->numWhite ;
			numOneBlack = temp->numOneBlack ;
			numOneBlack2 = temp->numOneBlack2 ;
			numOneWhite = temp->numOneWhite ;
			numOpposite = temp->numOpposite ;

			for ( i = 0 ; i < numBlack ; i ++ )
			{
				black[i] = temp->black[i] ;
			}
			for ( i = 0 ; i < numWhite ; i ++ )
			{
				white[i] = temp->white[i] ;
			}
			for ( i = 0 ; i < numOneBlack ; i ++ )
			{
				oneBlack[i] = temp->oneBlack[i] ;
			}
			for ( i = 0 ; i < numOneBlack2 ; i ++ )
			{
				oneBlack2[i] = temp->oneBlack2[i] ;
			}
			for ( i = 0 ; i < numOneWhite ; i ++ )
			{
				oneWhite[i] = temp->oneWhite[i] ;
			}
			for ( i = 0 ; i < numOpposite ; i ++ )
			{
				opposite[i] = temp->opposite[i] ;
			}

			for ( i = 0 ; i < 27 ; i ++ )
			{
				switch( axis )
				{
				case 0 : // X
					coords[ i ][ 0 ] = 2 - temp->coords[ i ][ 0 ] ;
					coords[ i ][ 1 ] = temp->coords[ i ][ 1 ];
					coords[ i ][ 2 ] = temp->coords[ i ][ 2 ] ;
					break ;
					
				case 1 : // Y
					coords[ i ][ 0 ] = temp->coords[ i ][ 0 ] ;
					coords[ i ][ 1 ] = 2 - temp->coords[ i ][ 1 ];
					coords[ i ][ 2 ] = temp->coords[ i ][ 2 ] ;
					break ;
					
				case 2 : // Z
					coords[ i ][ 0 ] = temp->coords[ i ][ 0 ] ;
					coords[ i ][ 1 ] = temp->coords[ i ][ 1 ];
					coords[ i ][ 2 ] = 2 - temp->coords[ i ][ 2 ] ;
					break ;
					
				}
			}
			
		}

			/* Rotation to generate a new template */
		ThinningTemplate::ThinningTemplate ( ThinningTemplate* temp, int axis, int dir ) {
			int i ;
			
			numBlack = temp->numBlack ;
			numWhite = temp->numWhite ;
			numOneBlack = temp->numOneBlack ;
			numOneBlack2 = temp->numOneBlack2 ;
			numOneWhite = temp->numOneWhite ;
			numOpposite = temp->numOpposite ;

			for ( i = 0 ; i < numBlack ; i ++ )
			{
				black[i] = temp->black[i] ;
			}
			for ( i = 0 ; i < numWhite ; i ++ )
			{
				white[i] = temp->white[i] ;
			}
			for ( i = 0 ; i < numOneBlack ; i ++ )
			{
				oneBlack[i] = temp->oneBlack[i] ;
			}
			for ( i = 0 ; i < numOneBlack2 ; i ++ )
			{
				oneBlack2[i] = temp->oneBlack2[i] ;
			}
			for ( i = 0 ; i < numOneWhite ; i ++ )
			{
				oneWhite[i] = temp->oneWhite[i] ;
			}
			for ( i = 0 ; i < numOpposite ; i ++ )
			{
				opposite[i] = temp->opposite[i] ;
			}

			for ( i = 0 ; i < 27 ; i ++ )
			{
				switch( axis )
				{
				case 0 : // X
					coords[ i ][ 0 ] = temp->coords[ i ][ 0 ] ;
					if ( dir == 1 )
					{
						coords[ i ][ 1 ] = 2 - temp->coords[ i ][ 2 ];
						coords[ i ][ 2 ] = temp->coords[ i ][ 1 ] ;
					}
					else
					{
						coords[ i ][ 1 ] = temp->coords[ i ][ 2 ];
						coords[ i ][ 2 ] = 2 - temp->coords[ i ][ 1 ] ;
					}
					break ;
					
				case 1 : // Y
					coords[ i ][ 1 ] = temp->coords[ i ][ 1 ] ;
					if ( dir == 1 )
					{
						coords[ i ][ 0 ] = temp->coords[ i ][ 2 ];
						coords[ i ][ 2 ] = 2 - temp->coords[ i ][ 0 ] ;
					}
					else
					{
						coords[ i ][ 0 ] = 2 - temp->coords[ i ][ 2 ];
						coords[ i ][ 2 ] = temp->coords[ i ][ 0 ] ;
					}
					break ;
					
				case 2 : // Z
					coords[ i ][ 2 ] = temp->coords[ i ][ 2 ] ;
					if ( dir == 1 )
					{
						coords[ i ][ 0 ] = 2 - temp->coords[ i ][ 1 ];
						coords[ i ][ 1 ] = temp->coords[ i ][ 0 ] ;
					}
					else
					{
						coords[ i ][ 0 ] = temp->coords[ i ][ 1 ];
						coords[ i ][ 1 ] = 2 - temp->coords[ i ][ 0 ] ;
					}
					break ;
					
				}
			}
			
		}


		/* Checking with the template */
		/* Input: a 3by3 array of zero (white) and ones (black) */
		int ThinningTemplate::isMatch( int vox[3][3][3] ) {
			int i;

			for ( i = 0 ; i < numWhite ; i ++ )
			{
				if ( vox[ coords[white[i]][0] ][ coords[white[i]][1] ][ coords[white[i]][2] ] != 0 )
				{
					return 0 ;
				}
			}

			for ( i = 0 ; i < numBlack ; i ++ )
			{
				if ( vox[ coords[black[i]][0] ][ coords[black[i]][1] ][ coords[black[i]][2] ] != 1 )
				{
					return 0 ;
				}
			}

			int hasOne = 0 ;
			for ( i = 0 ; i < numOneBlack ; i ++ )
			{
				if ( vox[ coords[oneBlack[i]][0] ][ coords[oneBlack[i]][1] ][ coords[oneBlack[i]][2] ] == 1 )
				{
					hasOne = 1 ;
					break ;
				}
			}
			if ( hasOne == 0 && numOneBlack > 0 )
			{
				return 0 ;
			}

			hasOne = 0 ;
			for ( i = 0 ; i < numOneBlack2 ; i ++ )
			{
				if ( vox[ coords[oneBlack2[i]][0] ][ coords[oneBlack2[i]][1] ][ coords[oneBlack2[i]][2] ] == 1 )
				{
					hasOne = 1 ;
					break ;
				}
			}
			if ( hasOne == 0 && numOneBlack2 > 0 )
			{
				return 0 ;
			}

			hasOne = 0 ;
			for ( i = 0 ; i < numOneWhite ; i ++ )
			{
				if ( vox[ coords[oneWhite[i]][0] ][ coords[oneWhite[i]][1] ][ coords[oneWhite[i]][2] ] == 0 )
				{
					hasOne = 1 ;
					break ;
				}
			}
			if ( hasOne == 0 && numOneWhite > 0 )
			{
				return 0 ;
			}

			if ( numOpposite > 0 )
			{
				if ( vox[ coords[opposite[0]][0] ][ coords[opposite[0]][1] ][ coords[opposite[0]][2] ] ==  
					 vox[ coords[opposite[1]][0] ][ coords[opposite[1]][1] ][ coords[opposite[1]][2] ])
				{
					return 0 ;
				}
			}

			return 1 ;
		}
		
	}
}

#endif
