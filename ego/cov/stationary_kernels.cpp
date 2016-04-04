#include "stationary_kernels.h"


namespace NEgo {
    namespace NKernels {

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

		TExp::Result TExp::UserCalc(const TMatrixD &r) const {
			TMatrixD K = NLa::Exp( - 0.5 * r);
            return TSqExp::Result()
				.SetValue(
					[=]() -> TMatrixD {
						return K;
					}
				)
				.SetArgDeriv(
					[=]() -> TMatrixD {
						return - 0.5 * K;
					}
				);
		}

		TMatern32::Result TMatern32::UserCalc(const TMatrixD &r) const {
			TMatrixD sqrt3r = sqrt(3.0) * r;
			TMatrixD Ke = NLa::Exp( - sqrt3r);
			TMatrixD K = (1.0 + sqrt3r) % Ke;
            return TSqExp::Result()
				.SetValue(
					[=]() -> TMatrixD {
						return K;
					}
				)
				.SetArgDeriv(
					[=]() -> TMatrixD {
						return  - sqrt(3.0) * sqrt3r % Ke;
					}
				);
		}
		
		TMatern52::Result TMatern52::UserCalc(const TMatrixD &r) const {
			TMatrixD sqrt5r = sqrt(5.0) * r;
			TMatrixD sq = NLa::Pow(r, 2.0) * 5.0/3.0;
			TMatrixD Ke = NLa::Exp( - sqrt5r);
			TMatrixD K = (1.0 + sqrt5r + sq) % Ke;
            return TSqExp::Result()
				.SetValue(
					[=]() -> TMatrixD {
						return K;
					}
				)
				.SetArgDeriv(
					[=]() -> TMatrixD {
						return  - (sqrt(5.0) * sq +  r * ( sqrt(5.0) * sqrt(5.0)  - 2.0 * 5.0/3.0)) % Ke;
					}
				);
		}

    } // namespace NKernels
} //namespace NEgo