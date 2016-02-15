#pragma once

#include "config.h"

#include <ego/base/entities.h>
#include <ego/base/la.h>
#include <ego/opt/opt.h>

#include <ego/util/log/log.h>
#include <ego/util/optional.h>
#include <ego/util/serial.h>

#include <ego/distr/distr.h>

namespace NEgo {

    class IAcq;

    using TOptimCallback = std::function<double(const TVectorD&)>;

    class TModel
        : public TOneArgFunctor<TPair<TVectorD, TVectorD>, TMatrixD>
        , public ISerial<NEgoProto::TModelState>
    {
    public:
        using TParent = TOneArgFunctor<TPair<TVectorD, TVectorD>, TMatrixD>;

        static const double ParametersDefault;

        TModel();

        TModel(const TModelConfig& config, ui32 D);

        TModel(const TModelConfig& config, const TMatrixD& x, const TVectorD& y);

        TModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq);

        TModel(const TModel& model);

        void InitWithConfig(const TModelConfig& config, ui32 D);

        // Setters

        void SetModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq);

        void SetData(const TMatrixD &x, const TVectorD &y);

        TPair<TRefWrap<const TMatrixD>, TRefWrap<const TVectorD>> GetData() const;

        void SetConfig(const TModelConfig& config);

        const double& GetMinimumY() const;

        TVectorD GetMinimumX() const;

        void SetMinimum(double v, ui32 idx);

        ui32 GetDimSize() const;

        // Functor methods

        size_t GetParametersSize() const override;

        TVector<double> GetParameters() const override;

        void SetParameters(const TVector<double> &v) override;

        TModel::Result UserCalc(const TMatrixD& Xnew) const override;

        // Helpers

        void OptimizeHyp();

        void Optimize(TOptimCallback cb);

        void OptimizeStep(TOptimCallback cb);

        TInfResult GetNegativeLogLik(const TVector<double>& v);

        TInfResult GetNegativeLogLik() const;

        TDistrVec GetPrediction(const TMatrixD &Xnew);

        TDistrVec GetPredictionWithDerivative(const TMatrixD &Xnew);

        SPtr<IDistr> GetPointPrediction(const TVectorD& Xnew);

        SPtr<IDistr> GetPointPredictionWithDerivative(const TVectorD& Xnew);

        void SerialProcess(TSerializer& serial) override;

        void AddPoint(const TVectorD& x, double y);

        void Update();

        bool Empty() const;

        SPtr<IAcq> GetAcq() const;
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

        TPair<double, ui32> MinF;
    };

} // namespace NEgo


