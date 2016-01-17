#pragma once

#include <ego/func/two_arg.h>

#include <ego/distr/distr.h>

namespace NEgo {

    using TDistrVec = TVector<SPtr<IDistr>>;


    struct TPredictiveDistributionParams {
        TVectorD LogP;
        TVectorD Mean;
        TVectorD Variance;
    };


    class ILik : public TTwoArgFunctor<TVectorD, TVectorD, TVectorD> {
    public:
    	using TParent = TTwoArgFunctor<TVectorD, TVectorD, TVectorD>;

        ILik(size_t dimSize)
            : TParent(dimSize)
        {
        }
        
        virtual TPair<TVectorD, TVectorD> GetMarginalMeanAndVariance(const TVectorD& mean, const TVectorD& variance) const = 0;

		virtual SPtr<IDistr> GetDistribution(double mean, double sd, ui32 seed) const = 0;
        
        TPredictiveDistributionParams CalculatePredictiveDistribution(const TVectorD &Y, const TVectorD &mean, const TVectorD &variance) const;

        TPredictiveDistributionParams CalculatePredictiveDistribution(const TVectorD &mean, const TVectorD &variance) const;

        TDistrVec GetPredictiveDistributions(TPredictiveDistributionParams params, ui32 seed);
    };

} // namespace NEgo