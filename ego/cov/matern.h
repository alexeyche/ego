#pragma once

#include "cov.h"

#include <ego/base/errors.h>
#include <ego/base/factory.h>

#include <numeric>
#include <functional>

namespace NEgo {
    namespace NMaternFuncs {

        using TMaternFunSpec = TMatrixD(*)(const TMatrixD&);

        TMatrixD Matern1(const TMatrixD &K);

        TMatrixD MaternDeriv1(const TMatrixD &K);

    } // namespace NMaternFuncs


    template <size_t Power, NMaternFuncs::TMaternFunSpec MaternFun, NMaternFuncs::TMaternFunSpec MaternFunDeriv>
    class TCovMaternARD : public ICov {
    public:
        TCovMaternARD(size_t dim_size)
            : ICov(dim_size)
        {
        }

        TMatrixD CalculateDerivative(const TMatrixD &left, const TMatrixD &right) override final {
            TMatrixD K(left.n_rows, right.n_rows);
            return MaternFunDeriv(K) % NLa::Exp(-K);
        }

        TMatrixD CalculateKernel(const TMatrixD &left, const TMatrixD &right) override final {
            ENSURE(Params.size() > 0, "Need hyperparameters be set");

            TMatrixD K = NLa::SquareDist(
                NLa::Trans(NLa::Diag(sqrt(Power)/Params) * NLa::Trans(left)),
                NLa::Trans(NLa::Diag(sqrt(Power)/Params) * NLa::Trans(right))
            );
            NLa::Print(K);
            K = NLa::Sqrt(K);
            return SignalVariance * MaternFun(K) % NLa::Exp(-K);
        }

        void SetHyperParameters(const TVectorD &params) override final {
            ENSURE(params.size() == DimSize + 1, "Need DimSize + 1 parameters for kernel");

            Params = NLa::Exp(params.head(params.size()-1));
            SignalVariance = NLa::Exp(2.0 * NLa::GetLastElem(params));
        }

    private:

        TVectorD Params;
        double SignalVariance;
    };


    using TCovMaternARD1 = TCovMaternARD<1, NMaternFuncs::Matern1, NMaternFuncs::MaternDeriv1>;


    REGISTER_COV(TCovMaternARD1);

} //namespace NEgo