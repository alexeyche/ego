#pragma once

#include <ego/util/optional.h>

#include "functor.h"

namespace NEgo {

	template <typename T, typename A>
	struct TPartialDerivative {
		static TVector<T> Default(const T& dArg) {
			throw TEgoNotImplemented() << "Calculation of partial argument derivative was not implemented";
		}
	};


	// template <typename T, typename A>
	// T PartialDerivativeDefault(const T& dArg, ui32 index) {

	// 	// T v = NLa::CreateSameShape<T>(dArg, /* fill_zeros = */ true);
	// 	// ENSURE(index < dArg.size(), "Trying to take partial derivative of something bigger: " << index << " >= " << dArg.size());
	// 	// v(index) = dArg(index);
	// 	// return v;
	// }

	// template <>
	// double PartialDerivativeDefault(const double& dArg, ui32 index);

	// template <>
	// TPair<TVectorD, TVectorD> PartialDerivativeDefault(const TPair<TVectorD, TVectorD>& dArg, ui32 index);

	// template <typename T>
	// ui32 GetReturnValueSize(const T& v) {
	// 	return v.size();
	// }

	// template <>
	// ui32 GetReturnValueSize(const double& v);

	// template <>
	// ui32 GetReturnValueSize(const TPair<TVectorD, TVectorD>& v);

	template <typename T, typename A, typename Derived>
	class TOneArgFunctorResultBase : public TFunctorResult<T, Derived> {
	public:
		using TCalcArgDerivCb = std::function<T()>;
		using TCalcPartialArgDerivCb = std::function<TVector<T>()>;

		TOneArgFunctorResultBase() :
			CalcArgDerivCb([=]() -> T {
				throw TEgoNotImplemented() << "Calculation of argument derivative was not implemented";
			}),
			CalcPartialArgDerivCb([=]() -> TVector<T> {
				return TPartialDerivative<T,A>::Default(ArgDeriv());
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

		TVector<T> PartialArgDeriv() const {
			return CalcPartialArgDerivCb();
		}

	private:
		TCalcArgDerivCb CalcArgDerivCb;
		TCalcPartialArgDerivCb CalcPartialArgDerivCb;
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
