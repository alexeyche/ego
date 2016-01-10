#include "linear.h"

#include <ego/util/log/log.h>


namespace NEgo {

    TMeanLinear::TMeanLinear(size_t dim_size)
        : IMean(dim_size)
    {
    }

    TMeanRet TMeanLinear::CalculateMean(const TMatrixD &m) {
        ENSURE(Params.size() > 0, "Need hyperparameters be set");
        ENSURE(m.n_cols ==  DimSize, "Col size of input matrix are not satisfy to mean function params: " << DimSize);
        
        return TMeanRet(
            [=]() -> TVectorD {
                return m * Params;
            }, 
            [=]() -> TMatrixD {
                return m;
            }
        );
    }

    void TMeanLinear::SetHyperParameters(const TVectorD &params) {
        ENSURE(params.size() == DimSize, "Need " << DimSize << " parameters for mean function, got " << params.size());

        Params = params;
    }

    size_t TMeanLinear::GetHyperParametersSize() const {
        return DimSize;
    }

    TVectorD TMeanLinear::GetHyperParameters() const {
        return Params;
    }

} //namespace NEgo