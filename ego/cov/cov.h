#pragma once

#include <ego/base/entity.h>
#include <ego/base/value.h>

namespace NEgo {

    
    using TCovRet = TValue<TMatrixD, TCubeD>;

    class ICov : public IEntity {
    public:
        
        ICov(size_t dim_size)
            : IEntity(dim_size)
        {
        }


        virtual TCovRet CalculateKernel(const TMatrixD &left, const TMatrixD &right) = 0;

        TCovRet CalculateKernel(const TMatrixD &m);

        virtual void SetHyperParameters(const TVectorD &params) = 0;        

        virtual size_t GetHyperParametersSize() const = 0;

    };


} // namespace NEgo
