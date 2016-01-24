#pragma once

#include <ego/func/two_arg.h>

namespace NEgo {

    class ICov : public TTwoArgFunctor<TMatrixD, TMatrixD, TMatrixD> {
    public:
        using TParent = TTwoArgFunctor<TMatrixD, TMatrixD, TMatrixD>;

        ICov(size_t dimSize)
            : TParent(dimSize)
        {
        }

        ICov::Result CrossCovariance(const TMatrixD &m);

    };


} // namespace NEgo
