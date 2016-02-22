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
        using StatResult = TTwoArgFunctorResult<TPair<TVectorD, TVectorD>, TVectorD, TVectorD>;

        ILik(size_t dimSize)
            : TParent(dimSize)
        {
        }

        virtual StatResult GetMarginalMeanAndVariance(const TVectorD& mean, const TVectorD& variance) const = 0;

		virtual SPtr<IDistr> GetDistribution(double mean, double sd, ui32 seed) const = 0;

        SPtr<IDistr> GetDistributionsWithDerivative(
            double mean, double sd,
            const TVectorD& meanDeriv, const TVectorD& sdDeriv, ui32 seed);

        TDistrVec GetPredictiveDistributions(
            const TVectorD& mean, const TVectorD& variance, ui32 seed);

    };

} // namespace NEgo