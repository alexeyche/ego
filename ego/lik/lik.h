#pragma once

#include <ego/base/entity.h>

namespace NEgo {

    struct TPredictiveDistribution {
        TVectorD LogP;
        TVectorD Mean;
        TVectorD Variance;
    };

    class ILik : public IEntity {
    public:
        ILik(size_t dim_size)
            : IEntity(dim_size)
        {
        }

        virtual TPredictiveDistribution Evaluate(const TMatrixD &left, const TMatrixD &right) = 0;

    };


} // namespace NEgo
