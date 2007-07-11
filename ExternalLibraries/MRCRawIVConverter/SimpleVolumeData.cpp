/*****************************************************************************/
/*                                                                           */
/*   Blurmaps, Create volumes, curvatures, surfaces from union of balls      */
/*                                                                           */
/*   Copyright (C) The University of Texas at Austin                         */
/*                                                                           */
/*     Author:     Vinay Siddavanahalli <skvinay@cs.utexas.edu>   2004-2005  */
/*     Author:     John Wiggins         <prok@ices.utexas.edu>    2004-2005  */
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

// SimpleVolumeData.cpp: definition of the SimpleVolumeData class.
//
//////////////////////////////////////////////////////////////////////

#include "SimpleVolumeData.h"
#include "ByteSwapping.h"

#include <string.h>
#include <stdio.h>
#include <math.h>

#ifdef USE_MPI
#include <mpi.h>
#endif

SimpleVolumeData::SimpleVolumeData(unsigned int dims[3])
{
	setDefaults();

	m_Dims[0] = dims[0];
	m_Dims[1] = dims[1];
	m_Dims[2] = dims[2];
	m_Dims[3] = dims[2]; // an extra copy of the z dimension (for MPI runs)

	// So here's how the extra z dimension is used:
	// Running MPI or not, m_Dims[3] will always contain the z dimension
	// of the actual data pointed to by this object. For that reason it will
	// always be used in place of m_Dims[2] when iterating over m_Data entries.
	// If running under MPI, m_Dims[3] <= m_Dims[2]. m_Dims[2] is the z dimension
	// of the dataset. m_Dims[3] is the z dimension of the slab that this object
	// represents. If not running under MPI, m_Dims[2] == m_Dims[3].
}

SimpleVolumeData::SimpleVolumeData(unsigned int dim1, unsigned int dim2, unsigned int dim3 )
{	
	setDefaults();

	m_Dims[0] = dim1;
	m_Dims[1] = dim2;
	m_Dims[2] = dim3;
	m_Dims[3] = dim3; // an extra copy of the z dimension (for MPI runs)

	// So here's how the extra z dimension is used:
	// Running MPI or not, m_Dims[3] will always contain the z dimension
	// of the actual data pointed to by this object. For that reason it will
	// always be used in place of m_Dims[2] when iterating over m_Data entries.
	// If running under MPI, m_Dims[3] <= m_Dims[2]. m_Dims[2] is the z dimension
	// of the dataset. m_Dims[3] is the z dimension of the slab that this object
	// represents. If not running under MPI, m_Dims[2] == m_Dims[3].
}

SimpleVolumeData::~SimpleVolumeData()
{
	unsigned int i;
	
	if (m_Data) {
		for (i=0; i < m_NumberOfVariables; i++)
			delete [] m_Data[i];
	}
	if (m_VarNames) {
		for (i=0; i < m_NumberOfVariables; i++)
			delete [] m_VarNames[i];
	}
	
	delete [] m_DataTypes;
	delete [] m_VarNames;
	delete [] m_Data;
}

void SimpleVolumeData::setNumberOfVariables( unsigned int num )
{
	// clean up any pre-existing data
	if (m_Data) {
		for (unsigned int i=0; i < m_NumberOfVariables; i++) {
			delete [] m_Data[i];
			delete [] m_VarNames[i];
		}
		delete [] m_DataTypes;
		delete [] m_Data;
	}

	// allocate arrays
	m_Data = new unsigned char * [num];
	m_DataTypes = new DataType [num];
	m_VarNames = new char * [num];

	// init arrays
	for (unsigned int i=0; i < num; i++) {
		m_Data[i] = 0;
		m_DataTypes[i] = NO_TYPE;
		m_VarNames[i] = 0;
	}

	m_NumberOfVariables = num;
}

void SimpleVolumeData::removeVariable( unsigned int var )
{
	// make sure there is a variable to remove
	if (var < m_NumberOfVariables) {
		// make copies of the data
		unsigned char **nData;
		char **nVarNames;
		DataType *nDataTypes;
		unsigned int i,j;
	
		nData = new unsigned char * [m_NumberOfVariables-1];
		nVarNames = new char * [m_NumberOfVariables-1];
		nDataTypes = new DataType [m_NumberOfVariables-1];

		for (i=0,j=0; i < m_NumberOfVariables; i++) {
			// don't copy variable var
			if (i != var) {
				nData[j] = m_Data[i];
				nVarNames[j] = m_VarNames[i];
				nDataTypes[j] = m_DataTypes[i];
				j++;
			}
			else {
				// clean up the removed variable's data
				delete [] m_Data[i];
				delete [] m_VarNames[i];
			}
			m_Data[i] = 0;
			m_VarNames[i] = 0;
			m_DataTypes[i] = NO_TYPE;
		}

		// clean up the old data
		delete [] m_Data;
		delete [] m_VarNames;
		delete [] m_DataTypes;

		// reassign pointers
		m_Data = nData;
		m_VarNames = nVarNames;
		m_DataTypes = nDataTypes;
		
		// change the variable count
		m_NumberOfVariables -= 1;
	}
}

void SimpleVolumeData::setDimensions( unsigned int dims[3] )
{
	for (int i=0; i < 3; i++)
		m_Dims[i] = dims[i];
	// don't forget the extra copy of the z dimension (MPI related)
	m_Dims[3] = dims[2];
}

void SimpleVolumeData::setMinExtent( float minExt[3] )
{
	for (int i=0; i < 3; i++)
		m_Min[i] = minExt[i];
}

void SimpleVolumeData::setMaxExtent( float maxExt[3] )
{
	for (int i=0; i < 3; i++)
		m_Max[i] = maxExt[i];
}

void SimpleVolumeData::setData( unsigned int variable, void* data )
{
	if (m_Data && variable < m_NumberOfVariables) {
		if (m_Data[variable])
			delete [] m_Data[variable];

		m_Data[variable] = (unsigned char *)data;
	}
}

void SimpleVolumeData::setType( unsigned int variable, DataType type )
{
	if (m_DataTypes && variable < m_NumberOfVariables) {
		m_DataTypes[variable] = type;
	}
}

void SimpleVolumeData::setName( unsigned int variable, const char* name )
{
	if (m_VarNames && variable < m_NumberOfVariables) {
		if (m_VarNames[variable])
			delete [] m_VarNames[variable];

		m_VarNames[variable] = new char [strlen(name)+1];
		strcpy(m_VarNames[variable], name);
	}
}

#ifdef USE_MPI
void SimpleVolumeData::parallelAdjustDimsMerged()
{
	int rank, size;
	unsigned int zslack;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	// outputs are being merged into one. no overlap between slabs
	m_Dims[3] = m_Dims[2] / (unsigned int)size;
	zslack = m_Dims[2] % (unsigned int)size;
	// determine the starting slice offset for this processor.
	// note that excess slack is picked up by the root node.
	// (this is so that root can just reuse its buffers when receiving slabs
	// 	from other processors)
	if (rank == 0)
	{
		m_ZOff = 0;
		m_Dims[3] += zslack;
	}
	else
		m_ZOff = zslack + m_Dims[3]*rank;	

	// set the merged output flag for later
	m_MergeOutput = true;
}

void SimpleVolumeData::parallelAdjustDimsUnmerged()
{
	int rank, size;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	// outputs are individual files. slabs will overlap.
	// m_Dims[3] calculation lifted verbatim from breakraw.cpp to
	// ensure compatibility with iotree
	m_Dims[3] = (m_Dims[2]-1) / size + 1 + (((m_Dims[2]-1)%size > rank)? 1:0);

	// calculate the offset for non-root slabs
	if (rank != 0)
		for (int i=0; i < rank; i++)
			m_ZOff += (m_Dims[2]-1) / size + 1 + (((m_Dims[2]-1)%size > i)?1:0) - 1;
	else
		m_ZOff = 0;
	
	// set the merged output flag for later
	m_MergeOutput = false;
}

