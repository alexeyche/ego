#include "gaboriso.h"


namespace NEgo {

	TCovGaborISO::TCovGaborISO(size_t dim_size)
        : ICov(dim_size)
    {
    }

    TCovRet TCovGaborISO::CalculateKernel(const TMatrixD &left, const TMatrixD &right)  {
        ENSURE(Params.size() == GetHyperParametersSize(), "Need hyperparameters be set");
        ENSURE(left.n_cols ==  DimSize, "Col size of left input matrix are not satisfy to kernel params: " << DimSize);
        ENSURE(right.n_cols ==  DimSize, "Col size of right input matrix are not satisfy to kernel params: " << DimSize);

        double ell = NLa::Exp(Params(0));
        double p = NLa::Exp(Params(1));

        TMatrixD d2 = NLa::SquareDist(
            left/ell,
            right/ell
        );

        TMatrixD dp = NLa::Zeros(d2.n_rows, d2.n_cols);
        for(size_t pIdx=0; pIdx<DimSize; ++pIdx) {
            dp += (
                left.col(pIdx) * NLa::Trans(NLa::Ones(right.n_rows)) -
                NLa::Ones(left.n_rows) * NLa::Trans(right.col(pIdx))
            )/p;
        }

        TMatrixD K = NLa::Exp(-d2/2.0);
        TMatrixD Kres = NLa::Cos(2.0 * M_PI * dp) % K;
        return TCovRet(
        	[=]() -> TMatrixD {
            	return Kres;
        	},
        	[=]() -> TCubeD {
	            TCubeD dK(left.n_rows, right.n_rows, Params.size());
                dK.slice(0) = d2 % Kres;
                dK.slice(1) = 2.0 * M_PI * dp % NLa::Sin(2.0 * M_PI * dp) % K;
                return dK;
        	}
        );
    }

    void TCovGaborISO::SetHyperParameters(const TVectorD &params) {
        ENSURE(params.size() == GetHyperParametersSize(), "Another hyperparameter size: " << GetHyperParametersSize());
        Params = params;

    }

    size_t TCovGaborISO::GetHyperParametersSize() const {
        return 2;
    }

    TVectorD TCovGaborISO::GetHyperParameters() const {
        return Params;
    }

} // namespace NEgo