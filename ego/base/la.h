#pragma once

#include <armadillo>

namespace NEgo {
    namespace NLa {
        using TMatrixD = arma::Mat<double>;
        using TVectorD = arma::Col<double>;

        TMatrixD Exp(const TMatrixD &m);

        TMatrixD MatrixFromConstant(size_t r, size_t c, double val);

        double GetLastElem(const TVectorD &m);

        TMatrixD SquareDist(const TMatrixD &left, const TMatrixD &right);

        TMatrixD Diag(const TVectorD &v);

        TMatrixD Diag(double v, size_t size);

        TVectorD ColMean(const TMatrixD &m);

        TVectorD RowMean(const TMatrixD &m);

        TMatrixD Trans(const TMatrixD &m);

        TMatrixD Trans(const TMatrixD &&m);

        TMatrixD RepMat(const TMatrixD &v, size_t per_row, size_t per_col);

        TVectorD ColSum(const TMatrixD &m);

        TVectorD RowSum(const TMatrixD &m);

        TMatrixD Sqrt(const TMatrixD &m);

        double Exp(double v);
    } // namespace NLa

    using NLa::TMatrixD;
    using NLa::TVectorD;


} //namespace NEgo