bool SimpleVolumeData::parallelWriteRawVFile(const char *filename)
{
	// we don't support this yet
	//return false;
				
	// no data? bail.
	if (!m_Data)
		return false;

	int rank, size;
	FILE *fp = NULL;
	char *header=NULL, fname[255];
	unsigned int hsize=0;

	// standard MPI need-to-know info
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (m_MergeOutput && rank == 0) {
		// open the output file on the root node
		sprintf(fname, "%s", filename);
		fp = fopen(fname, "w");

		// check it
		if (!fp)
			return false; // XXX: what about the other nodes ?
	}
	else if (!m_MergeOutput) {
		// open an output file on every node
		sprintf(fname, "%s_%03d", filename, rank);
		fp = fopen(fname, "w");

		// check it
		if (!fp)
			return false; // XXX: what if this fails on some nodes ?
	}

	// make the data big endian
	makeVariablesBigEndian();

	// create and write the header
	if (m_MergeOutput && rank == 0) {
		createRawVHeader(&header, &hsize);
		fwrite(header, hsize, 1, fp);
		delete [] header;
	}
	else if (!m_MergeOutput) {
		// adjust the minZ and maxZ before creating the header
		float tMin = m_Min[2], tMax = m_Max[2], zspan = getSpanZ();
		m_Min[2] += m_ZOff * zspan;
		m_Max[2] = m_Min[2] + parallelGetDepth()*zspan;

		createRawVHeader(&header, &hsize);
		fwrite(header, hsize, 1, fp);
		delete [] header;

		// restore the old minZ value
		m_Min[2] = tMin;
		m_Max[2] = tMax;
	}

	// write the data
	if (m_MergeOutput) {
		// merged output
		long *varOffsets = NULL;
		unsigned int i,j,k, dim[3];

		// write out the root node's slabs
    if (rank == 0) {
			// build an array of file offsets for variable data
			varOffsets = new long [getNumberOfVariables()];
			varOffsets[0] = ftell(fp); // immediately following the header
			for (i=1; i < getNumberOfVariables(); i++)
				varOffsets[i] = varOffsets[i-1]
											+ (getTypeSize(i-1)*m_Dims[0]*m_Dims[1]*m_Dims[2]);

			//for (i=0; i < getNumberOfVariables(); i++)
			//	printf("Variable Offset %d: %d\n", i, varOffsets[i]);

      // write the data
			for (i=0; i < getNumberOfVariables(); i++) {
				fseek(fp, varOffsets[i], SEEK_SET);
				fwrite(getData(i), getTypeSize(i)*m_Dims[0]*m_Dims[1]*m_Dims[3], 1, fp);
				varOffsets[i] = ftell(fp);
			}
    }
    // gather remaining slabs at the root node and write them as they arrive
    for (i=1; i < (unsigned int)size; i++) {
      if (rank == 0) {
        // root node always receives
        MPI_Status stat;
        // receive the dimensions of the slab
        MPI_Recv(dim, 3, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, &stat);
        // receive the data for every variable
				for (j=0; j < getNumberOfVariables(); j++) {
          for (k=0; k < dim[2]; k++)
            MPI_Recv(m_Data[j]+(dim[0]*dim[1]*getTypeSize(j)*k),
									dim[0]*dim[1]*getTypeSize(j), MPI_UNSIGNED_CHAR,
									i,0, MPI_COMM_WORLD, &stat);
				}

        // write the data
				for (j=0; j < getNumberOfVariables(); j++) {
					fseek(fp, varOffsets[j], SEEK_SET);
					fwrite(m_Data[j], dim[0]*dim[1]*dim[2]*getTypeSize(j), 1, fp);
					varOffsets[j] = ftell(fp);
				}
      }
      else if (i == (unsigned int)rank) {
        // other nodes send if it's their turn
				// fill in the dimensions of the slab
				dim[0] = m_Dims[0];
				dim[1] = m_Dims[1];
				dim[2] = m_Dims[3];
        // send the dimensions of the slab
        MPI_Send(dim, 3, MPI_UNSIGNED, 0,0, MPI_COMM_WORLD);
        // send the data for every variable
				for (j=0; j < getNumberOfVariables(); j++) {
					for (k=0; k < dim[2]; k++)
            MPI_Send(m_Data[j]+(dim[0]*dim[1]*getTypeSize(j)*k),
									dim[0]*dim[1]*getTypeSize(j), MPI_UNSIGNED_CHAR,
									0,0, MPI_COMM_WORLD);
        }
      }
      else
        ; // not root or rank i; do nothing

      // put a barrier here to keep the root from being steamrolled by
      // the other nodes.
      MPI_Barrier(MPI_COMM_WORLD);
    }
		// clean up
		if (rank == 0) {
			delete [] varOffsets;
			fclose(fp);
		}
	}
	else {
		// unmerged outputs
		for (unsigned int i=0; i < getNumberOfVariables(); i++)
			fwrite(getData(i), getDataSize(i), 1, fp);
		// clean up
		fclose(fp);
	}
	
	// convert back to little endian (if running on a little endian machine)
	makeVariablesBigEndian();

	return true;
}

bool SimpleVolumeData::parallelWriteRawIVFile(const char *filename)
{
	// no data? bail.
	if (!m_Data)
		return false;

	FILE *fp = NULL;
	char header[68], fname[255];
	unsigned int dim[3];
	int rank, size;

	// standard MPI need-to-know info
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	// (maybe) open a file
	if (m_MergeOutput && rank == 0) {
		// open the output file on the root node
		sprintf(fname, "%s", filename);
		fp = fopen(fname, "w");

		// check it
		if (!fp)
			return false; // XXX: what about the other nodes ?
	}
	else if (!m_MergeOutput) {
		// open an output file on every node
		sprintf(fname, "%s_%03d", filename, rank);
		fp = fopen(fname, "w");

		// check it
		if (!fp)
			return false; // XXX: what if this fails on some nodes ?
	}

	// make sure the data is big endian
	makeVariablesBigEndian();

	// create and write the header
	if ((m_MergeOutput && rank == 0) || !m_MergeOutput) {
		createRawIVHeader(header);
		fwrite(header, sizeof(header), 1, fp);
	}

	// write the data
	if (m_MergeOutput) {
		// Merged output. Send all data back to root and write one file
		// 
		// sync
		MPI_Barrier(MPI_COMM_WORLD);
		
		// only write data on the root node
		if (rank == 0) {
			// write the root node's data
			fwrite(m_Data[0], getDataSize(0), 1, fp);
		}

		// gather remaining slabs at the root node and write them as they arrive
		for (int i=1; i < size; i++)
		{
			unsigned int j;
			
			if (rank == 0)
			{
				// root node always receives
				MPI_Status stat;
				float *dptr = (float *)m_Data[0];
				
				// receive the dimensions of the slab
				MPI_Recv(dim, 3, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, &stat);
				// receive the data
				for (j=0; j < dim[2]; j++)
					MPI_Recv(dptr+(dim[0]*dim[1]*j), dim[0]*dim[1], MPI_FLOAT,
													i,0, MPI_COMM_WORLD, &stat);
	
				// write the data
				fwrite(dptr, dim[0]*dim[1]*dim[2]*sizeof(float), 1, fp);
			}
			else if (rank == i)
			{
				// other nodes send if it's their turn
				float *dptr = (float *)m_Data[0];
				
				// fill in the dimensions of the slab
				dim[0] = m_Dims[0];
				dim[1] = m_Dims[1];
				dim[2] = m_Dims[3];
				// send the dimensions of the slab
				MPI_Send(dim, 3, MPI_UNSIGNED, 0,0, MPI_COMM_WORLD);
				// send the data
				for (j=0; j < dim[2]; j++)
					MPI_Send(dptr+(dim[0]*dim[1]*j), dim[0]*dim[1], MPI_FLOAT,
													0,0, MPI_COMM_WORLD);
			}
			else
				; // not root or rank i; do nothing
	
			// put a barrier here to keep the root from being steamrolled by
			// the other nodes.
			MPI_Barrier(MPI_COMM_WORLD);
		}
		
		// clean up
		if (rank == 0)
			fclose(fp);
	}
	else { // !m_MergeOutput
		// write the data
		int size = getDataSize(0);
		fwrite(m_Data[0], getDataSize(0), 1, fp);
		// clean up
		fclose(fp);
	}
	
	// convert back to little endian (if running on a little endian machine)
	makeVariablesBigEndian();

	return true;
}
#endif

