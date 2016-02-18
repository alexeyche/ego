#pragma once

namespace NEgo {

    template <typename T, typename A>
    struct TDerivativeOf;


    template <>
    struct TDerivativeOf<TVectorD, TMatrixD> {
        using Value = TVector<TMatrixD>;
    };

    template <>
    struct TDerivativeOf<double, TVectorD> {
        using Value = TVectorD;
    };

    template <>
    struct TDerivativeOf<TMatrixD, TMatrixD> {
        using Value = TVector<TVector<TMatrixD>>;
    };

} // namespace NEgo