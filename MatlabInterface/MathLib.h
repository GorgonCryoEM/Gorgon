#ifndef MATH_LIB_H
#define MATH_LIB_H

#include "MatlabWrapper.h"
#include "Combinatorics.h"
#include "VectorLib.h"
#include "ComplexNumber.h"

namespace wustl_mm {
	namespace MatlabInterface {

		class MathLib {
		public:
			MathLib();
			~MathLib();

			void EigenAnalysis(EigenVectorsAndValues2D & eigenInformation);
			void EigenAnalysis(EigenVectorsAndValues3D & eigenInformation);
			void GetBinomialDistribution(ProbabilityDistribution1D & distributionInfo);
			void GetBinomialDistribution(ProbabilityDistribution2D & distributionInfo);
			void GetBinomialDistribution(ProbabilityDistribution3D & distributionInfo);
			unsigned long long Combinations(int n, int r);
			unsigned long long Permutations(int n, int r);

		private:
			MatlabWrapper * mathWrapper;
		};

		MathLib::MathLib() {
			mathWrapper = new MatlabWrapper();
		}

		MathLib::~MathLib() {
			delete mathWrapper;
		}

		void MathLib::EigenAnalysis(EigenVectorsAndValues2D & eigenInformation) {
			return mathWrapper->EigenAnalysis(eigenInformation);
		}

		void MathLib::EigenAnalysis(EigenVectorsAndValues3D & eigenInformation) {
			return mathWrapper->EigenAnalysis(eigenInformation);
		}

		void MathLib::GetBinomialDistribution(ProbabilityDistribution1D & distributionInfo) {
			return Combinatorics::GetBinomialDistribution(distributionInfo);
		}

		void MathLib::GetBinomialDistribution(ProbabilityDistribution2D & distributionInfo) {
			return Combinatorics::GetBinomialDistribution(distributionInfo);
		}

		void MathLib::GetBinomialDistribution(ProbabilityDistribution3D & distributionInfo) {
			return Combinatorics::GetBinomialDistribution(distributionInfo);
		}

		unsigned long long MathLib::Combinations(int n, int r) {
			return Combinatorics::Combinations(n, r);
		}

		unsigned long long MathLib::Permutations(int n, int r) {
			return Combinatorics::Permutations(n, r);
		}
	}
}

#endif