bool SimpleVolumeData::writeRawVFile(const char *filename)
{
	// no data? bail.
	if (!m_Data)
		return false;

	FILE *fp = fopen(filename, "wb");
	char *header=NULL;
	unsigned int hsize=0;
	
	// failed to open the file? bail.
	if (!fp)
		return false;

	// make the data big endian
	makeVariablesBigEndian();

	// create the header
	createRawVHeader(&header, &hsize);
	// write the header
	fwrite(header, hsize, 1, fp);
	// write the data
	for (unsigned int i=0; i < m_NumberOfVariables; i++)
		fwrite(getData(i), getDataSize(i), 1, fp);
	
	// make the data native endian
	makeVariablesBigEndian();

	// clean up
	delete [] header;
	fclose(fp);
	
	return true;
}

bool SimpleVolumeData::writeRawIVFile(const char *filename, unsigned int variable)
{
	// no data? bail.
	if (!m_Data)
		return false;

	// unsupported datatype for the requested variable? bail.
	if (getType(variable) == ULONG || getType(variable) == DOUBLE)
		return false;

	FILE *fp = fopen(filename, "wb");
	char header[68];
	
	// failed to open the file? bail.
	if (!fp)
		return false;

	// make the data big endian
	makeVariablesBigEndian();

	// create the header
	createRawIVHeader(header);

	// write the header
	int headerSize = sizeof(header);
	fwrite(header, sizeof(header), 1, fp);


	// write the data
	int dataSize = getDataSize(variable);
	fwrite(getData(variable), getDataSize(variable), 1, fp);

	// make the data native endian (poorly named function)
	makeVariablesBigEndian();
	
	// clean up
	fclose(fp);
	
	return true;
}

bool SimpleVolumeData::createRawVHeader(char **header, unsigned int *hsize)
{
	if (!m_Data)
		return false;

	*hsize = 56+65*m_NumberOfVariables;
	*header = new char [*hsize];

	unsigned int magic = 0xBAADBEEF, timesteps=1;
	char *bptr;
	float zeroVal = 0.0;

	bptr = *header;
	
	// the magic value
	memcpy(bptr, &magic, 4);
	bptr += 4;
	
	// the dimensions
	// X
	memcpy(bptr, &m_Dims[0], 4);
	bptr += 4;
	// Y
	memcpy(bptr, &m_Dims[1], 4);
	bptr += 4;
	// Z
#ifdef USE_MPI
	if (!m_MergeOutput)
		memcpy(bptr, &m_Dims[3], 4);
	else
#endif
	memcpy(bptr, &m_Dims[2], 4);
	bptr += 4;
	
	// # timesteps
	memcpy(bptr, &timesteps, 4);
	bptr += 4;
	
	// # variables
	memcpy(bptr, &m_NumberOfVariables, 4);
	bptr += 4;
	
	// minimums
	// X
	memcpy(bptr, &m_Min[0], 4);
	bptr += 4;
	// Y
	memcpy(bptr, &m_Min[1], 4);
	bptr += 4;
	// Z
	memcpy(bptr, &m_Min[2], 4);
	bptr += 4;
	// T
	memcpy(bptr, &zeroVal, 4);
	bptr += 4;
	
	// maximums
	// X
	memcpy(bptr, &m_Max[0], 4);
	bptr += 4;
	// Y
	memcpy(bptr, &m_Max[1], 4);
	bptr += 4;
	// Z
	memcpy(bptr, &m_Max[2], 4);
	bptr += 4;
	// T
	memcpy(bptr, &zeroVal, 4);
	bptr += 4;

	// swap the first part of the header
	if (isLittleEndian())
		swapByteOrder((unsigned int *)*header, 14);
	
	// variable types and names
	for (unsigned int i=0; i < m_NumberOfVariables; i++)
	{
		char name[64];
		unsigned char type = (unsigned char)m_DataTypes[i];
		// type
		memcpy(bptr, &type, 1);
		bptr += 1;
		// name
		strncpy(name, m_VarNames[i], 63);
		name[63] = '\0';
		memcpy(bptr, name, 64);
		bptr += 64;
	}

	return true;
}

bool SimpleVolumeData::createRawIVHeader(char *header)
{
	if (!m_Data)
		return false;

	float span[3], orig[3];
	unsigned int numverts, numcells;

	span[0] = getSpanX();
	span[1] = getSpanY();
	span[2] = getSpanZ();
	
	orig[0] = m_Min[0];
	orig[1] = m_Min[1];
	orig[2] = m_Min[2];

#ifdef USE_MPI
	if (m_MergeOutput) {
		numverts = m_Dims[0]*m_Dims[1]*m_Dims[2];
		numcells = (m_Dims[0]-1)*(m_Dims[1]-1)*(m_Dims[2]-1);
	}
	else {
		numverts = m_Dims[0]*m_Dims[1]*m_Dims[3];
		numcells = (m_Dims[0]-1)*(m_Dims[1]-1)*(m_Dims[3]-1);
		orig[2] += m_ZOff*span[2];
	}
#else
	numverts = m_Dims[0]*m_Dims[1]*m_Dims[2];
	numcells = (m_Dims[0]-1)*(m_Dims[1]-1)*(m_Dims[2]-1);
#endif
	
	// minX
	memcpy(header, &m_Min[0], sizeof(float));
	header += 4;
	// minY
	memcpy(header, &m_Min[1], sizeof(float));
	header += 4;
	// minZ
	memcpy(header, &m_Min[2], sizeof(float));
	header += 4;
	
	// maxX
	memcpy(header, &m_Max[0], sizeof(float));
	header += 4;
	// maxY
	memcpy(header, &m_Max[1], sizeof(float));
	header += 4;
	// maxZ
	memcpy(header, &m_Max[2], sizeof(float));
	header += 4;
	
	// numverts 
	memcpy(header, &numverts, sizeof(numverts));
	header += 4;
	
	// numcells 
	memcpy(header, &numcells, sizeof(numcells));
	header += 4;
	
	// dimX
	memcpy(header, &m_Dims[0], sizeof(unsigned int));
	header += 4;
	// dimY
	memcpy(header, &m_Dims[1], sizeof(unsigned int));
	header += 4;
	// dimZ
#ifdef USE_MPI
	if (!m_MergeOutput)
		memcpy(header, &m_Dims[3], sizeof(unsigned int));
	else
#endif
	memcpy(header, &m_Dims[2], sizeof(unsigned int));
	header += 4;
	
	// originX
	memcpy(header, &orig[0], sizeof(float));
	header += 4;
	// originY
	memcpy(header, &orig[1], sizeof(float));
	header += 4;
	// originZ
	memcpy(header, &orig[2], sizeof(float));
	header += 4;
	
	// spanX
	memcpy(header, &span[0], sizeof(float));
	header += 4;
	// spanY
	memcpy(header, &span[1], sizeof(float));
	header += 4;
	// spanZ 
	memcpy(header, &span[2], sizeof(float));
	
	header -= 64;
	if (isLittleEndian())
		swapByteOrder((unsigned int *)header, 17);

	return true;
}

void SimpleVolumeData::makeVariablesBigEndian()
{
	if (m_Data && isLittleEndian()) {
		for (unsigned int i=0; i < m_NumberOfVariables; i++) {
			switch (m_DataTypes[i])
			{
				case USHORT:
					swapByteOrder((unsigned short *)m_Data[i], m_Dims[0]*m_Dims[1]*m_Dims[3]);
					break;
				case ULONG:
					swapByteOrder((unsigned int *)m_Data[i], m_Dims[0]*m_Dims[1]*m_Dims[3]);
					break;
				case FLOAT:
					swapByteOrder((float *)m_Data[i], m_Dims[0]*m_Dims[1]*m_Dims[3]);
					break;
				case DOUBLE:
					swapByteOrder((double *)m_Data[i], m_Dims[0]*m_Dims[1]*m_Dims[3]);
					break;
				default:
					break;
			}
		}
	}
}

