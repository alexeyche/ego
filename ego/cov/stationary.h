
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

		TSquareDistFunctor(size_t dimSize)
			: TParent(dimSize)
		{
		}

		TSquareDistFunctor::Result UserCalc(const TMatrixD &left, const TMatrixD &right) const override final {
			TVectorD left2sum = NLa::RowSum(left % left);
            TVectorD right2sum = NLa::RowSum(right % right);
            TMatrixD leftSums = NLa::RepMat(left2sum, 1, right.n_rows);
            TMatrixD rightSums = NLa::RepMat(NLa::Trans(right2sum), left.n_rows, 1);

            TMatrixD res2 = - 2.0 * left * NLa::Trans(right) + leftSums + rightSums;
            NLa::ForEach(res2, [](double &v) { if(v<0.0) v = 0.0; });
            TMatrixD r = NLa::Sqrt(res2);

            return TSquareDistFunctor::Result()
				.SetValue(
					[=]() -> TMatrixD {
						return r;
					}
				)
				.SetFirstArgDeriv(
					[=]() -> TMatrixD {
                        TMatrixD dR(left.n_rows, right.n_rows);
                        for(size_t ri=0; ri < left.n_rows; ++ri) {
                            for(size_t rj=0; rj < right.n_rows; ++rj) {
                            	double sum = NLa::Sum(left.row(ri) - right.row(rj));
                            	if(std::abs(sum) < std::numeric_limits<double>::epsilon()) {
                            		dR(ri, rj) = 0.0;	
                            	} else {
                            		dR(ri, rj) = sum/r(ri, rj);	
                            	}
                                
                            }
                        }
                        return dR;
					}
				)
				.SetSecondArgDeriv(
					[=]() -> TMatrixD {
                        TMatrixD dR(left.n_rows, right.n_rows);
                        for(size_t ri=0; ri < left.n_rows; ++ri) {
                            for(size_t rj=0; rj < right.n_rows; ++rj) {
                            	double sum = - NLa::Sum(left.row(ri) - right.row(rj));
                                if(std::abs(sum) < std::numeric_limits<double>::epsilon()) {
                            		dR(ri, rj) = 0.0;	
                            	} else {
                            		dR(ri, rj) = sum/r(ri, rj);	
                            	}
                            }
                        }
                        return dR;
					}
				);
		}
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
        	ENSURE(left.n_cols == DimSize, "Col size of left input matrix are not satisfy to kernel params: " << DimSize << " != " << left.n_cols);
	        ENSURE(right.n_cols == DimSize, "Col size of right input matrix are not satisfy to kernel params: " << DimSize << " != " << right.n_cols);

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

	REGISTER_COV(TCovSqExpISO);

} //namespace NEgo