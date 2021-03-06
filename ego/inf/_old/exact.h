#pragma once

#include "inf.h"

#include <ego/base/errors.h>
#include <ego/base/factory.h>

#include <ego/lik/gauss.h>

namespace NEgo {

	class TInfExactOld : public IInf {
	public:
        TInfExactOld(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik);
		
		TInfValue CalculateNegativeLogLik(const TMatrixD &X, const TVectorD &Y) override final;

		void UpdatePosterior(const TMatrixD &X, const TVectorD &Y, TPosterior& post) override final {} // not implemented
	};
 
	// REGISTER_INF(TInfExact);

} // namespace NEgo
