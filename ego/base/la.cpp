#include "la.h"

#include <ego/base/errors.h>

namespace NEgo {
    namespace NLa {

        TMatrixD Exp(const TMatrixD &m) {
            return arma::exp(m);
        }

        TMatrixD MatrixFromConstant(size_t r, size_t c, double val) {
            TMatrixD m(r, c);
            m.fill(val);
            return m;
        }

        double GetLastElem(const TVectorD &m) {
            ENSURE(m.n_rows > 0, "Got empty vector/matrix");
            return m.tail(1)(0);
        }

        TMatrixD SquareDist(const TMatrixD &m) {

        }

    } // namespace NLa
} //namespace NEgo
