#include "exact.h"

#include <ego/util/log/log.h>

#include <math.h>

namespace NEgo {

	TInfExact::TInfExact(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik)
        : IInf(mean, cov, lik)
    {
    	ENSURE(dynamic_cast<TLikGauss*>(Lik.get()), "Exact inference only possible with Gaussian likelihood");
    }

    TInfValue TInfExact::CalculateNegativeLogLik(const TMatrixD &X, const TVectorD &Y) {
        ENSURE(X.n_rows == Y.n_rows, "Need X and Y with the same number of rows");

        size_t n = X.n_rows;
    	size_t D = X.n_cols;

        auto covV = Cov->CalculateKernel(X);
        auto meanV = Mean->CalculateMean(X);

        auto K = covV.GetValue();
    	auto m = meanV.GetValue();
        
        double sn2 = exp(2*Lik->GetHyperParameters()(0));

        TMatrixD L;
    	TMatrixD pL;
        double sl;

    	if(fabs(sn2) < 1e-06) { // very tiny sn2 can lead to numerical trouble
    		L = NLa::Chol(K + sn2 * NLa::Eye(n));
    		sl = 1.0;
    		pL = - NLa::CholSolve(L, NLa::Eye(n));
    	} else {
    		L = NLa::Chol(K/sn2 + NLa::Eye(n));
    		sl = sn2;
    		pL = L;
    	}

        TVectorD alpha = NLa::AsVector(NLa::CholSolve(L, Y-m)/sl);
    	TVectorD diagW = NLa::Ones(n)/sqrt(sn2);

        return TInfValue(
            [=]() {
                return NLa::AsScalar(NLa::Trans(Y-m)*(alpha/2)) + NLa::Sum(NLa::Log(NLa::Diag(L))) + n*log(2*M_PI*sl)/2;;
            }, 
            [=]() {
                TMatrixD Q = NLa::CholSolve(pL, NLa::Eye(n))/sl - alpha * NLa::Trans(alpha);
                TVectorD dNLogLik(Cov->GetHyperParametersSize() + 1 + Mean->GetHyperParametersSize());

                size_t hypIdx=0;

                TMatrixD meanD = meanV.GetDerivative();
                for(size_t meanHypIdx=0; meanHypIdx < Mean->GetHyperParametersSize(); ++meanHypIdx, ++hypIdx) {
                    dNLogLik(hypIdx) =  NLa::AsScalar(- NLa::Trans(meanD.col(meanHypIdx)) * alpha);
                }

                TCubeD covD = covV.GetDerivative();
                for(size_t covHypIdx=0; covHypIdx < Cov->GetHyperParametersSize(); ++covHypIdx, ++hypIdx) {
                    dNLogLik(hypIdx) =  NLa::Sum(Q % covD.slice(covHypIdx))/2.0;
                }

                dNLogLik(hypIdx) = sn2 * NLa::Trace(Q);

                return dNLogLik;
            },
            [=]() {
                return TPosterior(pL, alpha, diagW);
            }
        );
    }


} // namespace NEgo
