#include "la.h"


namespace NEgo {
    namespace NLa {

        TMatrixD Exp(const TMatrixD &m) {
            return m.array().exp();
        }

        TMatrixD Constant(size_t rows, size_t cols, double val) {
            return TMatrixD::Constant(rows, cols, val);
        }

    } // namespace NLa
} //namespace NEgo
