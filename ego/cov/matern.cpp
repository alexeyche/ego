#include "matern.h"

namespace NEgo {
	namespace NMaternFuncs {

        TMatrixD Matern1(const TMatrixD &K) {
            return NLa::MatrixFromConstant(K.n_rows, K.n_rows, 1.0);
        }

        TMatrixD MaternDeriv1(const TMatrixD &K) {
        	TMatrixD ans = 1.0/K;
        	NLa::ForEach(ans, [](double &val) { if (NLa::IsNan(val)) val = 0.0; });
        	return ans;
        }


		TMatrixD Matern3(const TMatrixD &K) {
			return 1 + K;
		}

        TMatrixD MaternDeriv3(const TMatrixD &K) {
        	return NLa::MatrixFromConstant(K.n_rows, K.n_rows, 1.0);
        }

        TMatrixD Matern5(const TMatrixD &K) {
        	return 1.0 + K % (1.0 + K/3.0);
        }

        TMatrixD MaternDeriv5(const TMatrixD &K) {
        	return (1.0 + K)/3.0;
        }

    } // namespace NMaternFuncs

}; // namespace NEgo
