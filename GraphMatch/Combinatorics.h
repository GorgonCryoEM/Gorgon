/*
Implementation of combinatorics functions.

Author: Sasakthi S. Abeysinghe
Date  : 04/20/2006
*/

#ifndef COMBINATORICS_H
#define COMBINATORICS_H

namespace wustl_mm {
	namespace GraphMatch {
		class Combinatorics {
		public:
			static unsigned long long Combinations(int n, int r);
			static unsigned long long Permutations(int n, int r);
		};

		unsigned long long Combinatorics::Combinations(int n, int r) {
			long long c = 1;
			for(int i = r+1; i <= n; i++) {
				c = c * i;
			}
			for(int i = 2; i <= (n-r); i++) {
				c = c / i;
			}
			return c;
		}

		unsigned long long Combinatorics::Permutations(int n, int r) {
			long long p = 1;
			for(int i = n-r+1; i <= n; i++) {
				p = p * i;
			}
			return p;
		}
	}
}




#endif