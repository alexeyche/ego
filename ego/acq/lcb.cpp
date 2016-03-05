#include "lcb.h"

#include <ego/model/model.h>
#include <ego/util/log/log.h>

namespace NEgo {

    TAcqLCB::Result TAcqLCB::UserCalc(const TVectorD& x) const {
    	ENSURE(Model, "Model is not set");

        SPtr<IDistr> d = Model->GetPointPredictionWithDerivative(x);

        const double& tradeoff = Parameters[0];

        double criteria = - d->GetMean() + tradeoff * d->GetSd();

        return TAcqLCB::Result()
            .SetValue(
                [=]() -> double {
                   return - criteria;
                }
            )
            .SetArgPartialDeriv(
                [=](ui32 index) -> double {
                    const TVectorD& meanDeriv = d->GetMeanDeriv();
                    const TVectorD& sdDeriv = d->GetSdDeriv();
                    double dCriteria = - meanDeriv(index) + tradeoff * sdDeriv(index);
                    return - dCriteria;
                }
            );
    }

    size_t TAcqLCB::GetParametersSize() const {
        return 1;
    }

} // namespace NEgo
