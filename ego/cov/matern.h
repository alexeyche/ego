#pragma once

#include "cov.h"

#include <ego/base/errors.h>

#include <numeric>
#include <functional>

namespace NEgo {
    namespace NMaternFuncs {

        using TMaternFunSpec = TMatrixD(*)(const TMatrixD&);

        TMatrixD Matern1(const TMatrixD &K) {
            return NLa::MatrixFromConstant(K.n_rows, K.n_rows, 1.0);
        }

        TMatrixD MaternDeriv1(const TMatrixD &K) {
            return 1.0/K;
        }

    } // namespace NMaternFuncs


    template <size_t Power, NMaternFuncs::TMaternFunSpec MaternFun, NMaternFuncs::TMaternFunSpec MaternFunDeriv>
    class TMaternCov : public ICov {
    public:
        TMaternCov(size_t dim_size)
            : ICov(dim_size)
        {
        }

        TMatrixD CalculateDerivative(const TMatrixD &left, const TMatrixD &right, size_t param_idx) override final {
            TMatrixD K(left.n_rows, right.n_rows);
            return MaternFunDeriv(K) * NLa::Exp(-K);
        }

        TMatrixD CalculateKernel(const TMatrixD &left, const TMatrixD &right) override final {
            TMatrixD K = SqDist(
                Trans(Dot(Diag(sqrt(Power)/Params), left)),
                Trans(Dot(Diag(sqrt(Power)/Params), right))
            );
            K = NLa::Sqrt(K);
            return SignalVariance * MaternFun(K) * NLa::Exp(-K);
        }

        void SetHyperParameters(const TVectorD &params) override final {
            ENSURE(params.size() == DimSize + 1, "Need DimSize + 1 parameters for kernel");

            Params = NLa::Exp(params.head(params.size()-1));
            SignalVariance = 2.0 * NLa::Exp(NLa::GetLastElem(params));
        }

    private:

        TVectorD Params;
        double SignalVariance;
    };


    using TMaternCov1 = TMaternCov<1, NMaternFuncs::Matern1, NMaternFuncs::MaternDeriv1>;

} //namespace NEgo