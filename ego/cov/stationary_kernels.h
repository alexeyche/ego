#pragma once

#include <ego/func/one_arg.h>

namespace NEgo {
    namespace NKernels {

    	class TSqExp : public TOneArgFunctor<TMatrixD, TMatrixD> {
    		TSqExp::Result UserCalc(const TMatrixD &r) override final {
    			TMatrixD K = NLa::Exp( - 0.5 * NLa::Pow(r, 2.0));
                return TSqExp::Result()
    				.SetCalc(
    					[=]() -> TMatrixD {
    						return K;
    					}
    				)
    				.SetCalcArgDeriv(
    					[=]() -> TMatrixD {
    						return - r % K;
    					}
    				);
    		}
    	};

    } // namespace NKernels
} //namespace NEgo