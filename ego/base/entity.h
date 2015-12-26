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

        const size_t& GetDimSize() const {
            return DimSize;
        }

    protected:
        size_t DimSize;
    };





} // namespace NEgo