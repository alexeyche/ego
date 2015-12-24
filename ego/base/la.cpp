#include "la.h"

#include <ego/base/errors.h>

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
            TMatrixD mu = (left.n_rows/(left.n_rows + right.n_rows)) * ColMean(left) +
                          (right.n_rows/(left.n_rows + right.n_rows)) * ColMean(right);

            TMatrixD leftM = left - RepMat(mu, left.n_rows, 1);
            TMatrixD rightM = right - RepMat(mu, right.n_rows, 1);
            return RepMat(ColSum(leftM * leftM), right.n_rows, 1) +
                   RepMat(ColSum(rightM * rightM), left.n_rows, 1) -
                   2.0 * Dot(leftM, Trans(rightM));
        }

        TMatrixD Diag(const TVectorD &v) {
            return arma::diagmat(v);
        }

        TMatrixD Diag(double v, size_t size) {
            TVectorD vecV(size);
            vecV.fill(v);
            return Diag(vecV);
        }

        TVectorD ColMean(const TMatrixD &m) {
            return arma::mean(m, 0);
        }

        TVectorD RowMean(const TMatrixD &m) {
            return arma::mean(m, 1);
        }

        TMatrixD Trans(const TMatrixD &m) {
            return arma::trans(m);
        }

        TMatrixD RepMat(const TMatrixD &v, size_t per_row, size_t per_col) {
            return arma::repmat(v, per_row, per_col);
        }

        TVectorD ColSum(const TMatrixD &m) {
            return arma::sum(m, 0);
        }

        TVectorD RowSum(const TMatrixD &m) {
            return arma::sum(m, 1);
        }

        TMatrixD Sqrt(const TMatrixD &m) {
            return arma::sqrt(m);
        }


    } // namespace NLa
} //namespace NEgo
