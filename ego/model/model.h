#pragma once

#include "base_model.h"

#include <ego/acq/acq.h>

namespace NEgo {

    class TModel: public IModel
    {
    public:
        using TBase = IModel;

        TModel(const TModelConfig& config, ui32 D);

        TModel(const TModelConfig& config, const TMatrixD& x, const TVectorD& y);

        TModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq);

        TModel(const TModel& model);

        // Functor methods

        virtual SPtr<IModel> Copy() const override;

        void SetModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq) override;

        SPtr<ILik> GetLikelihood() const override;

        size_t GetParametersSize() const override;

        TVector<double> GetParameters() const override;

        void SetParameters(const TVector<double> &v) override;

        TModel::Result UserCalc(const TMatrixD& Xnew) const override;

        IAcq::Result CalcCriterion(const TVectorD& x) const override;

        // Helpers

        TInfResult GetNegativeLogLik() const override final;

        void AddPoint(const TVectorD& x, double y) override;

        void Update() override;

    private:
        SPtr<IMean> Mean;
        SPtr<ICov> Cov;
        SPtr<ILik> Lik;
        SPtr<IInf> Inf;
        SPtr<IAcq> Acq;

        TOptional<TPosterior> Posterior;
    };

    REGISTER_MODEL(TModel);

} // namespace NEgo


