#include "gauss.h"

#include <ego/distr/gauss.h>
#include <ego/util/log/log.h>

namespace NEgo {

    TLikGauss::TLikGauss(size_t dimSize)
    	: ILik(dimSize)
   	{
    }

	TLikGauss::Result TLikGauss::UserCalc(const TVectorD& Y, const TVectorD& var) const {
        double sn2 = exp(2.0 * Parameters[0]);

        TVectorD logP = - (Y % Y) / ((sn2+var) * 2.0) - NLa::Log(2.0 * M_PI * (sn2+var))/2.0;

        return TLikGauss::Result()
			.SetValue(
				[=]() -> TVectorD {
					return logP;
				}
			);
	}

	TLikGauss::StatResult TLikGauss::GetMarginalMeanAndVariance(const TVectorD &mean, const TVectorD &variance) const {
        double sn2 = exp(2.0 * Parameters[0]);
        return TLikGauss::StatResult()
            .SetValue(
                [=]() {
                    return MakePair(mean, variance + sn2);
                }
            )
            .SetFirstArgDeriv(
                [=]() {
                    return MakePair(
                        NLa::Ones(mean.size())
                      , NLa::Zeros(mean.size())
                    );
                }
            )
            .SetSecondArgDeriv(
                [=]() {
                    return MakePair(
                        NLa::Zeros(mean.size())
                      , NLa::Ones(mean.size())
                    );
                }
            );
    }


	SPtr<IDistr> TLikGauss::GetDistribution(double mean, double sd, ui32 seed) const {
        return SPtr<IDistr>(new TDistrGauss(mean, sd, seed));
    }

	size_t TLikGauss::GetParametersSize() const {
		return 1;
	}

} // namespace NEgo
