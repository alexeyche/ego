#include "ei.h"

#include <ego/model/model.h>
#include <ego/util/log/log.h>

#include <sstream>

namespace NEgo {


    TAcqEI::Result TAcqEI::UserCalc(const TVectorD& x) const {
    	ENSURE(Model, "Model is not set");

        SPtr<IDistr> d = Model->GetPointPredictionWithDerivative(x);

        const double& tradeoff = Parameters[0];
        const double diff = Model->GetMinimumY() - d->GetMean() - tradeoff;
        const double u = diff / d->GetSd();
        const double pdf_u = d->StandardPdf(u);
        const double cdf_u = d->StandardCdf(u);

        double parenVal = (u * cdf_u + pdf_u);
        double criteria = - d->GetSd() * parenVal;

        return TAcqEI::Result()
            .SetValue(
                [=]() -> double {
                   return criteria;
                }
            )
            .SetArgPartialDeriv(
                [=](ui32 index) -> double {
                    const TVectorD& meanDeriv = d->GetMeanDeriv();
                    const TVectorD& sdDeriv = d->GetSdDeriv();
                    double dudx = - meanDeriv(index) / d->GetSd() - sdDeriv(index) * diff / (d->GetSd() * d->GetSd());
                    double deriv = d->GetSd() *
                        dudx * (
                            d->StandardPdfDeriv(u) +
                            u * d->StandardCdfDeriv(u) +
                            cdf_u
                        ) +
                        sdDeriv(index) * parenVal;
                    return - deriv;
                }
            );
            // .SetArgDeriv(
            //     [=]() -> double {
            //         double dudx = - d->GetMeanTotalDeriv() / d->GetSd() - d->GetSdTotalDeriv() * diff / (d->GetSd() * d->GetSd());
            //         double deriv = d->GetSd() *
            //             dudx * (
            //                 d->StandardPdfDeriv(u) +
            //                 u * d->StandardCdfDeriv(u) +
            //                 cdf_u
            //             ) +
            //             d->GetSdTotalDeriv() * parenVal;
            //         return - deriv;
            //     }
            // )
    }

    size_t TAcqEI::GetParametersSize() const {
        return 1;
    }

} // namespace NEgo
