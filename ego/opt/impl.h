#pragma once

#include "cg.h"

#include <ego/base/la.h>

#include <nlopt/api/nlopt.hpp>

#include <functional>

namespace NEgo {
	class TModel;
	class IAcq;
	
	namespace NOpt {
		using TCallback = std::function<TPair<double, TVectorD>(const TVectorD&)>;

		struct TOptimizeConfig {
			TOptimizeConfig() {}
			TOptimizeConfig(ui32 maxEval)
				: MaxEval(maxEval)
			{}
			
			ui32 MaxEval = 100;
			double Tol = 1e-10;
		};

		double NLoptModelMinimizer(const std::vector<double> &x, std::vector<double> &grad, void* f_data);
		
		double NLoptAcqMinimizer(const std::vector<double> &x, std::vector<double> &grad, void* f_data);

		TPair<TVectorD, double> NLoptModelMinimize(TModel &model, TVectorD init, nlopt::algorithm algo, TOptimizeConfig config);

		TPair<TVectorD, double> NLoptAcqMinimize(SPtr<IAcq> acq, nlopt::algorithm algo, TOptimizeConfig config);
		
	} // namespace NOpt
} // namespace NEgo