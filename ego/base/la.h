#pragma once

#include <ego/base/base.h>

#include <armadillo>

namespace NEgo {
    namespace NLa {
        using TCubeD = arma::Cube<double>;
        using TMatrixD = arma::Mat<double>;
        using TVectorD = arma::Col<double>;

        TMatrixD Exp(const TMatrixD &m);

        TMatrixD MatrixFromConstant(size_t r, size_t c, double val);

        double GetLastElem(const TVectorD &m);

        TMatrixD SquareDist(const TMatrixD &left, const TMatrixD &right);

        TMatrixD Diag(const TVectorD &v);

        TMatrixD Diag(double v, size_t size);

        TMatrixD ColMean(const TMatrixD &m);

        TMatrixD RowMean(const TMatrixD &m);

        TMatrixD Trans(const TMatrixD &m);

        TMatrixD Trans(const TMatrixD &&m);

        TMatrixD RepMat(const TMatrixD &v, size_t per_row, size_t per_col);

        TMatrixD ColSum(const TMatrixD &m);

        TMatrixD RowSum(const TMatrixD &m);

        TMatrixD Sqrt(const TMatrixD &m);

        double Exp(double v);

        TMatrixD ReadCsv(TString fname);

        void WriteCsv(const TMatrixD &m, TString fname);

        TMatrixD HeadCols(const TMatrixD& m, size_t num);

        TMatrixD TailCols(const TMatrixD& m, size_t num);

        void Print(const TMatrixD &m);

        TVectorD Ones(size_t n);

    } // namespace NLa

    using NLa::TMatrixD;
    using NLa::TVectorD;
    using NLa::TCubeD;


} //namespace NEgo
