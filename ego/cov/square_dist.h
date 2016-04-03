#pragma once

#include <ego/base/la.h>
#include <ego/func/two_arg.h>


namespace NEgo {

	class TSquareDistFunctor : public TTwoArgFunctor<TMatrixD, TMatrixD, TMatrixD> {
	public:
		using TParent = TTwoArgFunctor<TMatrixD, TMatrixD, TMatrixD>;

		TSquareDistFunctor(size_t dimSize);

		TSquareDistFunctor::Result UserCalc(const TMatrixD &left, const TMatrixD &right) const override final;
	};


} // namespace NEgo