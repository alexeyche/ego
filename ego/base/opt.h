#pragma once

// #include <dlib/optimization.h>
#include <nlopt.hpp>

#include <ego/base/la.h>

namespace NEgo {
	class TModel;

	namespace NOpt {
		using TCallback = std::function<TPair<double, TVectorD>(const TVectorD&)>;

		enum EMethod {
			CG = 0,
			MMA = 1
		};

		double NLoptMinimizer(const std::vector<double> &x, std::vector<double> &grad, void* f_data);

		TPair<TVectorD, double> NLoptMinimize(const TModel &model, TVectorD init, nlopt::algorithm algo);

		struct TCgMinimizeConfig {
			ui32 LineSearchNum = 10;
			ui32 MaxEval = 200;
			double InterruptWithin = 0.1;
			double ExtrapolateNums = 3.0;
			ui32 MaxLineSearchEval = 20;
			double MaxSlopeRatio = 10.0;
			double Sig = 0.1;
			double Rho = 0.05;
			double FirstReduction = 1.0;
		};
		
		TPair<TVectorD, double> CgMinimize(const TVectorD &X, TCallback f, TCgMinimizeConfig config = TCgMinimizeConfig());


		TPair<TVectorD, double> Minimize(const TModel &self, TVectorD init, EMethod optMethod);
	} // namespace NOpt
} // namespace NEgo