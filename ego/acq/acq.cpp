#include "acq.h"

#include <ego/model/model.h>

namespace NEgo {

	void IAcq::SetModel(TModel& model) {
		Model.emplace(model);
	}

	size_t IAcq::GetDimSize() const {
		ENSURE(Model, "Model is not set");
		return Model->GetDimSize();
	}

} // namespace NEgo