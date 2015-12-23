#pragma once

#include <armadillo>

namespace NEgo {
    namespace NLa {
        using TMatrixD = arma::Mat<double>;
        using TVectorD = arma::Col<double>;

        TMatrixD Exp(const TMatrixD &m);

        TMatrixD MatrixFromConstant(size_t r, size_t c, double val);

        double GetLastElem(const TVectorD &m);

        TMatrixD SquareDist(const TMatrixD &m);

    } // namespace NLa

    using NLa::TMatrixD;
    using NLa::TVectorD;

} //namespace NEgo
