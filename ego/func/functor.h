#pragma once

#include <ego/base/la.h>

namespace NEgo {
	template <typename T>
	class TFunctorBase {
	public:
		template <typename Derived>
		class TFunctorResult {
		public:
			using TCalcCb = std::function<T()>;
			using TCalcParamDerivCb = std::function<TVector<T>()>;

			TFunctorResult() : 
				CalcCb([=]() -> T {
					throw TEgoNotImplemented() << "Calculation was not implemented";
				}),
				CalcParamDerivCb([=]() -> TVector<T> {
					throw TEgoNotImplemented() << "Calculation of parameters derivative was not implemented";
				})
			{
			}
			
			Derived& SetCalc(TCalcCb cb) {
				CalcCb = cb;
				return *static_cast<Derived*>(this);
			}
		
			Derived& SetCalcParamDeriv(TCalcParamDerivCb cb) {
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
		
		
        virtual void SetParameters(const TVector<double>& parameters) {
        	Parameters = parameters;
        }

        virtual TVector<double> GetParameters() const {
        	return Parameters;
        }

	protected:
		TVector<double> Parameters;	
	};


} // namespace NEgo
