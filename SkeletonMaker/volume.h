/**
 * Volumetric data definition
 *
 * Author: Tao Ju
 * Date: 02/16/2005
 */


#ifndef VOLUME_H
#define VOLUME_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "PriorityQueue.h"

#define MAX_SHEETS 100000
#define MAX_QUEUELEN 100000

#define MAX_ERODE 1000

//#define NOISE_DIS_SHEET 1
//#define NOISE_DIS_HELIX 5


const int neighbor6[6][3]={{0,0,1},{0,0,-1},{0,1,0},{0,-1,0},{1,0,0},{-1,0,0}} ;
const int neighbor64[6][4][3] = { 
	{{0,1,0},{0,-1,0},{1,0,0},{-1,0,0}},
	{{0,1,0},{0,-1,0},{1,0,0},{-1,0,0}},
	{{0,0,1},{0,0,-1},{1,0,0},{-1,0,0}},
	{{0,0,1},{0,0,-1},{1,0,0},{-1,0,0}},
	{{0,0,1},{0,0,-1},{0,1,0},{0,-1,0}},
	{{0,0,1},{0,0,-1},{0,1,0},{0,-1,0}}} ;
	
const int sheetNeighbor[12][4][3] = {
	{{0,-1,-1},{0,-1,0},{0,0,-1},{0,0,0}},
	{{0,-1,0},{0,-1,1},{0,0,0},{0,0,1}},
	{{0,0,-1},{0,0,0},{0,1,-1},{0,1,0}},
	{{0,0,0},{0,0,1},{0,1,0},{0,1,1}},

	{{-1,0,-1},{-1,0,0},{0,0,-1},{0,0,0}},
	{{-1,0,0},{-1,0,1},{0,0,0},{0,0,1}},
	{{0,0,-1},{0,0,0},{1,0,-1},{1,0,0}},
	{{0,0,0},{0,0,1},{1,0,0},{1,0,1}},

	{{-1,-1,0},{-1,0,0},{0,-1,0},{0,0,0}},
	{{-1,0,0},{-1,1,0},{0,0,0},{0,1,0}},
	{{0,-1,0},{0,0,0},{1,-1,0},{1,0,0}},
	{{0,0,0},{0,1,0},{1,0,0},{1,1,0}}
	};

const int faceCells[12][2]={{0,4},{1,5},{2,6},{3,7},{0,2},{1,3},{4,6},{5,7},{0,1},{2,3},{4,5},{6,7}};

const int cubeFaces[6][4] = 
{ {1,5,7,3},{0,2,6,4},{2,3,7,6},{0,4,5,1},{5,4,6,7},{0,1,3,2}};


//const int faceEdges[12][2] = {{2,4},{2,5},{3,4},{3,5},
//{0,4},{0,5},{1,4},{1,5},
//{0,2},{0,3},{1,2},{1,3}};

const int faceEdges[12][2] = {{3,1},{3,0},{2,1},{2,0},
{5,1},{5,0},{4,1},{4,0},
{5,3},{5,2},{4,3},{4,2}};

const int edgeFaces[6][4] = {{1,3,5,7},{0,2,4,6},{2,3,9,11},{0,1,8,10},{6,7,10,11},{4,5,8,9}} ;

struct gridPoint
{
	int x, y, z;
};

struct gridQueueEle
{
	int x, y, z;
	int score ;
	gridQueueEle* next ;
};


class ThinningTemplate
{
public:
	// Coordinates
	int coords[27][3] ;
	
	// Must-blacks
	int black[27], numBlack ;
	// Must-whites
	int white[27], numWhite ;
	// At-least-one-blacks
	int oneBlack[27], numOneBlack ;
	int oneBlack2[27], numOneBlack2 ;
	// At-least-one-whites
	int oneWhite[27], numOneWhite ;
	// Opposites
	int opposite[2], numOpposite ;

public:
	/* Initialization */
	ThinningTemplate( int blk[], int nblk, int wht[], int nwht, 
		int oneblk[], int noneblk, int oneblk2[], int noneblk2, int onewht[], int nonewht, int opp[], int nopp  )
	{
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
	ThinningTemplate ( ThinningTemplate* temp, int axis )
	{
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
	ThinningTemplate ( ThinningTemplate* temp, int axis, int dir )
	{
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
	int isMatch( int vox[3][3][3] )
	{
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

};

class GridQueue2
{
	gridQueueEle* head ;
	gridQueueEle* pre ;
	gridQueueEle* prepre ;
	gridQueueEle* cur ;
	int numEles ;

public:

	GridQueue2( )
	{
		head = NULL ;
		cur = NULL ;
		pre = NULL ;
		prepre = NULL ;
		numEles = 0 ;
	}

	gridQueueEle* getNext( )
	{
		if ( cur == NULL )
		{
			prepre = NULL ;
			pre = NULL ;
			cur = head ;
		}
		else
		{
			prepre = pre ;
			pre = cur ;
			cur = cur->next ;
		}

		return cur ;
	}

	void reset( )
	{
		prepre = NULL ;
		pre = NULL ;
		cur = NULL ;
	}

	int getNumElements( )
	{
		return numEles ;
	}

	void prepend( int xx, int yy, int zz )
	{
		gridQueueEle* ele = new gridQueueEle ;
		ele->x = xx ;
		ele->y = yy ;
		ele->z = zz ;
		ele->score = 0 ;
		ele->next = head;
		head = ele ;
		numEles ++ ;

		reset() ;
	}

	/* Remove current element pointed by cur */
	gridQueueEle* remove( )
	{
		gridQueueEle* temp = cur ;
		if ( cur != NULL )
		{
			cur = cur->next ;
			delete temp ;

			if ( pre != NULL )
			{
				pre->next = cur ;
			}
			else
			{
				head = cur ;
			}
			numEles -- ;
		}


		return cur ;
	}

	/* Switching pre and cur */
	gridQueueEle* swap( )
	{
		if ( prepre != NULL )
		{
			pre->next = cur->next ;
			cur->next = prepre->next ;
			prepre->next = cur ;

		}
		else
		{
			pre->next = cur->next ;
			cur->next = pre ;
			head = cur ;
		}
		
		gridQueueEle* temp = pre ;
		pre = cur ;
		cur = temp ;

		return cur ;
	}
};

class GridQueue
{
	gridQueueEle* head ;
	gridQueueEle* tail ;
	int numEles ;

	/* Switching two elements */
	void swapEle( gridQueueEle* pre, gridQueueEle* e1, gridQueueEle* e2 )
	{
		if ( pre != NULL )
		{
			e1->next = e2->next ;
			e2->next = pre->next ;
			pre->next = e2 ;

			if ( tail == e2 )
			{
				tail = e1 ;
			}
		}
		else
		{
			e1->next = e2->next ;
			e2->next = e1 ;
			head = e2 ;

			if ( tail == e2 )
			{
				tail = e1 ;
			}
		}
	}

public:

	GridQueue( )
	{
		head = NULL ;
		tail = NULL ;
		numEles = 0 ;
	}

	gridQueueEle* getHead( )
	{
		return head ;
	}

	int getNumElements( )
	{
		return numEles ;
	}

	/* Naive bubble sort */
	void sort( int eles )
	{
		//printf("Sorting elements with descending scores...\n") ;
		gridQueueEle* pre ;
		gridQueueEle* e1 ;
		gridQueueEle* e2 ;

		for ( int i = eles - 1 ; i > 0 ; i -- )
		{
			pre = NULL ;
			e1 = head ;
			e2 = e1->next ;

			for ( int j = 0 ; j < i ; j ++ )
			{
				if ( e1->score < e2->score )
				{
					swapEle( pre, e1, e2 ) ;
				}
				else if ( e1->score == e2->score && rand() < RAND_MAX / 2)
				{
					swapEle( pre, e1, e2 ) ;
				}

				if ( pre == NULL )
				{
					pre = head ;
				}
				else
				{
					pre = pre->next;
				}
				e1 = pre->next ;
				e2 = e1->next ;
			}
		}

		/* Debugging 
		pre = head ;
		while ( pre != NULL )
		{
			printf("%d ", pre->score ) ;
			pre = pre->next ;
		}*/
	}


	void pushQueue( int xx, int yy, int zz )
	{
		gridQueueEle* ele = new gridQueueEle ;
		ele->x = xx ;
		ele->y = yy ;
		ele->z = zz ;
		ele->score = 0 ;
		ele->next = NULL ;
		if ( head == NULL )
		{
			head = ele ;
		}
		else
		{
			tail->next = ele ;
		}
		tail = ele ;
		numEles ++ ;
	}

	int popQueue( int& xx, int& yy, int& zz )
	{
		if ( head == NULL )
		{
			return 0 ;
		}

		xx = head->x ;
		yy = head->y ;
		zz = head->z ;

		gridQueueEle* temp = head ;
		head = head->next ;
		delete temp ;

		if ( head == NULL )
		{
			tail = NULL ;
		}
		numEles -- ;

		return 1 ;
	}

};

class VolumeBase
{
	
};

class Volume
{
public:
	/* Initialization */
	Volume( int x, int y, int z ) 
	{
		sizex = x ;
		sizey = y ; 
		sizez = z ;

		data = new float [ x * y * z ] ;
		for ( int i = 0 ; i < x * y * z ; i ++ )
		{
			data[ i ] = 0 ;
		}
	};

	Volume( int x, int y, int z, float val ) 
	{
		sizex = x ;
		sizey = y ; 
		sizez = z ;

		data = new float [ x * y * z ] ;
		for ( int i = 0 ; i < x * y * z ; i ++ )
		{
			data[ i ] = val ;
		}
	};

	Volume( int x, int y, int z, int offx, int offy, int offz, Volume* vol )
	{
		sizex = x ;
		sizey = y ; 
		sizez = z ;

		data = new float [ x * y * z ] ;

		int ct = 0 ;
		for ( int i = offx ; i < x + offx; i ++ )
			for ( int j = offy ; j < y + offy; j ++ )
				for ( int k = offz ; k < z + offz; k ++ )
				{
					data[ ct ] = (float)vol->getDataAt( i, j, k ) ;
					ct ++ ;
				}
	}

	void subtract( Volume* vol )
	{
		int i, j, k ;
		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i, j, k ) > 0 )
					{
						if ( vol->getDataAt(i,j,k) > 0 )
						{
							setDataAt( i, j, k, 0 ) ;
						}
					}
				}

	}

	/* Statistics function */
	int getSizeX( )
	{
		return sizex ;
	}
	int getSizeY( )
	{
		return sizey ;
	}
	int getSizeZ( )
	{
		return sizez ;
	}

	double getMin()
	{
		int size = sizex * sizey * sizez ;
		double rvalue = data[0] ;
		for ( int i = 1 ; i < size ; i ++ )
		{
			if ( rvalue > data[ i ] )
			{
				rvalue = data[ i ] ;
			}
		}
		return rvalue ;
	}

	double getMax()
	{
		int size = sizex * sizey * sizez ;
		double rvalue = data[0] ;
		for ( int i = 1 ; i < size ; i ++ )
		{
			if ( rvalue < data[ i ] )
			{
				rvalue = data[ i ] ;
			}
		}
		return rvalue ;
	}

	void fill( double val )
	{
		for ( int i = 0 ; i < sizex * sizey * sizez ; i ++ )
		{
			data[ i ] = (float)val ;
		}
	}

	/**
	 * Clustering routines
	 */
	int isBertrandBorder( int ox, int oy, int oz, int dir )
	{
		int nx = ox + neighbor6[dir][0] ;
		int ny = oy + neighbor6[dir][1] ;
		int nz = oz + neighbor6[dir][2] ;
		if ( getDataAt( nx, ny, nz) < 0 )
		{
			return 1 ;
		}
		return 0 ;
	}

	int isBertrandEndPoint( int ox, int oy, int oz )
	{
		// int flag = 0 ;
		double vox[3][3][3] ;

		int i, j, k ;
		for ( i = -1 ; i < 2 ; i ++ )
			for ( j = -1 ; j < 2 ; j ++ )
				for ( k = -1 ; k < 2 ; k ++ )
				{
					double tval = getDataAt( ox + i, oy + j, oz + k ) ;
					{
						vox[ i + 1 ][ j + 1 ][ k + 1 ] = tval ;
					}
				}

		// Count #X^x_6
		int xx6 = 0 ;
		for ( i = 0 ; i < 6 ; i ++ )
		{
			if ( vox[ neighbor6[i][0] + 1 ][ neighbor6[i][1] + 1 ][ neighbor6[i][2] + 1 ] >= 0 )
			{
				xx6 ++ ;
			}
		}

		// Mark /X^x_18
		/*
		int temp[5][5][5] ;
		for ( i = 0 ; i < 5 ; i ++ )
			for ( j = 0 ; j < 5 ; j ++ )
				for ( k = 0 ; k < 5 ; k ++ )
				{
					temp[i][j][k] = 0 ;
				}
		for ( i = 1 ; i < 4 ; i ++ )
			for ( j = 1 ; j < 4 ; j ++ )
				for ( k = 1 ; k < 4 ; k ++ )
				{
					if ( vox[i-1][j-1][k-1] < 0 )
					{
						temp[i][j][k] = 1 ;
					}
				}
		for ( i = 1 ; i < 4 ; i +=2 )
			for ( j = 1 ; j < 4 ; j +=2 )
				for ( k = 1 ; k < 4 ; k +=2 )
				{
					temp[i][j][k] = 0 ;
				}
		temp[2][2][2] = 0 ;
		*/

		// Now, count A26 and /A26
		int a26 = 0, na26 = 0 ;
		for ( i = 0 ; i < 3 ; i += 2 )
			for ( j = 0 ; j < 3 ; j += 2 )
				for ( k = 0 ; k < 3 ; k += 2 )
				{
					if ( vox[1][j][k] < 0 )
					{
						continue ;
					}
					if ( vox[i][1][k] < 0 )
					{
						continue ;
					}
					if ( vox[i][j][1] < 0 )
					{
						continue ;
					}

					if ( vox[i][1][1] < 0 )
					{
						continue ;
					}
					if ( vox[1][j][1] < 0 )
					{
						continue ;
					}
					if ( vox[1][1][k] < 0 )
					{
						continue ;
					}

					if ( vox[i][j][k] >= 0 )
					{
						a26 ++ ;
					}
					else
					{
						na26 ++ ;
					}
				}

		if (( na26 == 0 ) && ( a26 != 0 ) && ( xx6 <= a26 + 2 )) 
		{
			return 0 ;
		}

		if ( isFeatureFace(ox,oy,oz) )
		{
			//return 0 ;
		}

		return 1 ;
	}

	int isHelix( int ox, int oy, int oz )
	{
		int cn = 12 ;
		int nx, ny, nz ;
		int i, j, k ;

		double vox[3][3][3] ;
		for ( i = -1 ; i < 2 ; i ++ )
			for ( j = -1 ; j < 2; j ++ )
				for ( k = -1 ; k < 2 ; k ++ )
				{
					vox[i+1][j+1][k+1] = getDataAt( ox + i, oy + j, oz + k ) ;
				}

		for ( i = 0 ; i < 12 ; i ++ )
		{	
			for ( j = 0 ; j < 4 ; j ++ )
			{
				nx = sheetNeighbor[i][j][0] + 1;
				ny = sheetNeighbor[i][j][1] + 1;
				nz = sheetNeighbor[i][j][2] + 1;

				if ( vox[nx][ny][nz] <= 0 )
				{
					cn -- ;
					break ;
				}
			}
		}

		if ( cn >= 1 )
		{
			return 0 ;
		}
		else
		{
			return 1 ;
		}

	}



	int isSheet( int ox, int oy, int oz )
	{
		int cn = 12 ;
		int nx, ny, nz ;

		for ( int i = 0 ; i < 12 ; i ++ )
		{	
			for ( int j = 0 ; j < 4 ; j ++ )
			{
				nx = ox + sheetNeighbor[i][j][0] ;
				ny = oy + sheetNeighbor[i][j][1] ;
				nz = oz + sheetNeighbor[i][j][2] ;

				if ( getDataAt( nx, ny, nz ) <= 0 )
				{
					cn -- ;
					break ;
				}
			}
		}

		return ( cn >= 3 ) ;
	}

	Volume* getSheets( int minSize )
	{
		int i, j, k ;

		//Initialize volume
		printf("Initialize volume at %d %d %d\n", sizex, sizey, sizez ) ;
		Volume* svol = new Volume( sizex, sizey, sizez ) ;

		//Initialize cluster counters
		int sheets[MAX_SHEETS] ;
		for ( i = 0 ; i < MAX_SHEETS ; i ++ )
		{
			sheets[ i ] = 0 ;
		}
		int totSheets = 1 ;

		//Start clustering
		printf("Start clustering...\n" ) ;
		int ox, oy, oz ;
		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt(i,j,k) <= 0 || svol->getDataAt(i,j,k) != 0 )
					{
						// Not a data point or has been visited
						continue ;
					}
					if ( ! isSheet( i, j, k ) )
					{
						// Not a sheet point
						continue ;
					}

					//Initialize queue
					int numNodes = 1 ;
					svol->setDataAt( i, j, k, totSheets ) ;
					GridQueue* queue = new GridQueue() ;
					queue->pushQueue( i, j, k ) ;
					while ( queue->popQueue(ox, oy, oz) )
					{
						// Test if neighbors satisfy sheet condition
						if ( isSheet( ox, oy, oz ) )
						{
							for ( int m = 0 ; m < 6 ; m ++ )
							{
								int nx = ox + neighbor6[m][0] ;
								int ny = oy + neighbor6[m][1] ;
								int nz = oz + neighbor6[m][2] ;

								if ( getDataAt(nx,ny,nz) > 0 && svol->getDataAt(nx,ny,nz) == 0 )
								{
									svol->setDataAt(nx,ny,nz,totSheets);
									queue->pushQueue(nx,ny,nz) ;
									numNodes ++ ;
								}
							}
						}
					}

					delete queue ;
					if ( numNodes > 0 )
					{
					//	printf("Sheet %d contain %d nodes.\n", totSheets, numNodes) ;
						sheets[ totSheets ] = numNodes ;
						totSheets ++ ;
					}
				}

