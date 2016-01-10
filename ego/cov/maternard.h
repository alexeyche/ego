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
    class TCovMaternARD : public ICov {
    public:
        TCovMaternARD(size_t dim_size)
            : ICov(dim_size)
        {
        }

        TCovRet CalculateKernel(const TMatrixD &left, const TMatrixD &right) override final {
            ENSURE(Params.size() > 0, "Need hyperparameters be set");
            ENSURE(left.n_cols ==  DimSize, "Col size of left input matrix are not satisfy to kernel params: " << DimSize);
            ENSURE(right.n_cols ==  DimSize, "Col size of right input matrix are not satisfy to kernel params: " << DimSize);

            TVectorD ell = NLa::Exp(Params.head(Params.size()-1));
            double signalVariance = NLa::Exp(2.0 * NLa::GetLastElem(Params));

            TMatrixD K = NLa::SquareDist(
                NLa::Trans(NLa::DiagMat(sqrt(Power)/ell) * NLa::Trans(left)),
                NLa::Trans(NLa::DiagMat(sqrt(Power)/ell) * NLa::Trans(right))
            );

            K = NLa::Sqrt(K);
            TMatrixD KExpVar = signalVariance * NLa::Exp(-K);
            return TCovRet(
                [=]() -> TMatrixD {
                    return MaternFun(K) % KExpVar;
                },
                [=]() -> TCubeD {
                    TCubeD dK(left.n_rows, right.n_rows, Params.size());
                    size_t pIdx=0;
                    for(; pIdx < left.n_cols; ++pIdx) {
                        TMatrixD Ki = NLa::SquareDist(
                            NLa::Trans(sqrt(Power)/ell(pIdx) * NLa::Trans(left.col(pIdx))),
                            NLa::Trans(sqrt(Power)/ell(pIdx) * NLa::Trans(right.col(pIdx)))
                        );
                        dK.slice(pIdx) = MaternFunDeriv(K) % KExpVar % Ki;
                    }
                    dK.slice(pIdx) = 2 * MaternFun(K) % KExpVar;
                    return dK;
                }
            );
        }

        void SetHyperParameters(const TVectorD &params) override final {
            ENSURE(params.size() == DimSize + 1, "Need DimSize + 1 parameters for kernel");
            Params = params;
        }

        size_t GetHyperParametersSize() const override final {
            return DimSize + 1;
        }
        
        TVectorD GetHyperParameters() const override final {
            return Params;
        }
    private:

        TVectorD Params;
    };


    using TCovMaternARD1 = TCovMaternARD<1, NMaternFuncs::Matern1, NMaternFuncs::MaternARDDeriv1>;
    using TCovMaternARD3 = TCovMaternARD<3, NMaternFuncs::Matern3, NMaternFuncs::MaternARDDeriv3>;
    using TCovMaternARD5 = TCovMaternARD<5, NMaternFuncs::Matern5, NMaternFuncs::MaternARDDeriv5>;

    REGISTER_COV(TCovMaternARD1);
    REGISTER_COV(TCovMaternARD3);
    REGISTER_COV(TCovMaternARD5);

} //namespace NEgo