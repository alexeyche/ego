#pragma once

#include "config.h"

#include <ego/base/entities.h>
#include <ego/base/la.h>
#include <ego/opt/opt.h>

#include <ego/util/log/log.h>
#include <ego/util/optional.h>
#include <ego/util/serial/proto_serial.h>

#include <ego/distr/distr.h>

namespace NEgo {

    class IModel
        : public TOneArgFunctor<TPair<TVectorD, TVectorD>, TMatrixD>
        , public IProtoSerial<NEgoProto::TModelState>
    {
    public:
        using TParent = TOneArgFunctor<TPair<TVectorD, TVectorD>, TMatrixD>;

        // Ctors

        IModel();

        // Interface

        virtual void SetModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf, SPtr<IAcq> acq) = 0;
        
        virtual SPtr<IAcq> GetCriterion() const = 0;

        virtual void AddPoint(const TVectorD& x, double y);

        virtual SPtr<ILik> GetLikelihood() const = 0;

        virtual TInfResult GetNegativeLogLik() const = 0;

        virtual void Update() = 0;

        virtual SPtr<IModel> Copy() const = 0;

        // Common getters setters

        void InitWithConfig(const TModelConfig& config, ui32 D);

        TPair<TRefWrap<const TMatrixD>, TRefWrap<const TVectorD>> GetData() const;

        void SetConfig(const TModelConfig& config);

        const double& GetMinimumY() const;

        TVectorD GetMinimumX() const;

        void SetMinimum(double v, ui32 idx);

        ui32 GetDimSize() const;
       
        bool Empty() const;

        // Helpers

        IAcq::Result CalcCriterion(const TVectorD& x) const;

        void SetData(const TMatrixD &x, const TVectorD &y);

        SPtr<IDistr> GetPointPrediction(const TVectorD& Xnew);

        SPtr<IDistr> GetPointPredictionWithDerivative(const TVectorD& Xnew);

		TDistrVec GetPrediction(const TMatrixD &Xnew);

        TInfResult GetNegativeLogLik(const TVector<double>& v);

        void SerialProcess(TProtoSerial& serial) override;

    protected:
        TMatrixD X;
        TVectorD Y;

        TModelConfig Config;

        TPair<double, ui32> MinF;
    };

} // namespace NEgo


