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
        
        double criteria;
        if (Exp == 1) {
            criteria = ( diff * cdf_z + d->GetSd() * pdf_z );
        } else {
            const double fg = NLa::Factorial(Exp);

            double Tm2 = cdf_z;
            double Tm1 = pdf_z;
            double sumEI = std::pow(z, Exp) * Tm2 - Exp * std::pow(z, Exp-1) * Tm1;

            for (size_t ii = 2; ii < Exp; ++ii) {
                double Tact = (ii-1)*Tm2 - pdf_z * std::pow(z, ii-1);
                sumEI += std::pow(-1.0, ii) * 
                        (fg / ( NLa::Factorial(ii) * NLa::Factorial(Exp-ii) ) )*
                        std::pow(z, Exp-ii)*Tact;

                Tm2 = Tm1;   Tm1 = Tact;
            }
            criteria = std::pow(d->GetSd(), Exp) * sumEI;
        }

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
