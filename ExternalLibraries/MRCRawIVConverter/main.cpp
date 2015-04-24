/*****************************************************************************/
/*                                                                           */
/*   MRC to RawIV convertor                                                  */
/*                                                                           */
/*   Copyright (C) The University of Texas at Austin                         */
/*                                                                           */
/*     Author:     John Wiggins <prok@ices.utexas.edu> 2004-2005             */
/*     Author:     Vinay Siddavanahalli <skvinay@cs.utexas.edu> 2000-2005    */
/*                                                                           */
/*     Principal Investigator: Chandrajit Bajaj <bajaj@ices.utexas.edu>      */
/*                                                                           */
/*         Professor of Computer Sciences,                                   */
/*         Computational and Applied Mathematics Chair in Visualization,     */
/*         Director, Computational Visualization Center (CVC),               */
/*         Institute of Computational Engineering and Sciences (ICES)        */
/*         The University of Texas at Austin,                                */
/*         201 East 24th Street, ACES 2.324A,                                */
/*         1 University Station, C0200                                       */
/*         Austin, TX 78712-0027                                             */
/*         http://www.cs.utexas.edu/~bajaj                                   */
/*                                                                           */
/*         http://www.ices.utexas.edu/CVC                                    */
/*                                                                           */
/*   This library is free software; you can redistribute it and/or           */
/*   modify it under the terms of the GNU Lesser General Public              */
/*   License as published by the Free Software Foundation; either            */
/*   version 2.1 of the License, or (at your option) any later version.      */
/*   Specifically, this library is free for academic or personal non-profit  */
/*   use, with due acknowledgement. Any or all personal profit / industrial  */
/*   use needs to get a proper license approved from us.                     */
/*                                                                           */
/*   This library is distributed in the hope that it will be useful,         */
/*   but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       */
/*   Lesser General Public License for more details.                         */
/*                                                                           */
/*   You should have received a copy of the GNU Lesser General Public        */
/*   License along with this library; if not, write to the Free Software     */
/*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307    */
/*   USA                                                                     */
/*                                                                           */
/*****************************************************************************/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "SimpleVolumeData.h"
#include "VolumeLoader.h"

bool convert( const char* filename1, const char* filename2 )
{
	VolumeLoader* vLoader = new VolumeLoader();
	
	SimpleVolumeData* sData = vLoader->loadFile(filename1);
	if( !sData )
	{
		printf("There was an error in loading volume %s\n", filename1 );
		delete vLoader; vLoader = 0;
		delete sData; sData = 0;
		return false;
	}

	if( !vLoader->saveFile(filename2, sData ) )
	{
		printf("There was an error in saving volume %s\n", filename2 );
		delete vLoader; vLoader = 0;
		delete sData; sData = 0;
		return false;
	}

	delete vLoader; vLoader = 0;
	delete sData; sData = 0;
	
	return true;
}

int main( int argc, char** argv )
{
	if( argc != 3 )
	{
		printf("Wrong number of parameters\n");
		printf("Usage: <program name> <mrc/rawiv file> <rawiv/mrc file>\n");
		return -1;
	}

	if( convert( argv[1], argv[2] ) ) return 0;

	return -1;
}
