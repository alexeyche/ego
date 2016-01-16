#include "const.h"

#include <ego/base/errors.h>
#include <ego/util/log/log.h>

namespace NEgo {

    TMeanConst::TMeanConst(size_t dim_size)
        : IMean(dim_size)
    {
    }

    TMeanConst::Result TMeanConst::UserCalc(const TMatrixD &m) {
        TVectorD ones = NLa::Ones(m.n_rows);
        return TMeanConst::Result()
            .SetValue(
                [=]() -> TVectorD {
                    return Parameters[0] * ones;
                }
            )
            .SetParamDeriv(
                [=]() -> TVector<TVectorD> {
                    return {ones};
                }
            );
    }

    size_t TMeanConst::GetParametersSize() const {
        return 1;
    }
    
} //namespace NEgo