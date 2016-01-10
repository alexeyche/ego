#include "seard.h"


namespace NEgo {

	TCovSEARD::TCovSEARD(size_t dim_size)
        : ICov(dim_size)
    {
    }

    TCovRet TCovSEARD::CalculateKernel(const TMatrixD &left, const TMatrixD &right)  {
        ENSURE(Params.size() > 0, "Need hyperparameters be set");
        ENSURE(left.n_cols ==  DimSize, "Col size of left input matrix are not satisfy to kernel params: " << DimSize);
        ENSURE(right.n_cols ==  DimSize, "Col size of right input matrix are not satisfy to kernel params: " << DimSize);
        
        TVectorD ell = NLa::Exp(Params.head(Params.size()-1));
        double signalVariance = NLa::Exp(2.0 * NLa::GetLastElem(Params));

        TMatrixD K = NLa::SquareDist(
            NLa::Trans(NLa::DiagMat(1.0/ell) * NLa::Trans(left)), 
            NLa::Trans(NLa::DiagMat(1.0/ell) * NLa::Trans(right))
        );
        K = signalVariance * NLa::Exp(-K/2.0);
        
        return TCovRet(
        	[=]() -> TMatrixD {
            	return K;
        	}, 
        	[=]() -> TCubeD {
	            TCubeD dK(left.n_rows, right.n_rows, Params.size());
                size_t pIdx=0;
                for(; pIdx < left.n_cols; ++pIdx) {
                    TMatrixD Ki = NLa::SquareDist(
                        left.col(pIdx)/ell(pIdx),
                        right.col(pIdx)/ell(pIdx)
                    );
                    dK.slice(pIdx) = K % Ki;
                }
                dK.slice(pIdx) = 2.0 * K;
                return dK;
        	}
        );
    }

    void TCovSEARD::SetHyperParameters(const TVectorD &params) {
        ENSURE(params.size() == DimSize + 1, "Need DimSize + 1 parameters for kernel");
        Params = params;
        
    }
    
    size_t TCovSEARD::GetHyperParametersSize() const {
        return DimSize + 1;
    }

    TVectorD TCovSEARD::GetHyperParameters() const {
        return Params;
    }

} // namespace NEgo