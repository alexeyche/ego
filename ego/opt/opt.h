#pragma once

#include "impl.h"

namespace NEgo {
	namespace NOpt {

		EMethod MethodFromString(TString s);

 		TString MethodToString(EMethod m);

 		void PrintMethods();

		TPair<TVector<double>, double> OptimizeModelLogLik(TModel &self, const TVector<double>& start, const TOptConfig& config);

		TPair<TVectorD, double> OptimizeAcquisitionFunction(SPtr<IAcq> acq, const TVectorD& start, const TOptConfig& config);

	} // namespace NOpt
} // namespace NEgo