void SimpleVolumeData::setVariablesToZero()
{
	if (m_Data) {
		for (unsigned int i=0; i < m_NumberOfVariables; i++) {
			unsigned int j;
			switch (m_DataTypes[i]) {
				case UCHAR:
				{
					unsigned char *dp = (unsigned char *)m_Data[i];
					for (j=0; j < m_Dims[0]*m_Dims[1]*m_Dims[3]; j++)
						dp[j] = (unsigned char)0;
					break;
				}
				case USHORT:
				{
					unsigned short *dp = (unsigned short *)m_Data[i];
					for (j=0; j < m_Dims[0]*m_Dims[1]*m_Dims[3]; j++)
						dp[j] = (unsigned short)0;
					break;
				}
				case ULONG:
				{
					unsigned int *dp = (unsigned int *)m_Data[i];
					for (j=0; j < m_Dims[0]*m_Dims[1]*m_Dims[3]; j++)
						dp[j] = (unsigned int)0;
					break;
				}
				case FLOAT:
				{
					float *dp = (float *)m_Data[i];
					for (j=0; j < m_Dims[0]*m_Dims[1]*m_Dims[3]; j++)
						dp[j] = (float)0.0;
					break;
				}
				case DOUBLE:
				{
					double *dp = (double *)m_Data[i];
					for (j=0; j < m_Dims[0]*m_Dims[1]*m_Dims[3]; j++)
						dp[j] = (double)0.0;
					break;
				}
				default:
					break;
			}
		}
	}
}

void* SimpleVolumeData::getData( unsigned int variable)
{
	if (variable < m_NumberOfVariables) {
		return m_Data[variable];
	}

	return 0;
}

int SimpleVolumeData::getDataSize( unsigned int variable )
{
	return getTypeSize(variable) * m_Dims[0]*m_Dims[1]*m_Dims[3];
}

int SimpleVolumeData::getTypeSize( unsigned int variable )
{
	int ret=1;

	switch (m_DataTypes[variable])
	{
		case UCHAR:
			ret = 1;
			break;
		case USHORT:
			ret = 2;
			break;
		case ULONG:
			ret = 4;
			break;
		case FLOAT:
			ret = 4;
			break;
		case DOUBLE:
			ret = 8;
			break;
		default:
			break;
	}

	return ret;
}

SimpleVolumeData::DataType SimpleVolumeData::getType( unsigned int variable )
{
	if (variable < m_NumberOfVariables) {
		return m_DataTypes[variable];
	}

	return NO_TYPE;
}

unsigned int SimpleVolumeData::getNumberOfVariables()
{
	return m_NumberOfVariables;
}

void SimpleVolumeData::setDefaults()
{
	m_Data = 0;
	m_DataTypes = 0;
	m_VarNames = 0;

	m_NumberOfVariables = 0;

	m_Dims[0] = m_Dims[1] = m_Dims[2] = m_Dims[3] = 0;
	m_Min[0] = m_Min[1] = m_Min[2] = 0.0;
	m_Max[0] = m_Max[1] = m_Max[2] = 0.0;
#ifdef USE_MPI
	m_ZOff = 0;
	m_MergeOutput = false;
#endif
}

char** SimpleVolumeData::getVariableNames()
{
	return m_VarNames;
}

double SimpleVolumeData::getDistanceOfVoxel(int i, int j, int k, int width, int height, int depth)
{
	return (sqrt((double)((i)-width/2)*((i)-width/2)+((j)-height/2)*((j)-height/2)+((k)-depth/2)*((k)-depth/2)));
}

