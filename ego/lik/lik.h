#pragma once

#include <ego/base/entity.h>

namespace NEgo {

    struct TPredictiveDistribution {
        TVectorD LogP;
        TVectorD Mean;
        TVectorD Variance;
    };

    class ILik : public IEntity {
    public:
        ILik(size_t dim_size)
            : IEntity(dim_size)
        {
        }

        virtual TPredictiveDistribution Evaluate(const TVectorD &Y, const TVectorD &Ymean, const TVectorD &Ysd) = 0;
        
        virtual void SetHyperParameters(const TVectorD &params) = 0;
        virtual const TVectorD& GetHyperParameters() const = 0;
    };


} // namespace NEgo
