/*----------------------------------------------------
 * gene_mesh.h
 * Interface of gene_mesh.cc
 * Random generation of isomorphic ARGraphs which are
 * almost meshes.
 * See: argraph.h
 *
 * Author: P. Foggia
 * $Id$
 ----------------------------------------------------*/

/*-----------------------------------------------------------------
 * REVISION HISTORY
 *   $Log$
 *   Revision 1.2  1999/01/06 20:03:53  foggia
 *   Added the option to generate subgraphs
 *
 *   Revision 1.1  1998/12/26 17:28:14  foggia
 *   Initial revision
 *
 *   Revision 1.2  1998/12/08 13:31:01  foggia
 *   Minor changes
 *
 *---------------------------------------------------------------*/

#ifndef GENE_MESH_H

#include "argraph.h"



void GenerateMesh(int nodes, int extra_edges, Graph **g1, Graph **g2,
                  int sub_nodes=-1);

#endif
