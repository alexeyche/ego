#include "matern.h"

namespace NEgo {
	namespace NMaternFuncs {

        TMatrixD Matern1(const TMatrixD &K) {
            return NLa::MatrixFromConstant(K.n_rows, K.n_rows, 1.0);
        }

        TMatrixD MaternDeriv1(const TMatrixD &K) {
            return 1.0/K;
        }

    } // namespace NMaternFuncs

}; // namespace NEgo
