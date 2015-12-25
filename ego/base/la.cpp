#include "la.h"

#include <ego/base/errors.h>
#include <ego/util/log/log.h>

namespace NEgo {
    namespace NLa {
        TMatrixD Exp(const TMatrixD &m) {
            return arma::exp(m);
        }

        double Exp(double v) {
            return std::exp(v);
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
                   RepMat(RowSum(rightM % rightM), 1, left.n_rows) -
                   2.0 * leftM * Trans(rightM);
        }

        TMatrixD Diag(const TVectorD &v) {
            return arma::diagmat(v);
        }

        TMatrixD Diag(double v, size_t size) {
            TVectorD vecV(size);
            vecV.fill(v);
            return Diag(vecV);
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

    } // namespace NLa
} //namespace NEgo
