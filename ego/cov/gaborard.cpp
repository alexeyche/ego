#include "gaborard.h"


namespace NEgo {

	TCovGaborARD::TCovGaborARD(size_t dim_size)
        : ICov(dim_size)
    {
    }

    TCovRet TCovGaborARD::CalculateKernel(const TMatrixD &left, const TMatrixD &right)  {
        ENSURE(Params.size() == GetHyperParametersSize(), "Need hyperparameters be set");
        ENSURE(left.n_cols ==  DimSize, "Col size of left input matrix are not satisfy to kernel params: " << DimSize);
        ENSURE(right.n_cols ==  DimSize, "Col size of right input matrix are not satisfy to kernel params: " << DimSize);
        
        TVectorD ell = NLa::Exp(NLa::SubVec(Params, 0, DimSize));
        TVectorD p = NLa::Exp(NLa::SubVec(Params, DimSize, 2*DimSize));
        
        TMatrixD d2 = NLa::SquareDist(
            NLa::Trans(NLa::DiagMat(1.0/ell) * NLa::Trans(left)), 
            NLa::Trans(NLa::DiagMat(1.0/ell) * NLa::Trans(right))
        );
        
        TMatrixD dp = NLa::Zeros(d2.n_rows, d2.n_cols);
        for(size_t pIdx=0; pIdx<p.size(); ++pIdx) {
            dp += (
                left.col(pIdx) * NLa::Trans(NLa::Ones(right.n_rows)) - 
                NLa::Ones(left.n_rows) * NLa::Trans(right.col(pIdx))
            )/p(pIdx);
        }
        
        TMatrixD K = NLa::Exp(-d2/2.0);
        TMatrixD Kres = NLa::Cos(2.0 * M_PI * dp) % K;
        return TCovRet(
        	[=]() -> TMatrixD {
            	return Kres;
        	}, 
        	[=]() -> TCubeD {
	            TCubeD dK(left.n_rows, right.n_rows, Params.size());
                size_t pIdx=0;
                for(size_t idx=0; idx < DimSize; ++idx, ++pIdx) {
                    TMatrixD dd = left.col(idx) * NLa::Trans(NLa::Ones(right.n_rows)) - NLa::Ones(left.n_rows) * NLa::Trans(right.col(idx));
                    dK.slice(pIdx) = NLa::Pow(dd/ell(pIdx), 2.0) % Kres;
                }
                for(size_t idx=0; idx < DimSize; ++idx, ++pIdx) {
                    TMatrixD dd = left.col(idx) * NLa::Trans(NLa::Ones(right.n_rows)) - NLa::Ones(left.n_rows) * NLa::Trans(right.col(idx));
                    dK.slice(pIdx) = 2.0 * M_PI * dd/p(idx) % NLa::Sin(2.0 * M_PI * dp) % K;
                }
                return dK;
        	}
        );
    }

    void TCovGaborARD::SetHyperParameters(const TVectorD &params) {
        ENSURE(params.size() == GetHyperParametersSize(), "Another hyperparameter size: " << GetHyperParametersSize());
        Params = params;
        
    }
    
    size_t TCovGaborARD::GetHyperParametersSize() const {
        return DimSize*2;
    }

    TVectorD TCovGaborARD::GetHyperParameters() const {
        return Params;
    }

} // namespace NEgo