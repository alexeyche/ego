#pragma once

#include "register.h"

#include <ego/util/string.h>
#include <ego/base/la.h>
#include <ego/util/log/log.h>

using namespace NEgo;


constexpr double Epsilon = 1e-4;
constexpr double LilEpsilon = 1e-4;
constexpr ui32 DimSize = 5;
constexpr ui32 SampleSize = 25;


template <typename T>
void CheckDerivativeSanity(T derivVal, T leftVal, T rightVal, std::string name, double epsilon) {
	T approxDeriv = (rightVal - leftVal)/(2.0*Epsilon);
	double res = NLa::Sum(derivVal - approxDeriv);
	res = std::abs(res);
	if(res >= epsilon || std::isnan(res)) {
		L_ERROR << name << ", Derivative is bad: " << res << " >= " << epsilon;
		L_ERROR << "Those values are not almost equal:";
		L_ERROR << "Proposed exact value:";
		L_ERROR << "\n" << derivVal;
		L_ERROR << "Real approximate:";
		L_ERROR << "\n" << approxDeriv;
		throw TErrException() << "Derivative sanity check failed";
	}
	L_INFO << name << ", Got derivative sanity check ok: " << res << " < " << epsilon;
}

template <>
void CheckDerivativeSanity<TPair<TVectorD, TVectorD>>(TPair<TVectorD, TVectorD> derivVal, TPair<TVectorD, TVectorD> leftVal, TPair<TVectorD, TVectorD> rightVal, std::string name, double epsilon);

template <typename T>
T CreateTestData();


TVectorD CreateTestDataVectorY();

TVectorD CreateTestDataVectorX();


template <typename Functor, typename A>
struct TTestPartials;


template <typename Functor>
struct TTestPartials<Functor, TMatrixD> {
	static void Test(std::string functorName, SPtr<Functor> f,  const TMatrixD& a, double epsilon) {
		auto resCenter = f->Calc(a);

		for (ui32 indexRow=0; indexRow < a.n_rows; ++indexRow) {
			for (ui32 indexCol=0; indexCol < a.n_cols; ++indexCol) {
				TMatrixD leftA = a;
				leftA(indexRow, indexCol) -= Epsilon;
				TMatrixD rightA = a;
				rightA(indexRow, indexCol) += Epsilon;

				auto resLeftEpsPart = f->Calc(leftA);
				auto resRightEpsPart = f->Calc(rightA);
				try {
					CheckDerivativeSanity(
						resCenter.ArgPartialDeriv(indexRow, indexCol),
						resLeftEpsPart.Value(),
						resRightEpsPart.Value(),
						functorName + TString(NStr::TStringBuilder() << ", [" << indexRow << ":" << indexCol << "] partial argument derivative"),
						epsilon
					);
				} catch(const TErrNotImplemented &e) {
					L_INFO << "Some things are not implemented, keep calm and carry on: " << e.what();
				}
			}
		}
	}
	template <typename TSecond>
	static void TestFirstArg(std::string functorName, SPtr<Functor> f,  const TMatrixD& a, const TSecond& secArg, double epsilon) {
		auto resCenter = f->Calc(a, secArg);

		for (ui32 indexRow=0; indexRow < a.n_rows; ++indexRow) {
			for (ui32 indexCol=0; indexCol < a.n_cols; ++indexCol) {
				TMatrixD leftA = a;
				leftA(indexRow, indexCol) -= Epsilon;
				TMatrixD rightA = a;
				rightA(indexRow, indexCol) += Epsilon;

				auto resLeftEpsPart = f->Calc(leftA, secArg);
				auto resRightEpsPart = f->Calc(rightA, secArg);
				try {
					CheckDerivativeSanity(
						resCenter.FirstArgPartialDeriv(indexRow, indexCol),
						resLeftEpsPart.Value(),
						resRightEpsPart.Value(),
						functorName + TString(NStr::TStringBuilder() << ", [" << indexRow << ":" << indexCol << "] partial argument derivative"),
						epsilon
					);
				} catch(const TErrNotImplemented &e) {
					L_INFO << "Some things are not implemented, keep calm and carry on: " << e.what();
				}
			}
		}
	}
	template <typename TFirst>
	static void TestSecondArg(std::string functorName, SPtr<Functor> f,  const TMatrixD& a, const TFirst& firstArg, double epsilon) {
		auto resCenter = f->Calc(firstArg, a);

		for (ui32 indexRow=0; indexRow < a.n_rows; ++indexRow) {
			for (ui32 indexCol=0; indexCol < a.n_cols; ++indexCol) {
				TMatrixD leftA = a;
				leftA(indexRow, indexCol) -= Epsilon;
				TMatrixD rightA = a;
				rightA(indexRow, indexCol) += Epsilon;

				auto resLeftEpsPart = f->Calc(firstArg, leftA);
				auto resRightEpsPart = f->Calc(firstArg, rightA);
				try {
					CheckDerivativeSanity(
						resCenter.SecondArgPartialDeriv(indexRow, indexCol),
						resLeftEpsPart.Value(),
						resRightEpsPart.Value(),
						functorName + TString(NStr::TStringBuilder() << ", [" << indexRow << ":" << indexCol << "] partial argument derivative"),
						epsilon
					);
				} catch(const TErrNotImplemented &e) {
					L_INFO << "Some things are not implemented, keep calm and carry on: " << e.what();
				}
			}
		}
	}
};



