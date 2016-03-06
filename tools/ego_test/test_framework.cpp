#include "test_framework.h"


template <>
void CheckDerivativeSanity<TPair<TVectorD, TVectorD>>(TPair<TVectorD, TVectorD> derivVal, TPair<TVectorD, TVectorD> leftVal, TPair<TVectorD, TVectorD> rightVal, std::string name) {
	TVectorD approxDerivFirst = (rightVal.first - leftVal.first)/(2.0*Epsilon);
	TVectorD approxDerivSecond = (rightVal.second - leftVal.second)/(2.0*Epsilon);
	{
		double res = NLa::Sum(derivVal.first - approxDerivFirst);
		res = std::abs(res);
		if(res >= LilEpsilon || std::isnan(res)) {
			L_ERROR << name << ", Derivative of first element of pair is bad: " << res << " >= " << LilEpsilon;
			L_ERROR << "Those values are not almost equal:";
			L_ERROR << "Proposed exact value:";
			L_ERROR << "\n" << derivVal.first;
			L_ERROR << "Real approximate:";
			L_ERROR << "\n" << approxDerivSecond;
			throw TEgoException() << "Derivative sanity check failed for first element of pair";
		}
		L_INFO << name << ", Got derivative sanity check ok for first element of pair: " << res << " < " << LilEpsilon;
	}
	{
		double res = NLa::Sum(derivVal.second - approxDerivSecond);
		res = std::abs(res);
		if(res >= LilEpsilon || std::isnan(res)) {
			L_ERROR << name << ", Derivative second element of pair is bad: " << res << " >= " << LilEpsilon;
			L_ERROR << "Those values are not almost equal:";
			L_ERROR << "Proposed exact value:";
			L_ERROR << "\n" << derivVal.second;
			L_ERROR << "Real approximate:";
			L_ERROR << "\n" << approxDerivSecond;
			throw TEgoException() << "Derivative sanity check failed for second element of pair";
		}
		L_INFO << name << ", Got derivative sanity check ok for second element of pair: " << res << " < " << LilEpsilon;
	}
}

template <>
TMatrixD CreateTestData<TMatrixD>() {
	arma::arma_rng::set_seed_random();
	return TMatrixD(SampleSize, DimSize, arma::fill::randn);
}

template <>
TVectorD CreateTestData<TVectorD>() {
	arma::arma_rng::set_seed_random();
	return TVectorD(SampleSize, arma::fill::randn);
}

TVectorD CreateTestDataVectorY() {
	return CreateTestData<TVectorD>();
}

TVectorD CreateTestDataVectorX() {
	arma::arma_rng::set_seed_random();
	return TVectorD(DimSize, arma::fill::randn);
}
