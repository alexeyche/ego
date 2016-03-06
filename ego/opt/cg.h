#pragma once

#include <ego/base/la.h>
#include <ego/solver/config.h>

namespace NEgo {
	namespace NOpt {
		using TCallback = std::function<TPair<double, TVectorD>(const TVectorD&)>;

		struct TOptimizeConfig;

		struct TCgMinimizeConfig {
			TCgMinimizeConfig();

			TCgMinimizeConfig(const TOptConfig& config);

			ui32 LineSearchNum = 10;
			ui32 MaxEval = 100;
			double InterruptWithin = 0.1;
			double ExtrapolateNums = 3.0;
			ui32 MaxLineSearchEval = 20;
			double MaxSlopeRatio = 10.0;
			double Sig = 0.1;
			double Rho = 0.05;
			double FirstReduction = 1.0;
		};

		TPair<TVectorD, double> CgMinimize(const TVectorD &X, TCallback f, TCgMinimizeConfig config = TCgMinimizeConfig());

	} // namespace NOpt
} // namespace NEgo