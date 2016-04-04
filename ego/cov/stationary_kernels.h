#pragma once

#include <ego/func/one_arg.h>

namespace NEgo {
    namespace NKernels {

    	class TSqExp : public TOneArgFunctor<TMatrixD, TMatrixD> {
		public:
			using TParent = TOneArgFunctor<TMatrixD, TMatrixD>;

	    	TSqExp(size_t dimSize) : TParent(dimSize) {}

    		TSqExp::Result UserCalc(const TMatrixD &r) const override final;
    	};

    	class TExp : public TOneArgFunctor<TMatrixD, TMatrixD> {
		public:
			using TParent = TOneArgFunctor<TMatrixD, TMatrixD>;

			TExp(size_t dimSize) : TParent(dimSize) {}

    		TExp::Result UserCalc(const TMatrixD &r) const override final;
    	};


        class TMatern32 : public TOneArgFunctor<TMatrixD, TMatrixD> {
        public:
            using TParent = TOneArgFunctor<TMatrixD, TMatrixD>;

            TMatern32(size_t dimSize) : TParent(dimSize) {}

            TMatern32::Result UserCalc(const TMatrixD &r) const override final;
        };

        class TMatern52 : public TOneArgFunctor<TMatrixD, TMatrixD> {
        public:
            using TParent = TOneArgFunctor<TMatrixD, TMatrixD>;

            TMatern52(size_t dimSize) : TParent(dimSize) {}

            TMatern52::Result UserCalc(const TMatrixD &r) const override final;
        };


    } // namespace NKernels
} //namespace NEgo