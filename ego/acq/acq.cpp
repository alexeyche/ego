#include "acq.h"

#include <ego/model/base_model.h>

namespace NEgo {

	void IAcq::SetModel(IModel& model) {
		Model.emplace(model);
	}

} // namespace NEgo