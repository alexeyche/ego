#include "gauss.h"

#include <ego/base/base.h>
#include <ego/base/errors.h>

#include <ego/distr/gauss.h>

#include <ego/util/log/log.h>

namespace NEgo {

	TLikGauss::TLikGauss(size_t dim_size)
        : ILik(dim_size)
    {
    }

    TLogLikValue TLikGauss::CalculateLogLikelihood(const TVectorD &Y, const TVectorD &mean, const TVectorD &variance) const {
        ENSURE(GetHyperParametersSize() == 1, "One hyperparameter must be set");
        double sn2 = exp(2.0*GetHyperParameters()(0));
        
        TVectorD Ymean = Y-mean;
        TVectorD logP = - (Ymean % Ymean) / ((sn2+variance) * 2.0) - NLa::Log(2.0 * M_PI * (sn2 + variance))/2.0;
        
        return TLogLikValue(
            [=]() -> TVectorD {
                return logP;
            },
            [=]() -> TVectorD {
                return Ymean/sn2;
            },
            [=]() -> TVectorD {
                return - NLa::Ones(Ymean.n_rows)/sn2;
            },
            [=]() -> TVectorD {
                return NLa::Zeros(Ymean.n_rows);
            },
            [=]() -> TVectorD {
                return (Ymean % Ymean)/sn2 - 1.0;        
            },
            [=]() -> TVectorD {
                return 2.0 * (mean - Y)/sn2;
            },
            [=]() -> TVectorD {
                return 2.0 * NLa::Ones(mean.n_rows)/sn2;
            }
        );
    }


    TPair<TVectorD, TVectorD> TLikGauss::GetMarginalMeanAndVariance(const TVectorD &mean, const TVectorD &variance) const {
        return MakePair(mean, variance + exp(2.0*NLa::AsScalar(Params)));
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
        
    SPtr<IDistr> TLikGauss::GetDistribution(double mean, double sd, ui32 seed) {
        return SPtr<IDistr>(new TDistrGauss(mean, sd, seed));
    }

} // namespace NEgo
