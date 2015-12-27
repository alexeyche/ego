#include "const.h"

#include <ego/base/errors.h>

namespace NEgo {

    TMeanConst::TMeanConst(size_t dim_size)
        : IMean(dim_size)
    {
    }

    TMeanRet TMeanConst::CalculateMean(const TMatrixD &m) {
        ENSURE(Params.size() > 0, "Need hyperparameters be set");
        ENSURE(m.n_cols ==  DimSize, "Col size of input matrix are not satisfy to mean function params: " << DimSize);
        
        TVectorD ones = NLa::Ones(m.n_rows);
        
        return TMeanRet(
            [=]() {
                return Params(0) * ones;    
            }, 
            [=]() {
                return ones;
            }
        );
    }

    void TMeanConst::SetHyperParameters(const TVectorD &params) {
        ENSURE(params.size() == GetHyperParametersSize(), "Need " << GetHyperParametersSize() << " parameters for mean function");

        Params = params;
    }
    
    size_t TMeanConst::GetHyperParametersSize() const {
        return 1;
    }

} //namespace NEgo