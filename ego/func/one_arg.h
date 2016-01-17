#pragma once

#include "functor.h"

namespace NEgo {


	template <typename T, typename Derived>
	class TOneArgFunctorResultBase : public TFunctorResult<T, Derived> {
	public:
		using TCalcArgDerivCb = std::function<T()>;

		TOneArgFunctorResultBase() :
			CalcArgDerivCb([=]() -> T {
				throw TEgoNotImplemented() << "Calculation of argument derivative was not implemented";
			})
		{
		}

		Derived& SetArgDeriv(TCalcArgDerivCb cb) {
			CalcArgDerivCb = cb;
			return *static_cast<Derived*>(this);
		}

		T ArgDeriv() const {
			return CalcArgDerivCb();
		}

	private:
		TCalcArgDerivCb CalcArgDerivCb;
	};

	template <typename T>
	class TOneArgFunctorResult : public TOneArgFunctorResultBase<T, TOneArgFunctorResult<T>> {
	};

	template <typename T, typename A, typename R = TOneArgFunctorResult<T>>
	class TOneArgFunctor : public TFunctorBase<T> {
	public:
		using TArg = A;
		using TReturn = T;
		using Result = R;

		virtual Result UserCalc(const A& arg) const = 0;

		TOneArgFunctor(size_t dimSize)
			: TFunctorBase<T>(dimSize)
		{
		}

		virtual ~TOneArgFunctor() {}

		virtual size_t GetParametersSize() const { return 0; }

        Result Calc(const A& arg) const {
        	ENSURE(TFunctorBase<T>::MetaEntity || TFunctorBase<T>::Parameters.size() == GetParametersSize(),
        		"Parameters are not satisfying to functor parameter size: " << TFunctorBase<T>::Parameters.size() << " != " << GetParametersSize());
        	return UserCalc(arg);
        }

        Result operator()(const A& arg) const {
        	return Calc(arg);
        }
	};

} // namespace NEgo
