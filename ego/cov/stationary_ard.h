#pragma once

#include "cov.h"
#include "stationary_kernels.h"
#include "square_dist.h"

#include <ego/base/errors.h>
#include <ego/base/factory.h>
#include <ego/util/log/log.h>

#include <ego/base/la.h>

#include <ego/func/two_arg.h>
#include <ego/func/one_arg.h>


namespace NEgo {

	template <typename TKernelFunctor>
    class TCovStationaryARD : public ICov {

    	static_assert(std::is_base_of<TOneArgFunctor<TMatrixD, TMatrixD>, TKernelFunctor>::value,
    		"TKernelFunctor must be a descendant of TOneArgFunctor<TMatrixD, TMatrixD>");

    public:
        TCovStationaryARD(size_t dimSize)
        	: ICov(dimSize)
        	, SqDistFunctor(dimSize)
        	, KernelFunctor(dimSize)
        {
        	Parameters.resize(dimSize + 1);
        	Parameters[0] = -0.6931472;
        	std::fill(Parameters.begin()+1, Parameters.end(), -4.60517);
        }

        TCovStationaryARD::Result UserCalc(const TMatrixD& left, const TMatrixD& right) const override final {
        	ENSURE(left.n_cols == DimSize, "Col size of left input matrix are not satisfy to kernel params: " << left.n_cols << " != " << DimSize);
	        ENSURE(right.n_cols == DimSize, "Col size of right input matrix are not satisfy to kernel params: " << right.n_cols << " != " << DimSize);

	        double var = exp(2.0 * Parameters[0]);

	        TMatrixD l = NLa::Zeros(DimSize, DimSize);
	        for (ui32 di=0; di<DimSize; ++di) {
	        	l(di, di) = 1.0/exp(Parameters[di+1]);
	        }

	        auto sqDistRes = SqDistFunctor(left * l, right * l);
	        TMatrixD r = sqDistRes.Value();

	        auto Kres = KernelFunctor(r);

	        TMatrixD K = Kres.Value();
	        TMatrixD dKdArg = Kres.ArgDeriv();

	        return TCovStationaryARD::Result()
	        	.SetValue(
	        		[=]() -> TMatrixD {
	                	return var * K;
	            	}
	        	)
	        	.SetParamDeriv(
		            [=]() -> TVector<TMatrixD> {
		            	TVector<TMatrixD> dK(GetParametersSize());

		                dK[0] = 2.0 * var * K;
		                for (ui32 di=0; di<DimSize; ++di) {
		                	dK[di+1] = var * dKdArg;

                            for (ui32 ri=0; ri < left.n_rows; ++ri) {
		                		for (ui32 rj=0; rj < right.n_rows; ++rj) {
		                			if(std::abs(r(ri, rj)) < std::numeric_limits<double>::epsilon()) {
                                        dK[di+1](ri, rj) *= 0.0;
                                    } else {
                                        dK[di+1](ri, rj) *= l(di, di) * l(di, di) *
                                            (left(ri, di) - right(rj, di)) *
                                            (right(rj, di) - left(ri, di)) / r(ri, rj);
                                    }

		                		}
		                	}
		                }
		                return dK;
		            }
		        )
                .SetSecondArgDeriv(
                    [=]() -> TMatrixD {
                        TMatrixD secArgDeriv = var * dKdArg;
                        for (ui32 ri=0; ri < left.n_rows; ++ri) {
                            for (ui32 rj=0; rj < right.n_rows; ++rj) {
                                if(std::abs(r(ri, rj)) < std::numeric_limits<double>::epsilon()) {
                                    secArgDeriv(ri, rj) *= 0.0;
                                } else {
                                    TMatrixD diff = left.row(ri) - right.row(rj);
                                    double sum = - NLa::Sum((diff * l) * l);
                                    secArgDeriv(ri, rj) *= sum / r(ri, rj);
                                }

                            }
                        }
                        return secArgDeriv;
                    }
                )
                .SetSecondArgPartialDeriv(
                    [=](ui32 indexRow, ui32 indexCol) -> TMatrixD {
                        return var * dKdArg % sqDistRes.SecondArgPartialDeriv(indexRow, indexCol) * l(indexCol, indexCol);
                    }
                );
	    }

	    size_t GetParametersSize() const override final {
	        return 1 + DimSize + KernelFunctor.GetParametersSize();
	    }

		void SetParameters(const TVector<double>& parameters) override final {
        	ENSURE(parameters.size() >= DimSize + 1, "Bad number of paramters");

        	Parameters.resize(DimSize + 1);
			std::copy(parameters.begin(), parameters.begin() + DimSize + 1, Parameters.begin());

			TVector<double> kernParams;
        	for(size_t pi=DimSize+1; pi < (DimSize+1+KernelFunctor.GetParametersSize()); ++pi) {
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

    using TCovSqExpARD = TCovStationaryARD<NKernels::TSqExp>;
    using TCovExpARD = TCovStationaryARD<NKernels::TExp>;
    using TCovMatern32ARD = TCovStationaryARD<NKernels::TMatern32>;
    using TCovMatern52ARD = TCovStationaryARD<NKernels::TMatern52>;

    REGISTER_COV(TCovSqExpARD);
    REGISTER_COV(TCovExpARD);
    REGISTER_COV(TCovMatern32ARD);
    REGISTER_COV(TCovMatern52ARD);

} // namespace NEgo
