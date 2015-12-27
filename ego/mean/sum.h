#pragma once

#include "comp_mean.h"

#include <ego/base/factory.h>

namespace NEgo {


    class TMeanSum : public ICompMean {
    public:
        TMeanSum(TVector<SPtr<IMean>> means);

        TMeanRet CalculateMean(const TMatrixD &m) override final;

        void SetHyperParameters(const TVectorD &params) override final;
        
        size_t GetHyperParametersSize() const override final;
    };


    REGISTER_COMP_MEAN(TMeanSum);

} //namespace NEgo