SimpleVolumeData* SimpleVolumeData::createDepthColoredVolume( double* colorMap, int colorMapSize )
{
	if( !colorMap || (colorMapSize < 1) ) return 0;
	if( getNumberOfVariables() < 1 ) return 0;

	int width  = getWidth();
	int height = getHeight();
	int depth  = getDepth();

	/////// create new data ///////////////
	unsigned char* red   = new unsigned char[width*height*depth];
	unsigned char* green = new unsigned char[width*height*depth];
	unsigned char* blue  = new unsigned char[width*height*depth];
	float* alpha = new float[width*height*depth];

	int c;
	for( c=0; c<width*height*depth; c++ )
	{
		red[c] = green[c] = blue[c] = 0;
		alpha[c] = 0;
	}
	///////////////////////////////////////


	int i, j, k;
	double maxDistance = -1.0;
	double minDistance = 10000000.0;

	//////////// determine which data set and type to use ///////////
	DataType type = UCHAR;
	int volumeIndex = 0;
	if( getNumberOfVariables() < 4 )
	{
		volumeIndex = 0;
		type = getType(0);
	}
	else
	{
		volumeIndex = 3;
		type = getType(3);
	}
	//////////////////////////////////////////////////////////////////


	switch( type )
	{
	case UCHAR:
		{
			//////// get old data and min, max distances //////////////
			unsigned char* oldDensity = 0;
			oldDensity = (unsigned char*)getData(volumeIndex);

			for( i=0; i<width; i++ )
			{
				for( j=0; j<height; j++ )
				{
					for( k=0; k<depth; k++ )
					{
						if( oldDensity[i*height*depth + j*depth + k] > 0 )
						{
							double dist = getDistanceOfVoxel(i, j, k, width, height, depth);
							if( dist > maxDistance ) maxDistance = dist;
							if( dist < minDistance ) minDistance = dist;
						}
					}
				}
			}
			///////////////////////////////////////////////////////////


			//////////// depth color the new volume ///////////////////
			c = 0;
			for( i=0; i<width; i++ )
			{
				for( j=0; j<height; j++ )
				{
					for( k=0; k<depth; k++ )
					{
						double dist = getDistanceOfVoxel(i, j, k, width, height, depth);
						if( dist < minDistance ) { c++; continue; }
						if( dist > maxDistance ) { c++; continue; }

						int index = (int)(( dist - minDistance ) / ( maxDistance - minDistance +1 ) * colorMapSize);
						red[c]		= (unsigned char)(colorMap[index*4+0] * colorMapSize);
						green[c]	= (unsigned char)(colorMap[index*4+1] * colorMapSize);
						blue[c]		= (unsigned char)(colorMap[index*4+2] * colorMapSize);
						alpha[c]	= (float)(colorMap[index*4+3] * oldDensity[c]);
						c++;
					}
				}
			}
			/////////////////////////////////////////////////////////////
		}
		break;
	case USHORT:
		{
			//////// get old data and min, max distances //////////////
			unsigned short* oldDensity = 0;
			oldDensity = (unsigned short*)getData(volumeIndex);

			for( i=0; i<width; i++ )
			{
				for( j=0; j<height; j++ )
				{
					for( k=0; k<depth; k++ )
					{
						if( oldDensity[i*height*depth + j*depth + k] > 0 )
						{
							double dist = getDistanceOfVoxel(i, j, k, width, height, depth);
							if( dist > maxDistance ) maxDistance = dist;
							if( dist < minDistance ) minDistance = dist;
						}
					}
				}
			}
			///////////////////////////////////////////////////////////


			//////////// depth color the new volume ///////////////////
			c = 0;
			for( i=0; i<width; i++ )
			{
				for( j=0; j<height; j++ )
				{
					for( k=0; k<depth; k++ )
					{
						double dist = getDistanceOfVoxel(i, j, k, width, height, depth);
						if( dist < minDistance ) { c++; continue; }
						if( dist > maxDistance ) { c++; continue; }

						int index = (int)(( dist - minDistance ) / ( maxDistance - minDistance +1 ) * colorMapSize);
						red[c]		= (unsigned char)(colorMap[index*4+0] * colorMapSize);
						green[c]	= (unsigned char)(colorMap[index*4+1] * colorMapSize);
						blue[c]		= (unsigned char)(colorMap[index*4+2] * colorMapSize);
						alpha[c]	= (float)(colorMap[index*4+3] * oldDensity[c]);
						c++;
					}
				}
			}
			/////////////////////////////////////////////////////////////
		}
		break;
	case ULONG:
		{
			//////// get old data and min, max distances //////////////
			unsigned long* oldDensity = 0;
			oldDensity = (unsigned long*)getData(volumeIndex);

			for( i=0; i<width; i++ )
			{
				for( j=0; j<height; j++ )
				{
					for( k=0; k<depth; k++ )
					{
						if( oldDensity[i*height*depth + j*depth + k] > 0 )
						{
							double dist = getDistanceOfVoxel(i, j, k, width, height, depth);
							if( dist > maxDistance ) maxDistance = dist;
							if( dist < minDistance ) minDistance = dist;
						}
					}
				}
			}
			///////////////////////////////////////////////////////////


			//////////// depth color the new volume ///////////////////
			c = 0;
			for( i=0; i<width; i++ )
			{
				for( j=0; j<height; j++ )
				{
					for( k=0; k<depth; k++ )
					{
						double dist = getDistanceOfVoxel(i, j, k, width, height, depth);
						if( dist < minDistance ) { c++; continue; }
						if( dist > maxDistance ) { c++; continue; }

						int index = (int)(( dist - minDistance ) / ( maxDistance - minDistance +1 ) * colorMapSize);
						red[c]		= (unsigned char)(colorMap[index*4+0] * colorMapSize);
						green[c]	= (unsigned char)(colorMap[index*4+1] * colorMapSize);
						blue[c]		= (unsigned char)(colorMap[index*4+2] * colorMapSize);
						alpha[c]	= (float)(colorMap[index*4+3] * oldDensity[c]);
						c++;
					}
				}
			}
			/////////////////////////////////////////////////////////////
		}
		break;
	case FLOAT:
		{
			//////// get old data and min, max distances //////////////
			float* oldDensity = 0;
			oldDensity = (float*)getData(volumeIndex);

			for( i=0; i<width; i++ )
			{
				for( j=0; j<height; j++ )
				{
					for( k=0; k<depth; k++ )
					{
						if( oldDensity[i*height*depth + j*depth + k] > 0 )
						{
							double dist = getDistanceOfVoxel(i, j, k, width, height, depth);
							if( dist > maxDistance ) maxDistance = dist;
							if( dist < minDistance ) minDistance = dist;
						}
					}
				}
			}
			///////////////////////////////////////////////////////////

			{
				int i;
				for( i=0; i<colorMapSize; i++ )
				{
					printf("%d = [%lf %lf %lf %lf]\n", i, 
						colorMap[i*4+0],
						colorMap[i*4+1],
						colorMap[i*4+2],
						colorMap[i*4+3] );
				}
			}

			//////////// depth color the new volume ///////////////////
			c = 0;
			for( i=0; i<width; i++ )
			{
				for( j=0; j<height; j++ )
				{
					for( k=0; k<depth; k++ )
					{
						double dist = getDistanceOfVoxel(i, j, k, width, height, depth);
						if( dist < minDistance ) 
						{ 
							c++; 
							continue; 
						}
						if( dist > maxDistance ) 
						{ 
							c++; 
							continue; 
						}

						int index = (int)(( dist - minDistance ) / ( maxDistance - minDistance +1 ) * colorMapSize);
						red[c]		= (unsigned char)(colorMap[index*4+0] * colorMapSize);
						green[c]	= (unsigned char)(colorMap[index*4+1] * colorMapSize);
						blue[c]		= (unsigned char)(colorMap[index*4+2] * colorMapSize);
						alpha[c]	= (float)(colorMap[index*4+3] * oldDensity[c]);
						c++;
					}
				}
			}
			/////////////////////////////////////////////////////////////
		}
		break;
	case DOUBLE:
		{
			//////// get old data and min, max distances //////////////
			double* oldDensity = 0;
			oldDensity = (double*)getData(volumeIndex);

			for( i=0; i<width; i++ )
			{
				for( j=0; j<height; j++ )
				{
					for( k=0; k<depth; k++ )
					{
						if( oldDensity[i*height*depth + j*depth + k] > 0 )
						{
							double dist = getDistanceOfVoxel(i, j, k, width, height, depth);
							if( dist > maxDistance ) maxDistance = dist;
							if( dist < minDistance ) minDistance = dist;
						}
					}
				}
			}
			///////////////////////////////////////////////////////////


			//////////// depth color the new volume ///////////////////
			c = 0;
			for( i=0; i<width; i++ )
			{
				for( j=0; j<height; j++ )
				{
					for( k=0; k<depth; k++ )
					{
						double dist = getDistanceOfVoxel(i, j, k, width, height, depth);
						if( dist < minDistance ) { c++; continue; }
						if( dist > maxDistance ) { c++; continue; }

						int index = (int)(( dist - minDistance ) / ( maxDistance - minDistance +1 ) * colorMapSize);
						red[c]		= (unsigned char)(colorMap[index*4+0] * colorMapSize);
						green[c]	= (unsigned char)(colorMap[index*4+1] * colorMapSize);
						blue[c]		= (unsigned char)(colorMap[index*4+2] * colorMapSize);
						alpha[c]	= (float)(colorMap[index*4+3] * oldDensity[c]);
						c++;
					}
				}
			}
			/////////////////////////////////////////////////////////////
		}
		break;
	default:
		{
			delete []red; 
			delete []green;
			delete []blue;
			delete []alpha;
			return 0;
		}
		break;
	};

	//////////// set and return new volume ////////////////////
	SimpleVolumeData* newVolume = new SimpleVolumeData( width, height, depth );
	newVolume->setNumberOfVariables(4);

	newVolume->setType( 0, SimpleVolumeData::UCHAR );
	newVolume->setType( 1, SimpleVolumeData::UCHAR );
	newVolume->setType( 2, SimpleVolumeData::UCHAR );
	newVolume->setType( 3, SimpleVolumeData::FLOAT );

	newVolume->setName( 0, "red" );
	newVolume->setName( 1, "green" );
	newVolume->setName( 2, "blue" );
	newVolume->setName( 3, "density" );

	newVolume->setData( 0, red );
	newVolume->setData( 1, green );
	newVolume->setData( 2, blue );
	newVolume->setData( 3, alpha );


	newVolume->setMinExtent( m_Min );
	newVolume->setMaxExtent( m_Max );
	return newVolume;
	///////////////////////////////////////////////////////////
}

bool SimpleVolumeData::readRawVFile(const char *filename)
{
	FILE *fp = fopen(filename, "rb");
	unsigned int i, magic, dims[3], numTimeSteps, numVariables;
	float minExt[4], maxExt[4];

	// check to make sure the file exists
	if (!fp) {
		printf("Error: could not open file\n");
		return false;
	}

	// read the header
	//
	// read the magic number
	fread(&magic, 1, sizeof(unsigned int), fp);
	if (isLittleEndian()) swapByteOrder(&magic, 1);
	if (magic != 0xBAADBEEF) {
		// doh! this isn't a RawV file
		printf("Error: file format is not recognized. (expecting RawV)\n");
		fclose(fp);
		return false;
	}
	// the dimensions, # of timesteps, # of variables, and min/max extents
	fread(dims, 3, sizeof(unsigned int), fp);
	fread(&numTimeSteps, 1, sizeof(unsigned int), fp);
	fread(&numVariables, 1, sizeof(unsigned int), fp);
	fread(minExt, 4, sizeof(float), fp);
	fread(maxExt, 4, sizeof(float), fp);
	if (isLittleEndian()) {
		swapByteOrder(dims, 3);
		swapByteOrder(&numTimeSteps, 1);
		swapByteOrder(&numVariables, 1);
		swapByteOrder(minExt, 4);
		swapByteOrder(maxExt, 4);
	}

	// call a bunch of set...() functions
	//
	// setNumberOfVariables() has the convenient side effect of cleaning up
	// any data that this instance might already be holding. (effectively wiping
	// the slate clean)
	setNumberOfVariables(numVariables);
	// the rest ...
	setDimensions(dims);
	setMinExtent(minExt);
	setMaxExtent(maxExt);

	// finish reading the header (variable names and types)
	for (i=0; i < m_NumberOfVariables; i++) {
		char name[64];
		unsigned char type;

		// read the type and the name of a variable
		fread(&type, 1, sizeof(unsigned char), fp);
		fread(name, 64, sizeof(char), fp);
		name[63] = '\0'; // just in case
		// call set...() functions
		switch(type) {
			case 1:	setType(i, UCHAR); break;
			case 2: setType(i, USHORT); break;
			case 3: setType(i, ULONG); break;
			case 4: setType(i, FLOAT); break;
			case 5: setType(i, DOUBLE); break;
			default: break;
		}
		setName(i, name);
	}

	// now that all the meta-data is filled in, allocate space for the variables
	for (i=0; i < m_NumberOfVariables; i++)
		m_Data[i] = new unsigned char [getDataSize(i)];
	
	// finally, read the data
	for (i=0; i < m_NumberOfVariables; i++) {
		// read the data for this variables first (and only?) timestep
		fread(m_Data[i], getDataSize(i), 1, fp);
		// swap the byte order if needed
		if (isLittleEndian()) {
			switch(getTypeSize(i)) {
				case 1:
					break;
				case 2:
					swapByteOrder((unsigned short *)m_Data[i], m_Dims[0]*m_Dims[1]*m_Dims[2]);
					break;
				case 4:
					swapByteOrder((float *)m_Data[i], m_Dims[0]*m_Dims[1]*m_Dims[2]);
					break;
				case 8:
					swapByteOrder((double *)m_Data[i], m_Dims[0]*m_Dims[1]*m_Dims[2]);
					break;
				default:
					break;
			}
		}
		// seek past any remaining timesteps
		if (numTimeSteps > 1)
			fseek(fp, getDataSize(i)*(numTimeSteps-1), SEEK_CUR);
	}
	
	// close the file
	fclose(fp);
	
	return true;
}

