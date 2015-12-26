#include "const.h"

#include <ego/base/errors.h>

namespace NEgo {

    TMeanConst::TMeanConst(size_t dim_size)
        : IMean(dim_size)
    {
    }

    TMatrixD TMeanConst::CalculateDerivative(const TMatrixD &m) {
        return m;
    }

    TVectorD TMeanConst::CalculateMean(const TMatrixD &m) {
        ENSURE(Params.size() > 0, "Need hyperparameters be set");
        ENSURE(m.n_cols ==  DimSize, "Col size of input matrix are not satisfy to mean function params: " << DimSize);

        return Params(0) * NLa::Ones(m.n_rows);
    }

    void TMeanConst::SetHyperParameters(const TVectorD &params) {
        ENSURE(params.size() == 1, "Need 1 parameters for mean function");

        Params = params;
    }

} //namespace NEgo