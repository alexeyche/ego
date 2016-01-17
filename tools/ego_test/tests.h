#pragma once

#include "register.h"

#include <ego/util/string.h>
#include <ego/base/la.h>
#include <ego/util/log/log.h>

using namespace NEgo;

constexpr double Epsilon = 1e-4;
constexpr double LilEpsilon = 1e-5;
constexpr ui32 DimSize = 5;
constexpr ui32 SampleSize = 15;

template <typename T>
void CheckDerivativeSanity(T derivVal, T leftVal, T rightVal, std::string name) {
	T approxDeriv = (rightVal - leftVal)/(2.0*Epsilon);
	double res = NLa::Sum(derivVal - approxDeriv);
	res = std::abs(res);
	if(res >= LilEpsilon) {
		L_ERROR << name << ", Derivative is bad: " << res << " >= " << LilEpsilon;
		L_ERROR << "Those values are not almost equal:";
		L_ERROR << "Proposed exact value:";
		L_ERROR << "\n" << derivVal;
		L_ERROR << "Real approximate:";
		L_ERROR << "\n" << approxDeriv;
		throw TEgoException() << "Derivative sanity check failed";
	}
	L_INFO << name << ", Got derivative sanity check ok: " << res << " < " << LilEpsilon;
}

template <>
void CheckDerivativeSanity<TPair<TVectorD, TVectorD>>(TPair<TVectorD, TVectorD> derivVal, TPair<TVectorD, TVectorD> leftVal, TPair<TVectorD, TVectorD> rightVal, std::string name) {
	TVectorD approxDerivFirst = (rightVal.first - leftVal.first)/(2.0*Epsilon);
	TVectorD approxDerivSecond = (rightVal.second - leftVal.second)/(2.0*Epsilon);
	{
		double res = NLa::Sum(derivVal.first - approxDerivFirst);
		res = std::abs(res);
		if(res >= LilEpsilon) {
			L_ERROR << name << ", Derivative of first element of pair is bad: " << res << " >= " << LilEpsilon;
			L_ERROR << "Those values are not almost equal:";
			L_ERROR << "Proposed exact value:";
			L_ERROR << "\n" << derivVal.first;
			L_ERROR << "Real approximate:";
			L_ERROR << "\n" << approxDerivSecond;
			throw TEgoException() << "Derivative sanity check failed for first element of pair";
		}
		L_INFO << name << ", Got derivative sanity check ok: " << res << " < " << LilEpsilon;
	}
	{
		double res = NLa::Sum(derivVal.second - approxDerivSecond);
		res = std::abs(res);
		if(res >= LilEpsilon) {
			L_ERROR << name << ", Derivative second element of pair is bad: " << res << " >= " << LilEpsilon;
			L_ERROR << "Those values are not almost equal:";
			L_ERROR << "Proposed exact value:";
			L_ERROR << "\n" << derivVal.second;
			L_ERROR << "Real approximate:";
			L_ERROR << "\n" << approxDerivSecond;
			throw TEgoException() << "Derivative sanity check failed for second element of pair";
		}
		L_INFO << name << ", Got derivative sanity check ok: " << res << " < " << LilEpsilon;
	}
}



template <typename T>
T CreateTestData();

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


template <typename Functor, typename ... Params>
void OneArgFunctorTester(std::string functorName, Params ... params) {
	UPtr<Functor> f = UPtr<Functor>(new Functor(std::forward<Params>(params) ... ));

	if(f->GetParametersSize()>0) {
		arma::arma_rng::set_seed_random();
		TVectorD paramV(f->GetParametersSize(), arma::fill::randu);
		f->SetParameters(NLa::VecToStd(paramV));
	}

	typename Functor::TArg a = CreateTestData<typename Functor::TArg>();

	auto resCenter = f->Calc(a);
	auto resLeftEps = f->Calc(a - Epsilon);
	auto resRightEps = f->Calc(a + Epsilon);
	try {
		CheckDerivativeSanity(
			resCenter.ArgDeriv(),
			resLeftEps.Value(),
			resRightEps.Value(),
			functorName + ", argument derivative"
		);
	} catch(const TEgoNotImplemented &e) {
		L_INFO << "Some things are not implemented, keep calm and carry on: " << e.what();
	}
	if(f->GetParametersSize() > 0) {
		TVector<double> centerParams = f->GetParameters();
		try {
			TVector<typename Functor::TReturn> centerDerivs = resCenter.ParamDeriv();
			for(size_t pi=0; pi<f->GetParametersSize(); ++pi) {
				TVector<double> paramsLeft(centerParams);
				paramsLeft[pi] -= Epsilon;
				f->SetParameters(paramsLeft);
				auto resParamLeftEps = f->Calc(a).Value();

				TVector<double> paramsRight(centerParams);
				paramsRight[pi] += Epsilon;
				f->SetParameters(paramsRight);
				auto resParamRightEps = f->Calc(a).Value();
				
				CheckDerivativeSanity(
					typename Functor::TReturn(centerDerivs[pi])
				  , resParamLeftEps
				  , resParamRightEps
				  , NStr::TStringBuilder() << functorName << ", " << pi << " param derivative"
				);
			}
		} catch(const TEgoNotImplemented &e) {
			L_INFO << "Some things are not implemented, keep calm and carry on: " << e.what();
		}
	}
}

