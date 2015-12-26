#pragma once

#include <ego/base/entity.h>

namespace NEgo {

    class ICov : public IEntity {
    public:
        ICov(size_t dim_size)
            : IEntity(dim_size)
        {
        }

        virtual TCubeD CalculateDerivative(const TMatrixD &left, const TMatrixD &right) = 0;

        virtual TMatrixD CalculateKernel(const TMatrixD &left, const TMatrixD &right) = 0;

        virtual void SetHyperParameters(const TVectorD &params) = 0;        

        TMatrixD CalculateKernel(const TMatrixD &m);
    };


} // namespace NEgo
