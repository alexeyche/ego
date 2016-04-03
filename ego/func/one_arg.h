#pragma once

#include <ego/util/optional.h>

#include "functor.h"

namespace NEgo {

	template <typename T, typename A, typename Derived>
	class TOneArgFunctorResultBase : public TFunctorResult<T, Derived> {
	public:
		using TSelf = TOneArgFunctorResultBase<T, A, Derived>;

		using TCalcArgDerivCb = std::function<T()>;
		using TCalcArgPartialDerivCb = typename TPartialDerivative<T, A>::TCalcFunction;

		TOneArgFunctorResultBase() :
			CalcArgDerivCb([]() -> T {
				throw TErrNotImplemented() << "Calculation of argument derivative was not implemented";
			})
		{
		}

		Derived& SetArgDeriv(TCalcArgDerivCb cb) {
			CalcArgDerivCb = cb;
			return *static_cast<Derived*>(this);
		}

		Derived& SetArgPartialDeriv(TCalcArgPartialDerivCb cb) {
			CalcArgPartialDerivCb = cb;
			return *static_cast<Derived*>(this);
		}

		T ArgDeriv() const {
			return CalcArgDerivCb();
		}

		template <typename ... FunArgs>
		T ArgPartialDeriv(FunArgs ... args) const {
			if (!CalcArgPartialDerivCb) {
				if (!ArgDerivCache) {
					const_cast<TSelf*>(this)->ArgDerivCache = ArgDeriv();
				}
				return TPartialDerivative<T,A>::Default(*ArgDerivCache, args...);
			}
			return (*CalcArgPartialDerivCb)(args...);
		}

	private:
		TCalcArgDerivCb CalcArgDerivCb;
		TOptional<TCalcArgPartialDerivCb> CalcArgPartialDerivCb;

		TOptional<T> ArgDerivCache;
	};

	template <typename T, typename A>
	class TOneArgFunctorResult : public TOneArgFunctorResultBase<T, A, TOneArgFunctorResult<T, A>> {
	};


	template <typename T, typename A, typename R = TOneArgFunctorResult<T, A>>
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
