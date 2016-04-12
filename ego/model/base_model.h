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
    
    class TOptConfig;

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
        
        virtual IAcq::Result CalcCriterion(const TVectorD& x) const = 0;

        virtual void AddPoint(const TVectorD& x, double y) = 0;

        virtual TInfResult GetNegativeLogLik() const = 0;

        virtual void Update() = 0;

        virtual SPtr<IModel> Copy() const = 0;

        virtual void SetData(const TMatrixD &x, const TVectorD &y) = 0;
        
        virtual TMatrixD GetX() const = 0;

        virtual TVectorD GetY() const = 0;

        virtual ui32 GetDimSize() const = 0;
       
        virtual ui32 GetSize() const = 0; 

        virtual const double& GetMinimumY() const = 0;

        virtual TVectorD GetMinimumX() const = 0;

        virtual SPtr<IDistr> GetPointPrediction(const TVectorD& Xnew) = 0;

        virtual SPtr<IDistr> GetPointPredictionWithDerivative(const TVectorD& Xnew) = 0;

        virtual TDistrVec GetPrediction(const TMatrixD &Xnew) = 0;

        virtual void OptimizeHypers(const TOptConfig& config) = 0;

        // Common getters setters

        void InitWithConfig(const TModelConfig& config, ui32 D);

        void SetConfig(const TModelConfig& config);

        const TModelConfig& GetConfig() const;


        // Helpers

        TInfResult GetNegativeLogLik(const TVector<double>& v);

        bool Empty() const;
        
        TPair<TMatrixD, TVectorD> GetData() const;
        
        
    protected:

        TModelConfig Config;
    };

} // namespace NEgo


