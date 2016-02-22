#pragma once

#include "functor.h"

#include <ego/util/optional.h>

namespace NEgo {

	template <typename T, typename A1, typename A2, typename Derived>
	class TTwoArgFunctorResultBase : public TFunctorResult<T, Derived> {
	public:
		using TSelf = TTwoArgFunctorResultBase<T, A1, A2, Derived>;

		using TCalcArgDerivCb = std::function<T()>;
		using TCalcFirstArgPartialDerivCb = typename TPartialDerivative<T, A1>::TCalcFunction;
		using TCalcSecondArgPartialDerivCb = typename TPartialDerivative<T, A2>::TCalcFunction;

		TTwoArgFunctorResultBase() :
			CalcFirstArgDerivCb([=]() -> T {
				throw TEgoNotImplemented() << "Calculation of first argument derivative was not implemented";
			}),
			CalcSecondArgDerivCb([=]() -> T {
				throw TEgoNotImplemented() << "Calculation of second argument derivative was not implemented";
			})
		{
		}

		Derived& SetFirstArgDeriv(TCalcArgDerivCb cb) {
			CalcFirstArgDerivCb = cb;
			return *static_cast<Derived*>(this);
		}

		Derived& SetSecondArgDeriv(TCalcArgDerivCb cb) {
			CalcSecondArgDerivCb = cb;
			return *static_cast<Derived*>(this);
		}


		Derived& SetFirstArgPartialDeriv(TCalcFirstArgPartialDerivCb cb) {
			CalcFirstArgPartialDerivCb = cb;
			return *static_cast<Derived*>(this);
		}

		Derived& SetSecondArgPartialDeriv(TCalcSecondArgPartialDerivCb cb) {
			CalcSecondArgPartialDerivCb = cb;
			return *static_cast<Derived*>(this);
		}

		template <typename ... FunArgs>
		T FirstArgPartialDeriv(FunArgs ... args) const {
			if (!CalcFirstArgPartialDerivCb) {
				if (!FirstArgDerivCache) {
					const_cast<TSelf*>(this)->FirstArgDerivCache = FirstArgDeriv();
				}
				return TPartialDerivative<T,A1>::Default(*FirstArgDerivCache, args...);
			}
			return (*CalcFirstArgPartialDerivCb)(args...);
		}

		template <typename ... FunArgs>
		T SecondArgPartialDeriv(FunArgs ... args) const {
			if (!CalcSecondArgPartialDerivCb) {
				if (!SecondArgDerivCache) {
					const_cast<TSelf*>(this)->SecondArgDerivCache = SecondArgDeriv();
				}
				return TPartialDerivative<T,A2>::Default(*SecondArgDerivCache, args...);
			}
			return (*CalcSecondArgPartialDerivCb)(args...);
		}


		T FirstArgDeriv() const {
			return CalcFirstArgDerivCb();
		}

		T SecondArgDeriv() const {
			return CalcSecondArgDerivCb();
		}


	private:
		TCalcArgDerivCb CalcFirstArgDerivCb;
		TCalcArgDerivCb CalcSecondArgDerivCb;

		TOptional<TCalcFirstArgPartialDerivCb> CalcFirstArgPartialDerivCb;
		TOptional<TCalcSecondArgPartialDerivCb> CalcSecondArgPartialDerivCb;

		TOptional<T> FirstArgDerivCache;
		TOptional<T> SecondArgDerivCache;
	};

	template <typename T, typename A1, typename A2>
	class TTwoArgFunctorResult : public TTwoArgFunctorResultBase<T, A1, A2, TTwoArgFunctorResult<T, A1, A2>> {
	};

	template <typename T, typename A1, typename A2, typename R = TTwoArgFunctorResult<T, A1, A2>>
	class TTwoArgFunctor : public TFunctorBase<T> {
	public:
		using TFirst = A1;
		using TSecond = A2;
		using TReturn = T;
		using Result = R;

		TTwoArgFunctor(size_t dimSize)
			: TFunctorBase<T>(dimSize)
		{
		}

		virtual ~TTwoArgFunctor() {}

		virtual Result UserCalc(const A1& firstArg, const A2& secondArg) const = 0;

		virtual size_t GetParametersSize() const { return 0; }

		Result Calc(const A1& firstArg, const A2& secondArg) const {
			ENSURE(TFunctorBase<T>::MetaEntity || TFunctorBase<T>::Parameters.size() == GetParametersSize(),
        		"Parameters are not satisfying to functor parameter size: " << TFunctorBase<T>::Parameters.size() << " != " << GetParametersSize());
        	return UserCalc(firstArg, secondArg);
		}

        Result operator()(const A1& firstArg, const A2& secondArg) const {
        	return Calc(firstArg, secondArg);
        }

	};
} // namespace NEgo
