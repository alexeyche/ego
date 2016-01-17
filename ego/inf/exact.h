#pragma once

#include "inf.h"

#include <ego/base/errors.h>
#include <ego/base/factory.h>


namespace NEgo {

	class TInfExact : public IInf {
	public:
        TInfExact(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik);
		
		static constexpr double Log2Pi = 1.83787706640934533908;
		
		TInfResult UserCalc(const TMatrixD &X, const TVectorD &Y) const override final;

	};
 
	REGISTER_INF(TInfExact);

} // namespace NEgo
