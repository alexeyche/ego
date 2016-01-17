#pragma once

#include "functor.h"

namespace NEgo {

	template <typename T, typename Derived>
	class TTwoArgFunctorResultBase : public TFunctorResult<T, Derived> {
	public:
		using TCalcFirstArgDerivCb = std::function<T()>;
		using TCalcSecondArgDerivCb = std::function<T()>;

		TTwoArgFunctorResultBase() :
			CalcFirstArgDerivCb([=]() -> T {
				throw TEgoNotImplemented() << "Calculation of first argument derivative was not implemented";
			}),
			CalcSecondArgDerivCb([=]() -> T {
				throw TEgoNotImplemented() << "Calculation of second argument derivative was not implemented";
			})
		{
		}

		Derived& SetFirstArgDeriv(TCalcFirstArgDerivCb cb) {
			CalcFirstArgDerivCb = cb;
			return *static_cast<Derived*>(this);
		}

		Derived& SetSecondArgDeriv(TCalcSecondArgDerivCb cb) {
			CalcSecondArgDerivCb = cb;
			return *static_cast<Derived*>(this);
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

	template <typename T>
	class TTwoArgFunctorResult : public TTwoArgFunctorResultBase<T, TTwoArgFunctorResult<T>> {
	};
	
	template <typename T, typename A1, typename A2, typename R = TTwoArgFunctorResult<T>>
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
