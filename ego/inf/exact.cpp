#include "exact.h"

#include <ego/util/log/log.h>
#include <ego/lik/gauss.h>

#include <math.h>

namespace NEgo {

	TInfExact::TInfExact(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik)
        : IInf(mean, cov, lik)
    {
    	ENSURE(dynamic_cast<TLikGauss*>(Lik.get()), "Exact inference only possible with Gaussian likelihood");
    }

    TInfResult TInfExact::UserCalc(const TMatrixD& X, const TVectorD& Y) const {
        ENSURE(X.n_rows == Y.n_rows, "Need X and Y with the same number of rows");
        
        size_t n = X.n_rows;
    	size_t D = X.n_cols;

        auto covV = Cov->CrossCovariance(X);
        auto meanV = Mean->Calc(X);

        auto K = covV.Value();
    	auto m = meanV.Value();
        
        double sn2 = exp(2.0 * Lik->GetParameters()[0]);
        
        TMatrixD L = NLa::Chol(K + (sn2+1e-08) * NLa::Eye(n));
    	TMatrixD Kinv = NLa::CholSolve(L, NLa::Eye(n));

        TVectorD alpha = NLa::AsVector(Kinv * (Y-m));
    	TVectorD diagW = NLa::Ones(n)/sqrt(sn2);

    	return TInfResult()
    		.SetValue(
    			[=]() {
                    return 0.5 * (
                        NLa::AsScalar(NLa::Trans(Y-m) * alpha) + 
                        2.0 * NLa::Sum(NLa::Log(NLa::Diag(L))) + 
                        n * Log2Pi
                    );
            	}
    		)
    		.SetParamDeriv(
				[=]() -> TVector<double> {
	                TMatrixD Q  = Kinv - alpha * NLa::Trans(alpha);
	                TVector<double> dNLogLik(Mean->GetParametersSize() + Cov->GetParametersSize() + 1);

	                size_t hypIdx = 0;

	                for (const auto& dMdHyp: meanV.ParamDeriv()) {
	                	dNLogLik[hypIdx] = - NLa::AsScalar(NLa::Trans(dMdHyp) * alpha);
                        ++hypIdx;
	                }

	                for (const auto& dKdHyp: covV.ParamDeriv()) {
                        dNLogLik[hypIdx] = 0.5 * NLa::Trace(Q * dKdHyp);
                        ++hypIdx;
	                }

	                dNLogLik[hypIdx] = sn2 * NLa::Trace(Q);
	                return dNLogLik;
	            }
    		)
    		.SetPosterior(
    			[=]() {
            	    return TPosterior(L, alpha, diagW);
            	}
			);
    }
  
} // namespace NEgo