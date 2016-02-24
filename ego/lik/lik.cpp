#include "lik.h"


namespace NEgo {

    TDistrVec ILik::GetPredictiveDistributions(const TVectorD& mean, const TVectorD& variance, ui32 seed) {
		TDistrVec dvec;
        for(size_t pi=0; pi<mean.size(); ++pi) {
            const double& var = variance(pi);
            ENSURE(var>=0.0, "Got negative variance " << var << " at the point " << mean(pi));
            dvec.push_back(GetDistribution(mean(pi), sqrt(var), seed));
        }
        return dvec;
	}

    SPtr<IDistr> ILik::GetDistributionsWithDerivative(
            double mean, double sd,
            const TVectorD& meanDeriv, const TVectorD& sdDeriv, ui32 seed)
    {
        SPtr<IDistr> d = GetDistribution(
            mean
          , sd
          , seed
        );
        d->SetMeanDeriv(meanDeriv);
        d->SetSdDeriv(sdDeriv);
        return d;
    }


} // namespace NEgo