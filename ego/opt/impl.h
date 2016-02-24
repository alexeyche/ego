#pragma once

#include "cg.h"

#include <ego/base/la.h>

#include <nlopt/api/nlopt.hpp>
#include <ego/strategy/config.h>

#include <functional>



namespace NEgo {
	class TModel;
	class IAcq;

	namespace NOpt {
		using TCallback = std::function<TPair<double, TVectorD>(const TVectorD&)>;

		double NLoptModelMinimizer(const std::vector<double> &x, std::vector<double> &grad, void* f_data);

		double NLoptAcqMinimizer(const std::vector<double> &x, std::vector<double> &grad, void* f_data);

		TPair<TVector<double>, double> NLoptModelMinimize(TModel &model, const TVector<double>& start, const TOptConfig& config);

		TPair<TVectorD, double> NLoptAcqMinimize(SPtr<IAcq> acq, const TOptConfig& config);

	} // namespace NOpt
} // namespace NEgo