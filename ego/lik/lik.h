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
        using StatResult = TTwoArgFunctorResult<TPair<TVectorD, TVectorD>>;

        ILik(size_t dimSize)
            : TParent(dimSize)
        {
        }

        virtual StatResult GetMarginalMeanAndVariance(const TVectorD& mean, const TVectorD& variance) const = 0;

		virtual SPtr<IDistr> GetDistribution(double mean, double sd, ui32 seed) const = 0;

        TDistrVec GetPredictiveDistributions(
            const TVectorD& mean, const TVectorD& variance, ui32 seed);

        TDistrVec GetPredictiveDistributionsWithDerivative(
            const TVectorD& mean, const TVectorD& variance,
            const TVectorD& meanDeriv, const TVectorD& varianceDeriv, ui32 seed);
    };

} // namespace NEgo