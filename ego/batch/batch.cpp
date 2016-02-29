#include "batch.h"

#include <ego/model/model.h>

namespace NEgo {

	IBatchPolicy::IBatchPolicy(SPtr<TModel> model, const TStrategyConfig& config)
        : Model(model)
        , Config(config)
    {
    }


} // namespace NEgo
