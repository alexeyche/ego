#include "ei.h"

#include <ego/model/model.h>
#include <ego/util/log/log.h>

namespace NEgo {


    TAcqEI::Result TAcqEI::UserCalc(const TVectorD& x) const {
    	ENSURE(Model, "Model is not set");

        SPtr<IDistr> d = Model->GetPointPredictionWithDerivative(x);

        const double& tradeoff = Parameters[0];
        const double& tradeoffFastPenalty = Parameters[2];
        const double diff = Model->GetMinimumY() - d->GetMean() + tradeoff + tradeoffFastPenalty;
        const double u = diff / d->GetSd();
        const double pdf_u = d->StandardPdf(u);
        const double cdf_u = d->StandardCdf(u);
        
        double parenVal = (u * cdf_u + pdf_u);
        double criteria = - d->GetSd() * parenVal;
        
        // L_DEBUG << x(0) << " -> " << criteria << "; " << d->GetSd() << " " << parenVal << " ( " << u << " * " << cdf_u << " + " << pdf_u << " )";
        // L_DEBUG << "( " << Model->GetMinimumY() << " - " << d->GetMean() << " - " << tradeoff << " ) / " << d->GetSd();
        
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
        return 4;
    }

    void TAcqEI::SetParameters(const TVector<double>& parameters) {
        if (parameters.size() == 1) {
            Parameters[0] = parameters[0];
            return;
        }
        
        Parameters = parameters;
    }

    void TAcqEI::Update() {
        Parameters[0] += - Parameters[0]/Parameters[1];
        Parameters[2] += - Parameters[2]/Parameters[3];
    }

    void TAcqEI::EnhanceGlobalSearch() {
        Parameters[2] += 5.0;
    }

} // namespace NEgo
