#pragma once

#include <ego/base/errors.h>
#include <ego/base/value.h>
#include <ego/util/log/log.h>


#include <ego/mean/mean.h>
#include <ego/cov/cov.h>
#include <ego/lik/lik.h>

namespace NEgo {

    struct TPosterior {
        TPosterior()
            : IsCholesky(false)
        {
        }

        TPosterior(const TMatrixD &l, const TVectorD &alpha, const TVectorD &diagW)
            : L(l)
            , Alpha(alpha)
            , DiagW(diagW)
        {
            IsCholesky = NLa::Sum(NLa::TriangLow(L, true)) < std::numeric_limits<double>::epsilon();
            Linv = NLa::Trans(NLa::Solve(L, NLa::Eye(L.n_rows)));
        }

        TMatrixD L; // Given
        TVectorD Alpha;
        TVectorD DiagW;

        bool IsCholesky; // Calculated
        TMatrixD Linv;
    };


    class TInfResult : public TTwoArgFunctorResultBase<double, TMatrixD, TVectorD, TInfResult> {
    public:
        using TCalcPosteriorCb = std::function<TPosterior()>;

        TInfResult() :
            CalcPosteriorCb([=]() -> TPosterior {
                throw TEgoNotImplemented() << "Calculation of posterior is not implemented";
            })
        {
        }

        TInfResult& SetPosterior(TCalcPosteriorCb cb) {
            CalcPosteriorCb = cb;
            return *this;
        }

        TPosterior Posterior() const {
            return CalcPosteriorCb();
        }

    private:
        TCalcPosteriorCb CalcPosteriorCb;
    };



    class IInf : public TTwoArgFunctor<double, TMatrixD, TVectorD, TInfResult> {
    public:
    	using TParent = TTwoArgFunctor<double, TMatrixD, TVectorD, TInfResult>;

        IInf(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik)
            : TParent(mean->GetDimSize())
            , Mean(mean)
            , Cov(cov)
            , Lik(lik)
        {
            ENSURE(Mean->GetDimSize() == GetDimSize(), "Dimension size is not satisfied for likelihood");
            ENSURE(Cov->GetDimSize() == GetDimSize(), "Dimension size is not satisfied for likelihood");
            ENSURE(Lik->GetDimSize() == GetDimSize(), "Dimension size is not satisfied for likelihood");

            MetaEntity = true;
        }

		size_t GetParametersSize() const override;

		void SetParameters(const TVector<double>& parameters) override;

		TVector<double> GetParameters() const override;

    protected:
        SPtr<IMean> Mean;
        SPtr<ICov> Cov;
        SPtr<ILik> Lik;
    };


} // namespace NEgo
