#pragma once

#include "mean.h"

#include <ego/base/factory.h>

namespace NEgo {


    class TMeanConst : public IMean {
    public:
        TMeanConst(size_t dim_size);

        TMeanRet CalculateMean(const TMatrixD &m) override final;

        void SetHyperParameters(const TVectorD &params) override final;
        
        size_t GetHyperParametersSize() const override final;
        
        TVectorD GetHyperParameters() const override final;
    private:

        TVectorD Params;
    };


    REGISTER_MEAN(TMeanConst);

} //namespace NEgo