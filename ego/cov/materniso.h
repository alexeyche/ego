#pragma once

#include "cov.h"
#include "matern.h"

#include <ego/base/errors.h>
#include <ego/base/factory.h>
#include <ego/util/log/log.h>

#include <numeric>
#include <functional>

namespace NEgo {

    template <size_t Power, NMaternFuncs::TMaternFunSpec MaternFun, NMaternFuncs::TMaternFunSpec MaternFunDeriv>
    class TCovMaternISO : public ICov {
    public:
        TCovMaternISO(size_t dim_size)
            : ICov(dim_size)
        {
        }

        TCovRet CalculateKernel(const TMatrixD &left, const TMatrixD &right) override final {
            ENSURE(Params.size() > 0, "Need hyperparameters be set");
            ENSURE(left.n_cols ==  DimSize, "Col size of left input matrix are not satisfy to kernel params: " << DimSize);
            ENSURE(right.n_cols ==  DimSize, "Col size of right input matrix are not satisfy to kernel params: " << DimSize);

        	double ell = NLa::Exp(Params(0));
            double signalVariance = NLa::Exp(2.0 * Params(1));

            TMatrixD K = NLa::SquareDist(
                NLa::Trans((sqrt(Power)/ell) * NLa::Trans(left)),
                NLa::Trans((sqrt(Power)/ell) * NLa::Trans(right))
            );

            K = NLa::Sqrt(K);
            TMatrixD KExpVar = signalVariance * NLa::Exp(-K);
            return TCovRet(
                [=]() -> TMatrixD {
                    return MaternFun(K) % KExpVar;
                },
                [=]() -> TCubeD {
                    TCubeD dK(left.n_rows, right.n_rows, Params.size());
                    dK.slice(0) = MaternFunDeriv(K) % KExpVar % K;
                    dK.slice(1) = 2 * MaternFun(K) % KExpVar;
                    return dK;
                }
            );
        }

        void SetHyperParameters(const TVectorD &params) override final {
            ENSURE(params.size() == 2, "Need 2 parameters for kernel");
            Params = params;
        }

        size_t GetHyperParametersSize() const override final {
            return 2;
        }
        
        TVectorD GetHyperParameters() const override final {
            return Params;
        }
    private:

        TVectorD Params;
    };


    using TCovMaternISO1 = TCovMaternISO<1, NMaternFuncs::Matern1, NMaternFuncs::MaternISODeriv1>;
    using TCovMaternISO3 = TCovMaternISO<3, NMaternFuncs::Matern3, NMaternFuncs::MaternISODeriv3>;
    using TCovMaternISO5 = TCovMaternISO<5, NMaternFuncs::Matern5, NMaternFuncs::MaternISODeriv5>;

    REGISTER_COV(TCovMaternISO1);
    REGISTER_COV(TCovMaternISO3);
    REGISTER_COV(TCovMaternISO5);

} //namespace NEgo