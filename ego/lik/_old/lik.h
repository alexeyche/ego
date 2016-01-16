#pragma once

#include <ego/base/entity.h>
#include <ego/base/value.h>

#include <ego/distr/distr.h>

namespace NEgo {
    
    using TDistrVec = TVector<SPtr<IDistr>>;

    struct TPredictiveDistributionParams {
        TVectorD LogP;
        TVectorD Mean;
        TVectorD Variance;
    };

    class TLogLikValue : public TValue<TVectorD, TVectorD> {
        using Parent = TValue<TVectorD, TVectorD>;
        using THyperDerivativeCb = std::function<TMatrixD()>;

    public:
        TLogLikValue(
            Parent::TValueCb valueCb,
            Parent::TDerivativeCb derivativeCb,
            Parent::TDerivativeCb secondDerivativeCb,
            Parent::TDerivativeCb thirdDerivativeCb,
            THyperDerivativeCb hyperDerivativeCb,
            THyperDerivativeCb hyperFirstDerivativeCb,
            THyperDerivativeCb hyperSecondDerivativeCb
        )
            : TValue(valueCb, derivativeCb)
            , SecondDerivativeCb(secondDerivativeCb)
            , ThirdDerivativeCb(thirdDerivativeCb)
            , HyperDerivativeCb(hyperDerivativeCb)
            , HyperFirstDerivativeCb(hyperFirstDerivativeCb)
            , HyperSecondDerivativeCb(hyperSecondDerivativeCb)
        {
        }

        TVectorD GetSecondDerivative() const {
            return SecondDerivativeCb();
        }
        
        TVectorD GetThirdDerivative() const {
            return ThirdDerivativeCb();
        }

        TMatrixD GetHyperDerivative() const {
            return HyperDerivativeCb();
        }

        TMatrixD GetHyperFirstDerivative() const {
            return HyperFirstDerivativeCb();
        }

        TMatrixD GetHyperSecondDerivative() const {
            return HyperSecondDerivativeCb();
        }
    private:
        Parent::TDerivativeCb SecondDerivativeCb;
        Parent::TDerivativeCb ThirdDerivativeCb;
        THyperDerivativeCb HyperDerivativeCb;
        THyperDerivativeCb HyperFirstDerivativeCb;
        THyperDerivativeCb HyperSecondDerivativeCb;
    };


    class ILik : public IEntity {
    public:
        ILik(size_t dim_size)
            : IEntity(dim_size)
        {
        }

        TPredictiveDistributionParams CalculatePredictiveDistribution(const TVectorD &Y, const TVectorD &mean, const TVectorD &variance) const;

        virtual TLogLikValue CalculateLogLikelihood(const TVectorD &Y, const TVectorD &mean, const TVectorD &variance) const = 0;
        
        TLogLikValue CalculateLogLikelihood(const TVectorD &Y, const TVectorD &mean) const;

        virtual TPair<TVectorD, TVectorD> GetMarginalMeanAndVariance(const TVectorD &mean, const TVectorD &variance) const = 0;

        TPredictiveDistributionParams CalculatePredictiveDistribution(const TVectorD &mean, const TVectorD &variance) const;

        virtual void SetHyperParameters(const TVectorD &params) = 0;

        virtual const TVectorD& GetHyperParameters() const = 0;

        virtual size_t GetHyperParametersSize() const = 0;

        virtual SPtr<IDistr> GetDistribution(double mean, double sd, ui32 seed) = 0;

        TDistrVec GetPredictiveDistributions(TPredictiveDistributionParams params, ui32 seed);
    };


} // namespace NEgo
