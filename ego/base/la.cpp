#include "la.h"

#include <ego/contrib/digamma.h>

#include <ego/base/errors.h>
#include <ego/util/log/log.h>

#include <ego/util/fs.h>

#include <math.h>

extern "C" {
    int dpotrs_(char *, int *, int *, double *, int *, double *, int *, int *);
}

namespace NEgo {
    namespace NLa {

        const ui32 GaussianHermitQuadSize = 20;

        const double GaussianHermitQuad[] = {
            -7.6190485417, -6.5105901570, -5.5787388059, -4.7345813340, -3.9439673507,
            -3.1890148166, -2.4586636112, -1.7452473208, -1.0429453488, -0.3469641571,
            0.3469641571, 1.0429453488, 1.7452473208, 2.4586636112, 3.1890148166,
            3.9439673507, 4.7345813340, 5.5787388059, 6.5105901570, 7.6190485417
        };

        const double GaussianHermitQuadWeights[] = {
            0.0000000000, 0.0000000002, 0.0000000613, 0.0000044021, 0.0001288263,
            0.0018301031, 0.0139978374, 0.0615063721, 0.1617393340, 0.2607930634,
            0.2607930634, 0.1617393340, 0.0615063721, 0.0139978374, 0.0018301031,
            0.0001288263, 0.0000044021, 0.0000000613, 0.0000000002, 0.0000000000
        };

        TGauherTup Gauher() {
            return {
                TVectorD(GaussianHermitQuad, GaussianHermitQuadSize),
                TVectorD(GaussianHermitQuadWeights, GaussianHermitQuadSize)
            };
        }

        TMatrixD Exp(const TMatrixD &m) {
            return arma::exp(m);
        }

        double Exp(double v) {
            return std::exp(v);
        }

        TMatrixD Log(const TMatrixD &m) {
            return arma::log(m);
        }

        double Log(double v) {
            return std::log(v);
        }

        TMatrixD MatrixFromConstant(size_t r, size_t c, double val) {
            TMatrixD m(r, c);
            m.fill(val);
            return m;
        }

        TVectorD VectorFromConstant(size_t vsize, double val) {
            TVectorD v(vsize);
            v.fill(val);
            return v;
        }

        double GetLastElem(const TVectorD &m) {
            ENSURE(m.n_rows > 0, "Got empty vector/matrix");
            return m.tail(1)(0);
        }

        TMatrixD SquareDist(const TMatrixD &left, const TMatrixD &right) {
            ENSURE(left.n_cols == right.n_cols, "Column length must agree");
            TVectorD left2sum = NLa::RowSum(left % left);
            TVectorD right2sum = NLa::RowSum(right % right);
            NLa::Print(NLa::RepMat(left2sum, 1, left.n_cols));
            NLa::Print(NLa::RepMat(NLa::Trans(right2sum), right.n_cols, 1));
            TMatrixD res2 = - 2.0 * left * NLa::Trans(right) +
                NLa::RepMat(left2sum, 1, left.n_cols) +
                NLa::RepMat(NLa::Trans(right2sum), right.n_cols, 1);
            ForEach(res2, [](double &v) { if(v<0.0) v = 0.0; });
            return Sqrt(res2);
            // TMatrixD mu = left.n_rows/static_cast<double>(left.n_rows + right.n_rows) * ColMean(left) +
            //               right.n_rows/static_cast<double>(left.n_rows + right.n_rows) * ColMean(right);

            // TMatrixD leftM = left - RepMat(mu, left.n_rows, 1);
            // TMatrixD rightM = right - RepMat(mu, right.n_rows, 1);

            // return RepMat(RowSum(leftM % leftM), 1, right.n_rows) +
            //        Trans(RepMat(RowSum(rightM % rightM), 1, left.n_rows)) -
            //        2.0 * leftM * Trans(rightM);
        }

        TMatrixD DiagMat(const TVectorD &v) {
            return arma::diagmat(v);
        }

        TMatrixD DiagMat(double v, size_t size) {
            TVectorD vecV(size);
            vecV.fill(v);
            return DiagMat(vecV);
        }

        TVectorD Diag(const TMatrixD &m) {
            return arma::diagvec(m);
        }

        TMatrixD ColMean(const TMatrixD &m) {
            return arma::mean(m, 0);
        }

        TMatrixD RowMean(const TMatrixD &m) {
            return arma::mean(m, 1);
        }

