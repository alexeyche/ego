#pragma once

#include "lik.h"

#include <ego/base/factory.h>

namespace NEgo {

	class TLikGauss : public ILik {
	public:
        TLikGauss(size_t dim_size);

		TLogLikValue CalculateLogLikelihood(const TVectorD &Y, const TVectorD &mean, const TVectorD &variance) const override final;
        
        TPair<TVectorD, TVectorD> GetMarginalMeanAndVariance(const TVectorD &mean, const TVectorD &variance) const override final;

		void SetHyperParameters(const TVectorD &params) override final;
		
		const TVectorD& GetHyperParameters() const override final;
		
		size_t GetHyperParametersSize() const override final;
		
		SPtr<IDistr> GetDistribution(double mean, double sd, ui32 seed) override final;
		
	private:
		TVectorD Params;
	};
 
	REGISTER_LIK(TLikGauss);

} // namespace NEgo
