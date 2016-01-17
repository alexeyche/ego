#pragma once

#include "mean.h"

#include <ego/base/factory.h>

namespace NEgo {


    class TMeanConst : public IMean {
    public:
        TMeanConst(size_t dim_size);

        TMeanConst::Result UserCalc(const TMatrixD &m) const override final;

        size_t GetParametersSize() const override final;
    };


    REGISTER_MEAN(TMeanConst);

} //namespace NEgo