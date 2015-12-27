#include "seiso.h"


namespace NEgo {

	TCovSEISO::TCovSEISO(size_t dim_size)
        : ICov(dim_size)
    {
    }

    TCovRet TCovSEISO::CalculateKernel(const TMatrixD &left, const TMatrixD &right)  {
        ENSURE(Params.size() > 0, "Need hyperparameters be set");
        ENSURE(left.n_cols ==  DimSize, "Col size of left input matrix are not satisfy to kernel params: " << DimSize);
        ENSURE(right.n_cols ==  DimSize, "Col size of right input matrix are not satisfy to kernel params: " << DimSize);
        
        double ell = exp(Params(0));
        double sf2 = exp(2.0*Params(1));

        TMatrixD K = NLa::SquareDist(left/ell, right/ell);
        TMatrixD cov = sf2 * NLa::Exp(-K/2.0);

        return TCovRet(
        	[=]() -> TMatrixD {
            	return cov;
        	}, 
        	[=]() -> TCubeD {
	            TCubeD dK(left.n_rows, right.n_rows, GetHyperParametersSize());
	            dK.slice(0) = cov % K;
	            dK.slice(1) = 2.0 * cov;
	            return dK;
        	}
        );
    }

    void TCovSEISO::SetHyperParameters(const TVectorD &params) {
        ENSURE(params.size() == GetHyperParametersSize(), "Need " << GetHyperParametersSize() << " parameters for kernel");
        Params = params;
    }
    
    size_t TCovSEISO::GetHyperParametersSize() const {
        return 2;
    }
}