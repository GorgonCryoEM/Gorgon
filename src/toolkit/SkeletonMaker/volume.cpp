#include "volume.h"
#include <Foundation/StringUtils.h>
#include <GraySkeletonCPP/GlobalDefinitions.h>
#include <GraySkeletonCPP/VolumeSkeletonizer.h>
#include <SkeletonMaker/reader.h>

#include <fstream>
#include <iomanip>

using namespace SkeletonMaker;
using namespace Foundation;
using namespace GraySkeletonCPP;


#ifdef GORGON_DEBUG
    int Volume::id0=0;
    int Volume::id1=0;
    int Volume::id3=0;
#endif

//    ---------------------

void Volume::setVolume(Volume *vol) {
	*this = *vol;
}
//-----------------------------------

Volume::Volume(const Volume& obj)
      : VolumeData(static_cast<VolumeData>(obj)), histogram(obj.histogram),
        volData(dynamic_cast<VolumeData *>(this))
{
  #ifdef GORGON_DEBUG
        cout<<"\033[32mDEBUG: File:   volume.cpp"<<endl;
        cout<<"DEBUG: Method: Volume::Volume(const Volume&)\033[0m"<<endl;
        cout<<"Id1: "<<id1<<endl;
        id1++;

        cout<<"\033[35mobj.size: "<<obj.getSize()<<endl;
        cout<<"this->size: "<<this->getSize()<<"\033[0m"<<endl;
  #endif

}

Volume::Volume()
      : VolumeData(), volData(getVolumeData())
{
#ifdef GORGON_DEBUG
        cout<<"\033[32mDEBUG: File:   volume.h"<<endl;
        cout<<"DEBUG: Method: Volume::Volume()\033[0m"<<endl;
        cout<<"Id0: "<<id0<<endl;
        id0++;
  #endif
}

Volume::Volume(int x, int y, int z, float val)
      : VolumeData(x, y, z, val), volData(getVolumeData())
{
  #ifdef GORGON_DEBUG
        cout<<"\033[32mDEBUG: File:   volume.cpp"<<endl;
        cout<<"DEBUG: Method: Volume::Volume(int, int, int, float)\033[0m"<<endl;
        cout<<"Id3: "<<id3<<endl;
        id3++;
  #endif

}


Volume::~Volume( )
{}

VolumeData * Volume::getVolumeData() {
    return dynamic_cast<VolumeData *>(this);
}




int Volume::getNonZeroVoxelCount() {
    int count = 0;
    for(int x = 0; x < getSizeX(); x++){
        for(int y = 0; y < getSizeY(); y++){
            for(int z = 0; z < getSizeZ(); z++){
                if(this->getDataAt(x, y, z) > 0.0) {
                    count++;
                }
            }
        }
    }
    return count;
}
void Volume::print() {
    for(int x = 0; x < getSizeX(); x++) {
        printf("{ ");
        for(int y = 0; y < getSizeY(); y++) {
            printf("{ ");
            for(int z = 0; z < getSizeZ(); z++) {
                printf("%f, ", getDataAt(x, y, z));
            }
            printf("} ");
        }
        printf("} ");
    }
    printf("\n");
}


void Volume::subtract( Volume* vol ) {
    int i, j, k ;
    for ( i = 0 ; i < getSizeX() ; i ++ )
        for ( j = 0 ; j < getSizeY() ; j ++ )
            for ( k = 0 ; k < getSizeZ() ; k ++ ) {
                if ( getDataAt( i, j, k ) > 0 ) {
                    if ( vol->getDataAt(i,j,k) > 0 ) {
                        setDataAt( i, j, k, 0 ) ;
                    }
                }
            }

}

void Volume::applyMask(Volume * maskVol, double maskValue, bool keepMaskValue) {
    for(int x = 0; x < maskVol->getSizeX(); x++) {
        for(int y = 0; y < maskVol->getSizeY(); y++) {
            for(int z = 0; z < maskVol->getSizeZ(); z++) {
                if(((maskVol->getDataAt(x, y, z) == maskValue) && !keepMaskValue) ||
                    ((maskVol->getDataAt(x, y, z) != maskValue) && keepMaskValue)) {
                    setDataAt(x, y, z, 0);
                }
            }
        }
    }
}

double Volume::getMin() const {
    int size = volData->getMaxIndex();
    double rvalue = volData->getDataAt(0);
    for (int i=1; i < size; i++) {
        float val = volData->getDataAt(i);
        if ( rvalue > val) {
            rvalue = val;
        }
    }
    return rvalue;
}

double Volume::getMax() const {
    int size = volData->getMaxIndex();
    double rvalue = volData->getDataAt(0);
    for (int i=1; i < size; i++) {
        float val = volData->getDataAt(i);
        if ( rvalue < val) {
            rvalue = val;
        }
    }
    return rvalue ;
}

double Volume::getMaxValuePosition(int& maxX, int& maxY, int& maxZ) {
    double maxVal = getDataAt(0,0,0);
    maxX = 0; maxY = 0; maxZ = 0;
    double data;

    for(int x = 0; x < getSizeX(); x++) {
        for(int y = 0; y < getSizeY(); y++) {
            for(int z = 0; z < getSizeZ(); z++) {
                data = getDataAt(x, y, z);
                if(data > maxVal) {
                    maxVal = data;
                    maxX = x; maxY = y; maxZ = z;
                }
            }
        }
    }
    return maxVal;
}

int Volume::isHelix( int ox, int oy, int oz ) {
    int cn = 12 ;
    int nx, ny, nz ;
    int i, j, k ;

    double vox[3][3][3] ;
    for ( i = -1 ; i < 2 ; i ++ )
        for ( j = -1 ; j < 2; j ++ )
            for ( k = -1 ; k < 2 ; k ++ ) {
                vox[i+1][j+1][k+1] = getDataAt( ox + i, oy + j, oz + k ) ;
            }

    for ( i = 0 ; i < 12 ; i ++ ) {
        for ( j = 0 ; j < 4 ; j ++ ) {
            nx = sheetNeighbor[i][j][0] + 1;
            ny = sheetNeighbor[i][j][1] + 1;
            nz = sheetNeighbor[i][j][2] + 1;

            if ( vox[nx][ny][nz] <= 0 ) {
                cn -- ;
                break ;
            }
        }
    }

    if ( cn >= 1 ) {
        return 0 ;
    } else {
        return 1 ;
    }
}

