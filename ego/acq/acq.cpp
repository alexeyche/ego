#include "acq.h"

#include <ego/model/model.h>

namespace NEgo {

	void IAcq::SetModel(TModel& model) {
		Model.emplace(model);
	}

} // namespace NEgo