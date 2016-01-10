#pragma once

#include <ego/func/two_arg.h>
#include <ego/base/entity.h>

namespace NEgo {

    class ICov : public TTwoArgFunctor<TMatrixD, TMatrixD, TMatrixD>, public IEntity {
    public:
        using Parent = TTwoArgFunctor<TMatrixD, TMatrixD, TMatrixD>;
        
        ICov(size_t dim_size)
            : IEntity(dim_size)
        {
        }

        ICov::Result CrossCovariance(const TMatrixD &m);
        
    };


} // namespace NEgo