bool SimpleVolumeData::readRawIVFile(const char *filename)
{
	FILE *fp=fopen(filename, "rb");
	unsigned int dims[3], numverts, numcells;
	float minExt[3], maxExt[3], orig[3], span[3];

	// check to make sure the file exists
	if (!fp) {
		printf("Error: could not open file\n");
		return false;
	}

	// read the header
	// 
	fread(minExt, 3, sizeof(float), fp);
	fread(maxExt, 3, sizeof(float), fp);
	fread(&numverts, 1, sizeof(unsigned int), fp);
	fread(&numcells, 1, sizeof(unsigned int), fp);
	fread(dims, 3, sizeof(unsigned int), fp);
	fread(orig, 3, sizeof(float), fp);
	fread(span, 3, sizeof(float), fp);
	if (isLittleEndian()) {
		swapByteOrder(minExt, 3);
		swapByteOrder(maxExt, 3);
		swapByteOrder(&numverts, 1);
		swapByteOrder(&numcells, 1);
		swapByteOrder(dims, 3);
		swapByteOrder(orig, 3);
		swapByteOrder(span, 3);
	}

	// find out how large the data is
	long dataStart = ftell(fp), dataSize;
	fseek(fp, 0, SEEK_END);
	dataSize = ftell(fp) - dataStart;
	fseek(fp, dataStart, SEEK_SET);

	// a small sanity check to make sure this file is valid
	if ((dataSize % ((long)dims[0]*dims[1]*dims[2])) != 0) {
		printf("Error: rawiv file header dimensions don't match file size");
		fclose(fp);
		return false;
	}
	
	// call some set...() functions
	//
	setNumberOfVariables(1);
	setDimensions(dims);
	setMinExtent(minExt);
	setMaxExtent(maxExt);

	// figure out the data type
	switch (dataSize / ((long)dims[0]*dims[1]*dims[2])) {
		case 1: setType(0, UCHAR); break;
		case 2: setType(0, USHORT); break;
		case 4: setType(0, FLOAT); break;
		default: setType(0, NO_TYPE); break;
	}

	// allocate space for the data
	m_Data[0] = new unsigned char [dataSize];

	// read the data
	fread(m_Data[0], dataSize, 1, fp);
	// swap the byte order if needed
	if (isLittleEndian()) {
		switch(getTypeSize(0)) {
			case 1:
				break;
			case 2:
				swapByteOrder((unsigned short *)m_Data[0], m_Dims[0]*m_Dims[1]*m_Dims[2]);
				break;
			case 4:
				swapByteOrder((float *)m_Data[0], m_Dims[0]*m_Dims[1]*m_Dims[2]);
				break;
			case 8:
				swapByteOrder((double *)m_Data[0], m_Dims[0]*m_Dims[1]*m_Dims[2]);
				break;
			default:
				break;
		}
	}

	// close the file
	fclose(fp);

	return true;
}

bool SimpleVolumeData::load( const char* fname )
{
	int len = strlen(fname);

	// these functions need to be changed to be a case insensitive comparision.
	// Ofcourse MS is not compatible with g++. 


	//////////// check if rawv and load it /////////////////
	if( len < 6 ) return false;
	if( strcmp( &(fname[len-4]), "rawv" ) == 0 )
		return readRawVFile( fname );
	if( strcmp( &(fname[len-4]), "RAWV" ) == 0 )
		return readRawVFile( fname );
	if( strcmp( &(fname[len-4]), "RawV" ) == 0 )
		return readRawVFile( fname );
	/////////////////////////////////////////////////////////

	//////////// check if rawv and load it /////////////////
	if( len < 7 ) return false;

	if( strcmp( &(fname[len-5]), "rawiv" ) == 0 )
		return readRawIVFile( fname );
	if( strcmp( &(fname[len-5]), "RAWIV" ) == 0 )
		return readRawIVFile( fname );
	if( strcmp( &(fname[len-5]), "RawIV" ) == 0 )
		return readRawIVFile( fname );
	/////////////////////////////////////////////////////////
	
	return false;
}

bool SimpleVolumeData::getMinMax( unsigned char* data, int width, int height, int depth, unsigned char* minVal, unsigned char* maxVal )
{
	if( !data || !minVal || !maxVal || (width<1) || (height<1) || (depth<1) ) return false;

	int size = width*height*depth;

	///////// init  min max vals /////////
	*minVal = *maxVal = data[0];
	//////////////////////////////////////


	/////// search dataset ///////////////
	int i;
	for( i=1; i<size; i++ )
	{
		if( *minVal > data[i] ) 
			*minVal = data[i];
		if( *maxVal < data[i] )
			*maxVal = data[i];
	}
	///////////////////////////////////////

	
	return true;
}

bool SimpleVolumeData::getMinMax( unsigned short* data, int width, int height, int depth, unsigned short* minVal, unsigned short* maxVal )
{
	if( !data || !minVal || !maxVal || (width<1) || (height<1) || (depth<1) ) return false;

	int size = width*height*depth;

	///////// init  min max vals /////////
	*minVal = *maxVal = data[0];
	//////////////////////////////////////


	/////// search dataset ///////////////
	int i;
	for( i=1; i<size; i++ )
	{
		if( *minVal > data[i] ) 
			*minVal = data[i];
		if( *maxVal < data[i] )
			*maxVal = data[i];
	}
	///////////////////////////////////////

	
	return true;
}

bool SimpleVolumeData::getMinMax( unsigned long* data, int width, int height, int depth, unsigned long* minVal, unsigned long* maxVal )
{
	if( !data || !minVal || !maxVal || (width<1) || (height<1) || (depth<1) ) return false;

	int size = width*height*depth;

	///////// init  min max vals /////////
	*minVal = *maxVal = data[0];
	//////////////////////////////////////


	/////// search dataset ///////////////
	int i;
	for( i=1; i<size; i++ )
	{
		if( *minVal > data[i] ) 
			*minVal = data[i];
		if( *maxVal < data[i] )
			*maxVal = data[i];
	}
	///////////////////////////////////////

	
	return true;
}

bool SimpleVolumeData::getMinMax( float* data, int width, int height, int depth, float* minVal, float* maxVal )
{
	if( !data || !minVal || !maxVal || (width<1) || (height<1) || (depth<1) ) return false;

	int size = width*height*depth;

	///////// init  min max vals /////////
	*minVal = *maxVal = data[0];
	//////////////////////////////////////


	/////// search dataset ///////////////
	int i;
	for( i=1; i<size; i++ )
	{
		if( *minVal > data[i] ) 
			*minVal = data[i];
		if( *maxVal < data[i] )
			*maxVal = data[i];
	}
	///////////////////////////////////////

	
	return true;
}

bool SimpleVolumeData::getMinMax( double* data, int width, int height, int depth, double* minVal, double* maxVal )
{
	if( !data || !minVal || !maxVal || (width<1) || (height<1) || (depth<1) ) return false;

	int size = width*height*depth;

	///////// init  min max vals /////////
	*minVal = *maxVal = data[0];
	//////////////////////////////////////


	/////// search dataset ///////////////
	int i;
	for( i=1; i<size; i++ )
	{
		if( *minVal > data[i] ) 
			*minVal = data[i];
		if( *maxVal < data[i] )
			*maxVal = data[i];
	}
	///////////////////////////////////////

	
	return true;
}

