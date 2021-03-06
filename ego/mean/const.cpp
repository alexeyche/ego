#include "const.h"

#include <ego/base/errors.h>
#include <ego/util/log/log.h>

namespace NEgo {

    TMeanConst::TMeanConst(size_t dim_size)
        : IMean(dim_size)
    {
        Parameters = {0.0};
    }

    TMeanConst::Result TMeanConst::UserCalc(const TMatrixD &m) const {
        TVectorD ones = NLa::Ones(m.n_rows);
        const double& c = Parameters[0];

        return TMeanConst::Result()
            .SetValue(
                [=]() -> TVectorD {
                    return c * ones;
                }
            )
            .SetParamDeriv(
                [=]() -> TVector<TVectorD> {
                    return {ones};
                }
            )
            .SetArgDeriv(
                [=]() -> TVectorD {
                    return NLa::Zeros(m.n_rows);
                }
            )
            .SetArgPartialDeriv(
                [=](ui32 indexRow, ui32 indexCol) -> TVectorD {
                    return NLa::Zeros(m.n_rows);
                }
            );
    }

    size_t TMeanConst::GetParametersSize() const {
        return 1;
    }

} //namespace NEgo