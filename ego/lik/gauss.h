#pragma once

#include "lik.h"

#include <ego/base/factory.h>

namespace NEgo {

	class TLikGauss : public ILik {
	public:
        TLikGauss(size_t dim_size);
        
		TPredictiveDistribution Evaluate(const TVectorD &Y, const TVectorD &Ymean, const TVectorD &Ysd) override final;

		void SetHyperParameters(const TVectorD &params) override final;
		const TVectorD& GetHyperParameters() const override final;
		size_t GetHyperParametersSize() const override final;
		
	private:
		TVectorD Params;
	};
 
	REGISTER_LIK(TLikGauss);

} // namespace NEgo