/**************************************************************************/
/*                                                                        */
/*  The input data is copied as unsigned char into the output array.      */
/*                                                                        */
/**************************************************************************/
bool SimpleVolumeData::getUnsignedCharData( void* data1, int type, unsigned char* data, int width, int height, int depth )
{
	if( !data1 || !data || (width<1) || (height<1) || (depth<1) ) 
		return false;

	int j;
	int size = width*height*depth;

	
	//////////////switch on the type of the data and copy it ////////////
	switch( type )
	{
	case SimpleVolumeData::UCHAR:
		{
			unsigned char* inputData = (unsigned char*)data1;
			unsigned char minVal = 0, maxVal = 0;

			if( !SimpleVolumeData::getMinMax( inputData, width, height, depth, &minVal, &maxVal ) )
				return false;
			for( j=0; j<size; j++ )
			{
				unsigned char val = inputData[j];
				if( maxVal > minVal )
					data[j] = (unsigned char)(( val - minVal) * 255.0 / ( maxVal - minVal ));
				else
				{
					if( val < 0 ) val = 0;
					if( val > 255 ) val = 255;
					data[j] = (unsigned char)val;
				}
			}
		}
		break;
	case SimpleVolumeData::USHORT:
		{
			unsigned short* inputData = (unsigned short*)data1;
			unsigned short minVal = 0, maxVal = 0;

			if( !SimpleVolumeData::getMinMax( inputData, width, height, depth, &minVal, &maxVal ) )
				return false;
			for( j=0; j<size; j++ )
			{
				unsigned short val = inputData[j];
				if( maxVal > minVal )
					data[j] = (unsigned char)(( val - minVal) * 255.0 / ( maxVal - minVal ));
				else
				{
					if( val < 0 ) val = 0;
					if( val > 255 ) val = 255;
					data[j] = (unsigned char)val;
				}
			}
		}
		break;
	case SimpleVolumeData::ULONG:
		{
			unsigned long* inputData = (unsigned long*)data1;
			unsigned long minVal = 0, maxVal = 0;

			if( !SimpleVolumeData::getMinMax( inputData, width, height, depth, &minVal, &maxVal ) )
				return false;
			for( j=0; j<size; j++ )
			{
				unsigned long val = inputData[j];
				if( maxVal > minVal )
					data[j] = (unsigned char)(( val - minVal) * 255.0 / ( maxVal - minVal ));
				else
				{
					if( val < 0 ) val = 0;
					if( val > 255 ) val = 255;
					data[j] = (unsigned char)val;
				}
			}
		}
		break;
	case SimpleVolumeData::FLOAT:
		{
			float* inputData = (float*)data1;
			float minVal = 0, maxVal = 0;

			if( !SimpleVolumeData::getMinMax( inputData, width, height, depth, &minVal, &maxVal ) )
				return false;
			for( j=0; j<size; j++ )
			{
				float val = inputData[j];
				if( maxVal > minVal )
					data[j] = (unsigned char)(( val - minVal) * 255.0 / ( maxVal - minVal ));
				else
				{
					if( val < 0 ) val = 0;
					if( val > 255 ) val = 255;
					data[j] = (unsigned char)val;
				}
			}
		}
		break;
	case SimpleVolumeData::DOUBLE:
		{
			double* inputData = (double*)data1;
			double minVal = 0, maxVal = 0;

			if( !SimpleVolumeData::getMinMax( inputData, width, height, depth, &minVal, &maxVal ) )
				return false;
			for( j=0; j<size; j++ )
			{
				double val = inputData[j];
				if( maxVal > minVal )
					data[j] = (unsigned char)(( val - minVal) * 255.0 / ( maxVal - minVal ));
				else
				{
					if( val < 0 ) val = 0;
					if( val > 255 ) val = 255;
					data[j] = (unsigned char)val;
				}
			}
		}
		break;
	default:
		return false;
	}
	/////////////////////////////////////////////////////////////////////


	return true;
}

/**************************************************************************/
/*                                                                        */
/*  We take the input R, G, B, A data and combine them into one after     */
/*  converting them to unsigned char type. They are also packed byte by   */
/*  byte as unsigned char.                                                */
/*                                                                        */
/**************************************************************************/
bool SimpleVolumeData::getUnsignedCharData( void* data1, int type1, void* data2, int type2, 
											  void* data3, int type3, void* data4, int type4, 
											  unsigned char* data, int width, int height, int depth )
{
	if( !data1 || !data2 || !data3 || !data4 || !data || (width<1) || 
		(height<1) || (depth<1) ) 
		return false;

	

	///////////////// loop through all 4 data sets /////////////////////
	int i;
	for( i =0; i<4; i++ )
	{
		void* currentInputData = 0;
		int currentInputDataType = 0;


		/////////////// assign current data and its type /////////////
		switch( i )
		{
		case 0:
			currentInputData = data1;
			currentInputDataType = type1;
			break;
		case 1:
			currentInputData = data2;
			currentInputDataType = type2;
			break;
		case 2:
			currentInputData = data3;
			currentInputDataType = type3;
			break;
		case 3:
			currentInputData = data4;
			currentInputDataType = type4;
			break;
		default:
			return false;
		}
		///////////////////////////////////////////////////////////////


		//// assign current data to output data at correct location //////
		int j;
		int size = width*height*depth;
		
		switch( currentInputDataType )
		{
		case SimpleVolumeData::UCHAR:
			{
				unsigned char* inputData = (unsigned char*)currentInputData;
				unsigned char minVal = 0, maxVal = 0;

				if( !SimpleVolumeData::getMinMax( inputData, width, height, depth, &minVal, &maxVal ) )
					return false;
				for( j=0; j<size; j++ )
				{
					unsigned char val = inputData[j];
					if( maxVal > minVal )
						data[j*4+i] = (unsigned char)(( val - minVal) * 255.0 / ( maxVal - minVal ));
					else
					{
						if( val < 0 ) val = 0;
						if( val > 255 ) val = 255;
						data[j*4+i] = (unsigned char)val;
					}
				}
			}
			break;
		case SimpleVolumeData::USHORT:
			{
				unsigned short* inputData = (unsigned short*)currentInputData;
				unsigned short minVal = 0, maxVal = 0;

				if( !SimpleVolumeData::getMinMax( inputData, width, height, depth, &minVal, &maxVal ) )
					return false;
				for( j=0; j<size; j++ )
				{
					unsigned short val = inputData[j];
					if( maxVal > minVal )
						data[j*4+i] = (unsigned char)(( val - minVal) * 255.0 / ( maxVal - minVal ));
					else
					{
						if( val < 0 ) val = 0;
						if( val > 255 ) val = 255;
						data[j*4+i] = (unsigned char)val;
					}
				}
			}
			break;
		case SimpleVolumeData::ULONG:
			{
				unsigned long* inputData = (unsigned long*)currentInputData;
				unsigned long minVal = 0, maxVal = 0;

				if( !SimpleVolumeData::getMinMax( inputData, width, height, depth, &minVal, &maxVal ) )
					return false;
				for( j=0; j<size; j++ )
				{
					unsigned long val = inputData[j];
					if( maxVal > minVal )
						data[j*4+i] = (unsigned char)(( val - minVal) * 255.0 / ( maxVal - minVal ));
					else
					{
						if( val < 0 ) val = 0;
						if( val > 255 ) val = 255;
						data[j*4+i] = (unsigned char)val;
					}
				}
			}
			break;
		case SimpleVolumeData::FLOAT:
			{
				float* inputData = (float*)currentInputData;
				float minVal = 0, maxVal = 0;

				if( !SimpleVolumeData::getMinMax( inputData, width, height, depth, &minVal, &maxVal ) )
					return false;
				for( j=0; j<size; j++ )
				{
					float val = inputData[j];
					if( maxVal > minVal )
						data[j*4+i] = (unsigned char)(( val - minVal) * 255.0 / ( maxVal - minVal ));
					else
					{
						if( val < 0 ) val = 0;
						if( val > 255 ) val = 255;
						data[j*4+i] = (unsigned char)val;
					}
				}
			}
			break;
		case SimpleVolumeData::DOUBLE:
			{
				double* inputData = (double*)currentInputData;
				double minVal = 0, maxVal = 0;

				if( !SimpleVolumeData::getMinMax( inputData, width, height, depth, &minVal, &maxVal ) )
					return false;
				for( j=0; j<size; j++ )
				{
					double val = inputData[j];
					if( maxVal > minVal )
						data[j*4+i] = (unsigned char)(( val - minVal) * 255.0 / ( maxVal - minVal ));
					else
					{
						if( val < 0 ) val = 0;
						if( val > 255 ) val = 255;
						data[j*4+i] = (unsigned char)val;
					}
				}
			}
			break;
		default:
			return false;
		}
		////////////////////////////////////////////////////////////////////


	}
	/////////////////////////////////////////////////////////////////////


	return true;
}

