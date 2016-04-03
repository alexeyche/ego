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
	                	return  r; //var * K;
	            	}
	        	)
	        	.SetParamDeriv(
		            [=]() -> TVector<TMatrixD> {
		            	TVector<TMatrixD> dK(GetParametersSize());

		                // dK[0] = 2.0 * var * K;
		                dK[0] = NLa::Zeros(left.n_rows, right.n_rows);
		                for (ui32 di=0; di<DimSize; ++di) {
		                	auto dArg = NLa::Zeros(left.n_rows, right.n_rows);
		                	
		                	dArg = SqDistFunctor(left.col(di) * l(di, di), right.col(di) * l(di, di)).Value();
							dArg = - l(di, di) * dArg % r;

		                	// for (ui32 ri=0; ri < left.n_rows; ++ri) {
		                	// 	for (ui32 rj=0; rj < right.n_rows; ++rj) {
		                	// 		double leftV = left(ri, di);
		                	// 		double rightV = right(rj, di);
		                
		                	// 		double diff = rightV - leftV;
		                	// 		double v = - diff * diff * l(di, di);
		                			
		                	// 		dArg(ri, rj) = v / r(ri, rj);
		                	// 		// dArg(ri, rj) = - l(di, di) * leftV * (l(di, di) * leftV - rightV)/r(ri, rj);
		                	// 	}
		                	// }


		                	dK[di+1] = dArg;
		                }
		                return dK;
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

    REGISTER_COV(TCovSqExpARD);

} // namespace NEgo