Volume * Volume::getHelices( int minSize ) {
    printf("Segmenting helices from eroded volume.\n") ;
    int i, j, k ;

    //Initialize volume
    printf("Initialize volume at %d %d %d\n", getSizeX(), getSizeY(), getSizeZ() ) ;
    Volume* svol = new Volume( getSizeX(), getSizeY(), getSizeZ() ) ;

    //Initialize cluster counters
    int helices[MAX_SHEETS] ;
    for ( i = 0 ; i < MAX_SHEETS ; i ++ ) {
        helices[ i ] = 0 ;
    }
    int totHelices = 1 ;

    //Start clustering
    printf("Start clustering...\n" ) ;
    int ox, oy, oz ;
    for ( i = 0 ; i < getSizeX() ; i ++ )
        for ( j = 0 ; j < getSizeY() ; j ++ )
            for ( k = 0 ; k < getSizeZ() ; k ++ ) {
                if ( getDataAt(i,j,k) <= 0 || svol->getDataAt(i,j,k) != 0 ) {
                    // Not a data point or has been visited
                    continue ;
                }
                if ( ! isHelix( i, j, k ) ) {
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
                    if ( isHelix( ox, oy, oz ) ) {
                        for ( int m = 0 ; m < 6 ; m ++ ) {
                            int nx = ox + neighbor6[m][0] ;
                            int ny = oy + neighbor6[m][1] ;
                            int nz = oz + neighbor6[m][2] ;

                            if ( getDataAt(nx,ny,nz) > 0 && svol->getDataAt(nx,ny,nz) == 0 ) {
                                svol->setDataAt(nx,ny,nz,totHelices);
                                queue->pushQueue(nx,ny,nz) ;
                                numNodes ++ ;
                            }
                        }
                    }
                }

                delete queue ;
                if ( numNodes > 0 ) {
                //	printf("Helix %d contain %d nodes.\n", totHelices, numNodes) ;
                    helices[ totHelices ] = numNodes ;
                    totHelices ++ ;
                }
            }

    // Removing clusters less than minSize
    printf("Removing small clusters.\n") ;
    for ( i = 0 ; i < getSizeX() ; i ++ )
        for ( j = 0 ; j < getSizeY() ; j ++ )
            for ( k = 0 ; k < getSizeZ() ; k ++ ) {
                int cnt = (int) svol->getDataAt(i,j,k) ;
                if ( cnt > 0 && helices[ cnt ] < minSize ) {
                    svol->setDataAt(i,j,k,-1) ;
                }
            }

    // Finally, clean up
    #ifdef VERBOSE
    printf("Thresholding the volume to 0/1...\n") ;
    #endif
    svol->threshold( 0.1, 0, 1 ) ;

    return svol ;

}

int Volume::isEndPoint( int ox, int oy, int oz ) {
    if ( getDataAt( ox - 1, oy, oz ) < 0 && getDataAt( ox + 1, oy, oz ) < 0 ) {
        return 1 ;
    }
    if ( getDataAt( ox, oy - 1, oz ) < 0 && getDataAt( ox, oy + 1, oz ) < 0 ) {
        return 1 ;
    }
    if ( getDataAt( ox, oy, oz - 1 ) < 0 && getDataAt( ox, oy, oz + 1 ) < 0 ) {
        return 1 ;
    }
    return 0 ;
}

int Volume::getNumNeighbor6( int ox, int oy, int oz ) {
    int rvalue = 0 ;
    for ( int i = 0 ; i < 6 ; i ++ ) {
        int nx = ox + neighbor6[i][0] ;
        int ny = oy + neighbor6[i][1] ;
        int nz = oz + neighbor6[i][2] ;
        if ( getDataAt( nx, ny, nz ) >= 0 ) {
            rvalue ++ ;
        }
    }

    return rvalue ;
}

int Volume::isInternal2( int ox, int oy, int oz ) {
    // assuming it's -1/0 volume
    int i, j, k ;

    for ( i = -1 ; i < 2 ; i ++ )
        for ( j = -1 ; j < 2 ; j ++ )
            for ( k = -1 ; k < 2 ; k ++ ) {
                if ( getDataAt( ox + i, oy + j, oz + k ) < 0 ) {
                    return 0 ;
                }
            }

    return 1 ;
}

int Volume::hasCell( int ox, int oy, int oz ) {
    for ( int i = 0 ; i < 2 ; i ++ )
        for ( int j = 0 ; j < 2 ; j ++ )
            for ( int k = 0 ; k < 2 ; k ++ ) {
                if ( getDataAt( ox + i, oy + j, oz + k ) < 0 ) {
                    return 0 ;
                }
            }
    return 1 ;
}