bool SimpleVolumeData::getMinMax( unsigned int variable, double* minVal, double* maxVal )
{
	int size = getWidth() * getHeight() * getDepth();
	if( size < 1 ) return false;

	int i;
	switch (getType( variable ))
	{
		case UCHAR:
			{
				unsigned char* data = (unsigned char*)getData(variable);
				if( !data ) return false;
				*minVal = *maxVal = data[0];
				for( i=1; i<size; i++ )
				{
					if( data[i] > *maxVal ) *maxVal = data[i];
					if( data[i] < *minVal ) *minVal = data[i];
				}
				return true;
			}
		case USHORT:
			{
				unsigned short* data = (unsigned short*)getData(variable);
				if( !data ) return false;
				*minVal = *maxVal = data[0];
				for( i=1; i<size; i++ )
				{
					if( data[i] > *maxVal ) *maxVal = data[i];
					if( data[i] < *minVal ) *minVal = data[i];
				}
				return true;
			}
		case ULONG:
			{
				unsigned long* data = (unsigned long*)getData(variable);
				if( !data ) return false;
				*minVal = *maxVal = data[0];
				for( i=1; i<size; i++ )
				{
					if( data[i] > *maxVal ) *maxVal = data[i];
					if( data[i] < *minVal ) *minVal = data[i];
				}
				return true;
			}
		case FLOAT:
			{
				float* data = (float*)getData(variable);
				if( !data ) return false;
				*minVal = *maxVal = data[0];
				for( i=1; i<size; i++ )
				{
					if( data[i] > *maxVal ) *maxVal = data[i];
					if( data[i] < *minVal ) *minVal = data[i];
				}
				return true;
			}
		case DOUBLE:
			{
				double* data = (double*)getData(variable);
				if( !data ) return false;
				*minVal = *maxVal = data[0];
				for( i=1; i<size; i++ )
				{
					if( data[i] > *maxVal ) *maxVal = data[i];
					if( data[i] < *minVal ) *minVal = data[i];
				}
				return true;
			}
		default:
			return false;
	}
	return false;
}

double SimpleVolumeData::getValueAt( unsigned int variable, double x, double y, double z)
{
	if( !inVolume( x,y,z ) )
		return 0;


	int w = getWidth();
	int h = getHeight();
	int d = getDepth();

	x = (x-m_Min[0]) / (m_Max[0]-m_Min[0])*w;
	y = (y-m_Min[1]) / (m_Max[1]-m_Min[1])*h;
	z = (z-m_Min[2]) / (m_Max[2]-m_Min[2])*d;

	int lowx = (int)x;
	int lowy = (int)y;
	int lowz = (int)z;

	if( lowx < 0 ) lowx = 0;
	if( lowy < 0 ) lowy = 0;
	if( lowz < 0 ) lowz = 0;

	int highx = lowx+1;
	int highy = lowy+1;
	int highz = lowz+1;

	if( highx >= w ) { highx = w-1; lowx = highx-1; }
	if( highy >= h ) { highy = h-1; lowy = highy-1; }
	if( highz >= d ) { highz = d-1; lowz = highz-1; }

	int index1 = lowz*h*w  + lowy*w  + lowx;
	int index2 = lowz*h*w  + lowy*w  + highx;
	int index3 = lowz*h*w  + highy*w + lowx;
	int index4 = lowz*h*w  + highy*w + highx;
	int index5 = highz*h*w + lowy*w  + lowx;
	int index6 = highz*h*w + lowy*w  + highx;
	int index7 = highz*h*w + highy*w + lowx;
	int index8 = highz*h*w + highy*w + highx;

	double v1,v2,v3,v4,v5,v6,v7,v8;
	v1 = v2 = v3 = v4 = v5 = v6 = v7 = v8 = 0;

	switch (getType( variable ))
	{
		case UCHAR:
			{
				unsigned char* data = (unsigned char*)getData(variable);
				if( !data ) return 0;
				v1=(double)(data[index1]);
				v2=(double)(data[index2]);
				v3=(double)(data[index3]);
				v4=(double)(data[index4]);
				v5=(double)(data[index5]);
				v6=(double)(data[index6]);
				v7=(double)(data[index7]);
				v8=(double)(data[index8]);
				break;
			}
		case USHORT:
			{
				unsigned short* data = (unsigned short*)getData(variable);
				if( !data ) return 0;
				v1=(double)(data[index1]);
				v2=(double)(data[index2]);
				v3=(double)(data[index3]);
				v4=(double)(data[index4]);
				v5=(double)(data[index5]);
				v6=(double)(data[index6]);
				v7=(double)(data[index7]);
				v8=(double)(data[index8]);
				break;
			}
		case ULONG:
			{
				unsigned long* data = (unsigned long*)getData(variable);
				if( !data ) return 0;
				v1=(double)(data[index1]);
				v2=(double)(data[index2]);
				v3=(double)(data[index3]);
				v4=(double)(data[index4]);
				v5=(double)(data[index5]);
				v6=(double)(data[index6]);
				v7=(double)(data[index7]);
				v8=(double)(data[index8]);
				break;
			}
		case FLOAT:
			{
				float* data = (float*)getData(variable);
				if( !data ) return 0;
				v1=(double)(data[index1]);
				v2=(double)(data[index2]);
				v3=(double)(data[index3]);
				v4=(double)(data[index4]);
				v5=(double)(data[index5]);
				v6=(double)(data[index6]);
				v7=(double)(data[index7]);
				v8=(double)(data[index8]);
				break;
			}
		case DOUBLE:
			{
				double* data = (double*)getData(variable);
				if( !data ) return 0;
				v1=(double)(data[index1]);
				v2=(double)(data[index2]);
				v3=(double)(data[index3]);
				v4=(double)(data[index4]);
				v5=(double)(data[index5]);
				v6=(double)(data[index6]);
				v7=(double)(data[index7]);
				v8=(double)(data[index8]);
				break;
			}
		default:
			return 0;
	}

	double normx = x-lowx;
	double normy = y-lowy;
	double normz = z-lowz;

	double trilinearInterpVal = v8 * (1 - normx) * (1 - normy) * (1 - normz) +
								v7 * normx       * (1 - normy) * (1 - normz) +
								v6 *(1 - normx)  * normy       * (1 - normz) +
								v5 * normx       * normy       * (1 - normz) +
								v4 * (1 - normx) * (1 - normy) * normz + 
								v3 * normx       * (1 - normy) * normz + 
								v2 * (1 - normx) *normy        * normz +
								v1 * normx       *normy        * normz;
	//printf(" %lf ", trilinearInterpVal );
	return trilinearInterpVal;
}

bool SimpleVolumeData::inVolume( double x, double y, double z )
{
	if( x<m_Min[0] || x>m_Max[0] || 
		y<m_Min[1] || y>m_Max[1] || 
		z<m_Min[2] || z>m_Max[2] )
		return false;
	return true;
}

double SimpleVolumeData::getValueAt( unsigned int variable, int index)
{
	int w = getWidth();
	int h = getHeight();
	int d = getDepth();

	if( w*h*d <= index )
		return 0;

	switch (getType( variable ))
	{
		case UCHAR:
			{
				unsigned char* data = (unsigned char*)getData(variable);
				if( !data ) return 0;
				return (double)(data[index]);
				break;
			}
		case USHORT:
			{
				unsigned short* data = (unsigned short*)getData(variable);
				if( !data ) return 0;
				return (double)(data[index]);
				break;
			}
		case ULONG:
			{
				unsigned long* data = (unsigned long*)getData(variable);
				if( !data ) return 0;
				return (double)(data[index]);
				break;
			}
		case FLOAT:
			{
				float* data = (float*)getData(variable);
				if( !data ) return 0;
				return (double)(data[index]);
				break;
			}
		case DOUBLE:
			{
				double* data = (double*)getData(variable);
				if( !data ) return 0;
				return (double)(data[index]);
				break;
			}
		default:
			return 0;
	}

	return 0;
}