template <typename Functor, typename ... Params>
void TwoArgFunctorTester(std::string functorName, Params ... params) {
	UPtr<Functor> f = UPtr<Functor>(new Functor(std::forward<Params>(params) ... ));

	if (f->GetParametersSize()>0) {
		arma::arma_rng::set_seed_random();
		TVectorD paramV(f->GetParametersSize(), arma::fill::randu);
		f->SetParameters(NLa::VecToStd(paramV));
	}

	typename Functor::TFirst first = CreateTestData<typename Functor::TFirst>();
	typename Functor::TSecond second = CreateTestData<typename Functor::TSecond>();

	auto resCenter = f->Calc(first, second);

	auto resFirstLeftEps = f->Calc(first - Epsilon, second);
	auto resFirstRightEps = f->Calc(first + Epsilon, second);

	try {
		CheckDerivativeSanity(
			resCenter.FirstArgDeriv(),
			resFirstLeftEps.Value(),
			resFirstRightEps.Value(),
			functorName + ", first argument derivative"
		);
	} catch(const TEgoNotImplemented &e) {
		L_INFO << "Some things are not implemented, keep calm and carry on: " << e.what();
	}

	auto resSecondLeftEps = f->Calc(first, second - Epsilon);
	auto resSecondRightEps = f->Calc(first, second + Epsilon);

	try {
		CheckDerivativeSanity(
			resCenter.SecondArgDeriv(),
			resSecondLeftEps.Value(),
			resSecondRightEps.Value(),
			functorName + ", second argument derivative"
		);
	} catch(const TEgoNotImplemented &e) {
		L_INFO << "Some things are not implemented, keep calm and carry on: " << e.what();
	}

	if(f->GetParametersSize() > 0) {
		TVector<double> centerParams = f->GetParameters();
		try {
			TVector<typename Functor::TReturn> centerDerivs = resCenter.ParamDeriv();
			for(size_t pi=0; pi<f->GetParametersSize(); ++pi) {
				TVector<double> paramsLeft(centerParams);
				paramsLeft[pi] -= Epsilon;
				f->SetParameters(paramsLeft);
				auto resParamLeftEps = f->Calc(first, second).Value();

				TVector<double> paramsRight(centerParams);
				paramsRight[pi] += Epsilon;
				f->SetParameters(paramsRight);
				auto resParamRightEps = f->Calc(first, second).Value();

				CheckDerivativeSanity(
					typename Functor::TReturn(centerDerivs[pi])
				  , resParamLeftEps
				  , resParamRightEps
				  , NStr::TStringBuilder() << functorName << ", " << pi << " param derivative"
				);
			}
		} catch(const TEgoNotImplemented &e) {
			L_INFO << "Some things are not implemented, keep calm and carry on: " << e.what();
		}
	}
}

#define ONE_ARG_FUN_TEST(Typename) \
	TEST(Typename ## DerivativeSanityCheck) { \
		OneArgFunctorTester<Typename, size_t>(#Typename, DimSize); \
	} \

#define TWO_ARG_FUN_TEST(Typename) \
	TEST(Typename ## DerivativeSanityCheck) { \
		TwoArgFunctorTester<Typename, size_t>(#Typename, DimSize); \
	} \

#define INF_TEST(Typename, MeanTypename, CovTypename, LikTypename) \
	TEST(Typename ## DerivativeSanityCheck) { \
		TwoArgFunctorTester<Typename, SPtr<IMean>, SPtr<ICov>, SPtr<ILik>>( \
			#Typename \
		  , SPtr<IMean>(new MeanTypename(DimSize)) \
		  , SPtr<ICov>(new CovTypename(DimSize)) \
		  , SPtr<ILik>(new LikTypename(DimSize)) \
		); \
	} \