template <typename Functor>
struct TTestPartials<Functor, TVectorD> {
	static void Test(std::string functorName, SPtr<Functor> f,  const TVectorD& a, double epsilon) {
		auto resCenter = f->Calc(a);

		for (ui32 index=0; index < a.size(); ++index) {
			TVectorD leftA = a;
			leftA(index) -= Epsilon;
			TVectorD rightA = a;
			rightA(index) += Epsilon;

			auto resLeftEpsPart = f->Calc(leftA);
			auto resRightEpsPart = f->Calc(rightA);
			try {
				CheckDerivativeSanity(
					resCenter.ArgPartialDeriv(index),
					resLeftEpsPart.Value(),
					resRightEpsPart.Value(),
					functorName + TString(NStr::TStringBuilder() << ", [" << index << "] partial argument derivative"),
					epsilon
				);
			} catch(const TErrNotImplemented &e) {
				L_INFO << "Some things are not implemented, keep calm and carry on: " << e.what();
			}
		}
	}
	template <typename TSecond>
	static void TestFirstArg(std::string functorName, SPtr<Functor> f,  const TVectorD& a, const TSecond& secArg, double epsilon) {
		auto resCenter = f->Calc(a, secArg);

		for (ui32 index=0; index < a.size(); ++index) {
			TVectorD leftA = a;
			leftA(index) -= Epsilon;
			TVectorD rightA = a;
			rightA(index) += Epsilon;

			auto resLeftEpsPart = f->Calc(leftA, secArg);
			auto resRightEpsPart = f->Calc(rightA, secArg);
			try {
				CheckDerivativeSanity(
					resCenter.FirstArgPartialDeriv(index),
					resLeftEpsPart.Value(),
					resRightEpsPart.Value(),
					functorName + TString(NStr::TStringBuilder() << ", [" << index << "] partial argument derivative"),
					epsilon
				);
			} catch(const TErrNotImplemented &e) {
				L_INFO << "Some things are not implemented, keep calm and carry on: " << e.what();
			}
		}
	}

	template <typename TFirst>
	static void TestSecondArg(std::string functorName, SPtr<Functor> f,  const TVectorD& a, const TFirst& firstArg, double epsilon) {
		auto resCenter = f->Calc(firstArg, a);

		for (ui32 index=0; index < a.size(); ++index) {
			TVectorD leftA = a;
			leftA(index) -= Epsilon;
			TVectorD rightA = a;
			rightA(index) += Epsilon;

			auto resLeftEpsPart = f->Calc(firstArg, leftA);
			auto resRightEpsPart = f->Calc(firstArg, rightA);
			try {
				CheckDerivativeSanity(
					resCenter.SecondArgPartialDeriv(index),
					resLeftEpsPart.Value(),
					resRightEpsPart.Value(),
					functorName + TString(NStr::TStringBuilder() << ", [" << index << "] partial argument derivative"),
					epsilon
				);
			} catch(const TErrNotImplemented &e) {
				L_INFO << "Some things are not implemented, keep calm and carry on: " << e.what();
			}
		}
	}
};


template <typename Functor>
void OneArgFunctorTester(std::string functorName, SPtr<Functor> f, typename Functor::TArg a = CreateTestData<typename Functor::TArg>(), double epsilon = LilEpsilon) {
	if(f->GetParametersSize()>0) {
		arma::arma_rng::set_seed_random();
		TVectorD paramV(f->GetParametersSize(), arma::fill::randu);
		f->SetParameters(NLa::VecToStd(paramV));
	}

	auto resCenter = f->Calc(a);
	auto resLeftEps = f->Calc(a - Epsilon);
	auto resRightEps = f->Calc(a + Epsilon);
	try {
		CheckDerivativeSanity(
			resCenter.ArgDeriv(),
			resLeftEps.Value(),
			resRightEps.Value(),
			functorName + ", argument derivative",
			epsilon
		);
	} catch(const TErrNotImplemented &e) {
		L_INFO << "Some things are not implemented, keep calm and carry on: " << e.what();
	}

	TTestPartials<Functor, typename Functor::TArg>::Test(functorName, f, a, epsilon);

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
				  , epsilon
				);
			}
		} catch(const TErrNotImplemented &e) {
			L_INFO << "Some things are not implemented, keep calm and carry on: " << e.what();
		}
	}
}


