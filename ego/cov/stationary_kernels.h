#pragma once

#include <ego/func/one_arg.h>

namespace NEgo {
    namespace NKernels {

    	class TSqExp : public TOneArgFunctor<TMatrixD, TMatrixD> {
		public:
			using TParent = TOneArgFunctor<TMatrixD, TMatrixD>;

			TSqExp(size_t dimSize);
			
    		TSqExp::Result UserCalc(const TMatrixD &r) override final;
    	};

    } // namespace NKernels
} //namespace NEgo