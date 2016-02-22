
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
		using TParent = TTwoArgFunctor<TMatrixD, TMatrixD, TMatrixD>;

		TSquareDistFunctor(size_t dimSize);

		TSquareDistFunctor::Result UserCalc(const TMatrixD &left, const TMatrixD &right) const override final;
	};

	template <typename TKernelFunctor>
    class TCovStationaryISO : public ICov {

    	static_assert(std::is_base_of<TOneArgFunctor<TMatrixD, TMatrixD>, TKernelFunctor>::value,
    		"TKernelFunctor must be a descendant of TOneArgFunctor<TMatrixD, TMatrixD>");

    public:
        TCovStationaryISO(size_t dimSize)
        	: ICov(dimSize)
        	, SqDistFunctor(dimSize)
        	, KernelFunctor(dimSize)
        {
        }

        TCovStationaryISO::Result UserCalc(const TMatrixD &left, const TMatrixD &right) const override final {
        	ENSURE(left.n_cols == DimSize, "Col size of left input matrix are not satisfy to kernel params: " << left.n_cols << " != " << DimSize);
	        ENSURE(right.n_cols == DimSize, "Col size of right input matrix are not satisfy to kernel params: " << right.n_cols << " != " << DimSize);

	        double l = exp(Parameters[0]);
        	double var = exp(2.0 * Parameters[1]);

	        auto sqDistRes = SqDistFunctor(left, right);
            TMatrixD r = sqDistRes.Value();
	        auto Kres = KernelFunctor(r/l);

	        TMatrixD K = Kres.Value();
	        TMatrixD dKdArg = Kres.ArgDeriv();

	        return TCovStationaryISO::Result()
	        	.SetValue(
	        		[=]() -> TMatrixD {
	                	return var * K;
	            	}
	        	)
	        	.SetParamDeriv(
		            [=]() -> TVector<TMatrixD> {
		            	TVector<TMatrixD> dK(GetParametersSize());

		                dK[0] = - var * dKdArg % r / l;
		                dK[1] = 2.0 * var * K;
		                return dK;
		            }
		        )
		        .SetSecondArgDeriv(
	        		[=]() -> TMatrixD {
	        			return var * dKdArg % sqDistRes.SecondArgDeriv() / l;
	        		}
	        	)
	        	.SetSecondArgPartialDeriv(
        			[=](ui32 indexRow, ui32 indexCol) -> TMatrixD {
	        			return var * dKdArg % sqDistRes.SecondArgPartialDeriv(indexRow, indexCol) / l;
	        		}
	        	);
        }

	    size_t GetParametersSize() const override final {
	        return 2 + KernelFunctor.GetParametersSize();
	    }

        void SetParameters(const TVector<double>& parameters) override final {
        	Parameters = {parameters[0], parameters[1]};
			TVector<double> kernParams;
        	for(size_t pi=2; pi < (2+KernelFunctor.GetParametersSize()); ++pi) {
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
    using TCovExpISO = TCovStationaryISO<NKernels::TExp>;

	REGISTER_COV(TCovSqExpISO);
    REGISTER_COV(TCovExpISO);

} //namespace NEgo