#include "gauss.h"

#include <ego/base/errors.h>


namespace NEgo {

	TLikGauss::TLikGauss(size_t dim_size)
        : ILik(dim_size)
    {
    }

    void TLikGauss::SetHyperParameters(const TVectorD &params) {
    	ENSURE(params.size() == 1, "Gaussian likelihood expecting one parameter");
    	Params = params;
    }
    
    TPredictiveDistribution TLikGauss::Evaluate(const TVectorD &Y, const TVectorD &Ymean, const TVectorD &Ysd) {
    	return TPredictiveDistribution();
    }

    const TVectorD& TLikGauss::GetHyperParameters() const {
    	return Params;
    }

    size_t TLikGauss::GetHyperParametersSize() const {
        return 1;
    }
        
} // namespace NEgo
