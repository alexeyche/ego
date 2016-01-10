#pragma once

#include <ego/base/entity.h>
#include <ego/base/value.h>

namespace NEgo {

    using TMeanRet = TValue<TVectorD, TMatrixD>;

    class IMean : public IEntity {
    public:
        IMean(size_t dim_size)
            : IEntity(dim_size)
        {
        }

        virtual TMeanRet CalculateMean(const TMatrixD &m) = 0;
		
        virtual void SetHyperParameters(const TVectorD &params) = 0;

        virtual size_t GetHyperParametersSize() const = 0;

        virtual TVectorD GetHyperParameters() const = 0;

    };

} // namespace NEgo