#pragma once

#include "functor.h"

namespace NEgo {

	template <typename T, typename A1, typename A2>
	class TTwoArgFunctor : public TFunctorBase<T> {
	public:
		using TFirst = A1;
		using TSecond = A2;
		using TReturn = T;
		
		class TTwoArgFunctorResult : public TFunctorBase<T>::template TFunctorResult<TTwoArgFunctorResult> {
		public:
			using TCalcFirstArgDerivCb = std::function<T()>;
			using TCalcSecondArgDerivCb = std::function<T()>;
			
			TTwoArgFunctorResult() : 
				CalcFirstArgDerivCb([=]() -> T {
					throw TEgoNotImplemented() << "Calculation of first argument derivative was not implemented";
				}),
				CalcSecondArgDerivCb([=]() -> T {
					throw TEgoNotImplemented() << "Calculation of second argument derivative was not implemented";
				})
			{
			}
			
			TTwoArgFunctorResult& SetCalcFirstArgDeriv(TCalcFirstArgDerivCb cb) {
				CalcFirstArgDerivCb = cb;
				return *this;
			}
			
			TTwoArgFunctorResult& SetCalcSecondArgDeriv(TCalcSecondArgDerivCb cb) {
				CalcSecondArgDerivCb = cb;
				return *this;
			}
			
			T FirstArgDeriv() const {
				return CalcFirstArgDerivCb();
			}
			
			T SecondArgDeriv() const {
				return CalcSecondArgDerivCb();
			}

		private:
			TCalcFirstArgDerivCb CalcFirstArgDerivCb;
			TCalcSecondArgDerivCb CalcSecondArgDerivCb;
		};

		using Result = TTwoArgFunctorResult;

		virtual Result UserCalc(const A1& firstArg, const A2& secondArg) = 0;

		virtual size_t GetParametersSize() const { return 0; }

		Result Calc(const A1& firstArg, const A2& secondArg) {
			ENSURE(TFunctorBase<T>::Parameters.size() == GetParametersSize(), 
        		"Parameters are not satisfying to functor parameter size: " << TFunctorBase<T>::Parameters.size() << " != " << GetParametersSize());
        	return UserCalc(firstArg, secondArg);
		}
        
        Result operator()(const A1& firstArg, const A2& secondArg) {
        	return Calc(firstArg, secondArg);
        }

	};
} // namespace NEgo
