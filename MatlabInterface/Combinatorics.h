#ifndef COMBINATORICS_H
#define COMBINATORICS_H

namespace wustl_mm {
	namespace MatlabInterface {
		class Combinatorics {
		public:
			static unsigned long long Combinations(int n, int r);
			static unsigned long long Permutations(int n, int r);

			static void GetBinomialDistribution(ProbabilityDistribution1D & distributionInfo);
			static void GetBinomialDistribution(ProbabilityDistribution2D & distributionInfo);
			static void GetBinomialDistribution(ProbabilityDistribution3D & distributionInfo);
			static void GetUniformDistribution(ProbabilityDistribution3D & distributionInfo);
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

		void Combinatorics::GetBinomialDistribution(ProbabilityDistribution1D & distributionInfo) {
			int index = 0;
			double total = 0;

			for(int x = -distributionInfo.radius; x <= distributionInfo.radius; x++) {
				distributionInfo.values[index] = (double)Combinations(2 * distributionInfo.radius, x+distributionInfo.radius);
				total += distributionInfo.values[index];
				index++;
			}

			index = 0;
			for(int x = -distributionInfo.radius; x <= distributionInfo.radius; x++) {
				distributionInfo.values[index] = distributionInfo.values[index] / total;
				index++;
			}
		}

		void Combinatorics::GetBinomialDistribution(ProbabilityDistribution2D & distributionInfo) {
			ProbabilityDistribution1D dist1D;
			dist1D.radius = distributionInfo.radius;
			GetBinomialDistribution(dist1D);
			double total = 0;
			for(int x = 0; x < dist1D.radius * 2 +1; x++) {
				for(int y = 0; y < dist1D.radius * 2 +1; y++) {
					distributionInfo.values[x][y] = dist1D.values[x] * dist1D.values[y];
					total += distributionInfo.values[x][y];
				}
			}
		}

		void Combinatorics::GetBinomialDistribution(ProbabilityDistribution3D & distributionInfo) {
			ProbabilityDistribution1D dist1D;
			dist1D.radius = distributionInfo.radius;
			GetBinomialDistribution(dist1D);
			double total = 0;
			for(int x = 0; x < dist1D.radius * 2 +1; x++) {
				for(int y = 0; y < dist1D.radius * 2 +1; y++) {
					for(int z = 0; z < dist1D.radius * 2 +1; z++) {
						distributionInfo.values[x][y][z] = dist1D.values[x] * dist1D.values[y] * dist1D.values[z];
						total += distributionInfo.values[x][y][z];
					}
				}
			}
		}

		void Combinatorics::GetUniformDistribution(ProbabilityDistribution3D & distributionInfo) {
			double probability = 1.0 / (double)(distributionInfo.radius * 2 +1) * (distributionInfo.radius * 2 +1) * (distributionInfo.radius * 2 +1);
			for(int x = 0; x < distributionInfo.radius * 2 +1; x++) {
				for(int y = 0; y < distributionInfo.radius * 2 +1; y++) {
					for(int z = 0; z < distributionInfo.radius * 2 +1; z++) {
						distributionInfo.values[x][y][z] = probability;
					}
				}
			}
		}

	}
}

#endif