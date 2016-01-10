#pragma once

#include "lik.h"
#include "link_funcs.h"

#include <ego/base/factory.h>

#include <ego/base/base.h>
#include <ego/base/errors.h>

#include <ego/distr/gamma.h>

#include <ego/util/log/log.h>

namespace NEgo {

	template <TLinkFunSpec LinkFun>
	class TLikGamma : public ILik {
	public:
        TLikGamma(size_t dim_size)
        	: ILik(dim_size)
        {
        }

		TLogLikValue CalculateLogLikelihood(const TVectorD &Y, const TVectorD &mean, const TVectorD &variance) const override final {
	        ENSURE(GetHyperParametersSize() == 1, "One hyperparameter must be set");
	        double al = exp(Params(0));
			
			TLinkFunTup linkTup = LinkFun(mean);
			const TVectorD &lg = linkTup.get<NLinkFunTup::lp>();
			const TVectorD &dlg = linkTup.get<NLinkFunTup::dlp>();
			const TVectorD &d2lg = linkTup.get<NLinkFunTup::d2lp>();
			const TVectorD &d3lg = linkTup.get<NLinkFunTup::d3lp>();
			
			TVectorD elg = NLa::Exp(lg);

			TVectorD lZy = NLa::LogGamma(al) - al * NLa::Log(al) + (1.0 - al) * NLa::Log(Y);
		    
		    TVectorD logP = - al * (lg+Y/elg) - lZy;
	        if( (variance.size()>0) && (NLa::Norm(variance) > std::numeric_limits<double>::epsilon()) ) { // variance is not zero
				
			}
	        

	        TVectorD dlZy = al*NLa::Psi(al) - al*(NLa::Log(al) + 1.0 + NLa::Log(Y));
	        TVectorD YDivElg = Y/elg;

	        return TLogLikValue(
	            [=]() -> TVectorD {
	                return logP;
	            },
	            [=]() -> TVectorD {
	                return - al * dlg % (1.0 - YDivElg);
	            },
	            [=]() -> TVectorD {
	                return - al * d2lg % (1.0 - YDivElg) - al * dlg % dlg % YDivElg;
	            },
	            [=]() -> TVectorD {
	                return - al * d3lg % (1.0 - YDivElg) + al * dlg % (dlg % dlg - 3.0 * d2lg) % YDivElg;
	            },
	            [=]() -> TVectorD {
	                return - al * (lg + YDivElg) - dlZy;
	            },
	            [=]() -> TVectorD {
	                return - al * dlg % (1.0 - YDivElg);
	            },
	            [=]() -> TVectorD {
	                return - al * d2lg % (1.0 - YDivElg) - al * dlg % dlg % YDivElg;
	            }
	        );
	    }
        
	    TPair<TVectorD, TVectorD> GetMarginalMeanAndVariance(const TVectorD &mean, const TVectorD &variance) const override final {
			double al = exp(Params(0));
	    	
	    	auto gauh = NLa::Gauher();
	    	const TVectorD &w = gauh.get<NLa::NGauherTup::weights>();
	    	const TVectorD &t = gauh.get<NLa::NGauherTup::quad>();

            TMatrixD lw = NLa::RepMat(NLa::Trans(NLa::Log(w)), w.size(), 1);
            TVectorD sig = NLa::Sqrt(variance);
            TMatrixD lg = LinkFun(sig * NLa::Trans(t) + NLa::RepMat(mean, 1, t.size())).get<NLinkFunTup::lp>();
            TVectorD ymean = NLa::Exp(NLa::LogSumExp(lg+lw));

            TVectorD elg = NLa::Exp(lg);
	        TVectorD yv = (elg % elg)/al;
	        TVectorD ys2 = (yv + NLa::Pow(elg - ymean, 2.0)) * w;
	        return MakePair(ymean, ys2);
	    }


	    void SetHyperParameters(const TVectorD &params) override final {
	    	ENSURE(params.size() == 1, "Gamma likelihood expecting one parameter");
	        Params = params;
	    }
	    
	    const TVectorD& GetHyperParameters() const override final {
	    	return Params;
	    }

	    size_t GetHyperParametersSize() const override final {
	        return 1;
	    }
	        
	    SPtr<IDistr> GetDistribution(double mean, double sd, ui32 seed) override final {
	        return SPtr<IDistr>(new TDistrGamma(mean, sd, seed));
	    }

		
	private:
		TVectorD Params;
	};
 	
 	using TLikGammaExp = TLikGamma<NLinkFuncs::Exp>;
	
	REGISTER_LIK(TLikGammaExp);

} // namespace NEgo