template <typename Functor>
void TwoArgFunctorTester(std::string functorName, SPtr<Functor> f,
	typename Functor::TFirst first = CreateTestData<typename Functor::TFirst>(),
	typename Functor::TSecond second = CreateTestData<typename Functor::TSecond>(),
	double epsilon = LilEpsilon)
{
	if (f->GetParametersSize()>0) {
		arma::arma_rng::set_seed_random();
		TVectorD paramV(f->GetParametersSize(), arma::fill::randu);
		f->SetParameters(NLa::VecToStd(paramV));
	}

	auto resCenter = f->Calc(first, second);

	auto resFirstLeftEps = f->Calc(first - Epsilon, second);
	auto resFirstRightEps = f->Calc(first + Epsilon, second);

	try {
		CheckDerivativeSanity(
			resCenter.FirstArgDeriv(),
			resFirstLeftEps.Value(),
			resFirstRightEps.Value(),
			functorName + ", first argument derivative",
			epsilon
		);
	} catch(const TErrNotImplemented &e) {
		L_INFO << "Some things are not implemented, keep calm and carry on: " << e.what();
	}

	auto resSecondLeftEps = f->Calc(first, second - Epsilon);
	auto resSecondRightEps = f->Calc(first, second + Epsilon);

	try {
		CheckDerivativeSanity(
			resCenter.SecondArgDeriv(),
			resSecondLeftEps.Value(),
			resSecondRightEps.Value(),
			functorName + ", second argument derivative",
			epsilon
		);
	} catch(const TErrNotImplemented &e) {
		L_INFO << "Some things are not implemented, keep calm and carry on: " << e.what();
	}

	TTestPartials<Functor, typename Functor::TFirst>::TestFirstArg(functorName, f, first, second, epsilon);
	TTestPartials<Functor, typename Functor::TSecond>::TestSecondArg(functorName, f, second, first, epsilon);

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
				  , epsilon
				);
			}
		} catch(const TErrNotImplemented &e) {
			L_INFO << "Some things are not implemented, keep calm and carry on: " << e.what();
		}
	}
}

#define ONE_ARG_FUN_TEST(Typename) \
	TEST(Typename ## DerivativeSanityCheck) { \
		OneArgFunctorTester<Typename>(#Typename, MakeShared(new Typename(DimSize)), CreateTestData<Typename::TArg>()); \
	} \

#define TWO_ARG_FUN_TEST(Typename) \
	TEST(Typename ## DerivativeSanityCheck) { \
		TwoArgFunctorTester<Typename>(#Typename, MakeShared(new Typename(DimSize))); \
	} \

#define INF_TEST(Typename, MeanTypename, CovTypename, LikTypename) \
	TEST(Typename ## DerivativeSanityCheck) { \
		TwoArgFunctorTester<Typename>( \
			#Typename, \
			MakeShared(new Typename( \
			    MakeShared(new MeanTypename(DimSize)), \
			    MakeShared(new CovTypename(DimSize)), \
			    MakeShared(new LikTypename(DimSize)) \
			)) \
		); \
	} \




#define MODEL_TEST(Typename, MeanTypename, CovTypename, LikTypename, InfTypename, AcqTypename, epsilon) \
	TEST(Typename ## DerivativeSanityCheck) { \
		TMatrixD X(5*SampleSize, DimSize, arma::fill::randn); \
		TVectorD Y(5*SampleSize, arma::fill::randn); \
		\
		SPtr<IMean> mean = MakeShared(new MeanTypename(DimSize)); \
		SPtr<ICov> cov = MakeShared(new CovTypename(DimSize)); \
		SPtr<ILik> lik = MakeShared(new LikTypename(DimSize)); \
		SPtr<IInf> inf = MakeShared(new InfTypename(mean, cov, lik)); \
		SPtr<IAcq> acq = MakeShared(new AcqTypename(DimSize)); \
		SPtr<Typename> model = MakeShared(new Typename(mean, cov, lik, inf, acq)); \
		model->SetData(X, Y); \
		\
		OneArgFunctorTester<Typename>(#Typename, model, CreateTestData<typename Typename::TArg>(), epsilon);\
	} \

#define ACQ_TEST(AcqTypename, MeanTypename, CovTypename, LikTypename, InfTypename, ModelTypename) \
	TEST(AcqTypename ## DerivativeSanityCheck) { \
		TMatrixD X(5*SampleSize, DimSize, arma::fill::randn); \
		TVectorD Y(5*SampleSize, arma::fill::randn); \
		\
		SPtr<IMean> mean = MakeShared(new MeanTypename(DimSize)); \
		SPtr<ICov> cov = MakeShared(new CovTypename(DimSize)); \
		SPtr<ILik> lik = MakeShared(new LikTypename(DimSize)); \
		SPtr<IInf> inf = MakeShared(new InfTypename(mean, cov, lik)); \
		SPtr<AcqTypename> acq = MakeShared(new AcqTypename(DimSize)); \
		SPtr<TModel> model = MakeShared(new TModel(mean, cov, lik, inf, acq)); \
        model->SetData(X, Y); \
		\
		OneArgFunctorTester<AcqTypename>(#AcqTypename, acq, CreateTestDataVectorX());\
	} \


