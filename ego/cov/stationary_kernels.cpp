#include "stationary_kernels.h"


namespace NEgo {
    namespace NKernels {

    	TSqExp::TSqExp(size_t dimSize)
    		: TParent(dimSize)
		{
		}
		
		TSqExp::Result TSqExp::UserCalc(const TMatrixD &r) const {
			TMatrixD K = NLa::Exp( - 0.5 * NLa::Pow(r, 2.0));
            return TSqExp::Result()
				.SetValue(
					[=]() -> TMatrixD {
						return K;
					}
				)
				.SetArgDeriv(
					[=]() -> TMatrixD {
						return - r % K;
					}
				);
		}

    } // namespace NKernels
} //namespace NEgo