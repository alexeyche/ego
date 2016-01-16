#pragma once

#include "lik.h"

#include <ego/base/factory.h>

namespace NEgo {

	class TLikGauss : public ILik {
	public:
        TLikGauss(size_t dimSize);

        TLikGauss::Result UserCalc(const TVectorD& Y, const TVectorD& var) override final;
		
		TPair<TVectorD, TVectorD> GetMarginalMeanAndVariance(const TVectorD& mean, const TVectorD& variance) const override final;
	    
	    SPtr<IDistr> GetDistribution(double mean, double sd, ui32 seed) const override final;
	    
	    size_t GetParametersSize() const override final;
    };


	REGISTER_LIK(TLikGauss);

} // namespace NEgo
