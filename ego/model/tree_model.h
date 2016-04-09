#pragma once

#include "base_model.h"

#include <ego/acq/acq.h>

namespace NEgo {

    class TTreeModel: public IModel
    {
    public:

        static const ui32 MeanLeafSize;
        static const ui32 SplitSizeCriteria;

        using TBase = IModel;

        TTreeModel(const TModelConfig& config, ui32 D);

        TTreeModel(const TModelConfig& config, const TMatrixD& x, const TVectorD& y);

        TTreeModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq);

        TTreeModel(const TTreeModel& model);

        // Functor methods

        virtual SPtr<IModel> Copy() const override;

        void SetModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq) override;

        SPtr<ILik> GetLikelihood() const override;

        size_t GetParametersSize() const override;

        TVector<double> GetParameters() const override;

        void SetParameters(const TVector<double> &v) override;

        TTreeModel::Result UserCalc(const TMatrixD& Xnew) const override;

        SPtr<IAcq> GetCriterion() const override;

        // Helpers

        TInfResult GetNegativeLogLik() const override final;

        void AddPoint(const TVectorD& x, double y) override;

        void Update() override;

        void Split();

    private:
        TOptional<TPair<SPtr<TTreeModel>, SPtr<TTreeModel>>> Nodes;

        SPtr<IMean> Mean;
        SPtr<ICov> Cov;
        SPtr<ILik> Lik;
        SPtr<IInf> Inf;
        SPtr<IAcq> Acq;

        TOptional<TPosterior> Posterior;
    };

    REGISTER_MODEL(TTreeModel);

} // namespace NEgo