        double Mean(const TMatrixD &m) {
            return arma::mean(arma::mean(m));
        }

        TMatrixD Cov(const TMatrixD &m) {
            return arma::cov(m);
        }

        TMatrixD Trans(const TMatrixD &m) {
            return arma::trans(m);
        }

        TMatrixD Trans(const TMatrixD &&m) {
            return arma::trans(m);
        }

        TMatrixD RepMat(const TMatrixD &v, size_t per_row, size_t per_col) {
            return arma::repmat(v, per_row, per_col);
        }

        double Sum(const TMatrixD &m) {
            return arma::accu(m);
        }

        double Sum(const double &m) {
            return m;
        }

        TMatrixD ColSum(const TMatrixD &m) {
            return arma::sum(m, 0);
        }

        TMatrixD RowSum(const TMatrixD &m) {
            return arma::sum(m, 1);
        }

        TMatrixD Sqrt(const TMatrixD &m) {
            return arma::sqrt(m);
        }

        TMatrixD ReadCsv(TString fname) {
            TMatrixD res;
            ENSURE(res.load(fname), "Failed to read csv file: " << fname);
            return res;
        }

        void WriteCsv(const TMatrixD &m, TString fname) {
            ENSURE(m.save(fname, arma::csv_ascii), "Failed to write csv file: " << fname);
        }

        TMatrixD HeadCols(const TMatrixD& m, size_t num) {
            return m.head_cols(num);
        }

        TMatrixD TailCols(const TMatrixD& m, size_t num) {
            return m.tail_cols(num);
        }

        void Print(const TMatrixD &m) {
            std::cout << "Matrix [" << m.n_rows << "x" <<  m.n_cols << "]====\n";
            std::cout.precision(7);
            std::cout.setf(std::ios::fixed);
            m.raw_print(std::cout);
            std::cout << "====\n";
        }

        void Print(const TCubeD &m) {
            std::cout << "Cube [" << m.n_rows << "x" <<  m.n_cols << "x" << m.n_slices << "]====\n";
            std::cout.precision(7);
            std::cout.setf(std::ios::fixed);
            m.raw_print(std::cout);
            std::cout << "====\n";
        }

        TVectorD Ones(size_t n) {
            return arma::ones(n);
        }

        TMatrixD Ones(size_t r, size_t c) {
            return arma::ones(r, c);
        }

        TVectorD Zeros(size_t n) {
            return arma::zeros(n);
        }

        TMatrixD Zeros(size_t r, size_t c) {
            return arma::zeros(r, c);
        }

        TMatrixD Eye(size_t n) {
            return arma::eye(n, n);
        }

        TMatrixD Chol(const TMatrixD &m) {
            TMatrixD ans;
            ENSURE_ERR(arma::chol(ans, m), TErrAlgebraError() << "Cholesky decomposition failed");
            return ans;
        }

        TMatrixD Solve(const TMatrixD &A, const TMatrixD &B) {
            TMatrixD ans;
            ENSURE(arma::solve(ans, A, B), "Solving system is failed");
            return ans;
        }



        TMatrixD CholSolve(const TMatrixD &A, const TMatrixD &B) {
            ENSURE(A.n_rows == A.n_cols, "First argument must me square");
            ENSURE(A.n_rows == B.n_rows, "Both arguments must have the same number of rows");
            int n = A.n_rows;
            int m = B.n_cols;
            TMatrixD C(n, m);
            C = B;
            int q;
            char uplo = 'U';
            double *v = const_cast<double*>(A.memptr());
            dpotrs_(&uplo, &n, &m, v, &n, C.memptr(), &n, &q);
            ENSURE(q == 0, "Failed to run solve system with cholesky factorization, error status: " << q);
            return C;
        }

        TVectorD AsVector(const TMatrixD &m) {
            ENSURE(m.n_cols == 1, "Trying to convert to vector matrix with many columns: 1<" << m.n_cols);
            return m.col(0);
        }

        double AsScalar(const TMatrixD &m) {
            ENSURE(m.n_rows == 1, "Trying to convert matrix with " << m.n_rows << " rows to scalar");
            ENSURE(m.n_cols == 1, "Trying to convert matrix with " << m.n_cols << " cols to scalar");
            return m(0, 0);
        }

        std::vector<double> VecToStd(const TVectorD &v) {
            return arma::conv_to<std::vector<double>>::from(v);
        }

