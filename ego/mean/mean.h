#pragma once

#include <ego/func/one_arg.h>

namespace NEgo {


    class IMean : public TOneArgFunctor<TVectorD, TMatrixD> {
    public:
        using TParent = TOneArgFunctor<TVectorD, TMatrixD>;
        IMean(size_t dim_size)
            : TParent(dim_size)
        {
        }

    };

} // namespace NEgo