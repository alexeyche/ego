#pragma once

#include <ego/base/la.h>

namespace NEgo {

    class IEntity {
    public:
        IEntity(size_t dim_size)
            : DimSize(dim_size)
        {
        }

        virtual ~IEntity() {
        }

        virtual void SetHyperParameters(const TVectorD &params) = 0;

    protected:
        size_t DimSize;
    };


} // namespace NEgo