#pragma once 

#include <ego/model/model.h>
#include <ego/solver/config.h>
#include <ego/util/sobol.h>


namespace NEgo {

	TPair<TVectorD, double> OptimizeAcquisition(SPtr<IModel> model, const TOptConfig& optConfig, TSobolGen& sobol);

	double Round(double dbVal, int nPlaces = 2);

} // namespace NEgo