		// Removing clusters less than minSize
		printf("Removing small clusters.\n") ;
		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					int cnt = (int) svol->getDataAt(i,j,k) ;
					if ( cnt > 0 && sheets[ cnt ] < minSize )
					{
						svol->setDataAt(i,j,k,-1) ;
					}
				}

		// Finally, clean up
		printf("Thresholding the volume to 0/1...\n") ;
		svol->threshold( 0.1, 0, 1 ) ;

		return svol ;
	}

	Volume* getHelices( int minSize )
	{
		printf("Segmenting helices from eroded volume.\n") ;
		int i, j, k ;

		//Initialize volume
		printf("Initialize volume at %d %d %d\n", sizex, sizey, sizez ) ;
		Volume* svol = new Volume( sizex, sizey, sizez ) ;

		/*Testing
		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					double oval = ovol->getDataAt( i, j, k ) ;
					double val = getDataAt( i, j, k ) ;
					if ( val > 0 && oval >= helixThresh && isHelix( i, j, k ) )
					{
						svol->setDataAt( i, j, k, 1 ) ;
					}
					else
					{
						svol->setDataAt( i, j, k, -1 ) ;
					}
				}

		return svol ;
		*/

		//Initialize cluster counters
		int helices[MAX_SHEETS] ;
		for ( i = 0 ; i < MAX_SHEETS ; i ++ )
		{
			helices[ i ] = 0 ;
		}
		int totHelices = 1 ;

		//Start clustering
		printf("Start clustering...\n" ) ;
		int ox, oy, oz ;
		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt(i,j,k) <= 0 || svol->getDataAt(i,j,k) != 0 )
					{
						// Not a data point or has been visited
						continue ;
					}
					if ( ! isHelix( i, j, k ) )
					{
						// Not a helix point
						continue ;
					}

					//Initialize queue
					int numNodes = 1 ;
					svol->setDataAt( i, j, k, totHelices ) ;
					GridQueue* queue = new GridQueue() ;
					queue->pushQueue( i, j, k ) ;
					while ( queue->popQueue(ox, oy, oz) )
					{
						// Test if neighbors satisfy helix condition
						if ( isHelix( ox, oy, oz ) )
						{
							for ( int m = 0 ; m < 6 ; m ++ )
							{
								int nx = ox + neighbor6[m][0] ;
								int ny = oy + neighbor6[m][1] ;
								int nz = oz + neighbor6[m][2] ;

								if ( getDataAt(nx,ny,nz) > 0 && svol->getDataAt(nx,ny,nz) == 0 )
								{
									svol->setDataAt(nx,ny,nz,totHelices);
									queue->pushQueue(nx,ny,nz) ;
									numNodes ++ ;
								}
							}
						}
					}

					delete queue ;
					if ( numNodes > 0 )
					{
					//	printf("Helix %d contain %d nodes.\n", totHelices, numNodes) ;
						helices[ totHelices ] = numNodes ;
						totHelices ++ ;
					}
				}

		// Removing clusters less than minSize
		printf("Removing small clusters.\n") ;
		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					int cnt = (int) svol->getDataAt(i,j,k) ;
					if ( cnt > 0 && helices[ cnt ] < minSize )
					{
						svol->setDataAt(i,j,k,-1) ;
					}
				}

		// Finally, clean up
		printf("Thresholding the volume to 0/1...\n") ;
		svol->threshold( 0.1, 0, 1 ) ;

		return svol ;

	}
	/**
	 * Erosion routines
	 */

	int isEndPoint( int ox, int oy, int oz )
	{
		
		if ( getDataAt( ox - 1, oy, oz ) < 0 && getDataAt( ox + 1, oy, oz ) < 0 )
		{
			return 1 ;
		}
		if ( getDataAt( ox, oy - 1, oz ) < 0 && getDataAt( ox, oy + 1, oz ) < 0 )
		{
			return 1 ;
		}
		if ( getDataAt( ox, oy, oz - 1 ) < 0 && getDataAt( ox, oy, oz + 1 ) < 0 )
		{
			return 1 ;
		}
		

		return 0 ;
	}

	int getNumNeighbor6( int ox, int oy, int oz )
	{
		int rvalue = 0 ;
		for ( int i = 0 ; i < 6 ; i ++ )
		{
			int nx = ox + neighbor6[i][0] ;
			int ny = oy + neighbor6[i][1] ;
			int nz = oz + neighbor6[i][2] ;
			if ( getDataAt( nx, ny, nz ) >= 0 ) 
			{
				rvalue ++ ;
			}
		}

		return rvalue ;
	}



	// Returns 1 if it lies on the edge of a non-noise sheet
	int testIsSheetEnd( int ox, int oy, int oz )
	{
		int i, j ;
		int nx, ny, nz ;

		int edge[6] = { 0,0,0,0,0,0 } ;
		int faceflag[ 12 ] ;
		int hasNoiseFace = 0 ;
		int tot = 0 ;

		for ( i = 0 ; i < 12 ; i ++ )
		{	
			faceflag[ i ] = 1 ;
			int hasNoise = 0 ;

			for ( j = 0 ; j < 4 ; j ++ )
			{
				nx = ox + sheetNeighbor[i][j][0] ;
				ny = oy + sheetNeighbor[i][j][1] ;
				nz = oz + sheetNeighbor[i][j][2] ;

				if ( getDataAt( nx, ny, nz ) == 0 )
				{
					hasNoise = 1 ;
				}
				if ( getDataAt( nx, ny, nz ) < 0 )
				{
					faceflag[ i ] = 0 ;
					break ;
				}
			}
			if ( faceflag[ i ] == 1 && hasNoise )
			{
				hasNoiseFace ++ ;
				// return 0 ;
			}

			if ( faceflag[ i ] )
			{
				int e0 = faceEdges[ i ][ 0 ], e1 = faceEdges[ i ][ 1 ] ;
				edge[ e0 ] ++ ;
				edge[ e1 ] ++ ;
				tot ++ ;
			}
		}

		if ( hasNoiseFace == tot )
		{
			return 0 ;
		}

		if ( tot == 0 )
		{
			return 0 ;
		}

		// Removing 1s
		int numones = 0 ;
		for ( i = 0 ; i < 6 ; i ++ )
		{
			if ( edge[ i ] == 1 )
			{
				numones ++ ;
			}
		}
		while( numones > 0 )
		{
			int e ;
			for ( i = 0 ; i < 6 ; i ++ )
			{
				if ( edge[ i ] == 1 )
				{
					e = i ;
					break ;
				}
			}
			/*
			if ( edge[ e ] != 1 )
			{
				printf("Wrong Again!********\n") ;
			}
			*/

			int f, e2 ;
			for ( j = 0 ; j < 4 ; j ++ )
			{
				f = edgeFaces[ e ][ j ] ;
				if ( faceflag[ f ] )
				{
					break ; 
				}
			}

			/*
			if ( faceflag[ f ] == 0 )
			{
				printf("Wrong!********\n") ;
			}
			*/

			if ( faceEdges[ f ][ 0 ] == e )
			{
				e2 = faceEdges[ f ][ 1 ] ;
			}
			else
			{
				e2 = faceEdges[ f ][ 0 ] ;
			}


			edge[ e ] -- ;
			numones -- ;
			edge[ e2 ] -- ;
			faceflag[ f ] = 0 ;
			tot -- ;

			if ( edge[ e2 ] == 1 )
			{
				numones ++ ;
			}
			else if ( edge[ e2 ] == 0 )
			{
				numones -- ;
			}
		}

		if ( tot > 0 )
		{
			return 0 ;
		}
		
		return 1 ;
	}

	int isNoiseSheetEnd( int ox, int oy, int oz )
	{
		int i, j ;
		int nx, ny, nz ;

		int edge[6] = { 0,0,0,0,0,0 } ;
		int faceflag[ 12 ] ;
		int hasNoiseFace = 0 ;
		int tot = 0 ;

		for ( i = 0 ; i < 12 ; i ++ )
		{	
			faceflag[ i ] = 1 ;
			int hasNoise = 0 ;

			for ( j = 0 ; j < 4 ; j ++ )
			{
				nx = ox + sheetNeighbor[i][j][0] ;
				ny = oy + sheetNeighbor[i][j][1] ;
				nz = oz + sheetNeighbor[i][j][2] ;

				if ( getDataAt( nx, ny, nz ) == 0 )
				{
					hasNoise = 1 ;
				}
				if ( getDataAt( nx, ny, nz ) < 0 )
				{
					faceflag[ i ] = 0 ;
					break ;
				}
			}
			if ( faceflag[ i ] == 1 && hasNoise )
			{
				hasNoiseFace ++ ;
				// return 0 ;
			}

			if ( faceflag[ i ] )
			{
				int e0 = faceEdges[ i ][ 0 ], e1 = faceEdges[ i ][ 1 ] ;
				edge[ e0 ] ++ ;
				edge[ e1 ] ++ ;
				tot ++ ;
			}
		}

		if ( hasNoiseFace < tot )
		{
			return 0 ;
		}

		if ( tot == 0 )
		{
			return 0 ;
		}

		// Removing 1s
		int numones = 0 ;
		for ( i = 0 ; i < 6 ; i ++ )
		{
			if ( edge[ i ] == 1 )
			{
				numones ++ ;
			}
		}
		while( numones > 0 )
		{
			int e ;
			for ( i = 0 ; i < 6 ; i ++ )
			{
				if ( edge[ i ] == 1 )
				{
					e = i ;
					break ;
				}
			}
			/*
			if ( edge[ e ] != 1 )
			{
				printf("Wrong Again!********\n") ;
			}
			*/

			int f, e2 ;
			for ( j = 0 ; j < 4 ; j ++ )
			{
				f = edgeFaces[ e ][ j ] ;
				if ( faceflag[ f ] )
				{
					break ; 
				}
			}

			/*
			if ( faceflag[ f ] == 0 )
			{
				printf("Wrong!********\n") ;
			}
			*/

			if ( faceEdges[ f ][ 0 ] == e )
			{
				e2 = faceEdges[ f ][ 1 ] ;
			}
			else
			{
				e2 = faceEdges[ f ][ 0 ] ;
			}


			edge[ e ] -- ;
			numones -- ;
			edge[ e2 ] -- ;
			faceflag[ f ] = 0 ;
			tot -- ;

			if ( edge[ e2 ] == 1 )
			{
				numones ++ ;
			}
			else if ( edge[ e2 ] == 0 )
			{
				numones -- ;
			}
		}

		if ( tot > 0 )
		{
			return 0 ;
		}
		
		return 1 ;
	}

	int hasIsolatedFace( int ox, int oy, int oz )
	{
		int i, j, k ;
		int nx, ny, nz ;

		double vox[3][3][3] ;
		for ( i = -1 ; i < 2 ; i ++ )
			for ( j = -1 ; j < 2 ; j ++ )
				for ( k = -1 ; k < 2 ; k ++ )
				{
					vox[ i + 1 ][ j + 1 ][ k + 1 ] = getDataAt( ox + i, oy + j, oz + k ) ;
				}

		int cells[8] = { 1, 1, 1, 1, 1, 1, 1, 1 } ;
		for ( i = 0 ; i < 8 ; i ++ )
		{	
			int x = ( ( i >> 2 ) & 1 ) ;
			int y = ( ( i >> 1 ) & 1 ) ;
			int z = ( i & 1 ) ;
			for ( j = 0 ; j < 8 ; j ++ )
			{
				nx = x + ( ( j >> 2 ) & 1 ) ;
				ny = y + ( ( j >> 1 ) & 1 ) ;
				nz = z + ( j & 1 ) ;
				
				if ( vox[nx][ny][nz] < 0 )
				{
					cells[i] = 0 ;
					break;
				}
			}
		}

		for ( i = 0 ; i < 12 ; i ++ )
		{	
			if ( cells[ faceCells[i][0] ] == 1 || cells[ faceCells[i][1] ] == 1 )
			{
				continue ;
			}
			int flag = 1 ;
			for ( j = 0 ; j < 4 ; j ++ )
			{
				nx = 1 + sheetNeighbor[i][j][0] ;
				ny = 1 + sheetNeighbor[i][j][1] ;
				nz = 1 + sheetNeighbor[i][j][2] ;

				if ( vox[nx][ny][nz] < 0 )
				{
					flag = 0 ;
					break;
				}
			}
			if ( flag )
			{
				return 1 ;
			}
		}

		return 0 ;
	}

	int hasIsolatedEdge( int ox, int oy, int oz )
	{
		int i, j, k ;
		int nx, ny, nz ;

		double vox[3][3][3] ;
		for ( i = -1 ; i < 2 ; i ++ )
			for ( j = -1 ; j < 2 ; j ++ )
				for ( k = -1 ; k < 2 ; k ++ )
				{
					vox[ i + 1 ][ j + 1 ][ k + 1 ] = getDataAt( ox + i, oy + j, oz + k ) ;
				}


		int edge[6] = { 0,0,0,0,0,0 } ;

		for ( i = 0 ; i < 12 ; i ++ )
		{	
			int flag = 1 ;
			for ( j = 0 ; j < 4 ; j ++ )
			{
				nx = 1 + sheetNeighbor[i][j][0] ;
				ny = 1 + sheetNeighbor[i][j][1] ;
				nz = 1 + sheetNeighbor[i][j][2] ;

				if ( vox[nx][ny][nz] < 0 )
				{
					flag = 0 ;
					break ;
				}
			}

			if ( flag )
			{
				int e0 = faceEdges[ i ][ 0 ], e1 = faceEdges[ i ][ 1 ] ;
				edge[ e0 ] ++ ;
				edge[ e1 ] ++ ;
			}
		}

		for ( i = 0 ; i < 6 ; i ++ )
		{
			if ( edge[i] )
			{
				continue ;
			}
			
			nx = 1 + neighbor6[i][0] ;
			ny = 1 + neighbor6[i][1] ;
			nz = 1 + neighbor6[i][2] ;
			
			if ( vox[nx][ny][nz] >= 0 )
			{
				return 1 ;
			}

		}

		if ( hasCompleteSheet(ox,oy,oz) )
		{
			return 1 ;
		}

		return 0 ;
	}

	int countFace( int ox, int oy, int oz, int m )
	{
		int facenum = 4 ;
		for ( int i = 0 ; i < 4 ; i ++ )
		{
			for ( int j = 0 ; j < 4 ; j ++ )
			{
				int nx = ox + sheetNeighbor[edgeFaces[m][i]][j][0] ;
				int ny = oy + sheetNeighbor[edgeFaces[m][i]][j][1] ;
				int nz = oz + sheetNeighbor[edgeFaces[m][i]][j][2] ;

				if ( getDataAt( nx, ny, nz ) < 0 )
				{
					facenum -- ;
					break ;
				}
			}
		}

		return facenum ;
	}
	

	int hasCell( int ox, int oy, int oz )
	{
		for ( int i = 0 ; i < 2 ; i ++ )
			for ( int j = 0 ; j < 2 ; j ++ )
				for ( int k = 0 ; k < 2 ; k ++ )
				{
					if ( getDataAt( ox + i, oy + j, oz + k ) < 0 )
					{
						return 0 ;
					}
				}
		return 1 ;
	}

	Volume* markCellFace( )
	{
		int i, j, k ;
		Volume* fvol = new Volume( sizex, sizey, sizez ) ;

		//return fvol ;

		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if( getDataAt(i,j,k) >= 0 )
					{
						if ( hasCell(i,j,k) )
						{
							for ( int m = 0 ; m < 6 ; m ++ )
							{
								int nx = i + neighbor6[m][0] ;
								int ny = j + neighbor6[m][1] ;
								int nz = k + neighbor6[m][2] ;
								if ( ! hasCell(nx,ny,nz) )
								{
									fvol->setDataAt(i,j,k,(double)(1<<m)) ;
									break ;
								}
							}
						}
					}
				}


		return fvol ;
	}

	Volume* markFaceEdge( )
	{
		int x,y,z, i,j ;
		Volume* fvol = new Volume( sizex, sizey, sizez ) ;

		//return fvol ;

		for ( x = 0 ; x < sizex ; x ++ )
			for ( y = 0 ; y < sizey ; y ++ )
				for ( z = 0 ; z < sizez ; z ++ )
				{
					if( getDataAt(x,y,z) >= 0 )
					{
						
						for ( i = 0 ; i < 3 ; i ++ )
						{	
							int hasFace = 1 ;
							for ( j = 0 ; j < 4 ; j ++ )
							{
								int nx = x + sheetNeighbor[4 * i + 3][j][0] ;
								int ny = y + sheetNeighbor[4 * i + 3][j][1] ;
								int nz = z + sheetNeighbor[4 * i + 3][j][2] ;

								if ( getDataAt( nx, ny, nz ) < 0 )
								{
									hasFace = 0 ;
									break ;
								}
							}

							if ( hasFace )
							{
								// Look for open edges
								switch( i )
								{
								case 0: 
									if ( countFace( x, y, z, 0 ) == 1 )
									{
										fvol->setDataAt(x, y, z, (double)(1<<0)) ;
										break ;
									}
									if ( countFace( x, y, z, 2 ) == 1 )
									{
										fvol->setDataAt(x, y, z, (double)(1<<2)) ;
										break ;
									}
									if ( countFace( x, y + 1, z, 0 ) == 1 )
									{
										fvol->setDataAt(x, y + 1, z, (double)(1<<0)) ;
										break ;
									}
									if ( countFace( x, y, z + 1, 2 ) == 1 )
									{
										fvol->setDataAt(x, y, z + 1, (double)(1<<2)) ;
										break ;
									}
									printf("Hmmm... a face with no open edges.\n");
									break ;
								case 1: 
									if ( countFace( x, y, z, 0 ) == 1 )
									{
										fvol->setDataAt(x, y, z, (double)(1<<0)) ;
										break ;
									}
									if ( countFace( x, y, z, 4 ) == 1 )
									{
										fvol->setDataAt(x, y, z, (double)(1<<4)) ;
										break ;
									}
									if ( countFace( x + 1, y, z, 0 ) == 1 )
									{
										fvol->setDataAt(x + 1, y, z, (double)(1<<0)) ;
										break ;
									}
									if ( countFace( x, y, z + 1, 4 ) == 1 )
									{
										fvol->setDataAt(x, y, z + 1, (double)(1<<4)) ;
										break ;
									}
									printf("Hmmm... a face with no open edges.\n");
									break ;
								case 2: 
									if ( countFace( x, y, z, 2 ) == 1 )
									{
										fvol->setDataAt(x, y, z, (double)(1<<2)) ;
										break ;
									}
									if ( countFace( x, y, z, 4 ) == 1 )
									{
										fvol->setDataAt(x, y, z, (double)(1<<4)) ;
										break ;
									}
									if ( countFace( x + 1, y, z, 2 ) == 1 )
									{
										fvol->setDataAt(x + 1, y, z, (double)(1<<2)) ;
										break ;
									}
									if ( countFace( x, y + 1, z, 4 ) == 1 )
									{
										fvol->setDataAt(x, y + 1, z, (double)(1<<4)) ;
										break ;
									}
									printf("Hmmm... a face with no open edges.\n");
									break ;
								}
							}
						}

					}
				}


		return fvol ;
	}

	int hasCompleteSheet( int ox, int oy, int oz, Volume* fvol )
	{
		int i, j, k ;
		int nx, ny, nz ;

		int edge[6] = { 0,0,0,0,0,0 } ;
		int faceflag[ 12 ] ;
		int tot = 0 ;
		int cellflag[ 8 ] ;

		int ct = 0 ;
		for (  i = -1 ; i < 1 ; i ++ )
			for (  j = -1 ; j < 1 ; j ++ )
				for (  k = -1 ; k < 1 ; k ++ )
				{
					if ( hasCell( ox + i, oy + j, oz + k ) )
					{
						cellflag[ ct ] = 1 ;
					}
					else
					{
						cellflag[ ct ] = 0 ;
					} 
					ct ++ ;
				}

		for ( i = 0 ; i < 12 ; i ++ )
		{	
			faceflag[ i ] = 1 ;
			for ( j = 0 ; j < 4 ; j ++ )
			{
				nx = ox + sheetNeighbor[i][j][0] ;
				ny = oy + sheetNeighbor[i][j][1] ;
				nz = oz + sheetNeighbor[i][j][2] ;

				if ( getDataAt( nx, ny, nz ) < 0 )
				{
					faceflag[ i ] = 0 ;
					break ;
				}
			}

			if ( faceflag[ i ] )
			{
				if ( cellflag[ faceCells[i][0] ] ^ cellflag[ faceCells[i][1] ] )
				{
					int v1 = (int)( fvol->getDataAt( 
						ox - 1 + (( faceCells[i][0] >> 2 ) & 1 ),
						oy - 1 + (( faceCells[i][0] >> 1 ) & 1 ),
						oz - 1 + (( faceCells[i][0] ) & 1)) ) ;
					int v2 = (int)( fvol->getDataAt( 
						ox - 1 + (( faceCells[i][1] >> 2 ) & 1 ),
						oy - 1 + (( faceCells[i][1] >> 1 ) & 1 ),
						oz - 1 + (( faceCells[i][1] ) & 1)) ) ;
					if ( ((v1 >> (2 * (2 - i/4))) & 1) ||
						 ((v2 >> (2 * (2 - i/4) + 1 )) & 1) )
					{
						faceflag[ i ] = 0 ;
					}
				}
			}

			if ( faceflag[ i ] )
			{
				int e0 = faceEdges[ i ][ 0 ], e1 = faceEdges[ i ][ 1 ] ;
				edge[ e0 ] ++ ;
				edge[ e1 ] ++ ;
				tot ++ ;
			}
		}

		// Removing 1s
		int numones = 0 ;
		for ( i = 0 ; i < 6 ; i ++ )
		{
			if ( edge[ i ] == 1 )
			{
				numones ++ ;
			}
		}
		while( numones > 0 )
		{
			int e ;
			for ( i = 0 ; i < 6 ; i ++ )
			{
				if ( edge[ i ] == 1 )
				{
					e = i ;
					break ;
				}
			}
			/*
			if ( edge[ e ] != 1 )
			{
				printf("Wrong Again!********\n") ;
			}
			*/

			int f, e2 ;
			for ( j = 0 ; j < 4 ; j ++ )
			{
				f = edgeFaces[ e ][ j ] ;
				if ( faceflag[ f ] )
				{
					break ; 
				}
			}

			/*
			if ( faceflag[ f ] == 0 )
			{
				printf("Wrong!********\n") ;
			}
			*/

			if ( faceEdges[ f ][ 0 ] == e )
			{
				e2 = faceEdges[ f ][ 1 ] ;
			}
			else
			{
				e2 = faceEdges[ f ][ 0 ] ;
			}


			edge[ e ] -- ;
			numones -- ;
			edge[ e2 ] -- ;
			faceflag[ f ] = 0 ;
			tot -- ;

			if ( edge[ e2 ] == 1 )
			{
				numones ++ ;
			}
			else if ( edge[ e2 ] == 0 )
			{
				numones -- ;
			}
		}

		if ( tot > 0 )
		{
			return 1 ;
		}
		
		return 0 ;
	}


	// Returns 1 if it lies in the middle of a sheet
	int hasCompleteSheet( int ox, int oy, int oz )
	{
		int temp = countIntEuler( ox, oy, oz ) ;
		if ( temp > 0 )
		{
			return 1 ;
		}
		else
		{
			return 0 ;
		}
	}



	int hasCompleteSheetSlow( int ox, int oy, int oz )
	{
		int i, j ;
		int nx, ny, nz ;

		int edge[6] = { 0,0,0,0,0,0 } ;
		int faceflag[ 12 ] ;
		int tot = 0 ;

		for ( i = 0 ; i < 12 ; i ++ )
		{	
			faceflag[ i ] = 1 ;
			for ( j = 0 ; j < 4 ; j ++ )
			{
				nx = ox + sheetNeighbor[i][j][0] ;
				ny = oy + sheetNeighbor[i][j][1] ;
				nz = oz + sheetNeighbor[i][j][2] ;

				if ( getDataAt( nx, ny, nz ) < 0 )
				{
					faceflag[ i ] = 0 ;
					break ;
				}
			}

			if ( faceflag[ i ] )
			{
				int e0 = faceEdges[ i ][ 0 ], e1 = faceEdges[ i ][ 1 ] ;
				edge[ e0 ] ++ ;
				edge[ e1 ] ++ ;
				tot ++ ;
			}
		}

		// Removing 1s
		int numones = 0 ;
		for ( i = 0 ; i < 6 ; i ++ )
		{
			if ( edge[ i ] == 1 )
			{
				numones ++ ;
			}
		}
		while( numones > 0 )
		{
			int e ;
			for ( i = 0 ; i < 6 ; i ++ )
			{
				if ( edge[ i ] == 1 )
				{
					e = i ;
					break ;
				}
			}
			/*
			if ( edge[ e ] != 1 )
			{
				printf("Wrong Again!********\n") ;
			}
			*/

			int f, e2 ;
			for ( j = 0 ; j < 4 ; j ++ )
			{
				f = edgeFaces[ e ][ j ] ;
				if ( faceflag[ f ] )
				{
					break ; 
				}
			}

			/*
			if ( faceflag[ f ] == 0 )
			{
				printf("Wrong!********\n") ;
			}
			*/

			if ( faceEdges[ f ][ 0 ] == e )
			{
				e2 = faceEdges[ f ][ 1 ] ;
			}
			else
			{
				e2 = faceEdges[ f ][ 0 ] ;
			}


			edge[ e ] -- ;
			numones -- ;
			edge[ e2 ] -- ;
			faceflag[ f ] = 0 ;
			tot -- ;

			if ( edge[ e2 ] == 1 )
			{
				numones ++ ;
			}
			else if ( edge[ e2 ] == 0 )
			{
				numones -- ;
			}
		}

//		int temp = countIntEuler( ox, oy, oz ) ;
		if ( tot > 0 )
		{
//			if ( temp <= 0 )
			{
//				printf("Counting wrong: %d\n", temp ) ;
			}
			return 1 ;
		}
		else
		{
//			if ( temp > 0 )
			{
//				printf("Counting wrong: %d\n", temp ) ;
			}
			return 0 ;
		}
	}

	// Returns 1 if it has a complete helix
	int hasCompleteHelix( int ox, int oy, int oz )
	{
		
		int i ;
		int c1 = 0 , c2 = 0 ;
		int nx, ny, nz ;
		int j ;

		for ( i = 0 ; i < 6 ; i ++ )
		{
			nx = ox + neighbor6[i][0] ;
			ny = oy + neighbor6[i][1] ;
			nz = oz + neighbor6[i][2] ;
			if ( getDataAt( nx, ny, nz ) >= 0 ) 
			{
				c1 ++ ;
				j = i ;
			}

		}

		if ( c1 > 1 ) // || c1 == 0 )
		{
			return 1 ;
		}

		return 0 ;

		/*
		ox = ox + neighbor6[j][0] ;
		oy = oy + neighbor6[j][1] ;
		oz = oz + neighbor6[j][2] ;
		c1 = 0 ;
		for ( i = 0 ; i < 6 ; i ++ )
		{
			nx = ox + neighbor6[i][0] ;
			ny = oy + neighbor6[i][1] ;
			nz = oz + neighbor6[i][2] ;
			if ( getDataAt( nx, ny, nz ) >= 0 ) 
			{
				c1 ++ ;
			}

		}
		
		if ( c1 > 1 )
		{
			return 0 ;
		}
		else
		{
			return 1 ;
		}
		*/
	}

	int hasCompleteHelix( int ox, int oy, int oz, Volume* fvol )
	{
		
		int i ;
		int c1 = 0 , c2 = 0 ;
		int nx, ny, nz ;
		int j ;

		for ( i = 0 ; i < 6 ; i ++ )
		{
			nx = ox + neighbor6[i][0] ;
			ny = oy + neighbor6[i][1] ;
			nz = oz + neighbor6[i][2] ;
			if ( getDataAt( nx, ny, nz ) >= 0 ) 
			{
				if ( i % 2 == 0 )
				{
					nx = ox ;
					ny = oy ;
					nz = oz ;
				}

				int val = (int)fvol->getDataAt( nx, ny, nz) ;
				if ( (( val >> ( 2 * ( i / 2 ) ) ) & 1) == 0 )
				{
					c1 ++ ;
					j = i ;
				}
			}

		}

		if ( c1 > 1 ) 
		{
			return 1 ;
		}

		return 0 ;

		/*
		ox = ox + neighbor6[j][0] ;
		oy = oy + neighbor6[j][1] ;
		oz = oz + neighbor6[j][2] ;
		c1 = 0 ;
		for ( i = 0 ; i < 6 ; i ++ )
		{
			nx = ox + neighbor6[i][0] ;
			ny = oy + neighbor6[i][1] ;
			nz = oz + neighbor6[i][2] ;
			if ( getDataAt( nx, ny, nz ) >= 0 ) 
			{
				c1 ++ ;
			}

		}
		
		if ( c1 > 1 )
		{
			return 0 ;
		}
		else
		{
			return 1 ;
		}
		*/
	}


	// Returns 1 if it is a curve endpoint
	int isHelixEnd( int ox, int oy, int oz, Volume* nvol )
	{
		
		int i ;
		int c1 = 0 , c2 = 0 ;
		int nx, ny, nz ;

		for ( i = 0 ; i < 6 ; i ++ )
		{
			nx = ox + neighbor6[i][0] ;
			ny = oy + neighbor6[i][1] ;
			nz = oz + neighbor6[i][2] ;
			
			double val = getDataAt( nx, ny, nz ) ;

			if ( val >= 0 ) 
			{
				c1 ++ ;
				if ( val > 0 && val < MAX_ERODE && nvol->getDataAt( nx, ny, nz ) == 0 ) 
				{
					c2 ++ ;
				}
			}

		}

		if ( c1 == 1 && c2 == 1 )
		{
			return 1 ;
		}

		return 0 ;
	}

	int isFeature18( int ox, int oy, int oz )
	{
		// Border: > 0
		// Interior: == 0
		// Outside: < 0 
		if ( getDataAt( ox, oy, oz ) <= 0 )
		{
			return 0 ;
		}

		for ( int i = 0 ; i < 3 ; i ++ )
			for ( int j = 0 ; j < 3 ; j ++ )
				for ( int k = 0 ; k < 3 ; k ++ )
				{
					if ( i == 1 || j == 1 || k == 1 )
					{
						if( getDataAt( ox - 1 + i, oy - 1 + j, oz - 1 + k ) == 0 )
						{
							return 0 ;
						}
					}
				}
		return 1 ;
	}

	int isEdgeEnd( int ox, int oy, int oz )
	{
		int i ;
		int c1 = 0 ;
		int nx, ny, nz ;

		for ( i = 0 ; i < 6 ; i ++ )
		{
			nx = ox + neighbor6[i][0] ;
			ny = oy + neighbor6[i][1] ;
			nz = oz + neighbor6[i][2] ;
			
			double val = getDataAt( nx, ny, nz ) ;

			if ( val >= 0 ) 
			{
				c1 ++ ;
			}

		}

		if ( c1 == 1 )
		{
			return 1 ;
		}

		return 0 ;
	}

	int isFaceEnd( int ox, int oy, int oz )
	{
		// return isSheetEnd(ox,oy,oz) ;

		int i, j, k ;
		int cn = 12 ;
		int nx, ny, nz ;

		double vox[3][3][3] ;
		for ( i = -1 ; i < 2 ; i ++ )
			for ( j = -1 ; j < 2 ; j ++ )
				for ( k = -1 ; k < 2 ; k ++ )
				{
					vox[ i + 1 ][ j + 1 ][ k + 1 ] = getDataAt( ox + i, oy + j, oz + k ) ;
				}
		double val = vox[1][1][1] ;

		int edge[6] = { 4,4,4,4,4,4 } ;
		int edge2[6] = { 4,4,4,4,4,4 } ;

		for ( i = 0 ; i < 12 ; i ++ )
		{	
			for ( j = 0 ; j < 4 ; j ++ )
			{
				nx = 1 + sheetNeighbor[i][j][0] ;
				ny = 1 + sheetNeighbor[i][j][1] ;
				nz = 1 + sheetNeighbor[i][j][2] ;

				if ( vox[nx][ny][nz] < 0 )
				{
					edge[ faceEdges[ i ][ 0 ] ] -- ;
					edge[ faceEdges[ i ][ 1 ] ] -- ;
					break ;
				}
			}
			for ( j = 0 ; j < 4 ; j ++ )
			{
				nx = 1 + sheetNeighbor[i][j][0] ;
				ny = 1 + sheetNeighbor[i][j][1] ;
				nz = 1 + sheetNeighbor[i][j][2] ;

				if ( vox[nx][ny][nz] < 2 )
				{
					edge2[ faceEdges[ i ][ 0 ] ] -- ;
					edge2[ faceEdges[ i ][ 1 ] ] -- ;
					break ;
				}
			}
		}

		for ( i = 0 ; i < 6 ; i ++ )
		{
			if ( edge[ i ] == 1 && edge2[ i ] == 1 )
			{
				return 1 ;
			}
		}

		return 0 ;	
	}

	int isNoise( int ox, int oy, int oz, int noise )
	{
		if ( getDataAt(ox,oy,oz) == 1 )
		{
			return 1 ;
		}

		if ( noise > 0 )
		{
			for ( int i = 0 ; i < 6 ; i ++ )
			{
				int nx = ox + neighbor6[i][0] ;
				int ny = oy + neighbor6[i][1] ;
				int nz = oz + neighbor6[i][2] ;
				
				if ( getDataAt( nx, ny, nz ) > 0 )
				{
					if ( isNoise( nx, ny, nz, noise - 1 ) )
					{
						return 1 ;
					}
				}
			}
		}

		return 0 ;

	}

	int isNoiseHelixEnd( int ox, int oy, int oz )
	{
		
		int i ;
		int c1 = 0 , c2 = 0 ;
		int nx, ny, nz ;

		for ( i = 0 ; i < 6 ; i ++ )
		{
			nx = ox + neighbor6[i][0] ;
			ny = oy + neighbor6[i][1] ;
			nz = oz + neighbor6[i][2] ;
			
			double val = getDataAt( nx, ny, nz ) ;

			if ( val >= 0 ) 
			{
				c1 ++ ;
				if ( val > 0 && val < MAX_ERODE ) 
				{
					c2 ++ ;
				}
			}

		}

		if ( c1 == 1 && c2 == 0 )
		{
			return 1 ;
		}

		return 0 ;
	}


	int isHelixEnd( int ox, int oy, int oz )
	{
		
		int i ;
		int c1 = 0 , c2 = 0 ;
		int nx, ny, nz ;

		for ( i = 0 ; i < 6 ; i ++ )
		{
			nx = ox + neighbor6[i][0] ;
			ny = oy + neighbor6[i][1] ;
			nz = oz + neighbor6[i][2] ;
			
			double val = getDataAt( nx, ny, nz ) ;

			if ( val >= 0 ) 
			{
				c1 ++ ;
				if ( val > 0 && val < MAX_ERODE ) 
				{
					c2 ++ ;
				}
			}

		}

		if ( c1 == 1 && c2 == 1 )
		{
			return 1 ;
		}

		return 0 ;
	}

	
	// Returns 1 if it contains a sheet boundary
	// Noise-resistant
	int isSheetEnd( int ox, int oy, int oz, Volume* nvol )
	{
		int i, j ;
		int nx, ny, nz ;

		int edge[6] = { 0,0,0,0,0,0 } ;
		int faceflag[ 12 ] ;
		int hasFeatureFace = 0 ;
		int tot = 0 ;

		for ( i = 0 ; i < 12 ; i ++ )
		{	
			faceflag[ i ] = 1 ;
			int hasFeature = 1 ;

			for ( j = 0 ; j < 4 ; j ++ )
			{
				nx = ox + sheetNeighbor[i][j][0] ;
				ny = oy + sheetNeighbor[i][j][1] ;
				nz = oz + sheetNeighbor[i][j][2] ;

				if ( getDataAt( nx, ny, nz ) == 0 || nvol->getDataAt( nx, ny, nz ) == 1 )
				{
					hasFeature = 0 ;
				}
				if ( getDataAt( nx, ny, nz ) < 0 )
				{
					faceflag[ i ] = 0 ;
					break ;
				}
			}
			if ( faceflag[ i ] == 1 && hasFeature )
			{
				hasFeatureFace ++ ;
				// return 0 ;
			}

			if ( faceflag[ i ] )
			{
				int e0 = faceEdges[ i ][ 0 ], e1 = faceEdges[ i ][ 1 ] ;
				edge[ e0 ] ++ ;
				edge[ e1 ] ++ ;
				tot ++ ;
			}
		}

		if ( tot == 0 || hasFeatureFace == 0 )
		{
			return 0 ;
		}

		// Removing 1s
		int numones = 0 ;
		for ( i = 0 ; i < 6 ; i ++ )
		{
			if ( edge[ i ] == 1 )
			{
				numones ++ ;
			}
		}
		while( numones > 0 )
		{
			int e ;
			for ( i = 0 ; i < 6 ; i ++ )
			{
				if ( edge[ i ] == 1 )
				{
					e = i ;
					break ;
				}
			}
			/*
			if ( edge[ e ] != 1 )
			{
				printf("Wrong Again!********\n") ;
			}
			*/

			int f, e2 ;
			for ( j = 0 ; j < 4 ; j ++ )
			{
				f = edgeFaces[ e ][ j ] ;
				if ( faceflag[ f ] )
				{
					break ; 
				}
			}

			/*
			if ( faceflag[ f ] == 0 )
			{
				printf("Wrong!********\n") ;
			}
			*/

			if ( faceEdges[ f ][ 0 ] == e )
			{
				e2 = faceEdges[ f ][ 1 ] ;
			}
			else
			{
				e2 = faceEdges[ f ][ 0 ] ;
			}


			edge[ e ] -- ;
			numones -- ;
			edge[ e2 ] -- ;
			faceflag[ f ] = 0 ;
			tot -- ;

			if ( edge[ e2 ] == 1 )
			{
				numones ++ ;
			}
			else if ( edge[ e2 ] == 0 )
			{
				numones -- ;
			}
		}

		if ( tot > 0 )
		{
			return 0 ;
		}
		
		return 1 ;
	}

	int getNumFaces( int ox, int oy, int oz )
	{
		int i, j ;
		int nx, ny, nz ;

		int faces = 12 ;
		for ( i = 0 ; i < 12 ; i ++ )
		{	
			for ( j = 0 ; j < 4 ; j ++ )
			{
				nx = ox + sheetNeighbor[i][j][0] ;
				ny = oy + sheetNeighbor[i][j][1] ;
				nz = oz + sheetNeighbor[i][j][2] ;

				if ( getDataAt( nx, ny, nz ) < 0 )
				{
					faces -- ;
					break ;
				}
			}
		}

		return faces ;
	}

	int getNumCells( int ox, int oy, int oz )
	{
		int i, j, k ;

		int cells = 0 ;

		int ct = 0 ;
		for (  i = -1 ; i < 1 ; i ++ )
			for (  j = -1 ; j < 1 ; j ++ )
				for (  k = -1 ; k < 1 ; k ++ )
				{
					if ( hasCell( ox + i, oy + j, oz + k ) )
					{
						cells ++ ;
					}
				}

		// printf("Faces: %d\n", faces);
		return cells ;
	}


	int getNumIsolatedEdges( int ox, int oy, int oz )
	{
		int i, j, k ;
		int nx, ny, nz ;

		double vox[3][3][3] ;
		for ( i = -1 ; i < 2 ; i ++ )
			for ( j = -1 ; j < 2 ; j ++ )
				for ( k = -1 ; k < 2 ; k ++ )
				{
					vox[ i + 1 ][ j + 1 ][ k + 1 ] = getDataAt( ox + i, oy + j, oz + k ) ;
				}


		int edge[6] = { 0,0,0,0,0,0 } ;

		for ( i = 0 ; i < 12 ; i ++ )
		{	
			int flag = 1 ;
			for ( j = 0 ; j < 4 ; j ++ )
			{
				nx = 1 + sheetNeighbor[i][j][0] ;
				ny = 1 + sheetNeighbor[i][j][1] ;
				nz = 1 + sheetNeighbor[i][j][2] ;

				if ( vox[nx][ny][nz] < 0 )
				{
					flag = 0 ;
					break ;
				}
			}

			if ( flag )
			{
				int e0 = faceEdges[ i ][ 0 ], e1 = faceEdges[ i ][ 1 ] ;
				edge[ e0 ] ++ ;
				edge[ e1 ] ++ ;
			}
		}

		int edges = 0 ;
		for ( i = 0 ; i < 6 ; i ++ )
		{
			if ( edge[i] )
			{
				continue ;
			}
			
			nx = 1 + neighbor6[i][0] ;
			ny = 1 + neighbor6[i][1] ;
			nz = 1 + neighbor6[i][2] ;
			
			if ( vox[nx][ny][nz] >= 0 )
			{
				edges ++ ;
			}

		}

		return edges ;
	}

	int getNumIsolatedFaces( int ox, int oy, int oz )
	{
		int i, j, k ;
		int nx, ny, nz ;

		int faces = 0 ;
		int cellflag[ 8 ] ;

		int ct = 0 ;
		for (  i = -1 ; i < 1 ; i ++ )
			for (  j = -1 ; j < 1 ; j ++ )
				for (  k = -1 ; k < 1 ; k ++ )
				{
					if ( hasCell( ox + i, oy + j, oz + k ) )
					{
						cellflag[ ct ] = 1 ;
					}
					else
					{
						cellflag[ ct ] = 0 ;
					} 
					ct ++ ;
				}

		for ( i = 0 ; i < 12 ; i ++ )
		{	
			int hasFace = 1 ;
			for ( j = 0 ; j < 4 ; j ++ )
			{
				nx = ox + sheetNeighbor[i][j][0] ;
				ny = oy + sheetNeighbor[i][j][1] ;
				nz = oz + sheetNeighbor[i][j][2] ;

				if ( getDataAt( nx, ny, nz ) < 0 )
				{
					hasFace = 0 ;
					break ;
				}
			}

			if ( hasFace )
			{
				if (cellflag[ faceCells[i][0] ] == 0 && cellflag[ faceCells[i][1] ] == 0 ) 
				{
					faces ++ ;
				}
			}
		}

		// printf("Faces: %d\n", faces);
		return faces ;
	}

	int isFeatureFace2( int ox, int oy, int oz )
	{
		int i ;
		int nx, ny, nz ;

		for ( i = 0 ; i < 6 ; i ++ )
		{
			nx = ox + neighbor6[i][0] ;
			ny = oy + neighbor6[i][1] ;
			nz = oz + neighbor6[i][2] ;
			
			double val = getDataAt( nx, ny, nz ) ;

			if ( val == 0 ) 
			{
				return 0 ;
			}

		}

		return 1 ;
	}

	int isFeatureFace( int ox, int oy, int oz )
	{
		// return 1 ;

		int i, j ;
		int nx, ny, nz ;

		int faces = 12 ;
		for ( i = 0 ; i < 12 ; i ++ )
		{	
			int ct = 0 ;
			for ( j = 0 ; j < 4 ; j ++ )
			{
				nx = ox + sheetNeighbor[i][j][0] ;
				ny = oy + sheetNeighbor[i][j][1] ;
				nz = oz + sheetNeighbor[i][j][2] ;

				if ( getDataAt( nx, ny, nz ) < 0 )
				{
					ct = -1 ;
					break ;
				}
				if ( getDataAt( nx, ny, nz ) == 0 )
				{
					ct ++ ;
				}

			}
			if ( ct == -1 || ct >= 1 )
			{
				faces -- ;
			}
		}

		if ( faces > 0 )
		{
			return 1 ;
		}
		return 0 ;

	}

	int hasFeatureFace( int ox, int oy, int oz )
	{
		int i, j, k ;
		int nx, ny, nz ;

		int edge[6] = { 0,0,0,0,0,0 } ;
		int faceflag ;
		int tot = 0 ;
		int cellflag[ 8 ] ;

		// Get cells
		int ct = 0 ;
		for (  i = -1 ; i < 1 ; i ++ )
			for (  j = -1 ; j < 1 ; j ++ )
				for (  k = -1 ; k < 1 ; k ++ )
				{
					if ( hasCell( ox + i, oy + j, oz + k ) )
					{
						cellflag[ ct ] = 1 ;
					}
					else
					{
						cellflag[ ct ] = 0 ;
					} 
					ct ++ ;
				}

		// Find isolated and boundary faces
		for ( i = 0 ; i < 12 ; i ++ )
		{	
			faceflag = 1 ;
			for ( j = 0 ; j < 4 ; j ++ )
			{
				nx = ox + sheetNeighbor[i][j][0] ;
				ny = oy + sheetNeighbor[i][j][1] ;
				nz = oz + sheetNeighbor[i][j][2] ;

				if ( getDataAt( nx, ny, nz ) < 0 )
				{
					faceflag = 0 ;
					break ;
				}
			}

			if ( faceflag )
			{
				if ( cellflag[ faceCells[i][0] ] == 0 && cellflag[ faceCells[i][1] ] == 0 )
				{
					return 1 ;
				}
			}
		}

		return 0 ;

	}

	int isSheetEnd( int ox, int oy, int oz )
	{
		return ( ( hasCompleteSheet(ox,oy,oz) == 0 ) && isFeatureFace(ox,oy,oz) ) ;

		// return testIsSheetEnd( ox, oy, oz ) ;
		
		int i, j, k ;
		int cn = 12 ;
		int nx, ny, nz ;

		double vox[3][3][3] ;
		for ( i = -1 ; i < 2 ; i ++ )
			for ( j = -1 ; j < 2 ; j ++ )
				for ( k = -1 ; k < 2 ; k ++ )
				{
					vox[ i + 1 ][ j + 1 ][ k + 1 ] = getDataAt( ox + i, oy + j, oz + k ) ;
				}
		double val = vox[1][1][1] ;

		int edge[6] = { 4,4,4,4,4,4 } ;
		int edge2[6] = { 4,4,4,4,4,4 } ;

		for ( i = 0 ; i < 12 ; i ++ )
		{	
			for ( j = 0 ; j < 4 ; j ++ )
			{
				nx = 1 + sheetNeighbor[i][j][0] ;
				ny = 1 + sheetNeighbor[i][j][1] ;
				nz = 1 + sheetNeighbor[i][j][2] ;

				if ( vox[nx][ny][nz] < 0 )
				{
					edge[ faceEdges[ i ][ 0 ] ] -- ;
					edge[ faceEdges[ i ][ 1 ] ] -- ;
					break ;
				}
			}

			for ( j = 0 ; j < 4 ; j ++ )
			{
				nx = 1 + sheetNeighbor[i][j][0] ;
				ny = 1 + sheetNeighbor[i][j][1] ;
				nz = 1 + sheetNeighbor[i][j][2] ;

				if ( vox[nx][ny][nz] <= 0 )
				{
					edge2[ faceEdges[ i ][ 0 ] ] -- ;
					edge2[ faceEdges[ i ][ 1 ] ] -- ;
					break ;
				}
			}
		}

		
		/*
		for ( i = 0 ; i < 6 ; i ++ )
		{
			nx = 1 + neighbor6[i][0] ;
			ny = 1 + neighbor6[i][1] ;
			nz = 1 + neighbor6[i][2] ;
			if ( edge[i] == 0 && vox[nx][ny][nz] >= 0 ) 
			{
				return 0 ;
			}
		}
		*/
		


		for ( i = 0 ; i < 6 ; i ++ )
		{
			if ( edge[ i ] == 1 && edge2[ i ] == 1 )
			{
				return 1 ;
			}
		}

		return 0 ;
	}

	/* Test if this is a simple voxel */
	int isSimple( int ox, int oy, int oz )
	{
		// int flag = 0 ;
		double vox[3][3][3] ;
		double val = getDataAt( ox, oy, oz ) ;

		int i, j, k ;
		for ( i = -1 ; i < 2 ; i ++ )
			for ( j = -1 ; j < 2 ; j ++ )
				for ( k = -1 ; k < 2 ; k ++ )
				{
					double tval = getDataAt( ox + i, oy + j, oz + k ) ;

					/*
					if ( tval == 0 || tval > (va )
					{
						flag = 1 ;
					}
					*/
					/*
					if ( tval < 0 && tval == - curwid )
					{
					printf("Here %d", (int)-tval) ;
					vox[ i + 1 ][ j + 1 ][ k + 1 ] = - tval ;
					}
					else
					*/
					{
						vox[ i + 1 ][ j + 1 ][ k + 1 ] = tval ;
					}
				}

			/* Debugging 
			printf("{") ;
			for ( i = 0 ; i < 3 ; i ++ )
			{
				if ( i ) printf(",") ;
				printf("{") ;
				for ( j = 0 ; j < 3 ; j ++ )
				{
					if ( j ) printf(",") ;
					printf("{") ;
					for ( k = 0 ; k < 3 ; k ++ )
					{
						if ( k ) printf(",") ;
						printf("%d", (vox[i][j][k] >=0 ? 1: 0));
					}
					printf("}") ;
				}
				printf("}") ;
			}
			printf("} Int: %d, Ext: %d\n", countInt( vox ), countExt( vox )) ;
			*/

		if ( countInt( vox ) == 1 && countExt( vox ) == 1 ) 
		{
			return 1 ;
		}
		else
		{
			return 0 ;
		}
	}

	/* Test if this is a simple voxel */
	int isSimple2( int v[3][3][3] )
	{
		// int flag = 0 ;
		double vox[3][3][3] ;

		int i, j, k ;
		for ( i = 0 ; i < 3 ; i ++ )
			for ( j = 0 ; j < 3 ; j ++ )
				for ( k = 0 ; k < 3 ; k ++ )
				{
					if ( v[i][j][k] == 0 )
					{
						vox[ i ][ j ][ k ] = 1 ;
					}
					else
					{
						vox[i][j][k] = -1 ;
					}
				}
		if ( countInt( vox ) == 1 && countExt( vox ) == 1 ) 
		{
			return 1 ;
		}
		else
		{
			printf("Int: %d Ext: %d\n",countInt( vox ),countExt( vox ) );
			return 0 ;
		}
	}

	int getNumPotComplex3( int ox, int oy, int oz )
	{
		// return 0 ;
		

		int i, j, k ;
		if ( ! isSimple( ox, oy, oz ) || isSheetEnd( ox, oy, oz ) )
		{
			return 60 ;
		}
		double val = getDataAt( ox, oy, oz ) ;

		int rvalue = 30, nx, ny, nz ;


		int numSimple = 0 ;
		for ( i = -1 ; i < 2 ; i ++ )
			for ( j = -1 ; j < 2 ; j ++ )
				for ( k = -1 ; k < 2 ; k ++ )
				{
					nx = ox + i ;
					ny = oy + j ;
					nz = oz + k ;
					if ( getDataAt( nx, ny, nz ) > 0 )
					{
						if ( isSimple( nx, ny, nz ) && ! isSheetEnd( nx, ny, nz ) )
						{
							numSimple ++ ;
						}
					}
				}
		numSimple -- ;

		setDataAt( ox, oy, oz, -val ) ;

		int numPotSimple = 0 ;
		for ( i = -1 ; i < 2 ; i ++ )
			for ( j = -1 ; j < 2 ; j ++ )
				for ( k = -1 ; k < 2 ; k ++ )
				{
					nx = ox + i ;
					ny = oy + j ;
					nz = oz + k ;
					if ( getDataAt( nx, ny, nz ) > 0 )
					{
						if ( isSimple( nx, ny, nz ) && ! isSheetEnd( nx, ny, nz ) )
						{
							numPotSimple ++ ;
						}
					}
				}
				

		setDataAt( ox, oy, oz, val ) ;

				


		return 30 - ( numPotSimple  - numSimple ) ;
	}

	int getNumPotComplex4( int ox, int oy, int oz )
	{
		double val = getDataAt(ox,oy,oz) ;
		int rvalue = 0;

		int cells = getNumCells(ox,oy,oz) ;
		int ifaces = getNumIsolatedFaces(ox,oy,oz) ;
		int faces = getNumFaces(ox,oy,oz) ;
		int iedges = getNumIsolatedEdges(ox,oy,oz) ;

		return ( cells * 6 - 2 * faces ) ; // + 2 * ( faces * 4 - 4 * iedges ) ;
	}

	int getNumPotComplex( int ox, int oy, int oz )
	{
		//return 0 ;

		int i;
		double val = getDataAt( ox, oy, oz ) ;
		if ( val <= 0 )
		{
	//		return 70 ;
		}

		// return getNumNeighbor6( ox, oy, oz ) ;

		// int v = ((getNumNeighbor6( ox, oy, oz ) & 255) << 24) ;
		//int v = 0  ;
		
		int rvalue = 0, nx, ny, nz ;
		setDataAt( ox, oy, oz, -val ) ;

		/*
		for ( i = -1 ; i < 2 ; i ++ )
			for ( j = -1 ; j < 2 ; j ++ )
				for ( k = -1 ; k < 2 ; k ++ )
				{
					nx = ox + i ;
					ny = oy + j ;
					nz = oz + k ;
					if ( getDataAt( nx, ny, nz ) == val )
					{
						if ( isSheetEnd( nx, ny, nz) || ! isSimple ( nx, ny, nz ) )
						{
							rvalue ++ ;
						}
					}
				}
		*/

		for ( i = 0 ; i < 6 ; i ++ )
		{
			nx = ox + neighbor6[i][0] ;
			ny = oy + neighbor6[i][1] ;
			nz = oz + neighbor6[i][2] ;

			if ( getDataAt(nx,ny,nz) >= 0 )
			{
				int num = getNumNeighbor6( nx, ny, nz ) ;
				if ( num > rvalue )
				{
					rvalue = num ;
				}
			}
		}
				

		setDataAt( ox, oy, oz, val ) ;

		return rvalue + getNumNeighbor6( ox, oy, oz ) * 10 ;
		/*
		int v = (((rvalue + getNumNeighbor6( ox, oy, oz ) * 10) & 255) << 24) ;
		v |= ( ( ox & 255 ) << 16 )  ;
		v |= ( ( oy & 255 ) << 8 ) ;
		v |= ( ( oz & 255 ) ) ;
		return v ;
		*/

	}

	int getNumPotComplex2( int ox, int oy, int oz )
	{
		return getNumPotComplex( ox, oy, oz ) ;

		// return 0 ;
		// return getNumNeighbor6( ox, oy, oz ) ;

		int i, j, k ;
		double val = getDataAt( ox, oy, oz ) ;
		if ( val <= 0 )
		{
			return 0 ;
		}

		int rvalue = 0, nx, ny, nz ;
		setDataAt( ox, oy, oz, -val ) ;

		for ( i = -1 ; i < 2 ; i ++ )
			for ( j = -1 ; j < 2 ; j ++ )
				for ( k = -1 ; k < 2 ; k ++ )
				{
					nx = ox + i ;
					ny = oy + j ;
					nz = oz + k ;
					if ( getDataAt( nx, ny, nz ) == val )
					{
						if ( isHelixEnd( nx, ny, nz) || ! isSimple ( nx, ny, nz ) )
						{
							rvalue ++ ;
						}
					}
				}

		setDataAt( ox, oy, oz, val ) ;

		return rvalue + getNumNeighbor6( ox, oy, oz ) * 30 ;
	}

	int getNumNeighbor( int ox, int oy, int oz )
	{
		int rvalue = 0 ;
		double val = getDataAt( ox, oy, oz ) ;
		for ( int i = 0 ; i < 6 ; i ++ )
		{
			int nx = ox + neighbor6[i][0] ;
			int ny = oy + neighbor6[i][1] ;
			int nz = oz + neighbor6[i][2] ;

			if ( getDataAt( nx, ny, nz ) == val )
			{
				rvalue ++ ;
			}
		}
		/*
		for ( int i = -1 ; i < 2 ; i ++ )
			for ( int j = -1 ; j < 2 ; j ++ )
				for ( int k = -1 ; k < 2 ; k ++ )
				{
					int nx = ox + i ;
					int ny = oy + j ;
					int nz = oz + k ;

					if ( getDataAt( nx, ny, nz ) == val )
					{
						rvalue ++ ;
					}
				}
		*/
		return rvalue ;
	}


	void setScoreNeighbor( GridQueue* queue )
	{
		//printf("Scoring each node with number of neighbors...\n") ;
		gridQueueEle* ele = queue->getHead() ;
		while ( ele != NULL )
		{
			ele->score = getNumNeighbor( ele->x, ele->y, ele->z ) ;
			ele = ele->next ;
		}

		queue->sort( queue->getNumElements() ) ;
	}


	int components6( int vox[3][3][3] )
	{
		// Stupid flood fill 
		int tot = 0 ;
		int queue[27][3] ;
		int vis[3][3][3] ;
		int head = 0, tail = 1 ;
		int i, j, k ;
		for ( i = 0 ; i < 3 ; i ++ )
			for ( j = 0 ; j < 3 ; j ++ )
				for ( k = 0 ; k < 3 ; k ++ )
				{
					vis[i][j][k] = 0 ;
					if ( vox[i][j][k] )
					{
						if ( tot == 0 )
						{
							queue[0][0] = i ;
							queue[0][1] = j ;
							queue[0][2] = k ;
							vis[i][j][k] = 1 ;
						}
						tot ++ ;
					}
				}
		if ( tot == 0 )
		{
			return 0 ;
		}
		// printf("total: %d\n", tot) ;

		int ct = 1 ;
		while ( head != tail )
		{
			int x = queue[head][0] ;
			int y = queue[head][1] ;
			int z = queue[head][2] ;
			head ++ ;

			for ( i = 0 ; i < 6 ; i ++ )
			{
				int nx = x + neighbor6[i][0] ;
				int ny = y + neighbor6[i][1] ;
				int nz = z + neighbor6[i][2] ;
				if ( nx >=0 && nx < 3 && ny >=0 && ny < 3 && nz >=0 && nz < 3 )
				{
					if ( vox[nx][ny][nz] && ! vis[nx][ny][nz] )
					{
						queue[tail][0] = nx ;
						queue[tail][1] = ny ;
						queue[tail][2] = nz ;
						tail ++ ;
						vis[nx][ny][nz] = 1 ;
						ct ++ ;
					}
				}
			}
		}

		if ( ct == tot )
		{
			return 1 ;
		}
		else
		{
			return 2 ;
		}

	}
	int components26( int vox[3][3][3] )
	{
		// Stupid flood fill 
		int tot = 0 ;
		int queue[27][3] ;
		int vis[3][3][3] ;
		int head = 0, tail = 1 ;
		int i, j, k ;
		for ( i = 0 ; i < 3 ; i ++ )
			for ( j = 0 ; j < 3 ; j ++ )
				for ( k = 0 ; k < 3 ; k ++ )
				{
					vis[i][j][k] = 0 ;
					if ( vox[i][j][k] )
					{
						if ( tot == 0 )
						{
							queue[0][0] = i ;
							queue[0][1] = j ;
							queue[0][2] = k ;
							vis[i][j][k] = 1 ;
						}
						tot ++ ;
					}
				}
		if ( tot == 0 )
		{
			return 0 ;
		}

		int ct = 1 ;
		while ( head != tail )
		{
			int x = queue[head][0] ;
			int y = queue[head][1] ;
			int z = queue[head][2] ;
			head ++ ;

			for ( i = -1 ; i < 2 ; i ++ )
				for ( j = -1 ; j < 2 ; j ++ )
					for ( k = -1 ; k < 2 ; k ++ )
					{
						int nx = x + i ;
						int ny = y + j ;
						int nz = z + k ;
						if ( nx >=0 && nx < 3 && ny >=0 && ny < 3 && nz >=0 && nz < 3 )
						{
							if ( vox[nx][ny][nz] && ! vis[nx][ny][nz] )
							{
								queue[tail][0] = nx ;
								queue[tail][1] = ny ;
								queue[tail][2] = nz ;
								tail ++ ;
								vis[nx][ny][nz] = 1 ;
								ct ++ ;
							}
						}
					}
		}

		if ( ct == tot )
		{
			return 1 ;
		}
		else
		{
			return 2 ;
		}

	}

	int countExt( double vox[3][3][3] )
	{
		int tvox[3][3][3] ;

		for ( int i = 0 ; i < 3 ; i ++ )
			for ( int j = 0 ; j < 3 ; j ++ )
				for ( int k = 0 ; k < 3 ; k ++ )
				{
					if ( vox[i][j][k] < 0 )
					{
						tvox[i][j][k] = 1 ;
					}
					else
					{
						tvox[i][j][k] = 0 ;
					}
				}
		
		return components26( tvox ) ;
	}

	int countInt( double vox[3][3][3] )
	{
		int i, j, k ;
		int tvox[3][3][3] ;

		for ( i = 0 ; i < 3 ; i ++ )
			for ( j = 0 ; j < 3 ; j ++ )
				for ( k = 0 ; k < 3 ; k ++ )
				{
					tvox[i][j][k] = 0 ;
				}

		for ( i = 0 ; i < 6 ; i ++ )
		{
			int nx = 1 + neighbor6[i][0] ;
			int ny = 1 + neighbor6[i][1] ;
			int nz = 1 + neighbor6[i][2] ;
			if ( vox[ nx ][ ny ][ nz ] >= 0 )
			{
				tvox[ nx ][ ny ][ nz ] = 1 ;
				for ( j = 0 ; j < 4 ; j ++ )
				{
					int nnx = neighbor64[i][j][0] + nx ;
					int nny = neighbor64[i][j][1] + ny ;
					int nnz = neighbor64[i][j][2] + nz ;
					if ( vox[ nnx ][ nny ][ nnz ] >= 0 )
					{
						tvox[ nnx ][ nny ][ nnz ] = 1 ;
					}	
				}
			}
		}

		return components6( tvox ) ;
	}

	int countIntEuler( int ox, int oy, int oz )
	{
		int nv = 0 , ne = 0, nc = 0 ;

		int i, j, k ;
		int tvox[3][3][3] ;
		double vox[3][3][3] ;

		for ( i = 0 ; i < 3 ; i ++ )
			for ( j = 0 ; j < 3 ; j ++ )
				for ( k = 0 ; k < 3 ; k ++ )
				{
					vox[i][j][k] = getDataAt( ox - 1 + i, oy - 1 + j, oz - 1 + k ) ;
					tvox[i][j][k] = 0 ;
				}

		for ( i = 0 ; i < 6 ; i ++ )
		{
			int nx = 1 + neighbor6[i][0] ;
			int ny = 1 + neighbor6[i][1] ;
			int nz = 1 + neighbor6[i][2] ;
			if ( vox[nx][ny][nz] >= 0 )
			{
				tvox[ nx ][ ny ][ nz ] = 1 ;

				nv ++ ;

				for ( j = 0 ; j < 4 ; j ++ )
				{
					int nnx = neighbor64[i][j][0] + nx ;
					int nny = neighbor64[i][j][1] + ny ;
					int nnz = neighbor64[i][j][2] + nz ;
					if ( vox[nnx][nny][nnz] >= 0 )
					{
						if ( tvox[nnx][nny][nnz] == 0 )
						{
							tvox[nnx][nny][nnz] = 1 ;
							nv ++ ;
						}

						ne ++ ;
					}
				}
			}
		}

		nc = components6( tvox ) ;

		return ( nc - ( nv - ne ) ) ;
	}

	/* No topology check */
	void erodeNoTopo( float thr, int wid )
	{
		int i, j, k ;
		// First, threshold the volume
		printf("Thresholding the volume to -MAX_ERODE/0...\n") ;
		threshold( thr, -MAX_ERODE, 0 ) ;

		// Next, initialize the queue
		printf("Initializing queue...\n") ;
		GridQueue* queue = new GridQueue( ) ;

		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i, j, k ) >= 0 )
					{
						for ( int m = 0 ; m < 6 ; m ++ )
						{
							if ( getDataAt( i + neighbor6[m][0], j + neighbor6[m][1], k + neighbor6[m][2] ) < 0 )
							{
								setDataAt( i, j, k, 1 ) ;
								queue->pushQueue( i, j, k ) ;
								break ;
							}
						}
					}
				}
		printf("Total %d nodes\n", queue->getNumElements() ) ;

		// Perform erosion 
		printf("Start erosion to %d...\n", wid) ;
		double val = 0;
		int ox, oy, oz ;
		int curwid = 0 ;
		int total = 0, ct = 0 ;
		while ( 1 )
		{
			if ( ct == total )
			{
				printf("Layer %d has %d nodes.\n", (int) curwid, total ) ;
				curwid ++ ;
				ct = 0 ;
				total = queue->getNumElements() ;
				if ( total == 0 )
				{
					break ;
				}
			}

			queue->popQueue(ox, oy, oz) ;
			val = getDataAt( ox, oy, oz ) ;
			if ( val > wid )
			{
				break ;
			}
			ct ++ ;

			setDataAt( ox, oy, oz, -val ) ;


			for ( int m = 0 ; m < 6 ; m ++ )
			{
				int nx = ox + neighbor6[m][0] ;
				int ny = oy + neighbor6[m][1] ;
				int nz = oz + neighbor6[m][2] ;
				if ( getDataAt( nx, ny, nz ) == 0 )
				{
					setDataAt( nx, ny, nz, val + 1 ) ;
					queue->pushQueue( nx, ny, nz ) ;
				}
			}
		}

		// Finally, clean up
		printf("Thresholding the volume to 0/1...\n") ;
		threshold( 0, 0, 1 ) ;

	}

	/* Minimal topology check */
	void erodeTopo( float thr, int wid )
	{
		int i, j, k ;
		// First, threshold the volume
		printf("Thresholding the volume to -MAX_ERODE/0...\n") ;
		threshold( thr, -MAX_ERODE, 0 ) ;

		// Next, initialize the queue
		printf("Initializing queue...\n") ;
		GridQueue* queue = new GridQueue( ) ;

		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i, j, k ) >= 0 )
					{
						for ( int m = 0 ; m < 6 ; m ++ )
						{
							if ( getDataAt( i + neighbor6[m][0], j + neighbor6[m][1], k + neighbor6[m][2] ) < 0 )
							{
								setDataAt( i, j, k, 1 ) ;
								queue->pushQueue( i, j, k ) ;
								break ;
							}
						}
					}
				}
		printf("Total %d nodes\n", queue->getNumElements() ) ;

		// Perform erosion 
		printf("Start erosion to %d...\n", wid) ;
		double val = 0;
		int ox, oy, oz ;
		int curwid = 0 ;
		int total = 0, ct = 0 ;
		while ( 1 )
		{
			if ( ct == total )
			{
				printf("Layer %d has %d nodes.\n", (int) curwid, total ) ;
				curwid ++ ;
				ct = 0 ;
				total = queue->getNumElements() ;
				if ( total == 0 )
				{
					break ;
				}
			}

			queue->popQueue(ox, oy, oz) ;
			val = getDataAt( ox, oy, oz ) ;
			if ( val > wid )
			{
				break ;
			}
			ct ++ ;

			if (  isSimple( ox, oy, oz ) )
			{
				// Simple node, remove
				setDataAt( ox, oy, oz, -val ) ;
			}
			else
			{
				// Preserve for topology
				setDataAt( ox, oy, oz, val + 1 ) ;
				queue->pushQueue( ox, oy, oz ) ;
			}


			for ( int m = 0 ; m < 6 ; m ++ )
			{
				int nx = ox + neighbor6[m][0] ;
				int ny = oy + neighbor6[m][1] ;
				int nz = oz + neighbor6[m][2] ;
				if ( getDataAt( nx, ny, nz ) == 0 )
				{
					setDataAt( nx, ny, nz, val + 1 ) ;
					queue->pushQueue( nx, ny, nz ) ;
				}
			}
		}

		// Finally, clean up
		printf("Thresholding the volume to 0/1...\n") ;
		threshold( 0, 0, 1 ) ;

	}

	void erode2( float thr, int wid )
	{
		int i, j, k ;
		// First, threshold the volume
		printf("Thresholding the volume to -MAX_ERODE/0...\n") ;
		threshold( thr, -MAX_ERODE, 0 ) ;

		// Next, initialize the queue
		printf("Initializing queue...\n") ;
		GridQueue2* queue = new GridQueue2( ) ;
		GridQueue2* queue2 = new GridQueue2( ) ;

		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i, j, k ) >= 0 )
					{
						for ( int m = 0 ; m < 6 ; m ++ )
						{
							if ( getDataAt( i + neighbor6[m][0], j + neighbor6[m][1], k + neighbor6[m][2] ) < 0 )
							{
								setDataAt( i, j, k, 1 ) ;
								queue->prepend( i, j, k ) ;
								break ;
							}
						}
					}
				}
		printf("Total %d nodes\n", queue->getNumElements() ) ;

		// Perform erosion 
		// wid = MAX_ERODE ;
		printf("Start erosion to %d...\n", wid) ;
		gridQueueEle* ele ;
		double val = 0;
		int ox, oy, oz ;

		for ( int curwid = 1 ; curwid <= wid ; curwid ++ )
		{
			int numComplex = 0, numSimple = 0 ;
			printf("Processing %d nodes in layer %d\n", queue->getNumElements(), curwid) ;
			
			/* set nodes for next layer 
			while ( ( ele = queue->getNext() ) != NULL )
			{
				for ( int m = 0 ; m < 6 ; m ++ )
				{
					int nx = ele->x + neighbor6[m][0] ;
					int ny = ele->y + neighbor6[m][1] ;
					int nz = ele->z + neighbor6[m][2] ;
					if ( getDataAt( nx, ny, nz ) == 0 )
					{
						setDataAt( nx, ny, nz, curwid + 1 ) ;
						queue2->prepend( nx, ny, nz ) ;
					}
				}

			}
			*/

			// erosion
			int seed[3] = {-1,-1,-1};
			queue->reset() ;
			while ( queue->getNumElements() > 0 )
			{
				if ( seed[0] < 0 ) printf("After initial scoring...\n");
				queue->reset() ;
				ele = queue->getNext() ;

				// Secure complex nodes
				while ( ele != NULL )
				{
					ox = ele->x ;
					oy = ele->y ;
					oz = ele->z ;

					// Check simple only if within the last modified range
					if ( seed[0] < 0 || 
						( ox < seed[0] + 2 && ox > seed[0] - 2 &&
						oy < seed[1] + 2 && oy > seed[1] - 2 &&
						oz < seed[2] + 2 && oz > seed[2] - 2 ) )
					{
						if ( ! isSimple( ox, oy, oz ) )
						{
							// Complex, set to next layer
							setDataAt( ox, oy, oz, curwid + 1 ) ;
							queue2->prepend( ox, oy, oz ) ;
							ele = queue->remove() ;

							numComplex ++ ;
						}
						else
						{
							ele = queue->getNext() ;
						}
					}
					else
					{
						ele = queue->getNext() ;
					}
				}

				// Remove the simple node with the most potential neighbors
				queue->reset() ;
				ele = queue->getNext() ;
				int preScore = -1;
				while ( ele != NULL )
				{
					ox = ele->x ;
					oy = ele->y ;
					oz = ele->z ;

					// Update score only if within the last modified range
					if ( seed[0] < 0 || 
						( ox < seed[0] + 3 && ox > seed[0] - 3 &&
						  oy < seed[1] + 3 && oy > seed[1] - 3 &&
						  oz < seed[2] + 3 && oz > seed[2] - 3 ) )
					{
						ele->score = getNumPotComplex( ox, oy, oz ) ;
					}

					
					if ( ele->score < preScore )
					{
						// Swap
						ele = queue->swap() ;
					}
					else
					{
						preScore = ele->score ;
					}

					// At the end of the queue, remove this simple node
					if ( ele->next == NULL )
					{
						ox = ele->x ;
						oy = ele->y ;
						oz = ele->z ;
						setDataAt( ox, oy, oz, -1 ) ;
//						printf("%d %d %d\n", ox, oy, oz) ;
						seed[0] = ox ;
						seed[1] = oy ;
						seed[2] = oz ;
						queue->remove() ;
						// printf("Highest score: %d\n", preScore) ;


						for ( int m = 0 ; m < 6 ; m ++ )
						{
							int nx = ox + neighbor6[m][0] ;
							int ny = oy + neighbor6[m][1] ;
							int nz = oz + neighbor6[m][2] ;
							if ( getDataAt( nx, ny, nz ) == 0 )
							{
								setDataAt( nx, ny, nz, curwid + 1 ) ;
								queue2->prepend( nx, ny, nz ) ;
							}
						}
				
						
						numSimple ++ ;
						ele = NULL ;
					}
					else
					{
						ele = queue->getNext() ;
					}
				}

			}

			delete queue ;
			queue = queue2 ;
			queue2 = new GridQueue2() ;
			printf("%d complex, %d simple\n", numComplex, numSimple) ;

			if ( numSimple == 0 )
			{
				break ;
			}
		}

		// Finally, clean up
		printf("Thresholding the volume to 0/1...\n") ;
		threshold( 0, 0, 1 ) ;

	}

	/* Faster version of erode2 using priority queue */
	void erodeShapeTopo( float thr, int wid )
	{
		int i, j, k ;
		// First, threshold the volume
		printf("Thresholding the volume to -MAX_ERODE/0...\n") ;
		threshold( thr, -MAX_ERODE, 0 ) ;

		// Next, initialize the linked queue
		printf("Initializing queue...\n") ;
		GridQueue2* queue2 = new GridQueue2( ) ;
		GridQueue2* queue3 = new GridQueue2( ) ;
		PriorityQueue <gridPoint,int> * queue = new PriorityQueue <gridPoint,int> ( MAX_QUEUELEN );

		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i, j, k ) >= 0 )
					{
						for ( int m = 0 ; m < 6 ; m ++ )
						{
							if ( getDataAt( i + neighbor6[m][0], j + neighbor6[m][1], k + neighbor6[m][2] ) < 0 )
							{
								setDataAt( i, j, k, 1 ) ;
								queue2->prepend( i, j, k ) ;
								break ;
							}
						}
					}
				}
		printf("Total %d nodes\n", queue2->getNumElements() ) ;


		// Perform erosion 
		// wid = MAX_ERODE ;
		printf("Start erosion to %d...\n", wid) ;
		gridQueueEle* ele ;
		gridPoint* gp ;
		double val = 0;
		int ox, oy, oz ;
		int score ;
		Volume* scrvol = new Volume( this->sizex , this->sizey, this->sizez ) ;
		for ( i = 0 ; i < sizex * sizey * sizez ; i ++ )
		{
			scrvol->setDataAt( i, -1 ) ;
		}

		for ( int curwid = 1 ; curwid <= wid ; curwid ++ )
		{
			// At the start of each iteration, 
			// queue2 holds all the nodes for this layer
			// queue3 and queue are empty

			int numComplex = 0, numSimple = 0 ;
			printf("Processing %d nodes in layer %d\n", queue2->getNumElements(), curwid) ;
			
			// First, 
			// check for complex nodes in queue2 
			// move them from queue2 to queue3
			queue2->reset() ;
			ele = queue2->getNext() ;
			while ( ele != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;
				
				// Check simple 
				if ( ! isSimple( ox, oy, oz ) )
				{
					// Complex, set to next layer
					setDataAt( ox, oy, oz, curwid + 1 ) ;
					queue3->prepend( ox, oy, oz ) ;
					ele = queue2->remove() ;
					
					numComplex ++ ;
				}
				else
				{
					ele = queue2->getNext() ;
				}
			}

			// Next,
			// Compute score for each node left in queue2
			// move them into priority queue
			queue2->reset() ;
			ele = queue2->getNext() ;
			while ( ele != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;
				
				// Compute score
				score = getNumPotComplex( ox, oy, oz ) ;
				scrvol->setDataAt( ox, oy, oz, score ) ;

				// Push to queue
				gp = new gridPoint ;
				gp->x = ox ;
				gp->y = oy ;
				gp->z = oz ;
				queue->add( gp, -score ) ;
				
				ele = queue2->remove() ;
			}

			// Rename queue3 to be queue2, 
			// Clear queue3
			delete queue2 ;
			queue2 = queue3 ;
			queue3 = new GridQueue2( ) ;

			// Next, start priority queue iteration
			while ( ! queue->isEmpty() )
			{
				// Retrieve the node with the highest score
				queue->remove( gp, score ) ;
				ox = gp->x ;
				oy = gp->y ;
				oz = gp->z ;
				delete gp ;
				score = -score ;

				// Ignore the node 
				// if it has been processed before
				// or it has an updated score
				if ( getDataAt( ox, oy, oz ) != curwid || (int) scrvol->getDataAt( ox, oy, oz ) != score )
				{
					continue ;
				}

				// Remove this simple node
				setDataAt( ox, oy, oz, -1 ) ;
				numSimple ++ ;
				// printf("Highest score: %d\n", score) ;

				// Move its neighboring unvisited node to queue2
				for ( int m = 0 ; m < 6 ; m ++ )
				{
					int nx = ox + neighbor6[m][0] ;
					int ny = oy + neighbor6[m][1] ;
					int nz = oz + neighbor6[m][2] ;
					if ( getDataAt( nx, ny, nz ) == 0 )
					{
						setDataAt( nx, ny, nz, curwid + 1 ) ;
						queue2->prepend( nx, ny, nz ) ;
					}
				}
				
				// Find complex nodes in its 3x3 neighborhood
				// move them to queue2
				for ( i = -1 ; i < 2 ; i ++ )
					for ( j = -1 ; j < 2 ; j ++ )
						for ( k = -1 ; k < 2 ; k ++ )
						{
							int nx = ox + i ;
							int ny = oy + j ;
							int nz = oz + k ;

							// Check simple 
							if ( getDataAt( nx, ny, nz ) == curwid && ! isSimple( nx, ny, nz ) )
							{
								// Complex, set to next layer
								setDataAt( nx, ny, nz, curwid + 1 ) ;
								queue2->prepend( nx, ny, nz ) ;
								numComplex ++ ;
							}
						}

				// Update scores for nodes in its 5x5 neighborhood
				// insert them back into priority queue
						/*
				for ( i = -2 ; i < 3 ;i ++ )
					for ( j = -2 ; j < 3 ; j ++ )
						for ( k = -2 ; k < 3 ; k ++ )
						{
							int nx = ox + i ;
							int ny = oy + j ;
							int nz = oz + k ;

							if ( getDataAt( nx, ny, nz ) == curwid )
							{
								// Compute score
								score = getNumPotComplex( nx, ny, nz ) ;
								
								if ( score != (int) scrvol->getDataAt( nx, ny, nz ) )
								{
									// printf("Update\n") ;
									scrvol->setDataAt( nx, ny, nz, score ) ;
									// Push to queue
									gp = new gridPoint ;
									gp->x = nx ;
									gp->y = ny ;
									gp->z = nz ;
									queue->add( gp, -score ) ;
								}
							}
						}
						*/

			}

			printf("%d complex, %d simple\n", numComplex, numSimple) ;
			if ( numSimple == 0 )
			{
				break ;
			}
		}

		// Finally, clean up
		printf("Thresholding the volume to 0/1...\n") ;
		threshold( 0, 0, 1 ) ;

	}


	/* Erode to atoms */
	void erodeAtom( float thr, int wid, Volume* avol )
	{
		int i, j, k ;
		// First, threshold the volume
		printf("Thresholding the volume to -MAX_ERODE/0...\n") ;
		threshold( thr, -MAX_ERODE, 0 ) ;

		// Next, initialize the linked queue
		printf("Initializing queue...\n") ;
		GridQueue2* queue2 = new GridQueue2( ) ;
		GridQueue2* queue3 = new GridQueue2( ) ;
		PriorityQueue <gridPoint,int> * queue = new PriorityQueue <gridPoint,int> ( MAX_QUEUELEN );

		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i, j, k ) >= 0 )
					{
						if ( avol->getDataAt(i,j,k) > 0 )
						{
							setDataAt( i, j, k, MAX_ERODE ) ;
						}
						else
						{
							for ( int m = 0 ; m < 6 ; m ++ )
							{
								if ( getDataAt( i + neighbor6[m][0], j + neighbor6[m][1], k + neighbor6[m][2] ) < 0 )
								{
									setDataAt( i, j, k, 1 ) ;
									queue2->prepend( i, j, k ) ;
									break ;
								}
							}
						}
					}
				}
		printf("Total %d nodes\n", queue2->getNumElements() ) ;


		// Perform erosion 
		// wid = MAX_ERODE ;
		printf("Start erosion to %d...\n", wid) ;
		gridQueueEle* ele ;
		gridPoint* gp ;
		double val = 0;
		int ox, oy, oz ;
		int score ;
		Volume* scrvol = new Volume( this->sizex , this->sizey, this->sizez ) ;
		for ( i = 0 ; i < sizex * sizey * sizez ; i ++ )
		{
			scrvol->setDataAt( i, -1 ) ;
		}

		for ( int curwid = 1 ; curwid <= wid ; curwid ++ )
		{
			// At the start of each iteration, 
			// queue2 holds all the nodes for this layer
			// queue3 and queue are empty

			int numComplex = 0, numSimple = 0 ;
			printf("Processing %d nodes in layer %d\n", queue2->getNumElements(), curwid) ;
			
			// First, 
			// check for complex nodes in queue2 
			// move them from queue2 to queue3
			queue2->reset() ;
			ele = queue2->getNext() ;
			while ( ele != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;
				
				// Check simple 
				if ( ! isSimple( ox, oy, oz ) ) 
				{
					// Complex, set to next layer
					setDataAt( ox, oy, oz, curwid + 1 ) ;
					queue3->prepend( ox, oy, oz ) ;
					ele = queue2->remove() ;
					
					numComplex ++ ;
				}
				else
				{
					ele = queue2->getNext() ;
				}
			}

			// Next,
			// Compute score for each node left in queue2
			// move them into priority queue
			queue2->reset() ;
			ele = queue2->getNext() ;
			while ( ele != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;
				
				// Compute score
				score = getNumPotComplex( ox, oy, oz ) ;
				scrvol->setDataAt( ox, oy, oz, score ) ;

				// Push to queue
				gp = new gridPoint ;
				gp->x = ox ;
				gp->y = oy ;
				gp->z = oz ;
				queue->add( gp, -score ) ;
				
				ele = queue2->remove() ;
			}

			// Rename queue3 to be queue2, 
			// Clear queue3
			delete queue2 ;
			queue2 = queue3 ;
			queue3 = new GridQueue2( ) ;

			// Next, start priority queue iteration
			while ( ! queue->isEmpty() )
			{
				// Retrieve the node with the highest score
				queue->remove( gp, score ) ;
				ox = gp->x ;
				oy = gp->y ;
				oz = gp->z ;
				delete gp ;
				score = -score ;

				// Ignore the node 
				// if it has been processed before
				// or it has an updated score
				if ( getDataAt( ox, oy, oz ) != curwid || (int) scrvol->getDataAt( ox, oy, oz ) != score )
				{
					continue ;
				}

				// Remove this simple node
				setDataAt( ox, oy, oz, -1 ) ;
				numSimple ++ ;
				// printf("Highest score: %d\n", score) ;

				// Move its neighboring unvisited node to queue2
				for ( int m = 0 ; m < 6 ; m ++ )
				{
					int nx = ox + neighbor6[m][0] ;
					int ny = oy + neighbor6[m][1] ;
					int nz = oz + neighbor6[m][2] ;
					if ( getDataAt( nx, ny, nz ) == 0 )
					{
						setDataAt( nx, ny, nz, curwid + 1 ) ;
						queue2->prepend( nx, ny, nz ) ;
					}
				}
				
				// Find complex nodes in its 3x3 neighborhood
				// move them to queue2
				for ( i = -1 ; i < 2 ; i ++ )
					for ( j = -1 ; j < 2 ; j ++ )
						for ( k = -1 ; k < 2 ; k ++ )
						{
							int nx = ox + i ;
							int ny = oy + j ;
							int nz = oz + k ;

							// Check simple 
							if ( getDataAt( nx, ny, nz ) == curwid && ! isSimple( nx, ny, nz ) ) 

							{
								// Complex, set to next layer
								setDataAt( nx, ny, nz, curwid + 1 ) ;
								queue2->prepend( nx, ny, nz ) ;
								numComplex ++ ;
							}
						}

				// Update scores for nodes in its 5x5 neighborhood
				// insert them back into priority queue
						/*
				for ( i = -2 ; i < 3 ;i ++ )
					for ( j = -2 ; j < 3 ; j ++ )
						for ( k = -2 ; k < 3 ; k ++ )
						{
							int nx = ox + i ;
							int ny = oy + j ;
							int nz = oz + k ;

							if ( getDataAt( nx, ny, nz ) == curwid )
							{
								// Compute score
								score = getNumPotComplex( nx, ny, nz ) ;
								
								if ( score != (int) scrvol->getDataAt( nx, ny, nz ) )
								{
									// printf("Update\n") ;
									scrvol->setDataAt( nx, ny, nz, score ) ;
									// Push to queue
									gp = new gridPoint ;
									gp->x = nx ;
									gp->y = ny ;
									gp->z = nz ;
									queue->add( gp, -score ) ;
								}
							}
						}
						*/

			}

			printf("%d complex, %d simple\n", numComplex, numSimple) ;
			
			if ( numSimple == 0 )
			{
					break ;
			}
		}

		// Finally, clean up
		printf("Thresholding the volume to 0/1...\n") ;
		threshold( 0, 0, 1 ) ;

	}

	// Compute curve skeleton
	void curveSkeleton( float thr, Volume* svol )
	{
		int i, j, k ;
		// First, threshold the volume
		printf("Thresholding the volume to -1/0...\n") ;
		threshold( thr, -1, 0 ) ;

		// Next, apply convergent erosion 
		// by preserving: complex nodes, curve end-points, and sheet points

		// Next, initialize the linked queue
		printf("Initializing queue...\n") ;
		GridQueue2* queue2 = new GridQueue2( ) ;
		GridQueue2* queue3 = new GridQueue2( ) ;
		GridQueue2* queue4 = new GridQueue2( ) ;
		PriorityQueue <gridPoint,int> * queue = new PriorityQueue <gridPoint,int> ( MAX_QUEUELEN );

		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i, j, k ) >= 0 )
					{
						if ( svol->getDataAt(i,j,k) > 0 )
						{
							setDataAt( i, j, k, MAX_ERODE ) ;
						}
						else
						{
							for ( int m = 0 ; m < 6 ; m ++ )
							{
								if ( getDataAt( i + neighbor6[m][0], j + neighbor6[m][1], k + neighbor6[m][2] ) < 0 )
								{
									// setDataAt( i, j, k, 1 ) ;
									queue2->prepend( i, j, k ) ;
									break ;
								}
							}
						}
					}
				}
		printf("Total %d nodes\n", queue2->getNumElements() ) ;


		// Perform erosion 
		int wid = MAX_ERODE ;
		printf("Start erosion to %d...\n", wid) ;
		gridQueueEle* ele ;
		gridPoint* gp ;
		double val = 0;
		int ox, oy, oz ;
		int score ;
		Volume* scrvol = new Volume( this->sizex , this->sizey, this->sizez ) ;
		for ( i = 0 ; i < sizex * sizey * sizez ; i ++ )
		{
			scrvol->setDataAt( i, -1 ) ;
		}

