#include "la.h"

#include <ego/base/errors.h>
#include <ego/util/log/log.h>

extern "C" {
    int dpotrs_(char *, int *, int *, double *, int *, double *, int *, int *);  
}

namespace NEgo {
    namespace NLa {
        TMatrixD Exp(const TMatrixD &m) {
            return arma::exp(m);
        }

        double Exp(double v) {
            return std::exp(v);
        }

        TMatrixD Log(const TMatrixD &m) {
            return arma::log(m);
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

        TMatrixD SquareDist(const TMatrixD &left, const TMatrixD &right) {
            ENSURE(left.n_cols == right.n_cols, "Column length must agree");
            TMatrixD mu = left.n_rows/static_cast<double>(left.n_rows + right.n_rows) * ColMean(left) +
                          right.n_rows/static_cast<double>(left.n_rows + right.n_rows) * ColMean(right);
            
            TMatrixD leftM = left - RepMat(mu, left.n_rows, 1);
            TMatrixD rightM = right - RepMat(mu, right.n_rows, 1);
            
            return RepMat(RowSum(leftM % leftM), 1, right.n_rows) +
                   Trans(RepMat(RowSum(rightM % rightM), 1, left.n_rows)) -
                   2.0 * leftM * Trans(rightM);
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

        TMatrixD Trans(const TMatrixD &m) {
            return arma::trans(m);
        }

        TMatrixD Trans(const TMatrixD &&m) {
            return arma::trans(m);
        }

        TMatrixD RepMat(const TMatrixD &v, size_t per_row, size_t per_col) {
            return arma::repmat(v, per_row, per_col);
        }

        double Sum(const TVectorD &v) {
            return arma::sum(v);
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
            m.print(std::cout);
            std::cout << "====\n";
        }

        TVectorD Ones(size_t n) {
            return arma::ones(n);
        }

        TMatrixD Eye(size_t n) {
            return arma::eye(n, n);
        }

        TMatrixD Chol(const TMatrixD &m) {
            TMatrixD ans;
            ENSURE(arma::chol(ans, m), "Cholesky decomposition failed");
            return ans;
        }
        
        TVectorD Solve(const TMatrixD &A, const TMatrixD &B) {
            TVectorD ans;
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

        std::vector<std::vector<double>> MatToStdVec(const TMatrixD &m) {
            std::vector<std::vector<double>> V(m.n_rows);
            for (size_t i = 0; i < m.n_rows; ++i) {
                V[i] = arma::conv_to<std::vector<double>>::from(m.row(i));
            }
            return V;
        }
        
        TMatrixD StdVecToMat(const std::vector<std::vector<double>> &m) {
            if(m.size() == 0) return TMatrixD();
            size_t nrow = m.size();
            size_t ncol = m[0].size();
            TMatrixD r(nrow, ncol);
            for(size_t i=0; i<nrow; ++i) {
                r.row(i) = arma::conv_to<TVectorD>::from(m[i]);
            }
            return r;

        }
    } // namespace NLa
} //namespace NEgo
