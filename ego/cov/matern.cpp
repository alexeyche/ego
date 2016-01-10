#include "matern.h"

namespace NEgo {
	namespace NMaternFuncs {

        TMatrixD Matern1(const TMatrixD &K) {
            return NLa::MatrixFromConstant(K.n_rows, K.n_cols, 1.0);
        }

        TMatrixD MaternARDDeriv1(const TMatrixD &K) {
        	TMatrixD ans = 1.0/K;
        	NLa::ForEach(ans, [](double &val) { if (NLa::IsNan(val)) val = 0.0; });
        	return ans;
        }

        TMatrixD MaternISODeriv1(const TMatrixD &K) {
            return NLa::MatrixFromConstant(K.n_rows, K.n_cols, 1.0);   
        }

		TMatrixD Matern3(const TMatrixD &K) {
			return 1 + K;
		}

        TMatrixD MaternARDDeriv3(const TMatrixD &K) {
        	return NLa::MatrixFromConstant(K.n_rows, K.n_cols, 1.0);
        }

        TMatrixD MaternISODeriv3(const TMatrixD &K) {
            return K;
        }

        TMatrixD Matern5(const TMatrixD &K) {
        	return 1.0 + K % (1.0 + K/3.0);
        }

        TMatrixD MaternARDDeriv5(const TMatrixD &K) {
        	return (1.0 + K)/3.0;
        }

        TMatrixD MaternISODeriv5(const TMatrixD &K) {
            return K % (1 + K)/3.0;
        }
    } // namespace NMaternFuncs

}; // namespace NEgo
