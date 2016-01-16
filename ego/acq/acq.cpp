#include "acq.h"

#include <ego/model/model.h>

namespace NEgo {

	void IAcq::SetModel(TModel& model) {
		ENSURE(model.GetDimSize() == GetDimSize(), "Dimensions of model and Acq function are not in consistent state");
		Model.emplace(model);
	}

} // namespace NEgo