        TVectorD StdToVec(const std::vector<double> &v) {
            return arma::conv_to<TVectorD>::from(v);
        }


        std::vector<std::vector<double>> MatToStdVec(const TMatrixD &m) {
            std::vector<std::vector<double>> V(m.n_rows);
            for (size_t i = 0; i < m.n_rows; ++i) {
                V[i] = VecToStd(m.row(i));
            }
            return V;
        }

        TMatrixD StdVecToMat(const std::vector<std::vector<double>> &m) {
            if(m.size() == 0) return TMatrixD();
            size_t nrow = m.size();
            size_t ncol = m[0].size();
            TMatrixD r(nrow, ncol);
            for(size_t i=0; i<nrow; ++i) {
                r.row(i) = StdToVec(m[i]);
            }
            return r;
        }

        double Trace(const TMatrixD &d) {
            return arma::trace(d);
        }

        void ForEach(TMatrixD &m, std::function<void(double&)> f) {
            m.for_each(f);
        }

        bool IsNan(const double &v) {
            return !arma::is_finite(v);
        }

        bool IsNan(const TVectorD &v) {
            return v.has_nan();
        }

        TVectorD SubVec(const TVectorD &v, size_t from, size_t to) {
            return v.subvec(from, to-1);
        }

        TMatrixD ColBind(const TMatrixD &l, const TMatrixD &r) {
            return arma::join_rows(l, r);
        }

        TMatrixD RowBind(const TMatrixD &l, const TMatrixD &r) {
            return arma::join_cols(l, r);
        }

        TVectorD UnifVec(size_t size) {
            return TVectorD(size, arma::fill::randu);
        }

        TString VecToStr(const TVectorD &v, TString delim) {
            std::stringstream ss;
            for(const auto& val: v) {
                ss << val << delim;
            }
            return ss.str();
        }


        TMatrixD Abs(const TMatrixD &m) {
            return arma::abs(m);
        }

        TMatrixD Sign(const TMatrixD &m) {
            return arma::sign(m);
        }

        TVectorD Vectorize(const TMatrixD &m) {
            return arma::vectorise(m);
        }

        TLuTup Lu(const TMatrixD &X) {
            TMatrixD L, U, P;
            arma::lu(L, U, P, X);
            return {L, U, P};
        }

        double Prod(const TVectorD &v) {
            return arma::prod(v);
        }

        TVectorD Linspace(double start, double end, ui32 N) {
            return arma::linspace(start, end, N);
        }

        double LogGamma(double v) {
            return lgamma(v);
        }

        double Norm(const TVectorD &v) {
            return arma::norm(v);
        }

        TVectorD MaxOverCols(const TMatrixD &m) {
            return arma::max(m, 1);
        }

        double Max(const TVectorD& m) {
            return arma::max(m);
        }

        double Min(const TVectorD& m) {
            return arma::min(m);
        }

        TPair<double, ui32> MaxIdx(const TVectorD& m) {
            arma::uword index;
            TPair<double, ui32> ret;
            ret.first = m.max(index);
            ret.second = index;
            return ret;
        }

        TPair<double, ui32> MinIdx(const TVectorD& m) {
            arma::uword index;
            TPair<double, ui32> ret;
            ret.first = m.min(index);
            ret.second = index;
            return ret;
        }

        TVectorD LogExpAx(const TMatrixD &A, const TVectorD &x) {
            ui32 N = A.n_cols;
            TVectorD maxA = MaxOverCols(A);
            return NLa::Log(NLa::Exp(A - maxA * NLa::Trans(NLa::Ones(N))) * x) + maxA;
        }

        TVectorD Inf(size_t v) {
            TVectorD vec(v);
            vec.fill(arma::datum::inf);
            return vec;
        }

        double Psi(double v) {
            int ifault = 0;
            double ret = digamma(v, &ifault);
            ENSURE(ifault == 0, "Digamma calculations failed with x == " << v);
            return ret;
        }

        TMatrixD TriangLow(const TMatrixD &m, bool withoutMain) {
            TMatrixD res = arma::trimatl(m);
            if(withoutMain) {
                res.diag() = NLa::Zeros(m.n_rows);
            }
            return res;
        }

        void DebugSave(const TMatrixD &m, TString s) {
            size_t i = 0;
            TString fname(s);
            while(true) {
                std::stringstream ss;
                ss << "/var/tmp/" << s << "-" << i << ".csv";
                fname = ss.str();
                if(!FileExists(fname)) {
                    break;
                }
                i++;
            }
            L_DEBUG << "Saving " << s << ": " << fname;
            WriteCsv(m, fname);
        }

