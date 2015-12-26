#include "linear.h"

namespace NEgo {

    TMeanLinear::TMeanLinear(size_t dim_size)
        : IMean(dim_size)
    {
    }

    TMatrixD TMeanLinear::CalculateDerivative(const TMatrixD &m) {
        return m;
    }

    TVectorD TMeanLinear::CalculateMean(const TMatrixD &m) {
        ENSURE(Params.size() > 0, "Need hyperparameters be set");
        ENSURE(m.n_cols ==  DimSize, "Col size of input matrix are not satisfy to mean function params: " << DimSize);

        return m * Params;
    }

    void TMeanLinear::SetHyperParameters(const TVectorD &params) {
        ENSURE(params.size() == DimSize, "Need DimSize parameters for mean function");

        Params = params;
    }

} //namespace NEgo