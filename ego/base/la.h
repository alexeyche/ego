#pragma once

#include <Eigen/Core>

namespace NEgo {
    namespace NLa {
        using TMatrixD = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;
        using TVectorD = Eigen::Matrix<double, Eigen::Dynamic, 1>;

        TMatrixD Exp(const TMatrixD &m);

        TMatrixD Constant(size_t rows, size_t cols, double val);

    } // namespace NLa

    using NLa::TMatrixD;
    using NLa::TVectorD;

} //namespace NEgo
