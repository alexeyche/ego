#pragma once

#include "config.h"

#include <ego/base/entities.h>
#include <ego/base/la.h>
#include <ego/opt/opt.h>

#include <ego/util/log/log.h>
#include <ego/util/optional.h>

#include <ego/distr/distr.h>

namespace NEgo {

    class IAcq;

    using TOptimCallback = std::function<double(const TVectorD&)>;

    class TModel : public TOneArgFunctor<TPair<TVectorD, TVectorD>, TMatrixD> {
    public:
        using TParent = TOneArgFunctor<TPair<TVectorD, TVectorD>, TMatrixD>;

        static const double ParametersDefault;

        TModel(TModelConfig config);

        TModel();

        TModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq, TMatrixD x, TVectorD y);

        // Setters

        void SetModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq);

        void SetData(const TMatrixD &x, const TVectorD &y);

        TPair<TMatrixD, TVectorD> GetData() const;

        void SetConfig(TModelConfig config);

        const double& GetMinimum() const;

        void SetMinimum(double v);

        // Functor methods

        size_t GetParametersSize() const override;

        TVector<double> GetParameters() const override;

        void SetParameters(const TVector<double> &v) override;

        TModel::Result UserCalc(const TMatrixD& Xnew) const override;

        // Helpers

        void OptimizeHyp();

        void Optimize(TOptimCallback cb);

        TInfResult GetNegativeLogLik(const TVector<double>& v);

        TInfResult GetNegativeLogLik() const;

        TDistrVec GetPrediction(const TMatrixD &Xnew);

        TDistrVec GetPredictionWithDerivative(const TMatrixD &Xnew);

        SPtr<IDistr> GetPointPrediction(const TVectorD& Xnew);

        SPtr<IDistr> GetPointPredictionWithDerivative(const TVectorD& Xnew);

    private:
        TMatrixD X;
        TVectorD Y;

        SPtr<IMean> Mean;
        SPtr<ICov> Cov;
        SPtr<ILik> Lik;
        SPtr<IInf> Inf;
        SPtr<IAcq> Acq;

        TModelConfig Config;

        TOptional<TPosterior> Posterior;

        double MinF;
    };

} // namespace NEgo


