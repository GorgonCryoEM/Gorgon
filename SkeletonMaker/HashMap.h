// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Tao Ju (taoju@cse.wustl.edu)
// Description:   A Hash Map implementation


#ifndef SKELETON_MAKER_HASH_MAP_H
#define SKELETON_MAKER_HASH_MAP_H

#include <iostream>
#include <cstdio>
#include <windows.h>

namespace wustl_mm {
	namespace SkeletonMaker {

		/* A hash table hashed by three int integers
		 *
		 */

		const int  MAX_HASH = 1<<15;
		const int HASH_LENGTH = 15;
		const int HASH_BIT_1 = 5;
		const int HASH_BIT_2 = 5;
		const int HASH_BIT_3 = 5;

		struct HashElement
		{
		/// Key of hash element
			int key[3];
		/// Actually content of hash element
			int index ;
		/// Link when collision
			HashElement  *nextHash;
		};

		class HashMap
		{
			/// Hash table
			HashElement *table[MAX_HASH];

			/// Create hash key
			int createKey( int k1, int k2, in k3 )
			{
				int ind = ((((( k1 & ( (1<<HASH_BIT_1) - 1 )) << ( HASH_BIT_2  )) |
							( k2 & ( (1<<HASH_BIT_2) - 1)) ) << ( HASH_BIT_3  )) |
							( k3 & ( (1<<HASH_BIT_3) - 1)))	& ((1<<HASH_LENGTH) - 1);

				return ind;
			}

		public:

			/// Constructor
			HashMap ( )
			{
				for (int i = 0; i < MAX_HASH; i ++)
				{
					table[i] = NULL;
				}
			};

			/// Lookup Method
			int findInsert( int k1, int k2, int k3, int index )
			{
				/// Create hash key
				int ind = createKey ( k1, k2, k3 );

				/// Find it in the table
				HashElement *p = table[ind];
				while (p)
				{
					if ((p->key[0] == k1) && (p->key[1] == k2) && (p->key[2] == k3) )
					{
						return p->index ;
					}
					p = p->nextHash;
				}

				// Not found
				p = new HashElement ;
				p->key[0] = k1;
				p->key[1] = k2;
				p->key[2] = k3;
				p->index = index ;
				p->nextHash = table[ind];
				table[ind] = p;

				return index ;
			};


			// Destruction method
			~HashMap()
			{
				HashElement *p, *pp;

				for (int i = 0; i < MAX_HASH; i ++)
				{
					p = table[i];

					while (p)
					{
						pp = p->nextHash;
						delete p;
						p = pp;
					}

				}

			};

		};
	}
}

#endif
