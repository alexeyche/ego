
#pragma once

#include "cov.h"
#include "stationary_kernels.h"

#include <ego/base/errors.h>
#include <ego/base/factory.h>
#include <ego/util/log/log.h>

#include <ego/base/la.h>

#include <ego/func/two_arg.h>
#include <ego/func/one_arg.h>

#include <numeric>
#include <functional>

namespace NEgo {
	
	class TSquareDistFunctor : public TTwoArgFunctor<TMatrixD, TMatrixD, TMatrixD> {
	public:
		TSquareDistFunctor::Result UserCalc(const TMatrixD &left, const TMatrixD &right) override final {
			return TSquareDistFunctor::Result()
				.SetCalc(
					[=]() -> TMatrixD {
						// return (left - right) % (left - right);
						return NLa::SquareDist(left, right);
					}
				)
				.SetCalcFirstArgDeriv(
					[=]() -> TMatrixD {
						return 2.0 * (left - right);
					}
				)
				.SetCalcSecondArgDeriv(
					[=]() -> TMatrixD {
						return - 2.0 * (left - right);
					}
				);
		}
	};

	template <typename TKernelFunctor>
    class TCovStationaryISO : public ICov {
    	
    	static_assert(std::is_base_of<TOneArgFunctor<TMatrixD, TMatrixD>, TKernelFunctor>::value, 
    		"TKernelFunctor must be a descendant of TOneArgFunctor<TMatrixD, TMatrixD>");

    public:
        TCovStationaryISO(size_t dim_size) 
        	: ICov(dim_size)
        {
        }

        TCovStationaryISO::Result UserCalc(const TMatrixD &left, const TMatrixD &right) override final {
        	ENSURE(left.n_cols == DimSize, "Col size of left input matrix are not satisfy to kernel params: " << DimSize << " != " << left.n_cols);
	        ENSURE(right.n_cols == DimSize, "Col size of right input matrix are not satisfy to kernel params: " << DimSize << " != " << right.n_cols);
	        
	        const double& l = Parameters[0];
        	const double& var = Parameters[1];
	        
	        auto sqDistRes = SqDistFunctor(left, right);

	        auto Kres = KernelFunctor(sqDistRes.Value()/l);
	        
	        TMatrixD K = Kres.Value();
	        TMatrixD dKdArg = Kres.ArgDeriv();

	        return TCovStationaryISO::Result()
	        	.SetCalc(
	        		[=]() -> TMatrixD {
	                	return var * K;
	            	}
	        	)
	        	.SetCalcParamDeriv(
		            [=]() -> TVector<TMatrixD> {
		            	TVector<TMatrixD> dK(GetParametersSize());

		                dK[0] = - var * K % dKdArg / (l*l);
		                dK[1] = K;
		                return dK;
		            }
		        )
		        .SetCalcSecondArgDeriv(
	        		[=]() -> TMatrixD {
	        			return var * dKdArg % sqDistRes.SecondArgDeriv() / l;
	        		}
	        	);
        }

	    size_t GetParametersSize() const override final {
	        return 2 + KernelFunctor.GetParametersSize();
	    }

        void SetParameters(const TVector<double>& parameters) override final {
        	Parameters = {parameters[0], parameters[1]};
			TVector<double> kernParams;
        	for(size_t pi=2; pi < KernelFunctor.GetParametersSize(); ++pi) {
        		kernParams.push_back(parameters[pi]);
        	}
        	KernelFunctor.SetParameters(kernParams);
        }

        TVector<double> GetParameters() const override final  {
        	TVector<double> params = Parameters;
        	TVector<double> kernParams = KernelFunctor.GetParameters();
        	params.insert(params.end(), kernParams.begin(), kernParams.end());
        	return params;
        }

    private:
    	TSquareDistFunctor SqDistFunctor;
		TKernelFunctor KernelFunctor;
    };



	using TCovSqExpISO = TCovStationaryISO<NKernels::TSqExp>;

	REGISTER_COV(TCovSqExpISO);

} //namespace NEgo