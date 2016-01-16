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

    TInfResult TInfExact::UserCalc(const TMatrixD& X, const TVectorD& Y) {
        ENSURE(X.n_rows == Y.n_rows, "Need X and Y with the same number of rows");
        
        size_t n = X.n_rows;
    	size_t D = X.n_cols;

        auto covV = Cov->CrossCovariance(X);
        auto meanV = Mean->Calc(X);

        auto K = covV.Value();
    	auto m = meanV.Value();
        
        double sn2 = Lik->GetParameters()[0];
        
        TMatrixD L;
    	TMatrixD pL;

        double sl;

    	if(fabs(sn2) < 1e-06) { // very tiny sn2 can lead to numerical trouble
    		sn2 = std::max(sn2, 1e-10);
            L = NLa::Chol(K + sn2 * NLa::Eye(n));
    		sl = 1.0;
    		pL = - NLa::CholSolve(L, NLa::Eye(n));
    	} else {
    		L = NLa::Chol(K/sn2 + NLa::Eye(n));
    		sl = sn2;
    		pL = L;
    	}

        // NLa::DebugSave(L, "L");
        // NLa::DebugSave(pL, "pL");
        
        TVectorD alpha = NLa::AsVector(NLa::CholSolve(L, Y-m)/sl);
    	TVectorD diagW = NLa::Ones(n)/sqrt(sn2);

    	return TInfResult()
    		.SetValue(
    			[=]() {
                	return NLa::AsScalar(NLa::Trans(Y-m)*(alpha/2)) + NLa::Sum(NLa::Log(NLa::Diag(L))) + n*log(2*M_PI*sl)/2;;
            	}
    		)
    		.SetParamDeriv(
				[=]() -> TVector<double> {
	                TMatrixD Q = NLa::CholSolve(pL, NLa::Eye(n))/sl - alpha * NLa::Trans(alpha);
	                TVector<double> dNLogLik(Cov->GetParametersSize() + 1 + Mean->GetParametersSize());

	                size_t hypIdx = 0;

	                for (const auto& dMdHyp: meanV.ParamDeriv()) {
	                	dNLogLik[hypIdx] =  NLa::AsScalar(- NLa::Trans(dMdHyp) * alpha);
	                }

	                for (const auto& dKdHyp: covV.ParamDeriv()) {
	                    dNLogLik[hypIdx] =  NLa::Sum(Q % dKdHyp)/2.0;
	                    ++hypIdx;
	                }

	                dNLogLik[hypIdx] = sn2 * NLa::Trace(Q);

	                return dNLogLik;
	            }
    		)
    		.SetPosterior(
    			[=]() {
            	    return TPosterior(pL, alpha, diagW);
            	}
			);
    }
  
} // namespace NEgo