#ifdef  NOISE_DIS_HELIX
		Volume* noisevol = new Volume( sizex, sizey, sizez ) ;
#endif

		for ( int curwid = 1 ; curwid <= wid ; curwid ++ )
		{
			// At the start of each iteration, 
			// queue2 holds all the nodes for this layer
			// queue3 and queue are empty

			int numComplex = 0, numSimple = 0 ;
			printf("Processing %d nodes in layer %d\n", queue2->getNumElements(), curwid) ;
			
			/*
			We first need to assign curwid + 1 to every node in this layer
			*/
			queue2->reset() ;
			ele = queue2->getNext() ;
			while ( ele != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;
				
				if ( getDataAt(ox,oy,oz) == curwid )
				{
					ele = queue2->remove() ;
				}
				else
				{
					setDataAt(ox,oy,oz, curwid) ;
					ele = queue2->getNext() ;
				}
			}
			queue4->reset() ;
			ele = queue4->getNext() ;
			while ( ele != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;

				queue2->prepend(ox,oy,oz) ;
				ele = queue4->remove() ;
			}

			// Now queue2 holds all the nodes for this layer

#ifdef NOISE_DIS_HELIX
			/* Extra step: classify nodes in queue2 into noise and non-noise nodes */
			queue2->reset() ;
			
			// First run
			int flag = 0 ;
			while ( ( ele = queue2->getNext() ) != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;
				if ( NOISE_DIS_HELIX <= 1 )
				{
					noisevol->setDataAt( ox, oy, oz, 0 ) ;
				}
				else
				{
					flag = 0 ;
					for ( int m = 0 ; m < 6 ; m ++ )
					{
						int nx = ox + neighbor6[m][0] ;
						int ny = oy + neighbor6[m][1] ;
						int nz = oz + neighbor6[m][2] ;
						if ( getDataAt( nx, ny, nz ) == 0 )
						{
							noisevol->setDataAt( ox, oy, oz, 1 ) ;
							flag = 1 ;
							break ;
						}
					}
					if ( ! flag )
					{
						noisevol->setDataAt( ox, oy, oz, 0 ) ;
					}
				}
			}

			int cur, visited ;
			for ( cur = 1 ; cur < NOISE_DIS_HELIX ; cur ++ )
			{
				queue2->reset() ;
				int count = 0 ;
				visited = 0 ;

				while ( ( ele = queue2->getNext() ) != NULL )
				{
					ox = ele->x ;
					oy = ele->y ;
					oz = ele->z ;
					
					if ( noisevol->getDataAt( ox, oy, oz ) == 1 )
					{
						visited ++ ;
						continue ;
					}

					flag = 0 ;
					for ( int m = 0 ; m < 6 ; m ++ )
					{
						int nx = ox + neighbor6[m][0] ;
						int ny = oy + neighbor6[m][1] ;
						int nz = oz + neighbor6[m][2] ;
						if ( getDataAt( nx, ny, nz ) > 0 && noisevol->getDataAt( nx, ny, nz ) == 1 )
						{
							noisevol->setDataAt( ox, oy, oz, 1 ) ;
							visited ++ ;
							count ++ ;
							break ;
						}
					}
				}

				if ( count == 0 )
				{
					break ;
				}
			}
			printf("Maximum feature distance: %d Un-touched: %d\n", cur, queue2->getNumElements() - visited ) ;


#endif
			/* Commented out for debugging
			
			// First, 
			// check for complex nodes in queue2 
			// move them from queue2 to queue3
			queue2->reset() ;
			ele = queue2->getNext() ;
			while ( ele != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;
				
				// Check simple 
#ifndef NOISE_DIS_HELIX
				if ( isHelixEnd( ox, oy, oz ) || ! isSimple( ox, oy, oz ) ) 
#else
				if ( isHelixEnd( ox, oy, oz, noisevol ) || ! isSimple( ox, oy, oz ) ) 
#endif
				{
					// Complex, set to next layer
					setDataAt( ox, oy, oz, curwid + 1 ) ;
					queue3->prepend( ox, oy, oz ) ;
					ele = queue2->remove() ;
					
					numComplex ++ ;
				}
				else
				{
					ele = queue2->getNext() ;
				}
			}
			*/

			// Next,
			// Compute score for each node left in queue2
			// move them into priority queue
			queue2->reset() ;
			ele = queue2->getNext() ;
			while ( ele != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;
				
				// Compute score
				score = getNumPotComplex2( ox, oy, oz ) ;
				scrvol->setDataAt( ox, oy, oz, score ) ;

				// Push to queue
				gp = new gridPoint ;
				gp->x = ox ;
				gp->y = oy ;
				gp->z = oz ;
				// queue->add( gp, -score ) ;
				queue->add( gp, score ) ;
				
				ele = queue2->remove() ;
			}

			// Rename queue3 to be queue2, 
			// Clear queue3
			// From now on, queue2 holds nodes of next level
			delete queue2 ;
			queue2 = queue3 ;
			queue3 = new GridQueue2( ) ;

			// Next, start priority queue iteration
			while ( ! queue->isEmpty() )
			{
				// Retrieve the node with the highest score
				queue->remove( gp, score ) ;
				ox = gp->x ;
				oy = gp->y ;
				oz = gp->z ;
				delete gp ;
//				score = -score ;

				// Ignore the node 
				// if it has been processed before
				// or it has an updated score
				if ( getDataAt( ox, oy, oz ) != curwid || (int) scrvol->getDataAt( ox, oy, oz ) != score )
				{
					continue ;
				}

				/* Commented out for debugging

				// Remove this simple node
				setDataAt( ox, oy, oz, -1 ) ;
				numSimple ++ ;
				// printf("Highest score: %d\n", score) ;
				*/

				/* Added for debugging */
				// Check simple 
#ifndef NOISE_DIS_HELIX
				// if ( hasIsolatedEdge( ox, oy, oz ) && ! isNoiseHelixEnd( ox, oy, oz ) ) 
				if ( isHelixEnd( ox, oy, oz ) || ! isSimple( ox, oy, oz ) ) 
#else
				if ( isHelixEnd( ox, oy, oz ) || ! isSimple( ox, oy, oz ) ) 
#endif
				{
					// Complex, set to next layer
					setDataAt( ox, oy, oz, curwid + 1 ) ;
					queue4->prepend( ox, oy, oz ) ;
					numComplex ++ ;
				}
				else
				{
					setDataAt( ox, oy, oz, -1 ) ;
					numSimple ++ ;
				}
				/* Adding ends */

				// Move its neighboring unvisited node to queue2
				for ( int m = 0 ; m < 6 ; m ++ )
				{
					int nx = ox + neighbor6[m][0] ;
					int ny = oy + neighbor6[m][1] ;
					int nz = oz + neighbor6[m][2] ;
					if ( getDataAt( nx, ny, nz ) == 0 )
					{
						// setDataAt( nx, ny, nz, curwid + 1 ) ;
						queue2->prepend( nx, ny, nz ) ;
					}
				}
				
				/* Commented out for debugging
			
				// Find complex nodes in its 3x3 neighborhood
				// move them to queue2
				for ( i = -1 ; i < 2 ; i ++ )
					for ( j = -1 ; j < 2 ; j ++ )
						for ( k = -1 ; k < 2 ; k ++ )
						{
							int nx = ox + i ;
							int ny = oy + j ;
							int nz = oz + k ;

							// Check simple 
							if ( getDataAt( nx, ny, nz ) == curwid && 
								// ( isSheetEnd( ox, oy, oz ) || ! isSimple( nx, ny, nz )) )
#ifndef NOISE_DIS_HELIX
								( isHelixEnd( nx, ny, nz ) || ! isSimple( nx, ny, nz ) ) )
#else
								( isHelixEnd( nx, ny, nz, noisevol ) || ! isSimple( nx, ny, nz ) ) )
#endif

							{
								// Complex, set to next layer
								setDataAt( nx, ny, nz, curwid + 1 ) ;
								queue2->prepend( nx, ny, nz ) ;
								numComplex ++ ;
							}
						}
				*/

				// Update scores for nodes in its 5x5 neighborhood
				// insert them back into priority queue
						
				for ( i = -2 ; i < 3 ;i ++ )
					for ( j = -2 ; j < 3 ; j ++ )
						for ( k = -2 ; k < 3 ; k ++ )
						{
							int nx = ox + i ;
							int ny = oy + j ;
							int nz = oz + k ;

							if ( getDataAt( nx, ny, nz ) == curwid )
							{
								// Compute score
								score = getNumPotComplex2( nx, ny, nz ) ;
								
								if ( score != (int) scrvol->getDataAt( nx, ny, nz ) )
								{
									// printf("Update\n") ;
									scrvol->setDataAt( nx, ny, nz, score ) ;
									// Push to queue
									gp = new gridPoint ;
									gp->x = nx ;
									gp->y = ny ;
									gp->z = nz ;
									// queue->add( gp, -score ) ;
									queue->add( gp, score ) ;
								}
							}
						}
						

			}

			printf("%d complex, %d simple\n", numComplex, numSimple) ;
			
			if ( numSimple == 0 )
			{
					break ;
			}
		}

		// Finally, clean up
		printf("Thresholding the volume to 0/1...\n") ;
		threshold( 0, 0, 1 ) ;		
	}

	// Compute minimal skeleton
	void skeleton( float thr, int off )
	{
		int i, j, k ;
		// First, threshold the volume
		printf("Thresholding the volume to -1/0...\n") ;
		threshold( thr, -1, 0 ) ;

		// Next, apply convergent erosion 
		// by preserving: complex nodes, curve end-points, and sheet points

		// Next, initialize the linked queue
		printf("Initializing queue...\n") ;
		GridQueue2* queue2 = new GridQueue2( ) ;
		GridQueue2* queue = new GridQueue2( ) ;

		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i, j, k ) >= 0 )
					{
						{
							for ( int m = 0 ; m < 6 ; m ++ )
							{
								if ( getDataAt( i + neighbor6[m][0], j + neighbor6[m][1], k + neighbor6[m][2] ) < 0 )
								{
									setDataAt( i, j, k, 1 ) ;
									queue2->prepend( i, j, k ) ;
									break ;
								}
							}
						}
					}
				}
		printf("Total %d nodes\n", queue2->getNumElements() ) ;


		// Perform erosion 
		int wid = 0 ;
		printf("Start erosion to %d...\n", wid) ;
		gridQueueEle* ele ;
		double val = 0;
		int ox, oy, oz ;

		while( 1 )
		{
			wid ++ ;

			// At the start of each iteration, 
			// queue2 holds all the nodes for this layer
			// queue is empty

			int numComplex = 0, numSimple = 0 ;
			printf("Processing %d nodes in layer %d\n", queue2->getNumElements(), wid) ;
			
			// Rename queue2 to be queue, 
			// Clear queue2
			// From now on, queue2 holds nodes of next level
			delete queue ;
			queue = queue2 ;
			queue2 = new GridQueue2( ) ;

			// Next, start queue iteration
			queue->reset() ;
			ele = queue->getNext();
			while ( ele != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;
//				delete ele ;

				// Check simple 
				if ( ! isSimple( ox, oy, oz ) ) 
				{
					// Complex, set to next layer
					queue2->prepend( ox, oy, oz ) ;
					numComplex ++ ;
				}
				/*
				else if ( ox == off || oy == off || oz == off || 
					ox == sizex - off - 1 || oy == sizey - off - 1 || oz == sizez - off - 1 )
				{
					// Wall, don't erode, set to next layer
					queue2->prepend( ox, oy, oz ) ;
					numComplex ++ ;
				}
				*/
				else
				{
					setDataAt( ox, oy, oz, -1 ) ;
					numSimple ++ ;

					for ( int m = 0 ; m < 6 ; m ++ )
					{
						int nx = ox + neighbor6[m][0] ;
						int ny = oy + neighbor6[m][1] ;
						int nz = oz + neighbor6[m][2] ;
						if ( getDataAt( nx, ny, nz ) == 0 )
						{
							setDataAt( nx, ny, nz, 1 ) ;
							queue2->prepend( nx, ny, nz ) ;
						}
					}

				}

				ele = queue->remove() ;
			}

			printf("Level %d: %d complex, %d simple\n", wid, numComplex, numSimple) ;
			
			if ( numSimple == 0 )
			{
				break ;
			}
		}

		// Finally, clean up
		printf("Thresholding the volume to 0/1...\n") ;
		threshold( 0, 0, 1 ) ;		
	}

	// Compute minimal skeleton
	void skeleton2( float thr, int off )
	{
		int i, j, k ;
		// First, threshold the volume
		printf("Thresholding the volume to -1/0...\n") ;
		threshold( thr, -1, 0 ) ;

		// Next, apply convergent erosion 
		// by preserving: complex nodes, curve end-points, and sheet points

		// Next, initialize the linked queue
		printf("Initializing queue...\n") ;
		GridQueue2* queue2 = new GridQueue2( ) ;
		GridQueue2* queue3 = new GridQueue2( ) ;
		PriorityQueue <gridPoint,int> * queue = new PriorityQueue <gridPoint,int> ( MAX_QUEUELEN );

		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i, j, k ) >= 0 )
					{
						if ( i == off || j == off || k == off || 
							 i == sizex - off - 1 || j == sizey - off - 1 || k == sizez - off - 1 )
						{
							setDataAt( i, j, k, MAX_ERODE ) ;
						}
						else
						{
							for ( int m = 0 ; m < 6 ; m ++ )
							{
								if ( getDataAt( i + neighbor6[m][0], j + neighbor6[m][1], k + neighbor6[m][2] ) < 0 )
								{
									setDataAt( i, j, k, 1 ) ;
									queue2->prepend( i, j, k ) ;
									break ;
								}
							}
						}
					}
				}
		printf("Total %d nodes\n", queue2->getNumElements() ) ;


		// Perform erosion 
		int wid = MAX_ERODE ;
		printf("Start erosion to %d...\n", wid) ;
		gridQueueEle* ele ;
		gridPoint* gp ;
		double val = 0;
		int ox, oy, oz ;
		int score ;

		Volume* scrvol = new Volume( this->sizex , this->sizey, this->sizez ) ;
		for ( i = 0 ; i < sizex * sizey * sizez ; i ++ )
		{
			scrvol->setDataAt( i, -1 ) ;
		}


		for ( int curwid = 1 ; curwid <= wid ; curwid ++ )
		{
			// At the start of each iteration, 
			// queue2 holds all the nodes for this layer
			// queue3 and queue are empty

			int numComplex = 0, numSimple = 0 ;
			printf("Processing %d nodes in layer %d\n", queue2->getNumElements(), curwid) ;
			

			// Next,
			// Compute score for each node left in queue2
			// move them into priority queue
			queue2->reset() ;
			ele = queue2->getNext() ;
			while ( ele != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;
				
				// Compute score
				score = getNumPotComplex2( ox, oy, oz ) ;
				scrvol->setDataAt( ox, oy, oz, score ) ;

				// Push to queue
				gp = new gridPoint ;
				gp->x = ox ;
				gp->y = oy ;
				gp->z = oz ;
				// queue->add( gp, -score ) ;
				queue->add( gp, score ) ;
				
				ele = queue2->remove() ;
			}

			// Rename queue3 to be queue2, 
			// Clear queue3
			// From now on, queue2 holds nodes of next level
			delete queue2 ;
			queue2 = queue3 ;
			queue3 = new GridQueue2( ) ;

			// Next, start priority queue iteration
			while ( ! queue->isEmpty() )
			{
				// Retrieve the node with the highest score
				queue->remove( gp, score ) ;
				ox = gp->x ;
				oy = gp->y ;
				oz = gp->z ;
				delete gp ;
//				score = -score ;

				// Ignore the node 
				// if it has been processed before
				// or it has an updated score
				if ( getDataAt( ox, oy, oz ) != curwid || (int) scrvol->getDataAt( ox, oy, oz ) != score )
				{
					continue ;
				}

				/* Added for debugging */
				// Check simple 
				if ( ! isSimple( ox, oy, oz ) ) 
				{
					// Complex, set to next layer
					setDataAt( ox, oy, oz, curwid + 1 ) ;
					queue2->prepend( ox, oy, oz ) ;
					numComplex ++ ;
				}
				else
				{
					setDataAt( ox, oy, oz, -1 ) ;
					numSimple ++ ;
				}
				/* Adding ends */

				// Move its neighboring unvisited node to queue2
				for ( int m = 0 ; m < 6 ; m ++ )
				{
					int nx = ox + neighbor6[m][0] ;
					int ny = oy + neighbor6[m][1] ;
					int nz = oz + neighbor6[m][2] ;
					if ( getDataAt( nx, ny, nz ) == 0 )
					{
						setDataAt( nx, ny, nz, curwid + 1 ) ;
						queue2->prepend( nx, ny, nz ) ;
					}
				}
				
				// Update scores for nodes in its 5x5 neighborhood
				// insert them back into priority queue
					/*	
				for ( i = -2 ; i < 3 ;i ++ )
					for ( j = -2 ; j < 3 ; j ++ )
						for ( k = -2 ; k < 3 ; k ++ )
						{
							int nx = ox + i ;
							int ny = oy + j ;
							int nz = oz + k ;

							if ( getDataAt( nx, ny, nz ) == curwid )
							{
								// Compute score
								score = getNumPotComplex2( nx, ny, nz ) ;
								
								if ( score != (int) scrvol->getDataAt( nx, ny, nz ) )
								{
									// printf("Update\n") ;
									scrvol->setDataAt( nx, ny, nz, score ) ;
									// Push to queue
									gp = new gridPoint ;
									gp->x = nx ;
									gp->y = ny ;
									gp->z = nz ;
									// queue->add( gp, -score ) ;
									queue->add( gp, score ) ;
								}
							}
						}
						
					*/
			}

			printf("%d complex, %d simple\n", numComplex, numSimple) ;
			
			if ( numSimple == 0 )
			{
					break ;
			}
		}

		// Finally, clean up
		delete scrvol ;
		printf("Thresholding the volume to 0/1...\n") ;
		threshold( 0, 0, 1 ) ;		
	}

	// Compute minimal skeleton
	void skeleton( float thr, Volume* svol, Volume* hvol )
	{
		int i, j, k ;
		// First, threshold the volume
		printf("Thresholding the volume to -1/0...\n") ;
		threshold( thr, -1, 0 ) ;

		// Next, apply convergent erosion 
		// by preserving: complex nodes, curve end-points, and sheet points

		// Next, initialize the linked queue
		printf("Initializing queue...\n") ;
		GridQueue2* queue2 = new GridQueue2( ) ;
		GridQueue2* queue3 = new GridQueue2( ) ;
		PriorityQueue <gridPoint,int> * queue = new PriorityQueue <gridPoint,int> ( MAX_QUEUELEN );

		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i, j, k ) >= 0 )
					{
						if ( svol->getDataAt(i,j,k) > 0 || hvol->getDataAt(i,j,k) > 0 )
						{
							setDataAt( i, j, k, MAX_ERODE ) ;
						}
						else
						{
							for ( int m = 0 ; m < 6 ; m ++ )
							{
								if ( getDataAt( i + neighbor6[m][0], j + neighbor6[m][1], k + neighbor6[m][2] ) < 0 )
								{
									setDataAt( i, j, k, 1 ) ;
									queue2->prepend( i, j, k ) ;
									break ;
								}
							}
						}
					}
				}
		printf("Total %d nodes\n", queue2->getNumElements() ) ;


		// Perform erosion 
		int wid = MAX_ERODE ;
		printf("Start erosion to %d...\n", wid) ;
		gridQueueEle* ele ;
		gridPoint* gp ;
		double val = 0;
		int ox, oy, oz ;
		int score ;
		Volume* scrvol = new Volume( this->sizex , this->sizey, this->sizez ) ;
		for ( i = 0 ; i < sizex * sizey * sizez ; i ++ )
		{
			scrvol->setDataAt( i, -1 ) ;
		}


		for ( int curwid = 1 ; curwid <= wid ; curwid ++ )
		{
			// At the start of each iteration, 
			// queue2 holds all the nodes for this layer
			// queue3 and queue are empty

			int numComplex = 0, numSimple = 0 ;
			printf("Processing %d nodes in layer %d\n", queue2->getNumElements(), curwid) ;
			

			// Next,
			// Compute score for each node left in queue2
			// move them into priority queue
			queue2->reset() ;
			ele = queue2->getNext() ;
			while ( ele != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;
				
				// Compute score
				score = getNumPotComplex2( ox, oy, oz ) ;
				scrvol->setDataAt( ox, oy, oz, score ) ;

				// Push to queue
				gp = new gridPoint ;
				gp->x = ox ;
				gp->y = oy ;
				gp->z = oz ;
				// queue->add( gp, -score ) ;
				queue->add( gp, score ) ;
				
				ele = queue2->remove() ;
			}

			// Rename queue3 to be queue2, 
			// Clear queue3
			// From now on, queue2 holds nodes of next level
			delete queue2 ;
			queue2 = queue3 ;
			queue3 = new GridQueue2( ) ;

			// Next, start priority queue iteration
			while ( ! queue->isEmpty() )
			{
				// Retrieve the node with the highest score
				queue->remove( gp, score ) ;
				ox = gp->x ;
				oy = gp->y ;
				oz = gp->z ;
				delete gp ;
//				score = -score ;

				// Ignore the node 
				// if it has been processed before
				// or it has an updated score
				if ( getDataAt( ox, oy, oz ) != curwid || (int) scrvol->getDataAt( ox, oy, oz ) != score )
				{
					continue ;
				}

				/* Added for debugging */
				// Check simple 
				if ( ! isSimple( ox, oy, oz ) ) 
				{
					// Complex, set to next layer
					setDataAt( ox, oy, oz, curwid + 1 ) ;
					queue2->prepend( ox, oy, oz ) ;
					numComplex ++ ;
				}
				else
				{
					setDataAt( ox, oy, oz, -1 ) ;
					numSimple ++ ;
				}
				/* Adding ends */

				// Move its neighboring unvisited node to queue2
				for ( int m = 0 ; m < 6 ; m ++ )
				{
					int nx = ox + neighbor6[m][0] ;
					int ny = oy + neighbor6[m][1] ;
					int nz = oz + neighbor6[m][2] ;
					if ( getDataAt( nx, ny, nz ) == 0 )
					{
						setDataAt( nx, ny, nz, curwid + 1 ) ;
						queue2->prepend( nx, ny, nz ) ;
					}
				}
				
				// Update scores for nodes in its 5x5 neighborhood
				// insert them back into priority queue
						
				for ( i = -2 ; i < 3 ;i ++ )
					for ( j = -2 ; j < 3 ; j ++ )
						for ( k = -2 ; k < 3 ; k ++ )
						{
							int nx = ox + i ;
							int ny = oy + j ;
							int nz = oz + k ;

							if ( getDataAt( nx, ny, nz ) == curwid )
							{
								// Compute score
								score = getNumPotComplex2( nx, ny, nz ) ;
								
								if ( score != (int) scrvol->getDataAt( nx, ny, nz ) )
								{
									// printf("Update\n") ;
									scrvol->setDataAt( nx, ny, nz, score ) ;
									// Push to queue
									gp = new gridPoint ;
									gp->x = nx ;
									gp->y = ny ;
									gp->z = nz ;
									// queue->add( gp, -score ) ;
									queue->add( gp, score ) ;
								}
							}
						}
						

			}

			printf("%d complex, %d simple\n", numComplex, numSimple) ;
			
			if ( numSimple == 0 )
			{
					break ;
			}
		}

		// Finally, clean up
		printf("Thresholding the volume to 0/1...\n") ;
		threshold( 0, 0, 1 ) ;		
	}


	// Apply helix erosion
	void erodeHelix( )
	{
		erodeHelix( 3 ) ;
	}
	
	
	void erodeHelix( int disthr )
	{
		int i, j, k ;
		// First, threshold the volume
		//printf("Thresholding the volume to -1/0...\n") ;
		threshold( 0.1f, -1, 0 ) ;
		
		/* Debug: remove faces */
		//Volume* facevol = markFaceEdge() ;
		/* End debugging */

		// Next, initialize the linked queue
		//printf("Initializing queue...\n") ;
		Volume * fvol = new Volume( sizex, sizey, sizez ) ;
		GridQueue2* queue2 = new GridQueue2( ) ;
		GridQueue2* queue3 = new GridQueue2( ) ;
		GridQueue2** queues = new GridQueue2* [ 10000 ] ;

		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i, j, k ) >= 0 )
					{
						if ( ! hasCompleteHelix( i, j, k ) )
						// if ( ! hasCompleteHelix( i, j, k, facevol ) )
						{
							queue2->prepend( i, j, k ) ;
							fvol->setDataAt( i, j, k, -1 ) ;
						}
					}
				}
		//printf("Total %d nodes\n", queue2->getNumElements() ) ;

		// Start erosion
		gridQueueEle* ele ;
		int dis = -1 ;
		while ( queue2->getNumElements() > 0 )
		{
			// First, set distance
			dis -- ;
			queues[ - dis ] = new GridQueue2( ) ;
			//printf("Distance transform to %d...", dis) ;
			queue2->reset() ;
			while( ( ele = queue2->getNext() ) != NULL )
			{
				setDataAt( ele->x, ele->y, ele->z, dis ) ;
				queues[ -dis ]->prepend( ele->x, ele->y, ele->z ) ;
			}
			//printf("%d nodes\n", queues[-dis]->getNumElements()) ;

			// Next, find next layer
			queue2->reset() ;
			ele = queue2->getNext() ;
			while ( ele != NULL )
			{
				for ( int m = 0 ; m < 6 ; m ++ )
				{
					int nx = ele->x + neighbor6[m][0] ;
					int ny = ele->y + neighbor6[m][1] ;
					int nz = ele->z + neighbor6[m][2] ;
					if ( getDataAt( nx, ny, nz ) == 0 )
					{
						fvol->setDataAt( nx, ny, nz, dis ) ;

						if ( ! hasCompleteHelix( nx, ny, nz ) )
						// if ( ! hasCompleteHelix( nx, ny, nz, facevol ) )
						{
							setDataAt( nx, ny, nz, 1 ) ;
							queue3->prepend( nx, ny, nz ) ;
						}
					}
				}
				
				ele = queue2->remove() ;
			}

			// Next, swap queues
			GridQueue2* temp = queue2 ;
			queue2 = queue3 ;
			queue3 = temp ;
		}

		// Deal with closed rings
		dis -- ;
		queues[ - dis ] = new GridQueue2( ) ;
		printf("Detecting closed rings %d...", dis) ;
		int ftot = 0 ;
		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i, j, k ) == 0 )
					{
						setDataAt( i, j, k, dis ) ;
						queues[ -dis ]->prepend( i, j, k ) ;
/*
						int fval = (int) fvol->getDataAt( i, j, k ) ;
						if ( fval == 0)
						{
							// queues[ -dis ]->prepend( i, j, k ) ;
						}
						else
						{
							setDataAt( i, j, k, fval - 1 ) ;
							// queues[ -fval + 1 ]->prepend( i, j, k ) ;
						}
*/
						ftot ++ ;
					}
				}
		printf("%d nodes\n", ftot) ;


		// return ;

		/* Find local minimum: to help determine erosion level
		int cts[ 64 ] ;
		for ( i = 0 ; i <= - dis ; i ++ )
		{
			cts[ i ] = 0 ;
		}
		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					double val = getDataAt( i, j, k ) ;
					if ( val < -1 )
					{
						int min = 1 ;
						for ( int m = 0 ; m < 6 ; m ++ )
						{
							int nx = i + neighbor6[m][0] ;
							int ny = j + neighbor6[m][1] ;
							int nz = k + neighbor6[m][2] ;
							if ( getDataAt( nx, ny, nz ) < val )
							{
								min = 0 ;
								break ;
							}
						}

						if ( min )
						{
							cts[ (int) - val ] ++ ;
						}
					}
				}

		for ( i = 2 ; i <= - dis ; i ++ )
		{
			printf("Local minima: %d with %d nodes.\n", -i, cts[ i ] ) ;
		}
		*/

		// Dilate back
		// Starting from nodes with distance - 2 - disthr

		if ( disthr + 2 > - dis )
		{
			disthr = - dis - 2 ;
		}
		int d; 
		
		for ( d = - dis ; d > disthr + 1 ; d -- )
		{
			queues[ d ]->reset() ;
			while ( (ele = queues[ d ]->getNext() ) != NULL )
			{
				setDataAt( ele->x, ele->y, ele->z, d ) ;
			}
		}
		

		for ( d = disthr + 1 ; d >= 2 ; d -- )
		{
			//delete queue3 ;
			//queue3 = new GridQueue2( ) ;
			queues[ d ]->reset() ;
			ele = queues[ d ]->getNext() ;
			while ( ele != NULL )
			{
				int dilatable = 0 ;
				for ( int m = 0 ; m < 6 ; m ++ )
						{
							int nx = ele->x + neighbor6[m][0] ;
							int ny = ele->y + neighbor6[m][1] ;
							int nz = ele->z + neighbor6[m][2] ;
							if ( getDataAt( nx, ny, nz ) == d + 1 )
							{
								dilatable = 1 ;
								break ;
							}
						}

				
				if ( ! dilatable )
				{
					/*
					setDataAt( ele->x, ele->y, ele->z, - 1 ) ;
					*/
					
					setDataAt( ele->x, ele->y, ele->z, - d + 1 ) ;
					if ( d > 2 )
					{
						queues[ d - 1 ]->prepend( ele->x, ele->y, ele->z ) ;
					}
					ele = queues[ d ]->remove() ;
				}
				else
				{
					setDataAt( ele->x, ele->y, ele->z, d ) ;
					ele = queues[ d ]->getNext() ;
				}
				
			}

			/* Debug: extract minimal set */
			while ( 1 )
			{
				int num = 0 ;
				queues[ d ]->reset() ;
				ele = queues[ d ]->getNext() ;
				while ( ele != NULL )
				{
					int critical = 0 ;
					setDataAt( ele->x, ele->y, ele->z, -1 ) ;
					
					for ( i = -1 ; i < 2 ; i ++ )
					{
						for ( j = -1 ; j < 2 ; j ++ )
						{
							for ( k = -1 ; k < 2 ; k ++ )
							{
								if ( i != 0 && j != 0 && k != 0 )
								{
									continue ;
								}
								int nx = ele->x + i ;
								int ny = ele->y + j ;
								int nz = ele->z + k ;
								if ( getDataAt(nx,ny,nz) == d + 1 && !hasCompleteHelix( nx,ny,nz ) ) //, facevol ) )
								{
									critical = 1 ;
									break ;
								}
							}
							if ( critical )
							{
								break ;
							}
						}
						if ( critical )
						{
							break ;
						}
					}
					
					if ( critical )
					{
						setDataAt( ele->x, ele->y, ele->z, d ) ;
						ele = queues[ d ]->getNext() ;
					}
					else
					{
						setDataAt( ele->x, ele->y, ele->z, - d + 1 ) ;
						if ( d > 2 )
						{
							queues[ d - 1 ]->prepend( ele->x, ele->y, ele->z ) ;
						}
						ele = queues[ d ]->remove() ;
						num ++ ;
					}
					
				}

				printf("Non-minimal: %d\n", num) ;

				if ( num == 0 )
				{
					break ;
				}
			}


			/* End of debugging */
	
			/*
			queue3->reset() ;
			ele = queue3->getNext() ;
			while ( ele != NULL )
			{
				setDataAt( ele->x, ele->y, ele->z, - 1 ) ;
				ele = queue3->remove() ;
			}
			*/
		}

		// Finally, threshold the volume
		//printf("Thresholding the volume to 0/1...\n") ;
		//threshold( -1, 1, 0, 0 ) ;
		threshold( 0, 0, 1 ) ;
		delete fvol ;

	}



	// Apply sheet erosion
	int erodeSheet( )
	{
		return erodeSheet( 3 ) ;
	}
	
	
	int erodeSheet( int disthr )
	{
		int i, j, k ;
		// First, threshold the volume
		//printf("Thresholding the volume to -1/0...\n") ;
		threshold( 0.1f, -1, 0 ) ;

		/* Debug: remove cells */
		Volume* facevol = markCellFace() ;
		/* End debugging */
		
		// Next, initialize the linked queue
		//printf("Initializing queue...\n") ;
		Volume * fvol = new Volume( sizex, sizey, sizez ) ;
		GridQueue2* queue2 = new GridQueue2( ) ;
		GridQueue2* queue3 = new GridQueue2( ) ;
		GridQueue2** queues = new GridQueue2* [ 10000 ] ;

		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i, j, k ) >= 0 )
					{
						if ( ! hasCompleteSheet( i, j, k ) )
						//if ( ! hasCompleteSheet( i, j, k, facevol ) )
						{
							queue2->prepend( i, j, k ) ;
							fvol->setDataAt( i, j, k, -1 ) ;
						}
					}
				}
		printf("Total %d nodes\n", queue2->getNumElements() ) ;

		// Start erosion
		gridQueueEle* ele ;
		int dis = -1 ;
		while ( queue2->getNumElements() > 0 )
		{
			// First, set distance
			dis -- ;
			queues[ - dis ] = new GridQueue2( ) ;
			//printf("Distance transform to %d...", dis) ;
			queue2->reset() ;
			while( ( ele = queue2->getNext() ) != NULL )
			{
				setDataAt( ele->x, ele->y, ele->z, dis ) ;
				queues[ -dis ]->prepend( ele->x, ele->y, ele->z ) ;
			}
			//printf("%d nodes\n", queues[-dis]->getNumElements()) ;

			// Next, find next layer
			queue2->reset() ;
			ele = queue2->getNext() ;
			while ( ele != NULL )
			{
				// for ( int m = 0 ; m < 6 ; m ++ )
				for ( int mx = -1 ; mx < 2 ; mx ++ )
					for ( int my = -1 ; my < 2 ; my ++ )
						for ( int mz = -1 ; mz < 2 ; mz ++ )
						{
							if ( mx != 0 && my != 0 && mz != 0 )
							{
								continue ;
							}
							//int nx = ele->x + neighbor6[m][0] ;
							//int ny = ele->y + neighbor6[m][1] ;
							//int nz = ele->z + neighbor6[m][2] ;
							int nx = ele->x + mx ;
							int ny = ele->y + my ;
							int nz = ele->z + mz ;

							if ( getDataAt( nx, ny, nz ) == 0 )
							{
								fvol->setDataAt( nx, ny, nz, dis ) ;

								if  ( ! hasCompleteSheet( nx, ny, nz ) )
								// if  ( ! hasCompleteSheet( nx, ny, nz, facevol ) )
								{
									setDataAt( nx, ny, nz, 1 ) ;
									queue3->prepend( nx, ny, nz ) ;
								}
							}
						}
				
				ele = queue2->remove() ;
			}

			// Next, swap queues
			GridQueue2* temp = queue2 ;
			queue2 = queue3 ;
			queue3 = temp ;
		}

		/* Deal with closed rings */
		
		dis -- ;
		queues[ - dis ] = new GridQueue2( ) ;
		printf("Detecting closed rings %d...", dis) ;
		int ftot = 0 ;
		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i, j, k ) == 0 )
					{
						int fval = (int) fvol->getDataAt( i, j, k ) ;
						/*
						if ( fval == 0)
						{
							setDataAt( i, j, k, dis - 2 ) ;
							// queues[ -dis ]->prepend( i, j, k ) ;
						}
						else
						{
							setDataAt( i, j, k, fval - 1 ) ;
							queues[ -fval + 1 ]->prepend( i, j, k ) ;
						}
						*/
						setDataAt( i, j, k, dis ) ;
						queues[ -dis ]->prepend( i, j, k ) ;

						ftot ++ ;
					}
				}
		printf("%d nodes\n", ftot) ;
		
		

		/* Find local minimum: to help determine erosion level 
		int cts[ 64 ] ;
		for ( i = 0 ; i <= - dis ; i ++ )
		{
			cts[ i ] = 0 ;
		}
		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					double val = getDataAt( i, j, k ) ;
					if ( val < -1 )
					{
						int min = 1 ;
						for ( int m = 0 ; m < 6 ; m ++ )
						{
							int nx = i + neighbor6[m][0] ;
							int ny = j + neighbor6[m][1] ;
							int nz = k + neighbor6[m][2] ;
							if ( getDataAt( nx, ny, nz ) < val )
							{
								min = 0 ;
								break ;
							}
						}

						if ( min )
						{
							cts[ (int) - val ] ++ ;
						}
					}
				}

		for ( i = 2 ; i <= - dis ; i ++ )
		{
			printf("Local minima: %d with %d nodes.\n", -i, cts[ i ] ) ;
		}
		*/

		// return ;

		// Dilate back
		// Starting from nodes with distance - 2 - disthr

		if ( disthr + 2 > - dis )
		{
			disthr = - dis - 2 ;

		}
		for ( int d = - dis ; d > disthr + 1 ; d -- )
		{
			queues[ d ]->reset() ;
			while ( (ele = queues[ d ]->getNext() ) != NULL )
			{
				setDataAt( ele->x, ele->y, ele->z, d ) ;
			}
		}

		for (int d = disthr + 1 ; d >= 2 ; d -- )
		{
			
			//delete queue3 ;
			//queue3 = new GridQueue2( ) ;
			queues[ d ]->reset() ;
			ele = queues[ d ]->getNext() ;
			while ( ele != NULL )
			{
				int dilatable = 0 ;
				// for ( int m = 0 ; m < 6 ; m ++ )
				/*
				for ( int mx = -1 ; mx < 2 ; mx ++ )
				for ( int my = -1 ; my < 2 ; my ++ )
				for ( int mz = -1 ; mz < 2 ; mz ++ )
				{
				if ( mx == 0 || my == 0 || mz == 0 )
				{
				int nx = ele->x + mx ; // neighbor6[m][0] ;
				int ny = ele->y + my ; // neighbor6[m][1] ;
				int nz = ele->z + mz ; // neighbor6[m][2] ;
				if ( getDataAt( nx, ny, nz ) == - d - 1 )
				{
				dilatable = 1 ;
				break ;
				}
				}
				}
				*/
				for ( i = 0 ; i < 12 ; i ++ )
				{	
					int flag = 1, flag2 = 0 ;
					for ( j = 0 ; j < 4 ; j ++ )
					{
						int nx = ele->x + sheetNeighbor[i][j][0] ;
						int ny = ele->y + sheetNeighbor[i][j][1] ;
						int nz = ele->z + sheetNeighbor[i][j][2] ;
						
						double val = getDataAt( nx, ny, nz ) ;
						
						if ( val > - d && val < 0)
						{
							flag = 0 ;
							break ;
						}
						if ( val == d + 1 )
						{
							flag2 ++ ;
						}
					}
					
					if ( flag && flag2 )
					{
						dilatable = 1 ;
						break ;
					}
				}
				
				if ( ! dilatable )
				{
					// setDataAt( ele->x, ele->y, ele->z, - 1 ) ;
					// queue3->prepend( ele->x, ele->y, ele->z ) ;
					
					setDataAt( ele->x, ele->y, ele->z, - d + 1 ) ;
					if ( d > 2 )
					{
						queues[ d - 1 ]->prepend( ele->x, ele->y, ele->z ) ;
					}
					ele = queues[ d ]->remove() ;
				}
				else
				{
					setDataAt( ele->x, ele->y, ele->z, d ) ;
					ele = queues[ d ]->getNext() ;
				}
			}

			/* Debug: extract minimal set */
			while ( 1 )
			{
				int num = 0 ;
				queues[ d ]->reset() ;
				ele = queues[ d ]->getNext() ;
				while ( ele != NULL )
				{
					int critical = 0 ;
					setDataAt( ele->x, ele->y, ele->z, -1 ) ;
					
					for ( i = -1 ; i < 2 ; i ++ )
					{
						for ( j = -1 ; j < 2 ; j ++ )
						{
							for ( k = -1 ; k < 2 ; k ++ )
							{
								if ( i != 0 && j != 0 && k != 0 )
								{
									continue ;
								}
								int nx = ele->x + i ;
								int ny = ele->y + j ;
								int nz = ele->z + k ;
								// if ( getDataAt(nx,ny,nz) == d + 1 && !hasCompleteSheet( nx,ny,nz, facevol ) )
								if ( getDataAt(nx,ny,nz) == d + 1 && !hasCompleteSheet( nx,ny,nz ) )
								{
									critical = 1 ;
									break ;
								}
							}
							if ( critical )
							{
								break ;
							}
						}
						if ( critical )
						{
							break ;
						}
					}
					
					if ( critical )
					{
						setDataAt( ele->x, ele->y, ele->z, d ) ;
						ele = queues[ d ]->getNext() ;
					}
					else
					{
						setDataAt( ele->x, ele->y, ele->z, - d + 1 ) ;
						if ( d > 2 )
						{
							queues[ d - 1 ]->prepend( ele->x, ele->y, ele->z ) ;
						}
						ele = queues[ d ]->remove() ;
						num ++ ;
					}
					
				}

				printf("Non-minimal: %d\n", num) ;

				if ( num == 0 )
				{
					break ;
				}
			}


			/* End of debugging */
			
			/*
			queue3->reset() ;
			ele = queue3->getNext() ;
			while ( ele != NULL )
			{
				setDataAt( ele->x, ele->y, ele->z, - 1 ) ;
				ele = queue3->remove() ;
			}
			*/
		}
		

		// Finally, threshold the volume
		//printf("Thresholding the volume to 0/1...\n") ;
		//threshold( -1, 1, 0, 0 ) ;
		threshold( 0, 0, 1 ) ;

		delete facevol ;
		delete fvol ;
	
		return - dis - 1 ;
	}
		
	void erodeSheetOld( int disthr )
	{
		int i, j, k ;
		// First, threshold the volume
		printf("Thresholding the volume to -1/0...\n") ;
		threshold( 0.1f, -1, 0 ) ;
		
		// Next, initialize the linked queue
		printf("Initializing queue...\n") ;
		GridQueue2* queue2 = new GridQueue2( ) ;
		GridQueue2* queue3 = new GridQueue2( ) ;
		GridQueue2** queues = new GridQueue2* [ 64 ] ;

		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i, j, k ) >= 0 )
					{
						if ( ! hasCompleteSheet( i, j, k ) )
						{
							queue2->prepend( i, j, k ) ;
						}
					}
				}
		printf("Total %d nodes\n", queue2->getNumElements() ) ;

		// Start erosion
		gridQueueEle* ele ;
		int dis = -1 ;
		while ( queue2->getNumElements() > 0 )
		{
			// First, set distance
			dis -- ;
			queues[ - dis ] = new GridQueue2( ) ;
			printf("Distance transform to %d...", dis) ;
			queue2->reset() ;
			while( ( ele = queue2->getNext() ) != NULL )
			{
				setDataAt( ele->x, ele->y, ele->z, dis ) ;
				queues[ -dis ]->prepend( ele->x, ele->y, ele->z ) ;
			}
			printf("%d nodes\n", queues[-dis]->getNumElements()) ;

			// Next, find next layer
			queue2->reset() ;
			ele = queue2->getNext() ;
			while ( ele != NULL )
			{
				for ( int m = 0 ; m < 6 ; m ++ )
				{
					int nx = ele->x + neighbor6[m][0] ;
					int ny = ele->y + neighbor6[m][1] ;
					int nz = ele->z + neighbor6[m][2] ;
					if ( getDataAt( nx, ny, nz ) == 0 && ! hasCompleteSheet( nx, ny, nz ) )
					{
						setDataAt( nx, ny, nz, 1 ) ;
						queue3->prepend( nx, ny, nz ) ;
					}
				}
				
				ele = queue2->remove() ;
			}

			// Next, swap queues
			GridQueue2* temp = queue2 ;
			queue2 = queue3 ;
			queue3 = temp ;
		}

		/* Deal with closed rings
		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i, j, k ) == 0 )
					{
						setDataAt( i, j, k, dis - 1 ) ;
					}
				}
		*/

		/* Find local minimum: to help determine erosion level */
		int cts[ 64 ] ;
		for ( i = 0 ; i <= - dis ; i ++ )
		{
			cts[ i ] = 0 ;
		}
		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					double val = getDataAt( i, j, k ) ;
					if ( val < -1 )
					{
						int min = 1 ;
						for ( int m = 0 ; m < 6 ; m ++ )
						{
							int nx = i + neighbor6[m][0] ;
							int ny = j + neighbor6[m][1] ;
							int nz = k + neighbor6[m][2] ;
							if ( getDataAt( nx, ny, nz ) < val )
							{
								min = 0 ;
								break ;
							}
						}

						if ( min )
						{
							cts[ (int) - val ] ++ ;
						}
					}
				}

		for ( i = 2 ; i <= - dis ; i ++ )
		{
			printf("Local minima: %d with %d nodes.\n", -i, cts[ i ] ) ;
		}
		

		// return ;

		// Dilate back
		// Starting from nodes with distance - 2 - disthr

		for ( int d = disthr + 1 ; d >= 2 ; d -- )
		{
			delete queue3 ;
			queue3 = new GridQueue2( ) ;
			queues[ d ]->reset() ;
			while ( (ele = queues[ d ]->getNext() ) != NULL )
			{
				int dilatable = 0 ;
				// for ( int m = 0 ; m < 6 ; m ++ )
				/*
				for ( int mx = -1 ; mx < 2 ; mx ++ )
					for ( int my = -1 ; my < 2 ; my ++ )
						for ( int mz = -1 ; mz < 2 ; mz ++ )
						{
							if ( mx == 0 || my == 0 || mz == 0 )
							{
								int nx = ele->x + mx ; // neighbor6[m][0] ;
								int ny = ele->y + my ; // neighbor6[m][1] ;
								int nz = ele->z + mz ; // neighbor6[m][2] ;
								if ( getDataAt( nx, ny, nz ) == - d - 1 )
								{
									dilatable = 1 ;
									break ;
								}
							}
						}
				*/
				for ( i = 0 ; i < 12 ; i ++ )
				{	
					int flag = 1, flag2 = 0 ;
					for ( j = 0 ; j < 4 ; j ++ )
					{
						int nx = ele->x + sheetNeighbor[i][j][0] ;
						int ny = ele->y + sheetNeighbor[i][j][1] ;
						int nz = ele->z + sheetNeighbor[i][j][2] ;

						double val = getDataAt( nx, ny, nz ) ;

						if ( val > - d )
						{
							flag = 0 ;
							break ;
						}
						if ( val == - d - 1 )
						{
							flag2 ++ ;
						}
					}

					if ( flag && flag2 )
					{
						dilatable = 1 ;
						break ;
					}
				}

				if ( ! dilatable )
				{
					// setDataAt( ele->x, ele->y, ele->z, - 1 ) ;
					queue3->prepend( ele->x, ele->y, ele->z ) ;
					/*
					setDataAt( ele->x, ele->y, ele->z, - d + 1 ) ;
					if ( d > 2 )
					{
						queues[ d - 1 ]->prepend( ele->x, ele->y, ele->z ) ;
					}
					*/
				}
			}

			queue3->reset() ;
			ele = queue3->getNext() ;
			while ( ele != NULL )
			{
				setDataAt( ele->x, ele->y, ele->z, - 1 ) ;
				ele = queue3->remove() ;
			}
		}

		// Finally, threshold the volume
		printf("Thresholding the volume to 0/1...\n") ;
		threshold( -1, 1, 0, 0 ) ;
	}

	

	void addNoise( float thr, float pos )
	{
		int i, j, k ;
		printf("Thresholding the volume to -MAX_ERODE/0...\n") ;
		threshold( thr, -MAX_ERODE, 0 ) ;
		Volume* tvol = new Volume( sizex, sizey, sizez, 0, 0, 0, this ) ;

		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( tvol->getDataAt( i, j, k ) >= 0  && isSimple( i, j, k ) )
					{
						for ( int m = 0 ; m < 6 ; m ++ )
						{
							if ( tvol->getDataAt( i + neighbor6[m][0], j + neighbor6[m][1], k + neighbor6[m][2] ) < 0 )
							{
								if ( rand() < RAND_MAX * pos )
								{
									setDataAt( i, j, k, thr - 1 ) ;
								}
								break ;
							}
						}
					}
				}

	}

	/************************************************************************/
	/* A sequential thinning method for extracting 6-connected skeletons                
	/* Properties: medial, topology preserving, shape preserving, noise resistance!
	/* @param thr: threshold
	/* @param type: 0 for curve preserving, 1 for surface preserving
	/* @param noise: 0 for no-noise-reduction, n for level-n noise reduction
	/************************************************************************/
	void sequentialSkeleton( float thr, int type, int noise )
	{
		int i, j, k, m ;
		// First, threshold the volume
		printf("Thresholding the volume to -1/0...\n") ;
		threshold( thr, -1, 0 ) ;

		// Values in the volume:
		// -1:	outside
		// 0:	unvisited
		// 1:	next layer
		// 2:	this layer - non complex nodes
		// 3:	complex nodes

		// Next, initialize the linked queue
		printf("Initializing queue...\n") ;
		GridQueue2* queue2 = new GridQueue2( ) ;
		GridQueue2* queue3 = new GridQueue2( ) ;
		GridQueue2* queue4 = new GridQueue2( ) ;
		gridQueueEle* ele ;

		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i, j, k ) >= 0 )
					{
						for ( m = 0 ; m < 6 ; m ++ )
						{
							if ( getDataAt( i + neighbor6[m][0], j + neighbor6[m][1], k + neighbor6[m][2] ) < 0 )
							{
								setDataAt( i, j, k, 1 ) ;
								queue2->prepend( i, j, k ) ;
								break ;
							}
						}
					}
				}
		// printf("Total %d nodes\n", queue2->getNumElements() ) ;


		// Perform erosion
		int dis = 0 ;
		int ox, oy, oz ;
		int nx, ny, nz ;
		while ( queue2->getNumElements() > 0 )
		{
			dis ++ ;
			printf("At distance %d, there are %d nodes.\n", dis, queue2->getNumElements()) ;

			// At the beginning of each iteration:
			// queue2 holds all nodes dis away from the background in city block metric
			// queue3 is empty

			// First, find out next layer and put into queue3
			queue2->reset() ;
			while ( (ele = queue2->getNext()) != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;

				setDataAt( ox, oy, oz, 2 ) ;
				for ( m = 0 ; m < 6 ; m ++ )
				{
						nx = ox + neighbor6[m][0] ;
						ny = oy + neighbor6[m][1] ;
						nz = oz + neighbor6[m][2] ;
						if ( getDataAt( nx, ny, nz ) == 0 )
						{
							setDataAt( nx, ny, nz, 1 ) ;
							queue3->prepend( nx, ny, nz ) ;
						}
				}
			}

			// Next, find out complex nodes in this layer and remove from queue2
			queue2->reset() ;
			ele = queue2->getNext() ;
			while ( ele != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;

				if ( (! isSimple( ox, oy, oz ))  ||
					 ( ( ( type == 0 && isEdgeEnd( ox, oy, oz ) ) ||
					     ( type == 1 && isFaceEnd( ox, oy, oz ) ) ) && ! isNoise( ox, oy, oz, noise ) ) )
				{
					setDataAt( ox, oy, oz, 3 ) ;
					ele = queue2->remove() ;
				}
				else
				{
					ele = queue2->getNext() ;
				}
			}

			// Now, the main loop: delete non-complex nodes until none
			queue2->reset() ;
			while ( ( ele = queue2->getNext() ) != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;
				queue2->remove() ;
				queue2->reset() ;

				if ( getDataAt(ox,oy,oz) != 2 )
				{
					continue ;
				}

				if ( ( ( ( type == 0 && isEdgeEnd( ox, oy, oz ) ) ||
								 ( type == 1 && isFaceEnd( ox, oy, oz ) ) ) && ! isNoise( ox, oy, oz, noise ) ) )
				{
					setDataAt( ox, oy, oz, 3 ) ;
				}
				else
				{
					setDataAt( ox, oy, oz, -1 ) ;
				}

				for ( i = -1 ; i < 2 ; i ++ )
					for ( j = -1 ; j < 2 ; j ++ )
						for ( k = -1 ; k < 2 ; k ++ )
						{
							nx = ox + i ;
							ny = oy + j ;
							nz = oz + k ;
							int val = (int)( getDataAt( nx, ny, nz ) ) ;
							if ( val > 1 )
							{
								int complex = 0 ;
								if ( (! isSimple( nx, ny, nz ))  ||
									( ( ( type == 0 && isEdgeEnd( nx, ny, nz ) ) ||
									( type == 1 && isFaceEnd( nx, ny, nz ) ) ) && ! isNoise( nx, ny, nz, noise ) ) )
								{
									complex = 1 ;
								}
								
								if ( val == 2 && complex )
								{
									// A non-complex node becomes complex
									setDataAt( nx, ny, nz, 3 ) ;
								}
								else if ( val == 3 && ! complex )
								{
									// A complex node turns non-complex
									setDataAt( nx, ny, nz, 2 ) ;
									queue2->prepend( nx, ny, nz ) ;
								}
								
							}
						}
						
				queue2->reset() ;
			}

			// Finally, swap queue3 and queue2
			GridQueue2* temp = queue3 ;
			queue3 = queue2 ;
			queue2 = temp ;
		}


		// Finally, clean up
		printf("Thresholding the volume to 0/1...\n") ;
		threshold( 0, 0, 1 ) ;

	}

	void dumbsurfaceSkeleton( float thr )
	{
		int i, j, k ;
		// First, threshold the volume
		printf("Thresholding the volume to -MAX_ERODE/0...\n") ;
		threshold( thr, -MAX_ERODE, 0 ) ;

		// Next, initialize the linked queue
		printf("Initializing queue...\n") ;
		GridQueue2* queue2 = new GridQueue2( ) ;
		gridQueueEle* ele ;


		while ( 1 )
		{
			int n = 0 ;

			queue2->reset() ;
			for ( i = 0 ; i < sizex ; i ++ )
				for ( j = 0 ; j < sizey ; j ++ )
					for ( k = 0 ; k < sizez ; k ++ )
					{
						if ( getDataAt( i, j, k ) == 0 )
						{
							{
								for ( int m = 0 ; m < 6 ; m ++ )
								{
									if ( getDataAt( i + neighbor6[m][0], j + neighbor6[m][1], k + neighbor6[m][2] ) < 0 )
									{
										queue2->prepend( i, j, k ) ;
										break ;
									}
								}
							}
						}
					}

			queue2->reset() ;
			ele = queue2->getNext() ;
			while ( ele != NULL )
			{
				int ox = ele->x ;
				int oy = ele->y ;
				int oz = ele->z ;
				if ( isSimple( ox, oy, oz ) && hasCompleteSheet( ox, oy, oz ) == 1 )
				{
					setDataAt( ox, oy, oz, -1 ) ;
					n ++ ;
				}

				ele = queue2->remove() ;
			}

			if ( n == 0 )
			{
				break ;
			}

			printf("%d simple nodes found.\n", n);
		}


		// Finally, clean up
		printf("Thresholding the volume to 0/1...\n") ;
		threshold( 0, 0, 1 ) ;
	}

	void surfaceSkeleton( float thr )
	{
		int i, j, k ;
		// First, threshold the volume
		printf("Thresholding the volume to -MAX_ERODE/0...\n") ;
		threshold( thr, -MAX_ERODE, 0 ) ;

		// Next, initialize the linked queue
		printf("Initializing queue...\n") ;
		GridQueue2* queue2 = new GridQueue2( ) ;
		GridQueue2* queue3 = new GridQueue2( ) ;
		GridQueue2* queue4 = new GridQueue2( ) ;

		PriorityQueue <gridPoint,int> * queue = new PriorityQueue <gridPoint,int> ( MAX_QUEUELEN );

		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i, j, k ) >= 0 )
					{
						{
							for ( int m = 0 ; m < 6 ; m ++ )
							{
								if ( getDataAt( i + neighbor6[m][0], j + neighbor6[m][1], k + neighbor6[m][2] ) < 0 )
								{
									// setDataAt( i, j, k, 1 ) ;
									queue2->prepend( i, j, k ) ;
									break ;
								}
							}
						}
					}
				}
		printf("Total %d nodes\n", queue2->getNumElements() ) ;


		// Perform erosion 
		int wid = MAX_ERODE ;
		printf("Start erosion to %d...\n", wid) ;
		gridQueueEle* ele ;
		gridPoint* gp ;
		double val = 0;
		int ox, oy, oz ;
		int score ;
		Volume* scrvol = new Volume( this->sizex , this->sizey, this->sizez ) ;
		for ( i = 0 ; i < sizex * sizey * sizez ; i ++ )
		{
			scrvol->setDataAt( i, -1 ) ;
		}

