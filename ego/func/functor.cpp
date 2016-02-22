#include "functor.h"


namespace NEgo {

	template <>
	TMatrixD TPartialDerivative<TMatrixD, TMatrixD>::Default(const TMatrixD& dArg, ui32 indexRow, ui32 indexCol) {
		TMatrixD part = NLa::CreateSameShape(dArg, /* fill_zeros = */ true);
		part(indexRow, indexCol) = dArg(indexRow, indexCol);
		return part;
	}

	template <>
	TVectorD TPartialDerivative<TVectorD, TVectorD>::Default(const TVectorD& dArg, ui32 index) {
		TVectorD part = NLa::CreateSameShape(dArg, /* fill_zeros = */ true);
		part(index) = dArg(index);
		return part;
	}


} // namespace NEgo