Volume * Volume::markCellFace( ) {
    int i, j, k ;
    Volume* fvol = new Volume( getSizeX(), getSizeY(), getSizeZ() ) ;

    //return fvol ;

    for ( i = 0 ; i < getSizeX() ; i ++ )
        for ( j = 0 ; j < getSizeY() ; j ++ )
            for ( k = 0 ; k < getSizeZ() ; k ++ )
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

int Volume::hasCompleteSheet( int ox, int oy, int oz ) {
    // Returns 1 if it lies in the middle of a sheet
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

int Volume::hasCompleteHelix( int ox, int oy, int oz )
{
    // Returns 1 if it has a complete helix
    int i ;
    int c1 = 0;
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

int Volume::isEdgeEnd( int ox, int oy, int oz )
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

int Volume::isFaceEnd( int ox, int oy, int oz )
{
    // return isSheetEnd(ox,oy,oz) ;

    int i, j, k ;
    int nx, ny, nz ;

    double vox[3][3][3] ;
    for ( i = -1 ; i < 2 ; i ++ )
        for ( j = -1 ; j < 2 ; j ++ )
            for ( k = -1 ; k < 2 ; k ++ )
            {
                vox[ i + 1 ][ j + 1 ][ k + 1 ] = getDataAt( ox + i, oy + j, oz + k ) ;
            }

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

int Volume::isNoise( int ox, int oy, int oz, int noise )
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

int Volume::isHelixEnd( int ox, int oy, int oz ) {

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
            if ( getNumNeighbor6(nx,ny,nz) < 6 ) // if ( val > 0 && val < MAX_ERODE )
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

int Volume::isFeatureFace( int ox, int oy, int oz )
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
            else if ( getNumNeighbor6( nx, ny, nz ) == 6 )
            {
                ct = -1 ;
                break ;
            }
//				else if ( getDataAt( nx, ny, nz ) == 0 )
//				{
//					ct ++ ;
//				}


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


int Volume::isSheetEnd( int ox, int oy, int oz )
{
    return ( ( hasCompleteSheet(ox,oy,oz) == 0 ) && isFeatureFace(ox,oy,oz) ) ;

    //// return testIsSheetEnd( ox, oy, oz ) ;
    //
    //int i, j, k ;
    //int nx, ny, nz ;

    //double vox[3][3][3] ;
    //for ( i = -1 ; i < 2 ; i ++ )
    //	for ( j = -1 ; j < 2 ; j ++ )
    //		for ( k = -1 ; k < 2 ; k ++ )
    //		{
    //			vox[ i + 1 ][ j + 1 ][ k + 1 ] = getDataAt( ox + i, oy + j, oz + k ) ;
    //		}

    //int edge[6] = { 4,4,4,4,4,4 } ;
    //int edge2[6] = { 4,4,4,4,4,4 } ;

    //for ( i = 0 ; i < 12 ; i ++ )
    //{
    //	for ( j = 0 ; j < 4 ; j ++ )
    //	{
    //		nx = 1 + sheetNeighbor[i][j][0] ;
    //		ny = 1 + sheetNeighbor[i][j][1] ;
    //		nz = 1 + sheetNeighbor[i][j][2] ;

    //		if ( vox[nx][ny][nz] < 0 )
    //		{
    //			edge[ faceEdges[ i ][ 0 ] ] -- ;
    //			edge[ faceEdges[ i ][ 1 ] ] -- ;
    //			break ;
    //		}
    //	}

    //	for ( j = 0 ; j < 4 ; j ++ )
    //	{
    //		nx = 1 + sheetNeighbor[i][j][0] ;
    //		ny = 1 + sheetNeighbor[i][j][1] ;
    //		nz = 1 + sheetNeighbor[i][j][2] ;

    //		if ( vox[nx][ny][nz] <= 0 )
    //		{
    //			edge2[ faceEdges[ i ][ 0 ] ] -- ;
    //			edge2[ faceEdges[ i ][ 1 ] ] -- ;
    //			break ;
    //		}
    //	}
    //}

    //
    ///*
    //for ( i = 0 ; i < 6 ; i ++ )
    //{
    //	nx = 1 + neighbor6[i][0] ;
    //	ny = 1 + neighbor6[i][1] ;
    //	nz = 1 + neighbor6[i][2] ;
    //	if ( edge[i] == 0 && vox[nx][ny][nz] >= 0 )
    //	{
    //		return 0 ;
    //	}
    //}
    //*/
    //


    //for ( i = 0 ; i < 6 ; i ++ )
    //{
    //	if ( edge[ i ] == 1 && edge2[ i ] == 1 )
    //	{
    //		return 1 ;
    //	}
    //}

    //return 0 ;
}

int Volume::isSimple( int ox, int oy, int oz )
{
    /* Test if this is a simple voxel */
    // int flag = 0 ;
    double vox[3][3][3] ;

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


int Volume::isPiercable( int ox, int oy, int oz )
{
    /* Test if this is a simple voxel */
    // int flag = 0 ;
    double vox[3][3][3] ;

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

    if ( countInt( vox ) == 1 && countExt( vox ) != 1 )
    {
        return 1 ;
    }
    else
    {
        return 0 ;
    }
}


int Volume::isSimple2( int v[3][3][3] )
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


int Volume::getNumPotComplex( int ox, int oy, int oz )
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

int Volume::getNumPotComplex2( int ox, int oy, int oz )
{
    return getNumPotComplex( ox, oy, oz ) ;

    //int i, j, k ;
    //double val = getDataAt( ox, oy, oz ) ;
    //if ( val <= 0 )
    //{
    //	return 0 ;
    //}

    //int rvalue = 0, nx, ny, nz ;
    //setDataAt( ox, oy, oz, -val ) ;

    //for ( i = -1 ; i < 2 ; i ++ )
    //	for ( j = -1 ; j < 2 ; j ++ )
    //		for ( k = -1 ; k < 2 ; k ++ )
    //		{
    //			nx = ox + i ;
    //			ny = oy + j ;
    //			nz = oz + k ;
    //			if ( getDataAt( nx, ny, nz ) == val )
    //			{
    //				if ( isHelixEnd( nx, ny, nz) || ! isSimple ( nx, ny, nz ) )
    //				{
    //					rvalue ++ ;
    //				}
    //			}
    //		}

    //setDataAt( ox, oy, oz, val ) ;

    //return rvalue + getNumNeighbor6( ox, oy, oz ) * 30 ;
}

int Volume::getNumNeighbor( int ox, int oy, int oz )
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

int Volume::components6( int vox[3][3][3] )
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
int Volume::components26( int vox[3][3][3] )
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

int Volume::countExt( double vox[3][3][3] )
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

int Volume::countInt( double vox[3][3][3] )
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

int Volume::countIntEuler( int ox, int oy, int oz )
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

/* Thin the current volume while preserving voxels with values > highthr or <= lowthr in grayvol
*  Assuming the current volume has already been thresholded to 0/1
*/
void Volume::curveSkeleton( Volume* grayvol, float lowthr, float highthr, Volume* svol )
{
    int i, j, k ;
    // First, threshold the volume
    #ifdef VERBOSE
    printf("Thresholding the volume to -1/0...\n") ;
    #endif
    threshold( 0.5f, -1, 0 ) ;

    // Next, apply convergent erosion
    // by preserving: complex nodes, curve end-points, and sheet points

    // Next, initialize the linked queue
    #ifdef VERBOSE
    printf("Initializing queue...\n") ;
    #endif
    GridQueue2* queue2 = new GridQueue2( ) ;
    GridQueue2* queue3 = new GridQueue2( ) ;
    GridQueue2* queue4 = new GridQueue2( ) ;
    PriorityQueue <gridPoint,int> * queue = new PriorityQueue <gridPoint,int> ( MAX_QUEUELEN );

    for ( i = 0 ; i < getSizeX() ; i ++ )
        for ( j = 0 ; j < getSizeY() ; j ++ )
            for ( k = 0 ; k < getSizeZ() ; k ++ )
            {
                if ( getDataAt( i, j, k ) >= 0 )
                {
                    float v = (float)grayvol->getDataAt(i,j,k) ;
                    if ( v <= lowthr || v > highthr || svol->getDataAt(i,j,k) > 0 )
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
    int wid = MAX_ERODE ;
    #ifdef VERBOSE
    printf("Total %d nodes\n", queue2->getNumElements() ) ;
    printf("Start erosion to %d...\n", wid) ;
    #endif


    // Perform erosion
    gridQueueEle* ele ;
    gridPoint* gp ;
    int ox, oy, oz ;
    int score ;
    Volume* scrvol = new Volume( this->getSizeX() , this->getSizeY(), this->getSizeZ() ) ;
    for ( i = 0 ; i < getSizeX() * getSizeY() * getSizeZ() ; i ++ )
    {
        scrvol->setDataAt( i, -1 ) ;
    }

#ifdef  NOISE_DIS_HELIX
    Volume* noisevol = new Volume( getSizeX(), getSizeY(), getSizeZ() ) ;
#endif

    for ( int curwid = 1 ; curwid <= wid ; curwid ++ )
    {
        // At the start of each iteration,
        // queue2 holds all the nodes for this layer
        // queue3 and queue are empty

        int numComplex = 0, numSimple = 0 ;
        #ifdef VERBOSE
        printf("Processing %d nodes in layer %d\n", queue2->getNumElements(), curwid) ;
        #endif

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

        #ifdef VERBOSE
        printf("%d complex, %d simple\n", numComplex, numSimple) ;
        #endif

        if ( numSimple == 0 )
        {
            if ( queue2->getNumElements() > 0 )
            {
                printf("*************************wierd here*************************\n");
            }
                break ;
        }
    }

    // Remove all internal voxels (contained in manifold surfaces)
    queue2->reset() ;
    queue4->reset() ;
    ele = queue4->getNext() ;
    while ( ele != NULL )
    {
        ox = ele->x ;
        oy = ele->y ;
        oz = ele->z ;

        if ( isPiercable(ox,oy,oz) == 1 )  // hasCompleteSheet( ox, oy, oz ) == 1 ) //
        {
            queue2->prepend(ox,oy,oz) ;
        //	setDataAt( ox, oy, oz, -1 ) ;
        }
        ele = queue4->remove() ;
    }

    for ( i = 0 ; i < getSizeX() ; i ++ )
        for ( j = 0 ; j < getSizeY() ; j ++ )
            for ( k = 0 ; k < getSizeZ() ; k ++ )
            {
                if ( getDataAt( i, j, k ) == 0 && isPiercable(i,j,k) ) //hasCompleteSheet(i,j,k) == 1) //
                {
                    queue2->prepend( i, j, k ) ;
                }
            }
    queue2->reset() ;
    ele = queue2->getNext() ;
    while ( ele != NULL )
    {
        ox = ele->x ;
        oy = ele->y ;
        oz = ele->z ;
        setDataAt( ox, oy, oz, -1 ) ;
        ele = queue2->remove() ;
    }


    // Finally, clean up
    delete scrvol;
    delete queue;
    delete queue2;
    delete queue3;
    delete queue4;
    #ifdef VERBOSE
    printf("Thresholding the volume to 0/1...\n") ;
    #endif
    threshold( 0, 0, 1 ) ;
}

// Compute curve skeleton
void Volume::curveSkeleton( float thr, Volume* svol )
{
    int i, j, k ;
    // First, threshold the volume
    #ifdef VERBOSE
    printf("Thresholding the volume to -1/0...\n") ;
    #endif
    threshold( thr, -1, 0 ) ;

    // Next, apply convergent erosion
    // by preserving: complex nodes, curve end-points, and sheet points

    // Next, initialize the linked queue
    #ifdef VERBOSE
    printf("Initializing queue...\n") ;
    #endif
    GridQueue2* queue2 = new GridQueue2( ) ;
    GridQueue2* queue3 = new GridQueue2( ) ;
    GridQueue2* queue4 = new GridQueue2( ) ;
    PriorityQueue <gridPoint,int> * queue = new PriorityQueue <gridPoint,int> ( MAX_QUEUELEN );

    for ( i = 0 ; i < getSizeX() ; i ++ )
        for ( j = 0 ; j < getSizeY() ; j ++ )
            for ( k = 0 ; k < getSizeZ() ; k ++ )
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

    int wid = MAX_ERODE ;
    #ifdef VERBOSE
    printf("Total %d nodes\n", queue2->getNumElements() ) ;
    printf("Start erosion to %d...\n", wid) ;
    #endif


    // Perform erosion
    gridQueueEle* ele ;
    gridPoint* gp ;
    int ox, oy, oz ;
    int score ;
    Volume* scrvol = new Volume( this->getSizeX() , this->getSizeY(), this->getSizeZ() ) ;
    for ( i = 0 ; i < getSizeX() * getSizeY() * getSizeZ() ; i ++ )
    {
        scrvol->setDataAt( i, -1 ) ;
    }

#ifdef  NOISE_DIS_HELIX
    Volume* noisevol = new Volume( getSizeX(), getSizeY(), getSizeZ() ) ;
#endif

    for ( int curwid = 1 ; curwid <= wid ; curwid ++ )
    {
        // At the start of each iteration,
        // queue2 holds all the nodes for this layer
        // queue3 and queue are empty

        int numComplex = 0, numSimple = 0 ;
        #ifdef VERBOSE
        printf("Processing %d nodes in layer %d\n", queue2->getNumElements(), curwid) ;
        #endif

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

        #ifdef VERBOSE
        printf("%d complex, %d simple\n", numComplex, numSimple) ;
        #endif

        if ( numSimple == 0 )
        {
                break ;
        }
    }

    // Finally, clean up
    delete scrvol;
    delete queue;
    delete queue2;
    delete queue3;
    delete queue4;
    #ifdef VERBOSE
    printf("Thresholding the volume to 0/1...\n") ;
    #endif
    threshold( 0, 0, 1 ) ;
}

// Compute curve skeleton in 2D
void Volume::curveSkeleton2D( float thr, Volume* svol )
{
    int i, j, k ;
    // First, threshold the volume
    #ifdef VERBOSE
    printf("Thresholding the volume to -1/0...\n") ;
    #endif
    threshold( thr, -1, 0 ) ;

    // Next, apply convergent erosion
    // by preserving: complex nodes, curve end-points, and sheet points

    // Next, initialize the linked queue
    #ifdef VERBOSE
    printf("Initializing queue...\n") ;
    #endif
    GridQueue2* queue2 = new GridQueue2( ) ;
    GridQueue2* queue3 = new GridQueue2( ) ;
    GridQueue2* queue4 = new GridQueue2( ) ;
    PriorityQueue <gridPoint,int> * queue = new PriorityQueue <gridPoint,int> ( MAX_QUEUELEN );

    for ( i = 0 ; i < getSizeX() ; i ++ )
        for ( j = 0 ; j < getSizeY() ; j ++ )
            for ( k = 0 ; k < getSizeZ() ; k ++ )
            {
                if ( getDataAt( i, j, k ) >= 0 )
                {
                    if ( svol->getDataAt(i,j,k) > 0 )
                    {
                        setDataAt( i, j, k, MAX_ERODE ) ;
                    }
                    else
                    {
                        for ( int m = 0 ; m < 4 ; m ++ )
                        {
                            if ( getDataAt( i + neighbor4[m][0], j + neighbor4[m][1], k ) < 0 )
                            {
                                // setDataAt( i, j, k, 1 ) ;
                                queue2->prepend( i, j, k ) ;
                                break ;
                            }
                        }
                    }
                }
            }
    int wid = MAX_ERODE ;
    #ifdef VERBOSE
    printf("Total %d nodes\n", queue2->getNumElements() ) ;
    printf("Start erosion to %d...\n", wid) ;
    #endif


    // Perform erosion
    gridQueueEle* ele ;
    gridPoint* gp ;
    int ox, oy, oz ;
    int score ;
    Volume* scrvol = new Volume( this->getSizeX() , this->getSizeY(), this->getSizeZ() ) ;
    for ( i = 0 ; i < getSizeX() * getSizeY() * getSizeZ() ; i ++ )
    {
        scrvol->setDataAt( i, -1 ) ;
    }

#ifdef  NOISE_DIS_HELIX
    Volume* noisevol = new Volume( getSizeX(), getSizeY(), getSizeZ() ) ;
#endif

    for ( int curwid = 1 ; curwid <= wid ; curwid ++ )
    {
        // At the start of each iteration,
        // queue2 holds all the nodes for this layer
        // queue3 and queue are empty

        int numComplex = 0, numSimple = 0 ;
        #ifdef VERBOSE
        printf("Processing %d nodes in layer %d\n", queue2->getNumElements(), curwid) ;
        #endif

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
            //score = getNumNeighbor6( ox, oy, oz ) ;
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
            for ( int m = 0 ; m < 4 ; m ++ )
            {
                int nx = ox + neighbor4[m][0] ;
                int ny = oy + neighbor4[m][1] ;
                int nz = oz ;
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
                            //score = getNumNeighbor6( nx, ny, nz ) ;

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

        #ifdef VERBOSE
        printf("%d complex, %d simple\n", numComplex, numSimple) ;
        #endif

        if ( numSimple == 0 )
        {
                break ;
        }
    }

    // Finally, clean up
    #ifdef VERBOSE
    printf("Thresholding the volume to 0/1...\n") ;
    #endif
    threshold( 0, 0, 1 ) ;
    delete scrvol;
    delete queue;
    delete queue2;
    delete queue3;
    delete queue4;
}

/* Thin the current volume while preserving voxels with values > highthr or <= lowthr in grayvol
*  Assuming the current volume has already been thresholded to 0/1
*/
void Volume::pointSkeleton( Volume* grayvol, float lowthr, float highthr, Volume* svol, Volume* hvol )
{
    int i, j, k ;
    // First, threshold the volume
    #ifdef VERBOSE
    printf("Thresholding the volume to -1/0...\n") ;
    #endif
    threshold( 0.5f, -1, 0 ) ;

    // Next, apply convergent erosion
    // by preserving: complex nodes, curve end-points, and sheet points

    // Next, initialize the linked queue
    #ifdef VERBOSE
    printf("Initializing queue...\n") ;
    #endif
    GridQueue2* queue2 = new GridQueue2( ) ;
    GridQueue2* queue3 = new GridQueue2( ) ;
    PriorityQueue <gridPoint,int> * queue = new PriorityQueue <gridPoint,int> ( MAX_QUEUELEN );

    for ( i = 0 ; i < getSizeX() ; i ++ )
        for ( j = 0 ; j < getSizeY() ; j ++ )
            for ( k = 0 ; k < getSizeZ() ; k ++ )
            {
                if ( getDataAt( i, j, k ) >= 0 )
                {
                    float v = (float)grayvol->getDataAt( i, j, k ) ;
                    if ( v <= lowthr || v > highthr || svol->getDataAt(i,j,k) > 0 || hvol->getDataAt(i,j,k) > 0 )
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
    #ifdef VERBOSE
    printf("Total %d nodes\n", queue2->getNumElements() ) ;
    #endif


    // Perform erosion
    int wid = MAX_ERODE ;
    #ifdef VERBOSE
    printf("Start erosion to %d...\n", wid) ;
    #endif
    gridQueueEle* ele ;
    gridPoint* gp ;
    int ox, oy, oz ;
    int score ;
    Volume* scrvol = new Volume( this->getSizeX() , this->getSizeY(), this->getSizeZ() ) ;
    for ( i = 0 ; i < getSizeX() * getSizeY() * getSizeZ() ; i ++ )
    {
        scrvol->setDataAt( i, -1 ) ;
    }


    for ( int curwid = 1 ; curwid <= wid ; curwid ++ )
    {
        // At the start of each iteration,
        // queue2 holds all the nodes for this layer
        // queue3 and queue are empty

        int numComplex = 0, numSimple = 0 ;
        #ifdef VERBOSE
        printf("Processing %d nodes in layer %d\n", queue2->getNumElements(), curwid) ;
        #endif


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
        #ifdef VERBOSE
        printf("%d complex, %d simple\n", numComplex, numSimple) ;
        #endif

        if ( numSimple == 0 )
        {
                break ;
        }
    }

    // Remove all internal voxels (contained in manifold curves)
    queue2->reset() ;
    ele = queue2->getNext() ;
    while ( ele != NULL )
    {
        ox = ele->x ;
        oy = ele->y ;
        oz = ele->z ;

        if ( hasCompleteHelix( ox, oy, oz ) == 1 )
        {
            ele = queue2->getNext() ;
        }
        else
        {
            ele = queue2->remove() ;
        }
    }

    for ( i = 0 ; i < getSizeX() ; i ++ )
        for ( j = 0 ; j < getSizeY() ; j ++ )
            for ( k = 0 ; k < getSizeZ() ; k ++ )
            {
                if ( getDataAt( i, j, k ) == 0 && hasCompleteHelix(i,j,k) == 1)
                {
                    queue2->prepend( i, j, k ) ;
                }
            }
    queue2->reset() ;
    ele = queue2->getNext() ;
    while ( ele != NULL )
    {
        ox = ele->x ;
        oy = ele->y ;
        oz = ele->z ;
        setDataAt( ox, oy, oz, -1 ) ;
        ele = queue2->remove() ;
    }

    // Finally, clean up
    delete scrvol;
    delete queue;
    delete queue2;
    delete queue3;
    #ifdef VERBOSE
    printf("Thresholding the volume to 0/1...\n") ;
    #endif
    threshold( 0, 0, 1 ) ;
}


// Compute minimal skeleton
void Volume::skeleton( float thr, Volume* svol, Volume* hvol )
{
    int i, j, k ;
    // First, threshold the volume
    #ifdef VERBOSE
    printf("Thresholding the volume to -1/0...\n") ;
    #endif
    threshold( thr, -1, 0 ) ;

    // Next, apply convergent erosion
    // by preserving: complex nodes, curve end-points, and sheet points

    // Next, initialize the linked queue
    #ifdef VERBOSE
    printf("Initializing queue...\n") ;
    #endif
    GridQueue2* queue2 = new GridQueue2( ) ;
    GridQueue2* queue3 = new GridQueue2( ) ;
    PriorityQueue <gridPoint,int> * queue = new PriorityQueue <gridPoint,int> ( MAX_QUEUELEN );

    for ( i = 0 ; i < getSizeX() ; i ++ )
        for ( j = 0 ; j < getSizeY() ; j ++ )
            for ( k = 0 ; k < getSizeZ() ; k ++ )
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
    int wid = MAX_ERODE ;
    #ifdef VERBOSE
    printf("Total %d nodes\n", queue2->getNumElements() ) ;


    // Perform erosion
    printf("Start erosion to %d...\n", wid) ;
    #endif
    gridQueueEle* ele ;
    gridPoint* gp ;
    int ox, oy, oz ;
    int score ;
    Volume* scrvol = new Volume( this->getSizeX() , this->getSizeY(), this->getSizeZ() ) ;
    for ( i = 0 ; i < getSizeX() * getSizeY() * getSizeZ() ; i ++ )
    {
        scrvol->setDataAt( i, -1 ) ;
    }


    for ( int curwid = 1 ; curwid <= wid ; curwid ++ )
    {
        // At the start of each iteration,
        // queue2 holds all the nodes for this layer
        // queue3 and queue are empty

        int numComplex = 0, numSimple = 0 ;
        #ifdef VERBOSE
        printf("Processing %d nodes in layer %d\n", queue2->getNumElements(), curwid) ;
        #endif


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

        #ifdef VERBOSE
        printf("%d complex, %d simple\n", numComplex, numSimple) ;
        #endif

        if ( numSimple == 0 )
        {
            delete queue2 ;
            break ;
        }
    }

    // Finally, clean up
    #ifdef VERBOSE
    printf("Thresholding the volume to 0/1...\n") ;
    #endif
    threshold( 0, 0, 1 ) ;
    delete scrvol;
    delete queue;
    delete queue3;
}

void Volume::erodeHelix( int disthr )
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
    Volume * fvol = new Volume( getSizeX(), getSizeY(), getSizeZ() ) ;
    GridQueue2* queue2 = new GridQueue2( ) ;
    GridQueue2* queue3 = new GridQueue2( ) ;
    GridQueue2** queues = new GridQueue2* [ 10000 ] ;

    for ( i = 0 ; i < getSizeX() ; i ++ )
        for ( j = 0 ; j < getSizeY() ; j ++ )
            for ( k = 0 ; k < getSizeZ() ; k ++ )
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
    #ifdef VERBOSE
    printf("Detecting closed rings %d...", dis) ;
    #endif
    int ftot = 0 ;
    for ( i = 0 ; i < getSizeX() ; i ++ )
        for ( j = 0 ; j < getSizeY() ; j ++ )
            for ( k = 0 ; k < getSizeZ() ; k ++ )
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
    #ifdef VERBOSE
    printf("%d nodes\n", ftot) ;
    #endif


    // return ;

    /* Find local minimum: to help determine erosion level
    int cts[ 64 ] ;
    for ( i = 0 ; i <= - dis ; i ++ )
    {
        cts[ i ] = 0 ;
    }
    for ( i = 0 ; i < getSizeX() ; i ++ )
        for ( j = 0 ; j < getSizeY() ; j ++ )
            for ( k = 0 ; k < getSizeZ() ; k ++ )
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

            #ifdef VERBOSE
            printf("Non-minimal: %d\n", num) ;
            #endif

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
    #ifdef VERBOSE
    //printf("Thresholding the volume to 0/1...\n") ;
    #endif
    //threshold( -1, 1, 0, 0 ) ;
    threshold( 0, 0, 1 ) ;
    delete fvol ;
    delete queue2;
    delete queue3;
    for ( d = -dis ; d >= 2 ; d -- ) {
        delete queues[d];
    }
    delete [] queues;

}

int Volume::erodeSheet( int disthr )
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
    Volume * fvol = new Volume( getSizeX(), getSizeY(), getSizeZ() ) ;
    GridQueue2* queue2 = new GridQueue2( ) ;
    GridQueue2* queue3 = new GridQueue2( ) ;
    GridQueue2** queues = new GridQueue2* [ 10000 ] ;

    for ( i = 0 ; i < getSizeX() ; i ++ )
        for ( j = 0 ; j < getSizeY() ; j ++ )
            for ( k = 0 ; k < getSizeZ() ; k ++ )
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
    #ifdef VERBOSE
    printf("Total %d nodes\n", queue2->getNumElements() ) ;
    #endif

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
    #ifdef VERBOSE
    printf("Detecting closed rings %d...", dis) ;
    #endif
    int ftot = 0 ;
    for ( i = 0 ; i < getSizeX() ; i ++ )
        for ( j = 0 ; j < getSizeY() ; j ++ )
            for ( k = 0 ; k < getSizeZ() ; k ++ )
            {
                if ( getDataAt( i, j, k ) == 0 )
                {
                    /*
                    int fval = (int) fvol->getDataAt( i, j, k ) ;
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
    #ifdef VERBOSE
    printf("%d nodes\n", ftot) ;
    #endif


    /* Find local minimum: to help determine erosion level
    int cts[ 64 ] ;
    for ( i = 0 ; i <= - dis ; i ++ )
    {
        cts[ i ] = 0 ;
    }
    for ( i = 0 ; i < getSizeX() ; i ++ )
        for ( j = 0 ; j < getSizeY() ; j ++ )
            for ( k = 0 ; k < getSizeZ() ; k ++ )
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
    int d ;
    if ( disthr + 2 > - dis )
    {
        disthr = - dis - 2 ;

    }
    for ( d = - dis ; d > disthr + 1 ; d -- )
    {
        queues[ d ]->reset() ;
        while ( (ele = queues[ d ]->getNext() ) != NULL )
        {
            setDataAt( ele->x, ele->y, ele->z, d ) ;
        }
    }

    for (d = disthr + 1 ; d >= 2 ; d -- )
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
            #ifdef VERBOSE
            printf("Non-minimal: %d\n", num) ;
            #endif

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
    #ifdef VERBOSE
    //printf("Thresholding the volume to 0/1...\n") ;
    #endif
    //threshold( -1, 1, 0, 0 ) ;
    threshold( 0, 0, 1 ) ;

    delete facevol ;
    delete fvol ;
    delete queue2;
    delete queue3;
    for (d = -dis ; d >= 2 ; d -- ) {
        delete queues[d];
    }
    delete [] queues;

    return - dis - 1 ;
}

/* Thin the current volume while preserving voxels with values > highthr or <= lowthr in grayvol
*  Assuming the current volume has already been thresholded to 0/1
*/
void Volume::surfaceSkeleton( Volume* grayvol, float lowthr, float highthr ) {
    int i, j, k ;
    threshold( 0.5f, -MAX_ERODE, 0 ) ;

    GridQueue2* queue2 = new GridQueue2( ) ;
    GridQueue2* queue3 = new GridQueue2( ) ;
    GridQueue2* queue4 = new GridQueue2( ) ;

    PriorityQueue <gridPoint,int> * queue = new PriorityQueue <gridPoint,int> ( MAX_QUEUELEN );
    int ct = 0 ;

    for ( i = 0 ; i < getSizeX() ; i ++ )
        for ( j = 0 ; j < getSizeY() ; j ++ )
            for ( k = 0 ; k < getSizeZ() ; k ++ )
            {
                if ( getDataAt( i, j, k ) >= 0 )
                {
                    float v = (float)grayvol->getDataAt(i,j,k) ;
                    if ( v > highthr || v <= lowthr )
                    {
                        setDataAt( i, j, k, MAX_ERODE ) ;
                    }
                    else
                    {
                        ct ++ ;

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


    // Perform erosion
    int wid = MAX_ERODE ;
    gridQueueEle* ele ;
    gridPoint* gp ;
    int ox, oy, oz ;
    int score ;
    Volume* scrvol = new Volume( this->getSizeX() , this->getSizeY(), this->getSizeZ() ) ;
    for ( i = 0 ; i < getSizeX() * getSizeY() * getSizeZ() ; i ++ )
    {
        scrvol->setDataAt( i, -1 ) ;
    }

    #ifdef  NOISE_DIS_SHEET
    Volume* noisevol = new Volume( getSizeX(), getSizeY(), getSizeZ() ) ;
    #endif

    for ( int curwid = 1 ; curwid <= wid ; curwid ++ )
    {


        int numComplex = 0, numSimple = 0 ;
        #ifdef VERBOSE
        printf("Processing %d nodes in layer %d\n", queue2->getNumElements(), curwid) ;
        #endif

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

            if ( getDataAt( ox, oy, oz ) != curwid || (int) scrvol->getDataAt( ox, oy, oz ) != score )
            {
                continue ;
            }


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

                nowComplex = 1 ;
            }
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
                        // setDataAt( nx, ny, nz, curwid + 1 ) ;
                        queue2->prepend( nx, ny, nz ) ;
                    }
                }

                if ( nowComplex )
                {

                    // printf("Error: %d\n", score);
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

        if ( numSimple == 0 )
        {
            if ( queue2->getNumElements() > 0 )
            {
                printf("*************************wierd**********************\n");
            }
                break ;
        }
    }

    // Remove all internal voxels (contained in cells)

    queue4->reset() ;
    ele = queue4->getNext() ;
    while ( ele != NULL )
    {
        ele = queue4->remove() ;
    }

    queue2->reset() ;
    for ( i = 0 ; i < getSizeX() ; i ++ )
        for ( j = 0 ; j < getSizeY() ; j ++ )
            for ( k = 0 ; k < getSizeZ() ; k ++ )
            {
                if ( getDataAt( i, j, k ) == 0 && isInternal2( i,j,k ) == 1 )
                {
                    queue2->prepend( i, j, k ) ;
                }
            }
    queue2->reset() ;
    ele = queue2->getNext() ;
    while ( ele != NULL )
    {
        ox = ele->x ;
        oy = ele->y ;
        oz = ele->z ;
        setDataAt( ox, oy, oz, -1 ) ;
        ele = queue2->remove() ;
    }



    // Finally, clean up
    delete scrvol;
    delete queue;
    delete queue2;
    delete queue3;
    delete queue4;
    #ifdef VERBOSE
    printf("Thresholding the volume to 0/1...\n") ;
    #endif
    threshold( 0, 0, 1 ) ;

}

void Volume::surfaceSkeletonPres( float thr, Volume * preserve )
{
    int i, j, k ;
    // First, threshold the volume
    #ifdef VERBOSE
    printf("Thresholding the volume to -MAX_ERODE/0...\n") ;
    #endif
    threshold( thr, -MAX_ERODE, 0 ) ;

    // Next, initialize the linked queue
    #ifdef VERBOSE
    printf("Initializing queue...\n") ;
    #endif
    GridQueue2* queue2 = new GridQueue2( ) ;
    GridQueue2* queue3 = new GridQueue2( ) ;
    GridQueue2* queue4 = new GridQueue2( ) ;

    PriorityQueue <gridPoint,int> * queue = new PriorityQueue <gridPoint,int> ( MAX_QUEUELEN );

    for ( i = 0 ; i < getSizeX() ; i ++ )
        for ( j = 0 ; j < getSizeY() ; j ++ )
            for ( k = 0 ; k < getSizeZ() ; k ++ )
            {
                if ( getDataAt( i, j, k ) >= 0 ) {
                    if(preserve->getDataAt(i, j, k) > 0) {
                        setDataAt(i, j, k, MAX_ERODE);
                    } else {
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
    int wid = MAX_ERODE ;
    #ifdef VERBOSE
    printf("Total %d nodes\n", queue2->getNumElements() ) ;
    printf("Start erosion to %d...\n", wid) ;
    #endif


    // Perform erosion
    gridQueueEle* ele ;
    gridPoint* gp ;
    int ox, oy, oz ;
    int score ;
    Volume* scrvol = new Volume( this->getSizeX() , this->getSizeY(), this->getSizeZ() ) ;
    for ( i = 0 ; i < getSizeX() * getSizeY() * getSizeZ() ; i ++ )
    {
        scrvol->setDataAt( i, -1 ) ;
    }

#ifdef  NOISE_DIS_SHEET
    Volume* noisevol = new Volume( getSizeX(), getSizeY(), getSizeZ() ) ;
#endif

    for ( int curwid = 1 ; curwid <= wid ; curwid ++ )
    {
        // At the start of each iteration,
        // queue2 and queue4 holds all the nodes for this layer
        // queue3 and queue are empty

        int numComplex = 0, numSimple = 0 ;
        #ifdef VERBOSE
        printf("Processing %d nodes in layer %d\n", queue2->getNumElements(), curwid) ;
        #endif

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

        #ifdef VERBOSE
        printf("%d complex, %d simple\n", numComplex, numSimple) ;
        #endif

        if ( numSimple == 0 )
        {
                break ;
        }
    }

    // Finally, clean up
    #ifdef VERBOSE
    printf("Thresholding the volume to 0/1...\n") ;
    #endif
    threshold( 0, 0, 1 ) ;

    delete scrvol;
    delete queue;
    delete queue2;
    delete queue3;
    delete queue4;

}

/**
 * Normalize to a given range
 */
void Volume::threshold( double thr )
{
    threshold( thr, 0, 1, 0, true) ;
}

void Volume::threshold( double thr, int out, int in )
{
    threshold( thr, out, in, out, true) ;
}

void Volume::threshold( double thr, int out, int in, int boundary, bool markBoundary)
{
    float val;
    for ( int i = 0 ; i < getSizeX() ; i ++ )
        for ( int j = 0 ; j < getSizeY() ; j ++ )
            for ( int k = 0 ; k < getSizeZ() ; k ++ )
            {
                val = (float)getDataAt(i, j, k);
                if(markBoundary) {
                    if ( i > 1 && i < getSizeX() - 2 && j > 1 && j < getSizeY() - 2 && k > 1 && k < getSizeZ() - 2 ) {
                        if(val < thr) {
                            setDataAt(i, j, k, out);
                        } else {
                            setDataAt(i, j, k, in);
                        }
                    }
                    else
                    {
                        setDataAt(i, j, k, boundary);
                    }
                } else {
                    if(val < thr) {
                        setDataAt(i, j, k, out);
                    } else {
                        setDataAt(i, j, k, in);
                    }
                }
            }
}

void Volume::threshold2( double thr, int out, int in )
{
    for ( int i = 0 ; i < getSizeX() ; i ++ )
        for ( int j = 0 ; j < getSizeY() ; j ++ )
            for ( int k = 0 ; k < getSizeZ() ; k ++ ) {
                double val = getDataAt(i, j, k);
                if(val <= thr) {
                    setDataAt(i, j, k, out);
                } else {
                    setDataAt(i, j, k, in);
                }
            }
}

void Volume::smooth( float alpha )
{
    VolumeData * smoothedData = new VolumeData(*volData);

    for (int i = 1; i < getSizeX() - 1; i++)
        for (int j = 1; j < getSizeY() - 1; j++)
            for (int k = 1; k < getSizeZ() - 1; k++) {
                float v = (float)getDataAt( i - 1, j, k ) +
                    (float)getDataAt( i + 1, j, k ) +
                    (float)getDataAt( i, j - 1, k ) +
                    (float)getDataAt( i, j + 1, k ) +
                    (float)getDataAt( i, j, k - 1 ) +
                    (float)getDataAt( i, j, k + 1 ) ;
                smoothedData->setDataAt(i, j, k, smoothedData->getDataAt(i, j, k) * alpha + ( 1 - alpha ) * v / 6);
            }
    delete volData;
    volData = smoothedData;
}

void Volume::normalize( double min, double max )
{
    double imin = getMin() ;
    double imax = getMax() ;
    double irange = imax - imin ;
    double range = max - min ;

    int size = volData->getMaxIndex();
    for(int i = 0 ; i < size ; i ++) {
        setDataAt(i, ((getDataAt(i) - (float)imin ) / (float)irange) * (float)range + (float)min);
    }
}

/* Set data at a pixel */

Volume * Volume::getDataRange(int x, int y, int z, int radius) {
    Volume * range = new Volume(radius*2+1, radius*2+1, radius*2+1);
    for(int xx = x-radius; xx <= x+radius; xx++) {
        for(int yy = y-radius; yy <= y+radius; yy++) {
            for(int zz = z-radius; zz <= z+radius; zz++) {
                range->setDataAt(xx-x+radius, yy-y+radius, zz-z+radius, getDataAt(xx, yy, zz));
            }
        }
    }
    return range;
}

void Volume::toMRCFile( string fname )
{
    FILE* fout = fopen( fname.c_str(), "wb" ) ;

    // Write header
    int sizeX = getSizeX();
    int sizeY = getSizeY();
    int sizeZ = getSizeZ();
    fwrite( &sizeX, sizeof( int ), 1, fout ) ;
    fwrite( &sizeY, sizeof( int ), 1, fout ) ;
    fwrite( &sizeZ, sizeof( int ), 1, fout ) ;

    int mode = 2 ;
    fwrite( &mode, sizeof ( int ), 1, fout ) ;

    int off[3] = {0,0,0} ;

    // Changed: MX == NX instead of MX = NX -1 (similar for Y and Z) because that is how EMAN2 and UCSF Chimera do it
    //int intv[3] = { getSizeX() - 1, getSizeY() - 1, getSizeZ() - 1 } ;
    int intv[3] = {getSizeX(), getSizeY(), getSizeZ()};

    fwrite( off, sizeof( int ), 3, fout ) ;
    fwrite( intv, sizeof( int ), 3, fout ) ;

    float cella[3] = {getSpacingX() * (float)(getSizeX()), getSpacingY() * (float)(getSizeY()), getSpacingZ() * (float)(getSizeZ())};
    float cellb[3] = {90,90,90} ;
    fwrite( cella, sizeof( float ), 3, fout ) ;
    fwrite( cellb, sizeof( float ), 3, fout ) ;

    int cols[3] = {1,2,3} ;
    fwrite( cols, sizeof( int ), 3, fout ) ;

    double dmin = 100000, dmax = -100000 ;
    int i ;
    int size = volData->getMaxIndex();
    for (i = 0 ; i < size; i++) {
        float val = (float)getDataAt(i);
        if (val < dmin) {
            dmin = val;
        }
        if (val > dmax) {
            dmax = val;
        }
    }
    float ds[3] = {(float)dmin, (float)dmax, (float)0} ;
    fwrite( ds, sizeof( float ), 3, fout ) ;

    int zero = 0 ;
    for (i = 23 ; i <= 49 ; i ++ )
    {
        fwrite( &zero, sizeof( int ), 1, fout );
    }

    float origins[3];
    origins[0] = getOriginX() / getSpacingX() + 0.5f * (float)getSizeX();
    origins[1] = getOriginY() / getSpacingY() + 0.5f * (float)getSizeY();
    origins[2] = getOriginZ() / getSpacingZ() + 0.5f * (float)getSizeZ();

    fwrite( origins, sizeof( float ), 3, fout) ;

    for (i = 53 ; i <= 256 ; i ++ )
    {
        fwrite( &zero, sizeof( int ), 1, fout ) ;
    }

    // Write contents
    for ( int z = 0 ; z < getSizeZ() ; z ++ )
        for ( int y = 0 ; y < getSizeY() ; y ++ )
            for ( int x = 0 ; x < getSizeX() ; x ++ )
            {
                float d = (float)getDataAt(x,y,z) ;
                fwrite( &d, sizeof( float ), 1, fout ) ;
            }

    fclose( fout ) ;
}

// Returns the mean value of all the voxels
float Volume::getMean()
{
    int N = volData->getMaxIndex();
    double mass = 0;
    for (int i = 0; i < N; i++)
        mass += getDataAt(i);
    float mean = (float) mass/N;
    return mean;
}

// Returns the population standard deviation of the values at all the voxels
float Volume::getStdDev()
{
    int N = volData->getMaxIndex();

    //Calculate the standard deviation of all the voxels in the image
    double voxel_sum = 0;
    double voxel_squared_sum = 0;
    float val;

    for (int i = 0; i < N; i++)
    {
        val = (float)getDataAt(i);
        voxel_sum += val;
        voxel_squared_sum += val*val;
    }
    float std_dev = (float) sqrt( (voxel_squared_sum - voxel_sum*voxel_sum/N) / N );
    return std_dev;
}

void Volume::normalizeVolume(){
  normalize(0, 1);
}

void Volume::loadFile(string inputFile) {

    *volData = *MRCReaderPicker::pick(inputFile.c_str())->getVolume();

          #ifdef GORGON_DEBUG
                cout<<"\033[35mDEBUG: File:   VolumeFormatConverter.h"<<endl;
                cout<<"DEBUG: Method: VolumeFormatConverter::LoadVolume\033[0m"<<endl;
                cout<<"DEBUG: Args: string, int, int, int, int\033[0m"<<endl;
                cout<<getSize()<<endl;
          #endif
}


void Volume::saveFile(string fileName) {
    int pos = fileName.rfind(".") + 1;
    string extension = fileName.substr(pos, fileName.length()-pos);

    extension = StringUtils::StringToUpper(extension);

    if(extension == "MRC") {
      toMRCFile(fileName.c_str());
    } else {
      cout<<"Input format "<<extension<<" not supported!"<<endl;
    }
}

Volume * Volume::PerformBinarySkeletonizationJu2007(double threshold, int minCurveSize, int minSurfaceSize) {
  VolumeSkeletonizer * skeletonizer = new VolumeSkeletonizer(0,0,0,DEFAULT_SKELETON_DIRECTION_RADIUS);
#ifdef GORGON_DEBUG
  cout<<"DEBUG: File:   Volume.h"<<endl;
  cout<<"DEBUG: Method: Volume::PerformBinarySkeletonizationJu2007"<<endl;
  cout<<getSize()<<endl;
#endif

  Volume * outputVol = skeletonizer->PerformPureJuSkeletonization(this, "", threshold, minCurveSize, minSurfaceSize);
  delete skeletonizer;
#ifdef GORGON_DEBUG
  cout<<"DEBUG: File:   Volume.h"<<endl;
  cout<<"DEBUG: Method: Volume::PerformBinarySkeletonizationJu2007"<<endl;
  cout<<"outputVol->getSize(): "<<outputVol->getSize()<<endl;
#endif

  return outputVol;
}

Volume * Volume::PerformGrayscaleSkeletonizationAbeysinghe2008(double startDensity, int stepCount, int minCurveSize, int minSurfaceSize, int curveRadius, int surfaceRadius, int skeletonRadius) {
  double stepSize = (getMax() - startDensity) / stepCount;
  if(!isZero(stepSize)) {
    VolumeSkeletonizer * skeletonizer = new VolumeSkeletonizer(0, curveRadius, surfaceRadius, skeletonRadius);
    Volume * outputVol = skeletonizer->PerformImmersionSkeletonizationAndPruning(this, NULL, startDensity, getMax(), stepSize, 0, 0, minCurveSize, minSurfaceSize, 0, 0, "", true, 1.0, DEFAULT_PRUNE_THRESHOLD, DEFAULT_PRUNE_THRESHOLD);
    delete skeletonizer;
    return outputVol;
  } else {
    return NULL;
  }
}
