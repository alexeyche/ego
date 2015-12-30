#include "gauss.h"

#include <ego/base/base.h>
#include <ego/base/errors.h>



namespace NEgo {

	TLikGauss::TLikGauss(size_t dim_size)
        : ILik(dim_size)
    {
    }

    TPredictiveDistribution TLikGauss::CalculatePredictiveDistribution(const TVectorD &Y, const TVectorD &mean, const TVectorD &variance) const {
        ENSURE(GetHyperParametersSize() == 1, "One hyperparameter must be set");
        double sn2 = GetHyperParameters()(0);
        TPredictiveDistribution ret;
        ret.LogP = -( ((Y-mean) % (Y-mean)) / (sn2+variance) )/2.0 - NLa::Log(2.0*M_PI*(sn2+variance))/2.0;
        Tie(ret.Mean, ret.Variance) = TLikGauss::GetMarginalMeanAndVariance(mean, variance);
        return ret;
    }


    TPair<TVectorD, TVectorD> TLikGauss::GetMarginalMeanAndVariance(const TVectorD &mean, const TVectorD &variance) const {
        return MakePair(mean, variance + NLa::AsScalar(Params));
    }


    void TLikGauss::SetHyperParameters(const TVectorD &params) {
    	ENSURE(params.size() == 1, "Gaussian likelihood expecting one parameter");
    	Params = params;
    }
    
    const TVectorD& TLikGauss::GetHyperParameters() const {
    	return Params;
    }

    size_t TLikGauss::GetHyperParametersSize() const {
        return 1;
    }
        
} // namespace NEgo
