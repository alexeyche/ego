#pragma once

#include <ego/base/entity.h>
#include <ego/base/errors.h>
#include <ego/base/value.h>

#include <ego/mean/mean.h>
#include <ego/cov/cov.h>
#include <ego/lik/lik.h>

namespace NEgo {

    struct TPosterior {
        TPosterior() {}

        TPosterior(const TMatrixD &l, const TVectorD &alpha, const TVectorD &diagW)
            : L(l)
            , Alpha(alpha)
            , DiagW(diagW)
        {
        }

        TMatrixD L;
        TVectorD Alpha;
        TVectorD DiagW;
    };


    class TInfValue : public TValue<double, TVectorD> {
        using Parent = TValue<double, TVectorD>;
    public:
        using TPosteriorCb = std::function<TPosterior()>;

        TInfValue(Parent::TValueCb valueCb, Parent::TDerivativeCb derivativeCb, TPosteriorCb posteriorCb) 
            : TValue(valueCb, derivativeCb)
            , PosteriorCb(posteriorCb)
        {
        }

        TPosterior GetPosterior() const {
            return PosteriorCb();
        }
    private:
        TPosteriorCb PosteriorCb;
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

        virtual TInfValue CalculateNegativeLogLik(const TMatrixD &X, const TVectorD &Y) = 0;

    protected:
        SPtr<IMean> Mean;
        SPtr<ICov> Cov;
        SPtr<ILik> Lik;
    };


} // namespace NEgo
