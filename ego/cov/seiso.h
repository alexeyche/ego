#pragma once

#include "cov.h"

#include <ego/base/errors.h>
#include <ego/base/factory.h>
#include <ego/util/log/log.h>

#include <numeric>
#include <functional>

namespace NEgo {

    class TCovSEISO : public ICov {
    public:
        TCovSEISO(size_t dim_size);

        TCovRet CalculateKernel(const TMatrixD &left, const TMatrixD &right) override final;

        void SetHyperParameters(const TVectorD &params) override final;
        
        size_t GetHyperParametersSize() const override final;
        
        TVectorD GetHyperParameters() const override final;

    private:

        TVectorD Params;
    };


    REGISTER_COV(TCovSEISO);

} //namespace NEgo