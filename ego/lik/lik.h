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

    class ILik : public IEntity {
    public:
        ILik(size_t dim_size)
            : IEntity(dim_size)
        {
        }

        virtual TPredictiveDistributionParams CalculatePredictiveDistribution(const TVectorD &Y, const TVectorD &mean, const TVectorD &variance) const = 0;

        virtual TPair<TVectorD, TVectorD> GetMarginalMeanAndVariance(const TVectorD &mean, const TVectorD &variance) const = 0;

        TPredictiveDistributionParams CalculatePredictiveDistribution(const TVectorD &mean, const TVectorD &variance) const;

        virtual void SetHyperParameters(const TVectorD &params) = 0;

        virtual const TVectorD& GetHyperParameters() const = 0;

        virtual size_t GetHyperParametersSize() const = 0;

        virtual SPtr<IDistr> GetDistribution(double mean, double sd, ui32 seed) = 0;

        TDistrVec GetPredictiveDistributions(TPredictiveDistributionParams params, ui32 seed);
    };


} // namespace NEgo
