#pragma once

#include <ego/base/la.h>

namespace NEgo {


	template <typename T, typename A>
	struct TPartialDerivative;

	template <typename T>
	struct TPartialDerivative<T, TMatrixD> {
		using TCalcFunction = std::function<T(ui32, ui32)>;

		static T Default(const T& dArg, ui32 indexRow, ui32 indexCol) {
			throw TErrNotImplemented() << "Calculation of partial argument derivative was not implemented";
		}
	};


	template <typename T>
	struct TPartialDerivative<T, TVectorD> {
		using TCalcFunction = std::function<T(ui32)>;

		static T Default(const T& dArg, ui32 index) {
			throw TErrNotImplemented() << "Calculation of partial argument derivative was not implemented";
		}
	};

	template <>
	TMatrixD TPartialDerivative<TMatrixD, TMatrixD>::Default(const TMatrixD& dArg, ui32 indexRow, ui32 indexCol);

	template <>
	TVectorD TPartialDerivative<TVectorD, TVectorD>::Default(const TVectorD& dArg, ui32 index);

	template <typename T, typename Derived>
	class TFunctorResult {
	public:
		using TCalcCb = std::function<T()>;
		using TCalcParamDerivCb = std::function<TVector<T>()>;

		TFunctorResult() :
			CalcCb([=]() -> T {
				throw TErrNotImplemented() << "Calculation was not implemented";
			}),
			CalcParamDerivCb([=]() -> TVector<T> {
				throw TErrNotImplemented() << "Calculation of parameters derivative was not implemented";
			})
		{
		}

		Derived& SetValue(TCalcCb cb) {
			CalcCb = cb;
			return *static_cast<Derived*>(this);
		}

		Derived& SetParamDeriv(TCalcParamDerivCb cb) {
			CalcParamDerivCb = cb;
			return *static_cast<Derived*>(this);
		}

		T Value() const {
			return CalcCb();
		}

		TVector<T> ParamDeriv() const {
			return CalcParamDerivCb();
		}

	private:
		TCalcCb CalcCb;
		TCalcParamDerivCb CalcParamDerivCb;
	};

	template <typename T>
	class TFunctorBase {
	public:

		TFunctorBase(size_t dimSize)
			: DimSize(dimSize)
			, MetaEntity(false)
		{
		}

        virtual void SetParameters(const TVector<double>& parameters) {
        	Parameters = parameters;
        }

        virtual TVector<double> GetParameters() const {
        	return Parameters;
        }
        const size_t& GetDimSize() const {
            return DimSize;
        }

	protected:
        size_t DimSize;
		TVector<double> Parameters;

		bool MetaEntity;
	};


} // namespace NEgo
