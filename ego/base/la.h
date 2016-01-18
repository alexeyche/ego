#pragma once

#include <ego/base/base.h>

#define ARMA_DONT_USE_WRAPPER
#define ARMA_USE_LAPACK
#define ARMA_USE_BLAS

#include <armadillo>

#include <ego/util/tagged_tuple.h>


namespace NEgo {
    namespace NLa {
        using TCubeD = arma::Cube<double>;
        using TMatrixD = arma::Mat<double>;
        using TVectorD = arma::Col<double>;
        using TVectorUW = arma::Col<arma::uword>;
        using TRunningStat = arma::running_stat<double>;

        namespace NGauherTup {
            struct quad {};
            struct weights {};
        } // NGauherTup

        using TGauherTup = TTagTuple<
            NGauherTup::quad, TVectorD,
            NGauherTup::weights, TVectorD
        >;

        TGauherTup Gauher();

        TMatrixD Exp(const TMatrixD &m);

        TMatrixD MatrixFromConstant(size_t r, size_t c, double val);

        TVectorD VectorFromConstant(size_t vsize, double val);
        
        TMatrixD Log(const TMatrixD &m);

        double Log(double v);

        double GetLastElem(const TVectorD &m);

        double GetFirstElem(const TVectorD &m);

        TMatrixD SquareDist(const TMatrixD &left, const TMatrixD &right);

        TMatrixD DiagMat(const TVectorD &v);

        TMatrixD DiagMat(double v, size_t size);

        TVectorD Diag(const TMatrixD &m);

        TMatrixD ColMean(const TMatrixD &m);

        TMatrixD RowMean(const TMatrixD &m);

        double Mean(const TMatrixD &m);

        TMatrixD Cov(const TMatrixD &m);

        TMatrixD Trans(const TMatrixD &m);

        TMatrixD Trans(const TMatrixD &&m);

        TMatrixD RepMat(const TMatrixD &v, size_t per_row, size_t per_col);

        double Sum(const TMatrixD &m);

        double Sum(const double &m);

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

        TMatrixD Ones(size_t r, size_t c);

        TVectorD Zeros(size_t n);
        
        TMatrixD Zeros(size_t r, size_t c);

        TMatrixD Eye(size_t n);

        TMatrixD Chol(const TMatrixD &m);

        TMatrixD Solve(const TMatrixD &A, const TMatrixD &B);

        TMatrixD CholSolve(const TMatrixD &A, const TMatrixD &B);

        TVectorD AsVector(const TMatrixD &m);

        double AsScalar(const TMatrixD &m);

        std::vector<double> VecToStd(const TVectorD &v);

        TVectorD StdToVec(const std::vector<double> &v);

        TMatrixD StdVecToMat(const std::vector<std::vector<double>> &m);

        std::vector<std::vector<double>> MatToStdVec(const TMatrixD &m);

        TMatrixD StdVecToMat(const std::vector<std::vector<double>> &m);

        double Trace(const TMatrixD &d);

        void ForEach(TMatrixD &m, std::function<void(double&)> f);

        bool IsNan(const double &v);

        bool IsNan(const TVectorD &v);

        TVectorD SubVec(const TVectorD &v, size_t from, size_t to);

        TMatrixD ColBind(const TMatrixD &l, const TMatrixD &r);

        TMatrixD RowBind(const TMatrixD &l, const TMatrixD &r);

        TVectorD UnifVec(size_t size);

        TString VecToStr(const TVectorD &v, TString delim = ", ");

        template <typename T>
        bool Any(T &&cond) {
            return arma::any(cond);
        }

        TMatrixD Abs(const TMatrixD &m);

        TMatrixD Sign(const TMatrixD &m);

        TVectorD Vectorize(const TMatrixD &m);

        namespace NLuTup {
            struct L {};
            struct U {};
            struct P {};
        };
        
        using TLuTup = TTagTuple<
            NLuTup::L, TMatrixD, 
            NLuTup::U, TMatrixD, 
            NLuTup::P, TMatrixD
        >;

        TLuTup Lu(const TMatrixD &X);

        double Prod(const TVectorD &v);

        TVectorD Linspace(double start, double end, ui32 N);

        template <typename T>
        TVectorUW Find(T &&cond) {
            return arma::find(cond);
        }

        double LogGamma(double v);

        double Norm(const TVectorD &v);

        TVectorD MaxOverCols(const TMatrixD &m);
        
        double Max(const TVectorD& m);
        
        double Min(const TVectorD& m);

        TVectorD LogExpAx(const TMatrixD &A, const TVectorD &x);

        TVectorD Inf(size_t v);

        double Psi(double v);

        TMatrixD TriangLow(const TMatrixD &m, bool withoutMain);

        void DebugSave(const TMatrixD &m, TString s);

        TMatrixD Cos(const TMatrixD& m);

        TMatrixD Sin(const TMatrixD& m);

        TMatrixD Pow(const TMatrixD& m, double pow);

        TVectorD LogSumExp(const TMatrixD &v);
        
        double Erf(double x);

        double NormPdf(double x, double mu = 0.0, double sigma = 1.0);
        
        double NormCdf(double x, double mu = 0.0, double sigma = 1.0);
        
        double Factorial(ui32 val);

        void AddCholeskyRow(TMatrixD &L, const TVectorD &v);

        TMatrixD SampleMultivariateGauss(const TVectorD &mu, const TMatrixD &cov, ui32 nsamp);

    } // namespace NLa

    using NLa::TMatrixD;
    using NLa::TVectorD;
    using NLa::TCubeD;

} //namespace NEgo

