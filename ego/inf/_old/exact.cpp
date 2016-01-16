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

        auto covV = Cov->CrossCovariance(X);
        auto meanV = Mean->Calc(X);

        auto K = covV.Value();
    	auto m = meanV.Value();
        
        double sn2 = exp(2.0*log(Lik->GetParameters()[0]));

        // NLa::DebugSave(K, "K");
        // NLa::DebugSave(m, "m");

        
        // L_DEBUG << "sn2: " << sn2;

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

        
        // NLa::DebugSave(alpha, "alpha");
        // NLa::DebugSave(diagW, "diagW");
        return TInfExact::Result();
        // return TInfValue(
        //     [=]() {
        //         return NLa::AsScalar(NLa::Trans(Y-m)*(alpha/2)) + NLa::Sum(NLa::Log(NLa::Diag(L))) + n*log(2*M_PI*sl)/2;;
        //     },
        //     [=]() {
        //         TMatrixD Q = NLa::CholSolve(pL, NLa::Eye(n))/sl - alpha * NLa::Trans(alpha);
        //         TVectorD dNLogLik(Cov->GetParametersSize() + 1 + Mean->GetHyperParametersSize());

        //         size_t hypIdx=0;

        //         TMatrixD meanD = meanV.GetDerivative();
        //         for(size_t meanHypIdx=0; meanHypIdx < Mean->GetHyperParametersSize(); ++meanHypIdx, ++hypIdx) {
        //             dNLogLik(hypIdx) =  NLa::AsScalar(- NLa::Trans(meanD.col(meanHypIdx)) * alpha);
        //         }

        //         for(const auto& dKdHyp: covV.ParamDeriv()) {
        //             dNLogLik(hypIdx) =  NLa::Sum(Q % dKdHyp)/2.0;
        //             ++hypIdx;
        //         }

        //         dNLogLik(hypIdx) = sn2 * NLa::Trace(Q);

        //         return dNLogLik;
        //     },
        //     [=]() {
        //         return TPosterior(pL, alpha, diagW);
        //     }
        // );
    }
    


} // namespace NEgo
