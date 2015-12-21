#pragma once

#include "cov.h"

#include <ego/base/errors.h>

#include <numeric>
#include <functional>

namespace NEgo {
    namespace NMaternFuncs {
        using TMaternFunSpec = TMatrixD(*)(const TMatrixD&);

        TMatrixD Matern1(const TMatrixD &K) {
            return NLa::Constant(K.rows(), K.cols(), 1.0);
        }

        TMatrixD MaternDeriv1(const TMatrixD &K) {
            return 1.0/K.array();
        }

    } // namespace NMaternFuncs


    template <NMaternFuncs::TMaternFunSpec MaternFun, NMaternFuncs::TMaternFunSpec MaternFunDeriv>
    class TMaternCov : public ICov {
    public:
        TMaternCov(size_t dim_size)
            : ICov(dim_size)
        {
        }

        TMatrixD CalculateDerivative(const TMatrixD &left, const TMatrixD &right, size_t param_idx) override final {
            TMatrixD K(left.rows(), right.rows());
            return MaternFunDeriv(K);
        }

        TMatrixD CalculateKernel(const TMatrixD &left, const TMatrixD &right) override final {
            TMatrixD K(left.rows(), right.rows());
            return MaternFun(K);
        }

        void SetHyperParameters(const TVectorD &params) override final {
            ENSURE(params.size() == DimSize + 1, "Need DimSize + 1 parameters for kernel");

            Params = NLa::Exp(params.head(params.size()-1));
            // double v = params.tail<1>();
            // SignalVariance = NLa::(2.0*v);
        }

    private:

        TVectorD Params;
        double SignalVariance;
    };


    using TMaternCov1 = TMaternCov<NMaternFuncs::Matern1, NMaternFuncs::MaternDeriv1>;

} //namespace NEgo