#include "exact.h"

#include <math.h>

namespace NEgo {

	TInfExact::TInfExact(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik)
        : IInf(mean, cov, lik)
    {
    	ENSURE(dynamic_cast<TLikGauss*>(Lik.get()), "Exact inference only possible with Gaussian likelihood");
    }

    double TInfExact::NegativeLogLik(const TMatrixD &X, const TVectorD &Y) {
    	ENSURE(X.n_rows == Y.n_rows, "Need X and Y with the same number of rows");
    	
    	size_t n = X.n_rows;    	
    	size_t D = X.n_cols;
    	
    	auto K = Cov->CalculateKernel(X);
    	auto m = Mean->CalculateMean(X);
    	double sn2 = exp(2*Lik->GetHyperParameters()(0));
        
        double sl;
    	TMatrixD L;
    	TMatrixD pL;

    	if(fabs(sn2) < 1e-06) { // very tiny sn2 can lead to numerical trouble
    		L = NLa::Chol(K + sn2 * NLa::Eye(n));
    		sl = 1.0;
    		pL = - NLa::CholSolve(L, NLa::Eye(n));
    	} else {
    		L = NLa::Chol(K/sn2 + NLa::Eye(n));
    		sl = sn2;
    		pL = L;
    	}
    	TPosterior post;
        post.Alpha = NLa::AsVector(NLa::CholSolve(L, Y-m)/sl);
    	post.DiagW = NLa::Ones(n)/sqrt(sn2);
    	post.L = pL;
        
        return NLa::AsScalar(NLa::Trans(Y-m)*(post.Alpha/2)) + NLa::Sum(NLa::Log(NLa::Diag(L))) + n*log(2*M_PI*sl)/2;
    }

} // namespace NEgo
