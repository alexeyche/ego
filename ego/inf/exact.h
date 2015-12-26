#pragma once

#include "inf.h"

#include <ego/base/errors.h>
#include <ego/base/factory.h>

#include <ego/lik/gauss.h>

namespace NEgo {

	class TInfExact : public IInf {
	public:
        TInfExact(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik);

        double NegativeLogLik(const TMatrixD &X, const TVectorD &Y) override final;

	};
 
	REGISTER_INF(TInfExact);

} // namespace NEgo