        TMatrixD Cos(const TMatrixD& m) {
            return arma::cos(m);
        }

        TMatrixD Sin(const TMatrixD& m) {
            return arma::sin(m);
        }

        TMatrixD Pow(const TMatrixD& m, double pow) {
            return arma::pow(m, pow);
        }

        TVectorD LogSumExp(const TMatrixD &v) {
            ui32 N = v.n_cols;
            TVectorD mv = MaxOverCols(v);
            return NLa::Log(
                NLa::RowSum(
                    NLa::Exp(v - NLa::RepMat(mv, 1, v.n_rows))
                )
            ) + mv;
        }


        // Returns the erf() of a value (not super precice, but ok)
        double Erf(double x) {
            double y = 1.0 / ( 1.0 + 0.3275911 * x);
            return 1 - (((((
                + 1.061405429  * y
                - 1.453152027) * y
                + 1.421413741) * y
                - 0.284496736) * y
                + 0.254829592) * y)
                * std::exp (-x * x);
        }

        double NormPdf(double x) {
            return std::exp( - x * x / 2.0) / std::sqrt(2 * M_PI);
        }

        double NormCdf(double x) {
            // constants
            double a1 =  0.254829592;
            double a2 = -0.284496736;
            double a3 =  1.421413741;
            double a4 = -1.453152027;
            double a5 =  1.061405429;
            double p  =  0.3275911;

            // Save the sign of x
            int sign = 1;
            if (x < 0)
                sign = -1;
            x = fabs(x)/sqrt(2.0);

            // A&S formula 7.1.26
            double t = 1.0/(1.0 + p*x);
            double y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*exp(-x*x);
            return 0.5*(1.0 + sign*y);
        }

        const double FactorialTable[] = {
            1.0, 1.0, 2.0, 6.0, 24.0,
            120.0, 720.0, 5040.0, 40320.0, 362880.0,
            3628800.0, 39916800.0, 479001600.0, 6227020800.0, 87178291200.0,
            1.307674e+12, 2.092279e+13, 3.556874e+14, 6.402374e+15, 1.216451e+17,
            2.432902e+18
        };

        double Factorial(ui32 val) {
            ENSURE(val < 21, "Can't calculate big factorials: " << val);
            return FactorialTable[val];
        }

        void AddCholeskyRow(TMatrixD &L, const TVectorD &v) {
            ENSURE(L.n_rows == L.n_cols, "Need quadratic matrix");
            ENSURE(L.n_rows+1 == v.size(), "Adding must be iterative: " << L.n_rows << " != " << v.size() + 1);

            const ui32 n = v.size();
            L.resize(n, n);

            double L_j;
            for (size_t j = 0; j < n-1; ++j) {
                // L_DEBUG << "ip: " << arma::dot(L(j, arma::span(0, j)), L(n-1, arma::span(0, j)));
                // if(j>0) {
                //     L_DEBUG << L(j, arma::span(0, j));
                // }
                L_j = v(j) - arma::dot(L(j, arma::span(0, j)), L(n-1, arma::span(0, j)));
                L(n-1, j) = L_j / L(j, j);
                // L_j = v(j) - arma::dot(L(arma::span(0, j), j), L(arma::span(0, j), n-1));
                // L(j, n-1) = L_j / L(j, j);
            }
            L_j = v(n-1) - arma::dot(L(n-1, arma::span(0, n-1)), L(n-1, arma::span(0, n-1)));
            // L_j = v(n-1) - arma::dot(L(arma::span(0, n-1), n-1), L(arma::span(0, n-1), n-1));
            L(n-1, n-1) = sqrt(L_j);
        }

        TVectorUW SortIndex(const TVectorD& v) {
            return arma::sort_index(v);
        }

        double Det(const TMatrixD& m) {
            double val = arma::det(m);
            if ((std::fabs(val) < 1e-15)||(std::fabs(val) > 1e+100)) {
                return 1e-15;
            }
            return val;
        }

        TVectorD SortDescend(const TVectorD& v) {
            return arma::sort(v, "descend");
        }

        // double LogDet(const TMatrixD& m) {
        //     return arma::log_det(m);   
        // }

    } // namespace NLa
} //namespace NEgo
