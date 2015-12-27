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
        
        TMatrixD Log(const TMatrixD &m);

        double GetLastElem(const TVectorD &m);

        TMatrixD SquareDist(const TMatrixD &left, const TMatrixD &right);

        TMatrixD DiagMat(const TVectorD &v);

        TMatrixD DiagMat(double v, size_t size);

        TVectorD Diag(const TMatrixD &m);

        TMatrixD ColMean(const TMatrixD &m);

        TMatrixD RowMean(const TMatrixD &m);

        TMatrixD Trans(const TMatrixD &m);

        TMatrixD Trans(const TMatrixD &&m);

        TMatrixD RepMat(const TMatrixD &v, size_t per_row, size_t per_col);

        double Sum(const TMatrixD &m);

        TMatrixD ColSum(const TMatrixD &m);

        TMatrixD RowSum(const TMatrixD &m);

        TMatrixD Sqrt(const TMatrixD &m);

        double Exp(double v);

        TMatrixD ReadCsv(TString fname);

        void WriteCsv(const TMatrixD &m, TString fname);

        TMatrixD HeadCols(const TMatrixD& m, size_t num);

        TMatrixD TailCols(const TMatrixD& m, size_t num);

        void Print(const TMatrixD &m);

        void Print(const TCubeD &m);

        TVectorD Ones(size_t n);

        TVectorD Zeros(size_t n);

        TMatrixD Eye(size_t n);

        TMatrixD Chol(const TMatrixD &m);

        TVectorD Solve(const TMatrixD &A, const TMatrixD &B);

        TMatrixD CholSolve(const TMatrixD &A, const TMatrixD &B);

        TVectorD AsVector(const TMatrixD &m);

        double AsScalar(const TMatrixD &m);

        std::vector<std::vector<double>> MatToStdVec(const TMatrixD &m);

        TMatrixD StdVecToMat(const std::vector<std::vector<double>> &m);

        double Trace(const TMatrixD &d);

        void ForEach(TMatrixD &m, std::function<void(double&)> f);

        bool IsNan(const double &v);

        TVectorD SubVec(const TVectorD &v, size_t from, size_t to);

        TMatrixD ColBind(const TMatrixD &l, const TMatrixD &r);

        TVectorD UnifVec(size_t size);
    } // namespace NLa

    using NLa::TMatrixD;
    using NLa::TVectorD;
    using NLa::TCubeD;

} //namespace NEgo

