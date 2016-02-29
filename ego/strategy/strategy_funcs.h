#pragma once 

#include <ego/model/model.h>
#include <ego/strategy/config.h>

namespace NEgo {

	TPair<TVectorD, double> OptimizeAcquisition(TModel& model, const TOptConfig& optConfig);

} // namespace NEgo