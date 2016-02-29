#include "random.h"

#include <ego/model/model.h>
#include <ego/strategy/strategy.h>
#include <ego/strategy/strategy_funcs.h>
#include <ego/strategy/config.h>

namespace NEgo {

	TBatchPolicyRandom::TBatchPolicyRandom(SPtr<TModel> model, const TStrategyConfig& config)
		: IBatchPolicy(model, config)
		, NewBatch(false)
	{
	}

    void TBatchPolicyRandom::InitNewBatch() {
    	NewBatch = true;
    }

    TVectorD TBatchPolicyRandom::GetNextElementInBatch() {
    	if (NewBatch) {
    		NewBatch = false;
    		return OptimizeAcquisition(*Model, Config.AcqOpt).first;
    	}
    	return NLa::UnifVec(Model->GetDimSize());
    }

} // namespace NEgo
