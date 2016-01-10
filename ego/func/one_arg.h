#pragma once

#include "functor.h"

namespace NEgo {

	template <typename T, typename A>
	class TOneArgFunctor : public TFunctorBase<T> {
	public:
		using TArg = A;
		using TReturn = T;

		class TOneArgFunctorResult : public TFunctorBase<T>::template TFunctorResult<TOneArgFunctorResult> {
		public:
			using TCalcArgDerivCb = std::function<T()>;
			
			TOneArgFunctorResult() : 
				CalcArgDerivCb([=]() -> T {
					throw TEgoNotImplemented() << "Calculation of argument derivative was not implemented";
				})
			{
			}
			
			TOneArgFunctorResult& SetCalcArgDeriv(TCalcArgDerivCb cb) {
				CalcArgDerivCb = cb;
				return *this;
			}

			T ArgDeriv() const {
				return CalcArgDerivCb();
			}
			
		private:
			TCalcArgDerivCb CalcArgDerivCb;
		};

		using Result = TOneArgFunctorResult;

		virtual Result UserCalc(const A& arg) = 0;

		virtual size_t GetParametersSize() const { return 0; }

        Result Calc(const A& arg) {
        	ENSURE(TFunctorBase<T>::Parameters.size() == GetParametersSize(), 
        		"Parameters are not satisfying to functor parameter size: " << TFunctorBase<T>::Parameters.size() << " != " << GetParametersSize());
        	return UserCalc(arg);
        }

        Result operator()(const A& arg) {
        	return Calc(arg);
        }
	};

} // namespace NEgo
