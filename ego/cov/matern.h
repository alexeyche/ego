#pragma once

#include <ego/base/la.h>

namespace NEgo {
    namespace NMaternFuncs {

        using TMaternFunSpec = TMatrixD(*)(const TMatrixD&);

        TMatrixD Matern1(const TMatrixD &K);

        TMatrixD MaternARDDeriv1(const TMatrixD &K);

        TMatrixD MaternISODeriv1(const TMatrixD &K);

        TMatrixD Matern3(const TMatrixD &K);

        TMatrixD MaternARDDeriv3(const TMatrixD &K);

        TMatrixD MaternISODeriv3(const TMatrixD &K);

        TMatrixD Matern5(const TMatrixD &K);

        TMatrixD MaternARDDeriv5(const TMatrixD &K);

        TMatrixD MaternISODeriv5(const TMatrixD &K);

    } // namespace NMaternFuncs

} //namespace NEgo