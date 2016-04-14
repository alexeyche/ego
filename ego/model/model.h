#pragma once

#include "base_model.h"

#include <ego/acq/acq.h>
#include <ego/util/sobol.h>

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

        SPtr<IModel> Copy() const override;

        void SetModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq) override;

        size_t GetParametersSize() const override;

        TVector<double> GetParameters() const override;

        void SetParameters(const TVector<double> &v) override;

        TModel::Result UserCalc(const TMatrixD& Xnew) const override;

        IAcq::Result CalcCriterion(const TVectorD& x) const override;

        TInfResult GetNegativeLogLik() const override final;

        void Update() override;

        void SerialProcess(TProtoSerial& serial) override;

        void SetData(const TMatrixD &x, const TVectorD &y) override;
        
        TMatrixD GetX() const override;

        TVectorD GetY() const override;

        ui32 GetDimSize() const override;
       
        ui32 GetSize() const override;
        
        const double& GetMinimumY() const override;

        TVectorD GetMinimumX() const override;

        void AddPoint(const TVectorD& x, double y) override;

        SPtr<IDistr> GetPointPrediction(const TVectorD& Xnew) override;  

        SPtr<IDistr> GetPointPredictionWithDerivative(const TVectorD& Xnew) override;

        TDistrVec GetPrediction(const TMatrixD &Xnew) override;

        void OptimizeHypers(const TOptConfig& config) override;
        
        void SetMinimum(double v, ui32 idx);

        SPtr<ICov> GetCovariance() const override;
        
        SPtr<IAcq> GetAcqusitionFunction() const override;

    private:
        TMatrixD X;
        TVectorD Y;

        TPair<double, ui32> MinF;

        SPtr<IMean> Mean;
        SPtr<ICov> Cov;
        SPtr<ILik> Lik;
        SPtr<IInf> Inf;
        SPtr<IAcq> Acq;

        TOptional<TPosterior> Posterior;
        TVector<double> StartParams;

        TSobolGen Sobol;
    };

    REGISTER_MODEL(TModel);

} // namespace NEgo


