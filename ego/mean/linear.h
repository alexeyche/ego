#pragma once

#include "mean.h"

#include <ego/base/errors.h>
#include <ego/base/factory.h>

namespace NEgo {


    class TMeanLinear : public IMean {
    public:
        TMeanLinear(size_t dim_size);

        TMatrixD CalculateDerivative(const TMatrixD &m) override final;

        TVectorD CalculateMean(const TMatrixD &m) override final;

        void SetHyperParameters(const TVectorD &params) override final;

    private:

        TVectorD Params;
    };


    REGISTER_MEAN(TMeanLinear);

} //namespace NEgo