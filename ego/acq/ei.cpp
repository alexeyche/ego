#include "ei.h"

#include <ego/model/model.h>

namespace NEgo {

    TAcqEI::Result TAcqEI::UserCalc(const TVectorD& x) const {
    	ENSURE(Model, "Model is not set");
        
        SPtr<IDistr> d = Model->GetPointPrediction(x);

        const double diff = Model->GetMinimum() - d->GetMean();
        const double z = diff / d->GetSd();
        const double pdf_z = d->StandardPdf(z);
        const double cdf_z = d->StandardCdf(z);
        
        double criteria = diff * cdf_z + d->GetSd() * pdf_z;
        
        return TAcqEI::Result()
            .SetValue(
                [=]() {
                   return criteria;
                }
            )
            .SetArgDeriv(
                [=]() {
                    return 0;
                }
            );
    }



} // namespace NEgo
