#pragma once

#include <ego/base/la.h>

namespace NEgo {

    class ICov {
    public:
        ICov(size_t dim_size)
            : DimSize(dim_size)
        {
        }
        virtual ~ICov() {
        }

        virtual TMatrixD CalculateKernel(const TMatrixD &left, const TMatrixD &right) = 0;
        virtual TMatrixD CalculateDerivative(const TMatrixD &left, const TMatrixD &right, size_t param_idx) = 0;
        virtual void SetHyperParameters(const TVectorD &params) = 0;

    protected:
        size_t DimSize;
    };


} // namespace NEgo
