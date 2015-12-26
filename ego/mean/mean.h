#pragma once

#include <ego/base/entity.h>

namespace NEgo {

    class IMean : public IEntity {
    public:
        IMean(size_t dim_size)
            : IEntity(dim_size)
        {
        }

        virtual TMatrixD CalculateDerivative(const TMatrixD &m) = 0;
        virtual TVectorD CalculateMean(const TMatrixD &m) = 0;
		virtual void SetHyperParameters(const TVectorD &params) = 0;
    };

} // namespace NEgo