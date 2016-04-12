#pragma once 

#include <ego/model/model.h>
#include <ego/solver/config.h>

namespace NEgo {

	TPair<TVectorD, double> OptimizeAcquisition(SPtr<IModel> model, const TOptConfig& optConfig);

	double Round(double dbVal, int nPlaces = 2);

} // namespace NEgo