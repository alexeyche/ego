#pragma once

#include "inf.h"

#include <ego/base/errors.h>
#include <ego/base/factory.h>

namespace NEgo {

	struct IterReweightedLeastSquaresConfig {
		ui32 MaxEval = 20;
		double Wmin = 0.0;
		double Tol = 1e-06;
	};

	namespace NPsiTup {
		struct psi {};
		struct dpsi {};
		struct f {};
		struct alpha {};
		struct dlp {};
		struct W {};
	}

	using TPsiTup = TTagTuple<
		NPsiTup::psi, double, 
		NPsiTup::dpsi, TVectorD, 
		NPsiTup::f, TVectorD, 
		NPsiTup::alpha, TVectorD, 
		NPsiTup::dlp, TVectorD, 
		NPsiTup::W, TVectorD
	>;

	namespace NLogDetTup {
		struct ld {};
		struct inv {};
		struct mwInv {};
	};

	using TLogDetTup = tagged_tuple<
		NLogDetTup::ld, double,
		NLogDetTup::inv, TMatrixD, 
		NLogDetTup::mwInv, TMatrixD
	>;


	class TInfLaplace : public IInf {
	public:
        TInfLaplace(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik);
		
		TInfValue CalculateNegativeLogLik(const TMatrixD &X, const TVectorD &Y) override final;

		void UpdatePosterior(const TMatrixD &X, const TVectorD &Y, TPosterior& post) override final;

		static void IterReweightedLeastSquares(
			TVectorD& alpha, const TVectorD& mean, const TMatrixD& K, SPtr<ILik> lik,
			const TVectorD& Y, IterReweightedLeastSquaresConfig config = IterReweightedLeastSquaresConfig()
		);
	
		static TPsiTup Psi(const TVectorD& alpha, const TVectorD& mean, const TMatrixD& K, SPtr<ILik> lik, const TVectorD& Y);

		static TLogDetTup LogDet(const TMatrixD& K, const TVectorD &w);
	};
 
	REGISTER_INF(TInfLaplace);

} // namespace NEgo
