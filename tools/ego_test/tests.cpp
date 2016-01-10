#include "register.h"

#include <ego/util/string.h>
#include <ego/cov/stationary.h>

using namespace NEgo;
using namespace NKernels;

constexpr double Epsilon = 1e-4;
constexpr double LilEpsilon = 1e-5;
constexpr ui32 DimSize = 10;

template <typename T>
void CheckDerivativeSanity(T &&derivVal, T &&leftVal, T &&rightVal, std::string name) {
	double res = NLa::Sum(derivVal - (rightVal - leftVal)/(2*Epsilon));
	res = std::abs(res);
	ENSURE(res < LilEpsilon, name << ", Derivative is bad: " << res << " > " << LilEpsilon);
	L_INFO << name << ", Got derivative sanity check ok: " << res << " < " << LilEpsilon;
}


template <typename T>
T CreateTestData();

template <>
TMatrixD CreateTestData<TMatrixD>() {
	return TMatrixD(DimSize, DimSize, arma::fill::randn);
}


template <typename Functor, typename ... Params>
void OneArgFunctorTester(std::string functorName, Params ... params) {
	UPtr<Functor> f = UPtr<Functor>(new Functor(std::forward<Params>(params) ... ));

	if(f->GetParametersSize()>0) {
		TVector<double> params(f->GetParametersSize(), 1.0);
		f->SetParameters(params);
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
		
		TVector<double> paramsLeft;
		TVector<double> paramsRight;
		for(const auto& p: centerParams) {
			paramsLeft.push_back(p - Epsilon);
			paramsRight.push_back(p + Epsilon);
		}
		f->SetParameters(paramsLeft);
		auto resParamLeftEps = f->Calc(a);
		f->SetParameters(paramsRight);
		auto resParamRightEps = f->Calc(a);

		try {
			TVector<typename Functor::TReturn> centerDerivs = resCenter.ParamDeriv();
			for(size_t pi=0; pi<f->GetParametersSize(); ++pi) {
				CheckDerivativeSanity(
					typename Functor::TReturn(centerDerivs[pi])
				  , resParamLeftEps.Value()
				  , resParamRightEps.Value()
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
	
	if(f->GetParametersSize()>0) {
		TVector<double> params(f->GetParametersSize(), 1.0);
		f->SetParameters(params);
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
		
		TVector<double> paramsLeft;
		TVector<double> paramsRight;
		for(const auto& p: centerParams) {
			paramsLeft.push_back(p - Epsilon);
			paramsRight.push_back(p + Epsilon);
		}
		f->SetParameters(paramsLeft);
		auto resParamLeftEps = f->Calc(first, second);
		f->SetParameters(paramsRight);
		auto resParamRightEps = f->Calc(first, second);

		try {
			TVector<typename Functor::TReturn> centerDerivs = resCenter.ParamDeriv();
			for(size_t pi=0; pi<f->GetParametersSize(); ++pi) {
				CheckDerivativeSanity(
					typename Functor::TReturn(centerDerivs[pi])
				  , resParamLeftEps.Value()
				  , resParamRightEps.Value()
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
		OneArgFunctorTester<Typename>(#Typename); \
	} \

#define TWO_ARG_FUN_TEST(Typename) \
	TEST(Typename ## DerivativeSanityCheck) { \
		TwoArgFunctorTester<Typename>(#Typename); \
	} \
	
#define TWO_ARG_COV_TEST(Typename) \
	TEST(Typename ## DerivativeSanityCheck) { \
		TwoArgFunctorTester<Typename, size_t>(#Typename, DimSize); \
	} \



ONE_ARG_FUN_TEST(TSqExp);
TWO_ARG_COV_TEST(TCovSqExpISO);
TWO_ARG_FUN_TEST(TSquareDistFunctor);
