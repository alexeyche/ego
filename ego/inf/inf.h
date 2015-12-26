#pragma once

#include <ego/base/entity.h>
#include <ego/base/errors.h>

#include <ego/mean/mean.h>
#include <ego/cov/cov.h>
#include <ego/lik/lik.h>

namespace NEgo {

    struct TPosterior {
        TVectorD Alpha;
        TVectorD DiagW;
        TMatrixD L;
    };

    class IInf : public IEntity {
    public:
        IInf(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik)
            : IEntity(mean->GetDimSize())
            , Mean(mean)
            , Cov(cov)
            , Lik(lik)
        {
            ENSURE(Mean->GetDimSize() == GetDimSize(), "Dimension size is not satisfied for likelihood");
            ENSURE(Cov->GetDimSize() == GetDimSize(), "Dimension size is not satisfied for likelihood");
            ENSURE(Lik->GetDimSize() == GetDimSize(), "Dimension size is not satisfied for likelihood");

        }

        virtual double NegativeLogLik(const TMatrixD &X, const TVectorD &Y) = 0;

    protected:
        SPtr<IMean> Mean;
        SPtr<ICov> Cov;
        SPtr<ILik> Lik;
    };


} // namespace NEgo