#ifdef  NOISE_DIS_SHEET
		Volume* noisevol = new Volume( sizex, sizey, sizez ) ;
#endif

		for ( int curwid = 1 ; curwid <= wid ; curwid ++ )
		{
			// At the start of each iteration, 
			// queue2 and queue4 holds all the nodes for this layer
			// queue3 and queue are empty

			int numComplex = 0, numSimple = 0 ;
			printf("Processing %d nodes in layer %d\n", queue2->getNumElements(), curwid) ;
			
			/*
			We first need to assign curwid + 1 to every node in this layer
			*/
			queue2->reset() ;
			ele = queue2->getNext() ;
			while ( ele != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;
				
				if ( getDataAt(ox,oy,oz) == curwid )
				{
					ele = queue2->remove() ;
				}
				else
				{
					setDataAt(ox,oy,oz, curwid) ;
					ele = queue2->getNext() ;
				}
			}
			queue4->reset() ;
			ele = queue4->getNext() ;
			while ( ele != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;

				queue2->prepend(ox,oy,oz) ;
				ele = queue4->remove() ;
			}

			// Now queue2 holds all the nodes for this layer

#ifdef NOISE_DIS_SHEET
			/* Extra step: classify nodes in queue2 into noise and non-noise nodes */
			queue2->reset() ;
			
			// First run
			int flag = 0 ;
			while ( ( ele = queue2->getNext() ) != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;
				if ( NOISE_DIS_SHEET <= 1 )
				{
					noisevol->setDataAt( ox, oy, oz, 0 ) ;
				}
				else
				{
					flag = 0 ;
					for ( int m = 0 ; m < 6 ; m ++ )
					{
						int nx = ox + neighbor6[m][0] ;
						int ny = oy + neighbor6[m][1] ;
						int nz = oz + neighbor6[m][2] ;
						if ( getDataAt( nx, ny, nz ) == 0 )
						{
							noisevol->setDataAt( ox, oy, oz, 1 ) ;
							flag = 1 ;
							break ;
						}
					}
					if ( ! flag )
					{
						noisevol->setDataAt( ox, oy, oz, 0 ) ;
					}
				}
			}

			for ( int cur = 1 ; cur < NOISE_DIS_SHEET ; cur ++ )
			{
				queue2->reset() ;
				int count = 0 ;

				while ( ( ele = queue2->getNext() ) != NULL )
				{
					ox = ele->x ;
					oy = ele->y ;
					oz = ele->z ;
					
					if ( noisevol->getDataAt( ox, oy, oz ) == 1 )
					{
						continue ;
					}

					flag = 0 ;
					for ( int m = 0 ; m < 6 ; m ++ )
					{
						int nx = ox + neighbor6[m][0] ;
						int ny = oy + neighbor6[m][1] ;
						int nz = oz + neighbor6[m][2] ;
						if ( getDataAt( nx, ny, nz ) > 0 && noisevol->getDataAt( nx, ny, nz ) == 1 )
						{
							noisevol->setDataAt( ox, oy, oz, 1 ) ;
							count ++ ;
							break ;
						}
					}
				}

				if ( count == 0 )
				{
					break ;
				}
			}


#endif

			/* Commented for debugging

			// First, 
			// check for complex nodes in queue2 
			// move them from queue2 to queue3
			queue2->reset() ;
			ele = queue2->getNext() ;
			while ( ele != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;
				
				// Check simple 
#ifndef NOISE_DIS_SHEET
				if ( isSheetEnd( ox, oy, oz ) || ! isSimple( ox, oy, oz ) ) 
#else
				if ( isSheetEnd( ox, oy, oz, noisevol ) || ! isSimple( ox, oy, oz ) ) 
#endif
				{
					// Complex, set to next layer
					setDataAt( ox, oy, oz, curwid + 1 ) ;
					queue3->prepend( ox, oy, oz ) ;
					ele = queue2->remove() ;
					
					numComplex ++ ;
				}
				else
				{
					ele = queue2->getNext() ;
				}
			}
			*/


			// Next,
			// Compute score for each node left in queue2
			// move them into priority queue
			queue2->reset() ;
			ele = queue2->getNext() ;
			while ( ele != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;
				
				// Compute score
				score = getNumPotComplex( ox, oy, oz ) ;
				scrvol->setDataAt( ox, oy, oz, score ) ;

				// Push to queue
				gp = new gridPoint ;
				gp->x = ox ;
				gp->y = oy ;
				gp->z = oz ;
				// queue->add( gp, -score ) ;
				queue->add( gp, score ) ;
				
				ele = queue2->remove() ;
			}

			// Rename queue3 to be queue2, 
			// Clear queue3
			// From now on, queue2 holds nodes of next level
			delete queue2 ;
			queue2 = queue3 ;
			queue3 = new GridQueue2( ) ;

			int nowComplex = 0 ;

			// Next, start priority queue iteration
			while ( ! queue->isEmpty() )
			{
				// Retrieve the node with the highest score
				queue->remove( gp, score ) ;
				ox = gp->x ;
				oy = gp->y ;
				oz = gp->z ;
				delete gp ;
				// printf("%d\n", score);
//				score = -score ;

				// Ignore the node 
				// if it has been processed before
				// or it has an updated score
				if ( getDataAt( ox, oy, oz ) != curwid || (int) scrvol->getDataAt( ox, oy, oz ) != score )
				{
					continue ;
				}

				/* Commented for debugging

				// Remove this simple node
				setDataAt( ox, oy, oz, -1 ) ;
				numSimple ++ ;
				// printf("Highest score: %d\n", score) ;
				*/

				/* Added for debugging */
				// Check simple 
#ifndef NOISE_DIS_SHEET
				// if ( hasFeatureFace( ox, oy, oz ) )
				if ( (! isSimple( ox, oy, oz )) || isSheetEnd( ox, oy, oz ) )
				// if ( hasIsolatedFace(ox,oy,oz)  && (! isNoiseSheetEnd(ox,oy,oz))) 
#else
				// if ( ! isSimple( ox, oy, oz ) || isSheetEnd( ox, oy, oz, noisevol ) ) 
				if ( ! isSimple( ox, oy, oz ) || isSheetEnd( ox, oy, oz, noisevol ) || isHelixEnd( ox, oy, oz, noisevol )) 
				// if ( isBertrandEndPoint( ox, oy, oz ) ) 
#endif
				{
					// Complex, set to next layer
					setDataAt( ox, oy, oz, curwid + 1 ) ;
					queue4->prepend( ox, oy, oz ) ;
					numComplex ++ ;

					nowComplex == 1 ;
				}
				else
				{
					setDataAt( ox, oy, oz, -1 ) ;
					numSimple ++ ;

					if ( nowComplex )
					{

						// printf("Error: %d\n", score);
					}
				}
				/* Adding ends */

				// Move its neighboring unvisited node to queue2
				for ( int m = 0 ; m < 6 ; m ++ )
				{
					int nx = ox + neighbor6[m][0] ;
					int ny = oy + neighbor6[m][1] ;
					int nz = oz + neighbor6[m][2] ;
					if ( getDataAt( nx, ny, nz ) == 0 )
					{
						// setDataAt( nx, ny, nz, curwid + 1 ) ;
						queue2->prepend( nx, ny, nz ) ;
					}
				}
				
				/* Commented for debugging
				
				// Find complex nodes in its 3x3 neighborhood
				// move them to queue2
				for ( i = -1 ; i < 2 ; i ++ )
					for ( j = -1 ; j < 2 ; j ++ )
						for ( k = -1 ; k < 2 ; k ++ )
						{
							int nx = ox + i ;
							int ny = oy + j ;
							int nz = oz + k ;

							// Check simple 
							if ( getDataAt( nx, ny, nz ) == curwid && 
								// ( isSheetEnd( ox, oy, oz ) || ! isSimple( nx, ny, nz )) )
#ifndef NOISE_DIS_SHEET
								( isSheetEnd( nx, ny, nz ) || ! isSimple( nx, ny, nz ) ) )
#else
								( isSheetEnd( nx, ny, nz, noisevol ) || ! isSimple( nx, ny, nz ) ) )
#endif

							{
								// Complex, set to next layer
								setDataAt( nx, ny, nz, curwid + 1 ) ;
								queue2->prepend( nx, ny, nz ) ;
								numComplex ++ ;
							}
						}
				*/

				// Update scores for nodes in its 5x5 neighborhood
				// insert them back into priority queue
					
				for ( i = -2 ; i < 3 ;i ++ )
					for ( j = -2 ; j < 3 ; j ++ )
						for ( k = -2 ; k < 3 ; k ++ )
						{
							int nx = ox + i ;
							int ny = oy + j ;
							int nz = oz + k ;

							if ( getDataAt( nx, ny, nz ) == curwid )
							{
								// Compute score
								score = getNumPotComplex( nx, ny, nz ) ;
								
								if ( score != (int) scrvol->getDataAt( nx, ny, nz ) )
								{
									// printf("Update\n") ;
									scrvol->setDataAt( nx, ny, nz, score ) ;
									// Push to queue
									gp = new gridPoint ;
									gp->x = nx ;
									gp->y = ny ;
									gp->z = nz ;
									// queue->add( gp, -score ) ;
									queue->add( gp, score ) ;
								}
							}
						}
						

			}

			printf("%d complex, %d simple\n", numComplex, numSimple) ;
			
			if ( numSimple == 0 )
			{
					break ;
			}
		}

		// Finally, clean up
		printf("Thresholding the volume to 0/1...\n") ;
		threshold( 0, 0, 1 ) ;

	}

	/* Bertrand's parallel 6-connected surface thinning */
	void bertrandSurfaceSkeleton2( float thr )
	{
		int i, j, k ;

		// First, threshold the volume
		printf("Thresholding the volume to -MAX_ERODE/0...\n") ;
		threshold( thr, -MAX_ERODE, 0 ) ;


		// Initialize queues
		printf("Initializing queues...\n") ;
		GridQueue2* queue2 = new GridQueue2( ) ;
		GridQueue2* queue3 = new GridQueue2( ) ;
		GridQueue2* queue4 = new GridQueue2( ) ;
		Volume* fvol = new Volume( this->sizex, this->sizey, this->sizez, 0 ) ;

		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i, j, k ) == 0 )
					{
						for ( int m = 0 ; m < 6 ; m ++ )
						{
							if ( getDataAt( i + neighbor6[m][0], j + neighbor6[m][1], k + neighbor6[m][2] ) < 0 )
							{
								fvol->setDataAt( i, j, k, 1 ) ;
								queue2->prepend( i, j, k ) ;
								break ;
							}
						}
					}
				}
		printf("Total %d nodes\n", queue2->getNumElements() ) ;

		// Start iteration
		int it = 0 ;
		gridQueueEle* ele ;
		while ( ++ it )
		{
			printf("Iteration %d... nodes in queue: %d...", it, queue2->getNumElements()) ;

			// queue2 holds all boundary nodes

			int deleted = 0 ;
			for ( i = 0 ; i < 6 ; i ++ )
			{
				// At the beginning of each iteration, 
				// queue2 holds all remaining boundary nodes
				// queue3 is a deletable array, starting empty
				// queue4 holds the candidates for next layer

				queue2->reset() ;
				ele = queue2->getNext() ;

				// For each sub-iteration, go through queue2 first and find points to delete
				while ( ele != NULL )
				{
					int ox = ele->x ;
					int oy = ele->y ;
					int oz = ele->z ;
					
					if ( isBertrandBorder( ox,oy,oz, i ) )
					{
						if ( ! isBertrandEndPoint( ox,oy,oz ) )
						{
							// Move this node to queue3
							ele = queue2->remove() ;
							queue3->prepend( ox, oy, oz ) ;

							// Move its neighboring unvisited node to queue4
							for ( int m = 0 ; m < 6 ; m ++ )
							{
								int nx = ox + neighbor6[m][0] ;
								int ny = oy + neighbor6[m][1] ;
								int nz = oz + neighbor6[m][2] ;
								if ( fvol->getDataAt( nx, ny, nz ) == 0 )
								{
									fvol->setDataAt( nx, ny, nz, 1 ) ;
									queue4->prepend( nx, ny, nz ) ;
								}
							}
						}
						else
						{
							ele = queue2->getNext() ;
						}
					}
					else
					{
						ele = queue2->getNext() ;
					}
				}

				// Now, queue2 holds all remaining nodes, 
				// queue3 holds all deletable nodes, 
				// and queue4 holds nodes to be added to the next layer

				// Simultaneous deletion
				if ( queue3->getNumElements() == 0 )
				{
					// break ;
				}
				else
				{
					queue3->reset() ;
					ele = queue3->getNext() ;
					while ( ele != NULL )
					{
						setDataAt( ele->x, ele->y, ele->z, -1 ) ;
						ele = queue3->remove() ;
						deleted ++ ;
					}
				}

				// return ;
			}

			// After all sub-iterations
			// Move all queue4 nodes to queue2
			queue4->reset() ;
			ele = queue4->getNext() ;
			while ( ele != NULL )
			{
				queue2->prepend( ele->x, ele->y, ele->z) ;
				ele = queue4->remove() ;
			}
			
			if ( deleted == 0 )
			{
				printf("No more deletable nodes.\n");
				break ;
			}
			else
			{
				printf("Deleted: %d\n", deleted) ;
			}
		}

		// Finally, clean up
		printf("Thresholding the volume to 0/1...\n") ;
		threshold( 0, 0, 1 ) ;

	}



	void bertrandSurfaceSkeleton( float thr )
	{
		int i, j, k ;

		// First, threshold the volume
		printf("Thresholding the volume to -MAX_ERODE/1...\n") ;
		threshold( thr, -MAX_ERODE, 0 ) ;

		// Start iteration
		GridQueue2* queue2 = new GridQueue2( ) ;
		int dir = 0, ct = 0 ;
		while( ++ct )
		{
			printf("Round %d, direction %d...", ct, dir) ;
			queue2->reset() ;

			for ( i = 0 ; i < sizex ; i ++ )
				for ( j = 0 ; j < sizey ; j ++ )
					for ( k = 0 ; k < sizez ; k ++ )
					{
						if ( getDataAt(i,j,k) >= 0 )
						{
							if ( isBertrandBorder( i,j,k, dir ) )
							{
								if ( ! isBertrandEndPoint( i,j,k ) )
								{
									queue2->prepend( i,j,k ) ;
								}
							}
						}

					}
			

			if ( queue2->getNumElements() == 0 )
			{
				printf("Done.\n");
				break ;
			}
			else
			{
				queue2->reset() ;
				printf("%d nodes deleted.\n", queue2->getNumElements()) ;
				gridQueueEle* ele = queue2->getNext() ;
				while ( ele != NULL )
				{
					setDataAt( ele->x, ele->y, ele->z, -1 ) ;
					ele = queue2->remove() ;
				}
			}
			dir = ( dir + 1 ) % 6 ;
		}

		// Finally, clean up
		printf("Thresholding the volume to 0/1...\n") ;
		threshold( 0, 0, 1 ) ;
	}


	/* Palagyi's parallel surface thinning */
	void palagyiSurfaceSkeleton( float thr )
	{
		int i, j, k ;

		// First, threshold the volume
		printf("Thresholding the volume to 0/1...\n") ;
		threshold( thr, 0, 1 ) ;

		// Next, initialize the templates
		printf("Initializing surface endpoints templates...\n") ;
		ThinningTemplate* US[6] ;

		int b0[] = {12,13} ;
		int w0[] = {2, 5, 8, 11, 14, 17, 20, 23, 26} ;
		int ob0[] = {10, 16} ;
		int ob20[] = {4, 22} ;
		int nu[] = {0} ;
		US[0] = new ThinningTemplate( b0, 2, w0, 9, ob0, 2, ob20, 2, nu, 0, nu, 0 ) ;
		US[1] = new ThinningTemplate( US[0], 0, 1 ) ;
		/*
		int b01[] = {13,16} ;
		int w01[] = {0,1,2,9,10,11,18,19,20} ;
		int ob01[] = {12, 14} ;
		int ob201[] = {4, 22} ;
		US[1] = new ThinningTemplate( b01, 2, w01, 9, ob01, 2, ob201, 2, nu, 0, nu, 0 ) ;
		*/

		int b1[] = {12,13,16,22} ;
		int w1[] = {2,10,11,14} ;
		int ow[] = {1,5} ;
		US[2] = new ThinningTemplate( b1, 4, w1, 4, nu, 0, nu, 0, ow, 2, nu, 0 ) ;
		US[3] = new ThinningTemplate( US[2], 0 ) ;

		int b2[] = {2,12,13,16,22} ;
		int w2[] = {10,11,14} ;
		int op[] = {1,5} ;
		US[4] = new ThinningTemplate( b2, 5, w2, 3, nu, 0, nu, 0, nu, 0, op, 2 ) ;
		US[5] = new ThinningTemplate( US[4], 0 ) ;

		ThinningTemplate * NE[6], * WD[6], * ES[6], * UW[6], * ND[6], * SW[6], * UN[6], * ED[6], * NW[6], * UE[6], * SD[6] ;

		for ( i = 0 ; i < 6 ; i ++ )
		{
			SD[i] = new ThinningTemplate( US[i], 0, 1 ) ;
			ND[i] = new ThinningTemplate( SD[i], 0, 1 ) ;
			UN[i] = new ThinningTemplate( ND[i], 0, 1 ) ;

			ES[i] = new ThinningTemplate( US[i], 1, 1 ) ;
			NE[i] = new ThinningTemplate( ES[i], 2, 1 ) ;
			NW[i] = new ThinningTemplate( NE[i], 2, 1 ) ;
			SW[i] = new ThinningTemplate( NW[i], 2, 1 ) ;

			UE[i] = new ThinningTemplate( US[i], 2, 1 ) ;
			ED[i] = new ThinningTemplate( UE[i], 1, 1 ) ;
			WD[i] = new ThinningTemplate( ED[i], 1, 1 ) ;
			UW[i] = new ThinningTemplate( WD[i], 1, 1 ) ;
		}

		ThinningTemplate** alltemps[12] = { US, NE, WD, ES, UW, ND, SW, UN, ED, NW, UE, SD } ;

		// Initialize queues
		printf("Initializing queues...\n") ;
		GridQueue2* queue2 = new GridQueue2( ) ;
		GridQueue2* queue3 = new GridQueue2( ) ;
		GridQueue2* queue4 = new GridQueue2( ) ;
		Volume* fvol = new Volume( this->sizex, this->sizey, this->sizez, 0 ) ;

		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i, j, k ) == 1 )
					{
						for ( int m = 0 ; m < 6 ; m ++ )
						{
							if ( getDataAt( i + neighbor6[m][0], j + neighbor6[m][1], k + neighbor6[m][2] ) == 0 )
							{
								fvol->setDataAt( i, j, k, 1 ) ;
								queue2->prepend( i, j, k ) ;
								break ;
							}
						}
					}
				}
		printf("Total %d nodes\n", queue2->getNumElements() ) ;

		// Start iteration
		int it = 0 ;
		int vox[3][3][3] ;
		gridQueueEle* ele ;
		while ( queue2->getNumElements() > 0 )
		{
			printf("Iteration %d... nodes in queue: %d...", it, queue2->getNumElements()) ;

			// queue2 holds all boundary nodes

			int deleted = 0 ;
			for ( i = 0 ; i < 12 ; i ++ )
			{
				// At the beginning of each iteration, 
				// queue2 holds all remaining boundary nodes
				// queue3 is a deletable array, starting empty
				// queue4 holds the candidates for next layer

				queue2->reset() ;
				ele = queue2->getNext()		;

				// For each sub-iteration, go through queue2 first and find points to delete
				while ( ele != NULL )
				{
					int ox = ele->x ;
					int oy = ele->y ;
					int oz = ele->z ;
					
					// Check with templates
					int match = 0 ;
					for ( int ci = -1 ; ci < 2 ; ci ++ )
						for ( int cj = -1 ; cj < 2 ; cj ++ )
							for ( int ck = -1 ; ck < 2 ; ck ++ )
							{
								vox[ ci + 1 ][cj + 1][ck + 1] = (int)getDataAt( ox + ci, oy + cj, oz + ck ) ;
							}
					
					for ( j = 0 ; j < 6 ; j ++ )
					// j = 1 ;
					{
						if ( alltemps[i][j]->isMatch( vox ) )
						{
							/* Debug */
							if ( ! isSimple2( vox ) )
							{
								printf("Wrong! %d %d\n", i, j) ;
								for ( int cci = 0 ; cci < 3 ; cci ++ )
								{
									for ( int ccj = 0 ; ccj < 3 ; ccj ++ )
									{
										for ( int cck = 0 ; cck < 3 ; cck ++ )
										{
											printf("%d ",vox[ cci ][ccj][cck]);
										}
										printf(" , ") ;
									}
									printf("\n") ;
								}
								exit(0) ;
							}

							// Move this node to queue3
							ele = queue2->remove() ;
							queue3->prepend( ox, oy, oz ) ;

							// Move its neighboring unvisited node to queue4
							for ( int m = 0 ; m < 6 ; m ++ )
							{
								int nx = ox + neighbor6[m][0] ;
								int ny = oy + neighbor6[m][1] ;
								int nz = oz + neighbor6[m][2] ;
								if ( fvol->getDataAt( nx, ny, nz ) == 0 )
								{
									fvol->setDataAt( nx, ny, nz, 1 ) ;
									queue4->prepend( nx, ny, nz ) ;
								}
							}

							match = 1 ;
							break ;
						}
					}

					if ( match == 0 )
					{
						ele = queue2->getNext() ;
					}
				}

				// Now, queue2 holds all remaining nodes, 
				// queue3 holds all deletable nodes, 
				// and queue4 holds nodes to be added to the next layer

				// Simultaneous deletion
				queue3->reset() ;
				ele = queue3->getNext() ;
				while ( ele != NULL )
				{
					setDataAt( ele->x, ele->y, ele->z, 0 ) ;
					ele = queue3->remove() ;
					deleted ++ ;
				}

				// return ;
			}

			// After all sub-iterations
			// Move all queue4 nodes to queue2
			queue4->reset() ;
			ele = queue4->getNext() ;
			while ( ele != NULL )
			{
				queue2->prepend( ele->x, ele->y, ele->z) ;
				ele = queue4->remove() ;
			}
			
			if ( deleted == 0 )
			{
				printf("No more deletable nodes.\n");
				break ;
			}
			else
			{
				printf("Deleted: %d\n", deleted) ;
			}
		}
	}
	
	/**
	 * Normalize to a given range 
	 */
	void threshold( double thr )
	{
		threshold( thr, 0, 1 ) ;
	}

	void threshold( double thr, int out, int in )
	{
		threshold( thr, out, in, out ) ;
	}

	void threshold( double thr, int out, int in, int boundary )
	{
		//int size = sizex * sizey * sizez ;
		//for ( int i = 0 ; i < size ; i ++ )
		//{
		//	data[ i ] = data[ i ] < thr ? out : in ;
		//}

		int ct = 0 ;
		for ( int i = 0 ; i < sizex ; i ++ )
			for ( int j = 0 ; j < sizey ; j ++ )
				for ( int k = 0 ; k < sizez ; k ++ )
				{
					if ( i > 1 && i < sizex - 2 && j > 1 && j < sizey - 2 && k > 1 && k < sizez - 2 )
					{
						data[ ct ] = data[ ct ] < thr ? (float)out : (float)in ;
					}
					else
					{
						data[ ct ] = (float)boundary ;
					}
					ct ++ ;
				}
	}

	void smooth( float alpha )
	{
		int size = sizex * sizey * sizez ;
		float* ndata = new float[ size ] ;
		for ( int i = 0 ; i < size ; i ++ )
		{
			ndata[ i ] = data[ i ] ;
		}

		for (int i = 1 ; i < sizex - 1 ; i ++ )
			for ( int j = 1 ; j < sizey - 1 ; j ++ )
				for ( int k = 1 ; k < sizez - 1 ; k ++ )
				{
					int ct =  i * sizey * sizez + j * sizez + k ;

					float v = (float)getDataAt( i - 1, j, k ) + 
						(float)getDataAt( i + 1, j, k ) +
						(float)getDataAt( i, j - 1, k ) +
						(float)getDataAt( i, j + 1, k ) +
						(float)getDataAt( i, j, k - 1 ) +
						(float)getDataAt( i, j, k + 1 ) ;
					ndata[ ct ] = ndata[ ct ] * alpha + ( 1 - alpha ) * v / 6 ;
				}

		for (int i = 0 ; i < size ; i ++ )
		{
			data[ i ] = ndata[ i ] ;
		}

		delete ndata ;
	}

	void normalize( double min, double max )
	{
		double imin = getMin() ;
		double imax = getMax() ;
		double irange = imax - imin ;
		double range = max - min ;

		int size = sizex * sizey * sizez ;
		for ( int i = 0 ; i < size ; i ++ )
		{
			data[ i ] = (( data[ i ] - (float)imin ) / (float)irange) * (float)range + (float)min ;
		}
	}

	void normalize( double min, double max, double thresh, double ithresh )
	{
		double imin = getMin() ;
		double imax = getMax() ;
		double irange1 = ithresh - imin ;
		double irange2 = imax - ithresh ;
		double range1 = thresh - min;
		double range2 = max - thresh ;

		int size = sizex * sizey * sizez ;
		for ( int i = 0 ; i < size ; i ++ )
		{
			if ( data[ i ] < ithresh )
			{
				data[ i ] = (( data[ i ] - (float)imin ) / (float)irange1) * (float)range1 + (float)min ;
			}
			else
			{
				data[ i ] = (float)max - (( (float)imax - data[ i ] ) / (float)irange2) * (float)range2 ;
			}
		}
	}

	/* Set data at a pixel */
	void setDataAt( int x, int y, int z, double d )
	{
		data[ x * sizey * sizez + y * sizez + z ] = (float)d ;
	}
	void setDataAt( int index, double d )
	{
		data[ index ] = (float)d ;
	}

	/* Get data at a single voxel */
	double getDataAt( int x, int y, int z ) 
	{
		return data[ x * sizey * sizez + y * sizez + z ] ;
	}
	double getDataAt( int index ) 
	{
		return data[ index ] ;
	}
	
	/* Get data at an interpolated voxel */
	double getInterpDataAt( double x, double y, double z ) 
	{
		/*
		double rad = sizex / 4.0 ;
		double cent = ( sizex - 1 ) / 2.0 ;
		
		double ox = x - cent ;
		double oy = y - cent ;
		double oz = z - cent ;
		
		double a = -0.3 ;
		double nx = ox ;
		double ny = cos( a ) * oy + sin( a ) * oz ;
		double nz = - sin( a ) * oy + cos( a ) * oz ;

		double b = 1.4 ;
		double nnx = cos( b ) * nx + sin( b ) * ny - 2;
		double nny = -sin( b ) * nx + cos ( b ) * ny - 1;
		double nnz = nz + 1;
		
		double dis = nnx * nnx + nny * nny ;
		return 10 - 10 * dis / ( rad * rad ) ;
		*/

		double rvalue ;
		int hx = (int) ceil( x ) ;
		int lx = (int) floor( x ) ;
		int hy = (int) ceil( y ) ;
		int ly = (int) floor( y ) ;
		int hz = (int) ceil( z ) ;
		int lz = (int) floor( z ) ;
		
		double x1 = x - lx, x2 = 1 - x1 ;
		double r1 = x2 * getDataAt( lx, ly, lz) + x1 * getDataAt( hx, ly, lz ) ; 
		double r2 = x2 * getDataAt( lx, ly, hz) + x1 * getDataAt( hx, ly, hz ) ; 
		double r3 = x2 * getDataAt( lx, hy, lz) + x1 * getDataAt( hx, hy, lz ) ; 
		double r4 = x2 * getDataAt( lx, hy, hz) + x1 * getDataAt( hx, hy, hz ) ; 
		
		double y1 = y - ly, y2 = 1 - y1 ;
		double r5 = y2 * r1 + y1 * r3 ;
		double r6 = y2 * r2 + y1 * r4 ;

		double z1 = z - lz, z2 = 1 - z1 ;
		rvalue = z2 * r5 + z1 * r6 ;

		return rvalue ;
	}

	/* Rotation routine */
	void rotateX ( double a )
	{
		double * ndata = new double[ sizex * sizey * sizez ] ;
		if ( sizex != sizey || sizex != sizez )
		{
			return ;
		}

		int ct = 0 ;
		double cent = ( sizex - 1 ) / 2.0 ;
		for ( int i = 0 ; i < sizex ; i ++ )
			for ( int j = 0 ; j < sizey ; j ++ )
				for ( int k = 0 ; k < sizez ; k ++ )
				{
					double x = i - cent ;
					double y = j - cent ;
					double z = k - cent ;

					double nx = x + cent ;
					double ny = cos( a ) * y + sin( a ) * z + cent ;
					double nz = - sin( a ) * y + cos( a ) * z + cent ;

					if ( nx < 0 )
					{
						nx = 0 ;
					}
					else if ( nx > sizex - 1 )
					{
						nx = sizex - 1 ;
					}

					if ( ny < 0 )
					{
						ny = 0 ;
					}
					else if ( ny > sizey - 1 )
					{
						ny = sizey - 1 ;
					}

					if ( nz < 0 )
					{
						nz = 0 ;
					}
					else if ( nz > sizez - 1 )
					{
						nz = sizez - 1 ;
					}
					
					ndata[ ct ] = getInterpDataAt( nx, ny, nz );

					ct ++ ;
				}

			for (int i = 0 ; i < sizex * sizey * sizez ; i ++ )
			{
				data[ct] = (float)ndata[ct] ;
			}

			delete ndata ;
	}


	/* Destruction */
	~Volume( )
	{
		delete data ;
	}

	/* Write to file */
	void toMathematicaFile( char* fname )
	{
		FILE* fout = fopen( fname, "w" ) ;

		fprintf( fout, "{" ) ;
		for ( int i = 0 ; i < sizex ; i ++ )
		{
			fprintf( fout, "{" ) ;
			for ( int j = 0 ; j < sizey ; j ++ )
			{
				fprintf( fout, "{" ) ;
				for ( int k = 0 ; k < sizez ; k ++ )
				{
					fprintf( fout, "%.15f", getDataAt( i, j, k ) ) ;
					if ( k < sizez - 1 )
					{
						fprintf( fout, "," ) ;
					}
				}
				fprintf( fout, "}" ) ;
				if ( j < sizey - 1 )
				{
					fprintf( fout, ",\n" ) ;
				} else {
					fprintf( fout, "\n" ) ;
				}
			}
			fprintf( fout, "}" ) ;
			if ( i < sizex - 1 )
			{
				fprintf( fout, ",\n\n\n" ) ;
			} else {
				fprintf( fout, "\n\n\n" ) ;
			}
		}
		fprintf(fout,"}") ;

		fclose( fout ) ;

	}

	/* Write to file */
	void toMathematicaFile( char* fname, int lx, int hx, int ly, int hy, int lz, int hz )
	{
		FILE* fout = fopen( fname, "w" ) ;

		fprintf( fout, "{" ) ;
		for ( int i = lx ; i < hx ; i ++ )
		{
			fprintf( fout, "{" ) ;
			for ( int j = ly ; j < hy ; j ++ )
			{
				fprintf( fout, "{" ) ;
				for ( int k = lz ; k < hz ; k ++ )
				{
					fprintf( fout, "%.15f", getDataAt( i, j, k ) ) ;
					if ( k < hz - 1 )
					{
						fprintf( fout, "," ) ;
					}
				}
				fprintf( fout, "}" ) ;
				if ( j < hy - 1 )
				{
					fprintf( fout, "," ) ;
				}
			}
			fprintf( fout, "}" ) ;
			if ( i < hx - 1 )
			{
				fprintf( fout, "," ) ;
			}
		}
		fprintf(fout,"}") ;

		fclose( fout ) ;

	}

	void toOFFCells( char* fname )
	{
		toOFFCells( fname, 0.0001f ) ;
	}
	void toOFFCells2( char* fname )
	{
		toOFFCells2( fname, 0.0001f ) ;
	}

	void toOFFCells2( char* fname, float thr )
	{
		int i, j, k ;
		Volume* indvol = new Volume( sizex, sizey, sizez, -1 ) ;

		// Get number of cells to write
		int numverts = 0, numfaces = 0 ;
		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i, j, k ) >= thr )
					{
						indvol->setDataAt( i,j,k, numverts ) ;
						numverts ++ ;

						for ( int mi = 0 ; mi < 3 ; mi ++ )
						{
							int find = mi * 4 + 3 ;
							int isFace = 1 ;
							for ( int mj = 0 ; mj < 4 ; mj ++ )
							{
								int nx = i + sheetNeighbor[find][mj][0] ;
								int ny = j + sheetNeighbor[find][mj][1] ;
								int nz = k + sheetNeighbor[find][mj][2] ;
								
								if ( getDataAt( nx, ny, nz ) < thr )
								{
									isFace = 0 ;
									break ;
								}
							}
							if ( isFace )
							{
								numfaces ++ ;
							}

							int eind = mi * 2 + 1 ;
							if ( getDataAt( i + neighbor6[eind][0], j + neighbor6[eind][1], k + neighbor6[eind][2]) >= thr )
							{
								numfaces ++ ;
							}
						}
					}
				}

		FILE* fin = fopen( fname, "w" ) ;
		fprintf( fin, "OFF\n" ) ;
		fprintf( fin, "%d %d 0\n", numverts, numfaces ) ;

		// Write vertices
		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i,j,k ) >= thr )
					{
						fprintf( fin, "%d %d %d\n", i, j, k ) ;
					}
				}

		// Write faces
		int ct = 0 ;
		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i,j,k ) >= thr )
					{
						int thisvt = (int)( indvol->getDataAt( i,j,k ) );
						for ( int mi = 0 ; mi < 3 ; mi ++ )
						{
							int find = mi * 4 + 3 ;
							int isFace = 1 ;
							int vts[4] ;
							for ( int mj = 0 ; mj < 4 ; mj ++ )
							{
								int nx = i + sheetNeighbor[find][mj][0] ;
								int ny = j + sheetNeighbor[find][mj][1] ;
								int nz = k + sheetNeighbor[find][mj][2] ;

								vts[ mj ] = (int)( indvol->getDataAt( nx, ny, nz ) );
								
								if ( getDataAt( nx, ny, nz ) < thr )
								{
									isFace = 0 ;
									break ;
								}
							}
							if ( isFace )
							{
								fprintf( fin, "4 %d %d %d %d\n", vts[0], vts[1], vts[3], vts[2] ) ;
							}

							int eind = mi * 2 + 1 ;
							int mx = i + neighbor6[eind][0] ;
							int my = j + neighbor6[eind][1] ;
							int mz = k + neighbor6[eind][2] ;
							int vt = (int)( indvol->getDataAt( mx, my, mz ) );
							if ( getDataAt( mx, my, mz ) >= thr )
							{
								fprintf( fin, "4 %d %d %d %d\n", thisvt, thisvt, vt, vt ) ;
							}
						}
					}
				}

		fclose( fin ) ;
		//delete indvol ;
	}

	void toOFFCells( char* fname, float thr )
	{
		int i, j, k ;
		// Volume* indvol = new Volume( sizex, sizey, sizez, -1 ) ;

		// Get number of cells to write
		int numverts = 0, numfaces = 0 ;
		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i, j, k ) >= thr )
					{
						numverts += 8 ;
						for ( int mi = 0 ; mi < 6 ; mi ++ )
						{
							if ( getDataAt( i + neighbor6[mi][0], j + neighbor6[mi][1], k + neighbor6[mi][2] ) < thr )
							{
								numfaces ++ ;
							}
						}
					}
				}

		FILE* fin = fopen( fname, "w" ) ;
		fprintf( fin, "OFF\n" ) ;
		fprintf( fin, "%d %d 0\n", numverts, numfaces ) ;

		// Write vertices
		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i,j,k ) >= thr )
					{
						float ox = i - 0.5f ;
						float oy = j - 0.5f ;
						float oz = k - 0.5f ;

						for ( int mi = 0 ; mi < 2 ; mi ++ )
							for ( int mj = 0 ; mj < 2 ; mj ++ )
								for ( int mk = 0 ; mk < 2 ; mk ++ )
								{
									fprintf( fin, "%f %f %f\n", ox + mi, oy + mj, oz + mk ) ;
								}
					}
				}

		// Write faces
		int ct = 0 ;
		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i,j,k ) >= thr )
					{
						for ( int mi = 0 ; mi < 6 ; mi ++ )
						{
							if ( getDataAt( i + neighbor6[mi][0], j + neighbor6[mi][1], k + neighbor6[mi][2] ) < thr )
							{
								fprintf( fin, "4 %d %d %d %d\n", cubeFaces[mi][0] + ct, cubeFaces[mi][1] + ct, cubeFaces[mi][2] + ct, cubeFaces[mi][3] + ct ) ;
							}
						}

						ct += 8 ;
					}
				}

		fclose( fin ) ;
		//delete indvol ;
	}

	void segment( float threshold, Volume* lowvol, Volume* highvol, char* mrcfile )
	{
		int i,j,k ;
		Volume* segvol = new Volume( sizex, sizey, sizez, 0 ) ;
		
		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( lowvol->getDataAt(i,j,k) > 0 )
					{
						segvol->setDataAt( i,j,k, 1 ) ;
					}
					else if ( highvol->getDataAt(i,j,k) > 0 )
					{
						segvol->setDataAt( i,j,k, 2 ) ;
					}
					else
					{
						segvol->setDataAt( i,j,k, 0 ) ;
					}
				}	

		writeSegmentation( threshold, segvol, NULL, mrcfile ) ;
	}

	void segment( float threshold, Volume* vol, int maxDis, char* mrcfile )
	{
		int i,j;
		Volume* testvol = NULL ;
		Volume* disvol = new Volume( sizex, sizey, sizez ) ;
		printf("Writing distance transform to %d levels.\n", maxDis);

		int totNodes = 0 ;
		int size = sizex * sizey * sizez ;
		for ( i = maxDis ; i >= 0 ; i -- )
		{
			if ( i == 1 ) continue ;

			int nodes = 0 ;
			testvol = new Volume( sizex, sizey, sizez, 0, 0, 0, vol ) ;
			testvol->erodeSheet( i ) ;
			
			for ( j = 0 ; j < size ; j ++ )
			{
				if ( disvol->getDataAt(j) == 0 && testvol->getDataAt(j) > 0 )
				{
					disvol->setDataAt( j, i + 1 ) ;
					nodes ++ ;
				}
			}
			printf("Level %d has %d nodes.\n", i, nodes );
			totNodes += nodes ;
			delete testvol ;
		}
		printf("Totally %d nodes.\n", totNodes );

		writeSegmentation( threshold, disvol, NULL, mrcfile ) ;
	}

	// Segment the volume using segvol
	// background voxels have values 0, others have values > 0
	void writeSegmentation( float threshold, Volume* segvol, char* txtfile, char* mrcfile )
	{
		printf("Start segmentation.\n") ;
		int i,j,k ;
		Volume* vvol = new Volume( sizex, sizey, sizez, 0 ) ;
		Volume* tvol1 = new Volume( sizex, sizey, sizez, 0 ) ;
		Volume* tvol2 = new Volume( sizex, sizey, sizez, 0 ) ;

		// Initialize
		GridQueue2* queue = new GridQueue2() ;
		GridQueue2* queue2 = new GridQueue2() ;
		GridQueue2* queue3 = new GridQueue2() ;
		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt( i,j,k ) < threshold || segvol->getDataAt(i,j,k) <= 0 )
					{
						continue ;
					}
					
					vvol->setDataAt(i,j,k,1) ;
					queue->prepend( i,j,k ) ;
				}

		// Dilation
		printf("Dilation...") ;
		int ox, oy, oz ;
		gridQueueEle* ele ;
		while ( queue->getNumElements() > 0 )
		{
			// At the beginning
			// queue holds all nodes of previous layer
			// queue2 is empty, ready to fill with the current layer

			// First, fill current layer and assign values
			queue->reset() ;
			ele = queue->getNext() ;
			while ( ele != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;
				double seg = segvol->getDataAt(ox,oy,oz) ;
				int isBorder = 0 ;

				for ( int m = 0 ; m < 6 ; m ++ )
				{
					int nx = ox + neighbor6[m][0] ;
					int ny = oy + neighbor6[m][1] ;
					int nz = oz + neighbor6[m][2] ;
					if ( getDataAt( nx, ny, nz ) >= threshold && vvol->getDataAt( nx, ny, nz ) == 0 )
					{
						double ct = (int) tvol1->getDataAt(nx,ny,nz) ;
						double val = tvol2->getDataAt(nx,ny,nz) ;

						tvol1->setDataAt(nx,ny,nz, ct + 1 ) ;
						tvol2->setDataAt(nx,ny,nz, val + seg ) ;

						if (ct == 0)
						{
							queue2->prepend( nx, ny, nz ) ;
						}
					}
					else if ( getDataAt( nx, ny, nz ) < threshold )
					{
						isBorder = 1 ;
					}
				}
				
				if ( isBorder )
				{
					queue3->prepend( ox, oy, oz ) ;
				}

				ele = queue->remove() ;
			}

			// Normalize values of nodes in queue2
			queue2->reset() ;
			while ( ( ele = queue2->getNext() ) != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;

				double ct = (int) tvol1->getDataAt(ox,oy,oz) ;
				double val = tvol2->getDataAt(ox,oy,oz) ;

				if ( ct == 0 )
				{
					printf("Wrong! %f\n", ct) ;
				}
				segvol->setDataAt(ox,oy,oz, val / ct ) ;
				vvol->setDataAt(ox,oy,oz, 1 ) ;
			}

			// Finally, swap queues
			GridQueue2* temp = queue2 ;
			queue2 = queue ;
			queue = temp ;
		}
		printf("Done.\n") ;

		/* Debug
		for ( i = 0 ; i < sizex * sizey * sizez ; i ++ )
		{
			if ( getDataAt(i) >= threshold && segvol->getDataAt(i) == 0 )
			{
				segvol->setDataAt(i, 1) ;
			}
			else
			{
				segvol->setDataAt(i, 0) ;
			}
		}
		*/

		// Write to MRC
		//printf("Writing to %s...", mrcfile) ;
		//segvol->toMRCFile( mrcfile ) ;


		// Growing out one layer for display
		printf("Growing out one layer...\n") ;
		// First, fill current layer and assign values
		queue2->reset() ;
		queue3->reset() ;
		ele = queue3->getNext() ;
		while ( ele != NULL )
		{
			ox = ele->x ;
			oy = ele->y ;
			oz = ele->z ;
			double seg = segvol->getDataAt(ox,oy,oz) ;
			
			for ( int mx = -1 ; mx < 2 ; mx ++ )
				for ( int my = -1 ; my < 2 ; my ++ )
					for ( int mz = -1 ; mz < 2 ; mz ++ )
			{
				int nx = ox + mx ; // neighbor6[m][0] ;
				int ny = oy + my ; // neighbor6[m][1] ;
				int nz = oz + mz ; // neighbor6[m][2] ;
				if ( vvol->getDataAt( nx, ny, nz ) == 0 )
				{
					double ct = (int) tvol1->getDataAt(nx,ny,nz) ;
					double val = tvol2->getDataAt(nx,ny,nz) ;
					
					tvol1->setDataAt(nx,ny,nz, ct + 1 ) ;
					tvol2->setDataAt(nx,ny,nz, val + seg ) ;
					
					if (ct == 0)
					{
						queue2->prepend( nx, ny, nz ) ;
					}
				}
			}
			ele = queue3->remove() ;
		}
		
		// Normalize values of nodes in queue2
		queue2->reset() ;
		while ( ( ele = queue2->getNext() ) != NULL )
		{
			ox = ele->x ;
			oy = ele->y ;
			oz = ele->z ;
			
			double ct = tvol1->getDataAt(ox,oy,oz) ;
			double val = tvol2->getDataAt(ox,oy,oz) ;
			
			if ( ct == 0 )
			{
				printf("Wrong! %f\n", ct) ;
			}
			segvol->setDataAt(ox,oy,oz, val / ct ) ;
		}

		printf("Writing...") ;
		segvol->toMRCFile( mrcfile ) ;
		segvol->toMRCFile( "../colors.mrc" ) ;
		printf("Done.\n") ;
	
		printf("Segmentation...") ;
		for ( i = 0 ; i < sizex * sizey * sizez ; i ++ )
		{
			float segval = (float)segvol->getDataAt(i) ;
			// High values
			if ( segval > 1.5f )
			{
				tvol1->setDataAt(i, getDataAt(i) ) ;
			}
			else
			{
				tvol1->setDataAt(i, -1) ;
			}
			
			// Low values
			if ( segval < 1.5f && segval >= 1 )
			{
				tvol2->setDataAt(i, getDataAt(i) ) ;
			}
			else
			{
				tvol2->setDataAt(i, -1) ;
			}
		}
		char nname[1024] ;
		sprintf_s( nname, "%s_sheet.mrc", mrcfile ) ;
		tvol1->toMRCFile( nname ) ;
		sprintf_s( nname, "%s_helix.mrc", mrcfile ) ;
		tvol2->toMRCFile( nname ) ;
		printf("Done.\n") ;
		return ;

		/* Write to text
		if ( txtfile != NULL )
		{
			printf("Writing to %s...", txtfile) ;
			// Count border points
			queue->reset() ;
			for ( i = 0 ; i < sizex ; i ++ )
				for ( j = 0 ; j < sizey ; j ++ )
					for ( k = 0 ; k < sizez ; k ++ )
					{
						if ( getDataAt( i, j, k ) >= threshold )
						{
							for ( int m = 0 ; m < 6 ; m ++ )
							{
								if ( getDataAt( i + neighbor6[m][0], j + neighbor6[m][1], k + neighbor6[m][2] ) < threshold )
								{
									queue->prepend( i, j, k ) ;
									break ;
								}
							}
						}
					}

			FILE* fout = fopen( txtfile, "w" ) ;
			fprintf( fout, "%d\n", queue->getNumElements() ) ;
			queue->reset() ;
			while( (ele=queue->getNext()) != NULL )
			{
				ox = ele->x ;
				oy = ele->y ;
				oz = ele->z ;
				fprintf( fout, "%d %d %d %f\n", ox, oy, oz, segvol->getDataAt(ox,oy,oz) ) ;
			}
			fclose( fout ) ;
			printf("Done.\n") ;
		}
		*/
	}

	void floodFill( float thr )
	{
		int i;
		// First, threshold the volume
		printf("Thresholding the volume to 0/1...\n") ;
		threshold( thr, 0, 1 ) ;

		// Next, initialize queue
		Volume* tvol = new Volume( sizex, sizey, sizez, 0 ) ;
		GridQueue2* queue = new GridQueue2() ;
		gridQueueEle* ele ;
		queue->prepend( 0, 0, 0 ) ;
		tvol->setDataAt( 0,0,0, -1 ) ;

		// Iteration
		printf("Flood filling...\n") ;
		queue->reset() ;
		ele = queue->getNext() ;
		int ct = 1 ;
		while( ele != NULL )
		{
			int ox = ele->x ;
			int oy = ele->y ;
			int oz = ele->z ;
			queue->remove() ;

			for ( int m = 0 ; m < 6 ; m ++ )
			{
				int nx = ox + neighbor6[m][0] ;
				int ny = oy + neighbor6[m][1] ;
				int nz = oz + neighbor6[m][2] ;
				if ( nx < 0 || nx >= sizex || ny < 0 || ny >= sizey || nz < 0 || nz >= sizez )
				{
					continue ;
				}
				if ( tvol->getDataAt( nx, ny, nz ) == 0 && getDataAt( nx, ny, nz ) == 0 )
				{
					queue->prepend( nx, ny, nz ) ;
					tvol->setDataAt(nx,ny,nz, -1) ;
					ct ++ ;

					if ( ct % 100000 == 0 )
					{
						printf("%d nodes processed.\n", ct);
					} 
				}
			}

			queue->reset() ;
			ele = queue->getNext() ;
		}
		printf("Done.\n") ;
		
		// Done
		for ( i = 0 ; i < sizex*sizey*sizez ; i ++ )
		{
			this->setDataAt(i, tvol->getDataAt(i) ) ;
		}
	}

	void reduceComponent( int size )
	{
		int i, j, k ;
		Volume* tvol = new Volume( sizex, sizey, sizez, 0 ) ;
		GridQueue2* queue = new GridQueue2() ;
		GridQueue2* queue2 = new GridQueue2() ;
		gridQueueEle* ele ;
		int numC = 0, numBC = 0 ;

		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt(i,j,k) == 1 && tvol->getDataAt(i,j,k) == 0 )
					{
						numC ++ ;
						// Start searching for this component
						queue->prepend( i,j,k ) ;
						queue2->prepend( i,j,k ) ;
						tvol->setDataAt( i,j,k, 1 ) ;
						int ct = 1 ;

						queue->reset() ;
						ele = queue->getNext() ;
						while( ele != NULL )
						{
							int ox = ele->x ;
							int oy = ele->y ;
							int oz = ele->z ;
							queue->remove() ;
							
							for ( int m = 0 ; m < 6 ; m ++ )
							{
								int nx = ox + neighbor6[m][0] ;
								int ny = oy + neighbor6[m][1] ;
								int nz = oz + neighbor6[m][2] ;
								if ( nx < 0 || nx >= sizex  || ny < 0 || ny >= sizey || nz < 0 || nz >= sizez )
								{
									continue ;
								}
								if ( tvol->getDataAt( nx, ny, nz ) == 0 && getDataAt( nx, ny, nz ) == 1 )
								{
									queue->prepend( nx, ny, nz ) ;
									queue2->prepend( nx, ny, nz ) ;
									tvol->setDataAt( nx, ny, nz, 1 ) ;
									ct ++ ;
								}
							}

							queue->reset() ;
							ele = queue->getNext() ;
						}	
						
						if ( ct < size )
						{
							// remove component
							
							queue2->reset() ;
							ele = queue2->getNext() ;
							while ( ele != NULL )
							{
								setDataAt( ele->x, ele->y, ele->z, 0 ) ;
								ele = queue2->remove() ;
							}
							queue2->reset() ;
							
						}
						else
						{
							queue2 = new GridQueue2() ;
							numBC ++ ;
						}
					}
				}

		printf("Total number of components: %d Remained: %d\n", numC, numBC ) ;

	}

	void reduceComponent2( int num )
	{
		int i, j, k ;
		Volume* tvol = new Volume( sizex, sizey, sizez, 0 ) ;
		GridQueue2* queue = new GridQueue2() ;
		GridQueue2* queue2 = new GridQueue2() ;
		gridQueueEle* ele ;
		int numC = 0, numBC = 0 ;
		int* tops = new int[ num ] ;
		int* topinds = new int[ num ] ;
		for ( i = 0 ; i < num ; i ++ )
		{
			tops[i] = 0 ;
			topinds[i] = -1 ;
		}

		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt(i,j,k) == 1 && tvol->getDataAt(i,j,k) == 0 )
					{
						numC ++ ;
						// Start searching for this component
						queue->prepend( i,j,k ) ;
						queue2->prepend( i,j,k ) ;
						tvol->setDataAt( i,j,k, 1 ) ;
						int ct = 1 ;

						queue->reset() ;
						ele = queue->getNext() ;
						while( ele != NULL )
						{
							int ox = ele->x ;
							int oy = ele->y ;
							int oz = ele->z ;
							queue->remove() ;
							
							for ( int m = 0 ; m < 6 ; m ++ )
							{
								int nx = ox + neighbor6[m][0] ;
								int ny = oy + neighbor6[m][1] ;
								int nz = oz + neighbor6[m][2] ;
								if ( nx < 0 || nx >= sizex  || ny < 0 || ny >= sizey || nz < 0 || nz >= sizez )
								{
									continue ;
								}
								if ( tvol->getDataAt( nx, ny, nz ) == 0 && getDataAt( nx, ny, nz ) == 1 )
								{
									queue->prepend( nx, ny, nz ) ;
									queue2->prepend( nx, ny, nz ) ;
									tvol->setDataAt( nx, ny, nz, 1 ) ;
									ct ++ ;
								}
							}

							queue->reset() ;
							ele = queue->getNext() ;
						}	
						
						queue2->reset() ;
						ele = queue2->getNext() ;
						while ( ele != NULL )
						{
							ele = queue2->remove() ;
						}
						queue2->reset() ;

						for ( int ind = 0 ; ind < num ; ind ++ )
						{
							if ( ct > tops[ ind ] )
							{
								for ( int nind = num - 1 ; nind > ind ; nind -- )
								{
									tops[nind] = tops[ nind - 1 ] ;
									topinds[nind] = topinds[ nind - 1 ] ;
								}
								tops[ ind ] = ct ;
								topinds[ ind ] = numC ;
								break ;
							}
						}
					}
				}

		printf("%d components total, %d selected components:\n", numC, num) ;
		for ( i = 0 ; i < num ; i ++ )
		{
			printf("Id %d: size %d\n", topinds[i], tops[i] ) ;
		}

		tvol->fill( 0 ) ;

		numC = 0 ;
		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( getDataAt(i,j,k) == 1 && tvol->getDataAt(i,j,k) == 0 )
					{
						numC ++ ;
						// Start searching for this component
						queue->prepend( i,j,k ) ;
						queue2->prepend( i,j,k ) ;
						tvol->setDataAt( i,j,k, 1 ) ;
						int ct = 1 ;

						queue->reset() ;
						ele = queue->getNext() ;
						while( ele != NULL )
						{
							int ox = ele->x ;
							int oy = ele->y ;
							int oz = ele->z ;
							queue->remove() ;
							
							for ( int m = 0 ; m < 6 ; m ++ )
							{
								int nx = ox + neighbor6[m][0] ;
								int ny = oy + neighbor6[m][1] ;
								int nz = oz + neighbor6[m][2] ;
								if ( nx < 0 || nx >= sizex  || ny < 0 || ny >= sizey || nz < 0 || nz >= sizez )
								{
									continue ;
								}
								if ( tvol->getDataAt( nx, ny, nz ) == 0 && getDataAt( nx, ny, nz ) == 1 )
								{
									queue->prepend( nx, ny, nz ) ;
									queue2->prepend( nx, ny, nz ) ;
									tvol->setDataAt( nx, ny, nz, 1 ) ;
									ct ++ ;
								}
							}

							queue->reset() ;
							ele = queue->getNext() ;
						}	

						int removing = 1 ;
						for ( int ind = 0 ; ind < num ; ind ++ )
						{
							if ( topinds[ ind ] == numC )
							{
								removing = 0 ;
								break ;
							}
						}
						
						if ( removing )
						{
							// remove component
							
							queue2->reset() ;
							ele = queue2->getNext() ;
							while ( ele != NULL )
							{
								setDataAt( ele->x, ele->y, ele->z, 0 ) ;
								ele = queue2->remove() ;
							}
							queue2->reset() ;
							
						}
						else
						{
							queue2 = new GridQueue2() ;
							numBC ++ ;
						}
					}
				}

		printf("Total number of components: %d Remained: %d\n", numC, numBC ) ;
		delete tvol ;
	}

	void floodFillPQR( int offset )
	{
		int i;

		// Next, initialize queue
		GridQueue2* queue = new GridQueue2() ;
		gridQueueEle* ele ;
		queue->prepend( 0, 0, 0 ) ;

		// Iteration
		printf("Flood filling outside from  (0,0,0)...\n") ;
		int ct = 1 ;
		/*
		queue->reset() ;
		ele = queue->getNext() ;
		while( ele != NULL )
		{
			int ox = ele->x ;
			int oy = ele->y ;
			int oz = ele->z ;
			queue->remove() ;

			int boundary = 0 ;
			for ( int m = 0 ; m < 6 ; m ++ )
			{
				int nx = ox + neighbor6[m][0] ;
				int ny = oy + neighbor6[m][1] ;
				int nz = oz + neighbor6[m][2] ;
				if ( nx < 0 || nx >= sizex  || ny < 0 || ny >= sizey || nz < 0 || nz >= sizez )
				{
					continue ;
				}
				if ( tvol->getDataAt( nx, ny, nz ) == 0 && getDataAt( nx, ny, nz ) == 0 )
				{
					queue->prepend( nx, ny, nz ) ;
					tvol->setDataAt( nx, ny, nz, 1 ) ;
					ct ++ ;
					if ( ct % 100000 == 0 )
					{
						printf("%d nodes processed.\n", ct);
					} 
				}
				else if ( getDataAt( nx, ny, nz ) == 1 )
				{
					boundary = 1 ;
				}
			}

			if ( boundary )
			{
				tvol->setDataAt( ox, oy, oz, 2 ) ;
			}

			queue->reset() ;
			ele = queue->getNext() ;
		}
		printf("Done.\n") ;
		for ( i = 0 ; i < sizex*sizey*sizez ; i ++ )
		{

			if ( tvol->getDataAt(i) == 2 )
			{
				setDataAt( i, 1 ) ;
			}
		}
		*/

		
		// Find inside seed point
		int maxRounds = 1 ;
		for ( int rounds = 0 ; rounds < maxRounds ; rounds ++ )
		{

		int isSolid = 0 ;
		for ( i = 0 ; i < sizex ; i ++ )
		{
			if ( getDataAt( i, sizey / 2, sizez / 2 ) == 1 ) 
			{
				isSolid = 1 ;
			}
			else if ( isSolid == 1 )
			{
				break ;
			}
		}

		Volume* invol = new Volume( sizex, sizey, sizez, 0 ) ;
		queue->prepend( i, sizey/2, sizez/2 ) ;
		invol->setDataAt( i, sizey/2, sizez/2, 1 ) ;
		printf("Flood filling inside from  (%d,%d,%d)...\n",i, sizey/2, sizez/2) ;

		// Iteration
		queue->reset() ;
		ele = queue->getNext() ;
		ct = 1 ;
		while( ele != NULL )
		{
			int ox = ele->x ;
			int oy = ele->y ;
			int oz = ele->z ;
			queue->remove() ;

			int boundary = 0 ;
			for ( int m = 0 ; m < 6 ; m ++ )
			{
				int nx = ox + neighbor6[m][0] ;
				int ny = oy + neighbor6[m][1] ;
				int nz = oz + neighbor6[m][2] ;
				if ( nx < 0 || nx >= sizex  || ny < 0 || ny >= sizey || nz < 0 || nz >= sizez )
				{
					continue ;
				}
				if ( invol->getDataAt( nx, ny, nz ) == 0 && getDataAt( nx, ny, nz ) == 0 )
				{
					queue->prepend( nx, ny, nz ) ;
					invol->setDataAt( nx, ny, nz, 1 ) ;
					ct ++ ;
					if ( ct % 100000 == 0 )
					{
						printf("%d nodes processed.\n", ct);
					} 
				}
				else if ( getDataAt( nx, ny, nz ) == 1 )
				{
					boundary = 1 ;
				}
			}

			if ( boundary )
			{
				invol->setDataAt( ox, oy, oz, 2 ) ;
			}

			queue->reset() ;
			ele = queue->getNext() ;
		}
		printf("Done.\n") ;
		
		// Done
		for ( i = 0 ; i < sizex*sizey*sizez ; i ++ )
		{
/*
			if ( tvol->getDataAt(i) == 2 )
			{
				setDataAt( i, 1 ) ;
			}
			*/
			if ( invol->getDataAt(i) == 2 )
			{
				setDataAt( i, 1 ) ;
			}
			
/*
			else if ( tvol->getDataAt(i) == 0 && invol->getDataAt(i) == 0 )
			{
				setDataAt( i, 1 ) ;
			}
*/			
		}

				delete invol ;

		}

//		delete tvol ;

}


	void writeDistances( char* fname, int maxDis )
	{
		int i, j, k ;
		Volume* testvol = NULL ;
		Volume* disvol = new Volume( getSizeX(), getSizeY(), getSizeZ() ) ;
		printf("Writing distance transform to %d levels.\n", maxDis);

		int totNodes = 0 ;
		int size = getSizeX() * getSizeY() * getSizeZ() ;
		float score = 10 ;
		for ( i = maxDis ; i >= 0 ; i -- )
		{
			int nodes = 0 ;
			testvol = new Volume( getSizeX(), getSizeY(), getSizeZ(), 0, 0, 0, this ) ;
			testvol->erodeSheet( i ) ;
			
			for ( j = 0 ; j < size ; j ++ )
			{
				if ( disvol->getDataAt(j) == 0 && testvol->getDataAt(j) > 0 )
				{
					disvol->setDataAt( j, i + 1 ) ;
					nodes ++ ;
				}
			}
			printf("Level %d has %d nodes.\n", i, nodes );
			totNodes += nodes ;
			score -= 0.5f ;
			delete testvol ;
		}
		printf("Totally %d nodes.\n", totNodes );

		//disvol->toMRCFile( "..\distance.mrc" ) ;
		FILE* fout = fopen( fname, "w" ) ;
		fprintf( fout, "%d\n", totNodes ) ;
		int ct = 0 ;
		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					float val = (float)disvol->getDataAt(i,j,k) ;
					if ( val > 0 )
					{
						fprintf( fout, "%d %d %d %f\n", i, j, k, val ) ;
						ct ++ ;
					}
				}
					
		if ( ct != totNodes )
		{
			printf("Counting wrong! %d %d\n", totNodes, ct) ;
		}
		fclose( fout ) ;

	}

	void toPQRFile( char* fname, float spc, float minx, float miny, float minz, int padding )
	{
		FILE* fout = fopen( fname, "w" ) ;
		int i, j, k ;

		for ( i = 0 ; i < sizex ; i ++ )
			for ( j = 0 ; j < sizey ; j ++ )
				for ( k = 0 ; k < sizez ; k ++ )
				{
					if ( i < padding || i >= sizex - padding || j < padding || j >= sizey - padding || k < padding || k >= sizez - padding )
					{
						continue ;
					}
					float val = (float)this->getDataAt(i,j,k) ;
					if ( val > 0 )
					{
						float x = (i - padding) * spc + minx ;
						float y = (j - padding) * spc + miny ; 
						float z = (k - padding) * spc + minz ;
						fprintf( fout, "ATOM      1  X   DUM     1     %4.3f %4.3f %4.3f 0.000 0.000\n", x, y, z ) ;
					}
				}

		fclose( fout ) ;
	}

	void toMRCFile( char* fname )
	{
		FILE* fout = fopen( fname, "wb" ) ;

		// Write header
		fwrite( &sizex, sizeof( int ), 1, fout ) ;
		fwrite( &sizey, sizeof( int ), 1, fout ) ;
		fwrite( &sizez, sizeof( int ), 1, fout ) ;

		int mode = 2 ;
		fwrite( &mode, sizeof ( int ), 1, fout ) ;
		
		int off[3] = {0,0,0} ;
		int intv[3] = { sizex - 1, sizey - 1, sizez - 1 } ;
		fwrite( off, sizeof( int ), 3, fout ) ;
		fwrite( intv, sizeof( int ), 3, fout ) ;

		float cella[3] = {2,2,2} ;
		float cellb[3] = {90,90,90} ;
		fwrite( cella, sizeof( float ), 3, fout ) ;
		fwrite( cellb, sizeof( float ), 3, fout ) ;

		int cols[3] = {1,2,3} ;
		fwrite( cols, sizeof( int ), 3, fout ) ;

		double dmin = 100000, dmax = -100000 ;
		for ( int i = 0 ; i < sizex * sizey * sizez ; i ++ )
		{
			if ( data[ i ] < dmin )
			{
				dmin = data[ i ] ;
			}
			if ( data[i] > dmax )
			{
				dmax = data[ i ] ;
			}
		}
		float ds[3] = {(float)dmin, (float)dmax, (float)0} ;
		fwrite( ds, sizeof( float ), 3, fout ) ;

		int zero = 0 ;
		for (int i = 22 ; i < 256 ; i ++ )
		{
			fwrite( &zero, sizeof( int ), 1, fout ) ;
		}

		// Write contents
		for ( int z = 0 ; z < sizez ; z ++ )
			for ( int y = 0 ; y < sizey ; y ++ )
				for ( int x = 0 ; x < sizex ; x ++ )
				{
					float d = (float)getDataAt(x,y,z) ;
					fwrite( &d, sizeof( float ), 1, fout ) ;
				}

		fclose( fout ) ;
	}

/*	void toMathematicaFile(char * fname) {
		FILE* fout = fopen( fname, "wb" ) ;

		fprintf(fout, "(");
		for ( int z = 0 ; z < sizez ; z ++ ) {
			fprintf(fout, "(");
			for ( int y = 0 ; y < sizey ; y ++ ) {
				fprintf(fout, "( ");
				for ( int x = 0 ; x < sizex ; x ++ ) {
					float d = (float)getDataAt(x,y,z) ;
					fprintf(fout, "%d", d);
					if(x != sizex-1) {
						fprintf(fout, ", ");
					}
				}
				if(y != sizey-1) {
					fprintf(fout, "}, \n");
				} else {
					fprintf(fout, "}\n");
				}
			}
			if(z != sizez-1) {
				fprintf(fout, "}, \n\n\n");
			} else {
				fprintf(fout, "}\n\n\n");
			}
		}
		fprintf(fout, "}\n");

		fclose(fout);
	}
private:

	/* Sizes */
	int sizex, sizey, sizez ;

	/* Data array */
	float * data ;
};


#endif