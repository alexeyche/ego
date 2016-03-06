#include "batch.h"

#include <ego/model/base_model.h>

namespace NEgo {

	IBatchPolicy::IBatchPolicy(SPtr<IModel> model, const TSolverConfig& config)
        : Model(model)
        , Config(config)
    {
    }


} // namespace NEgo
