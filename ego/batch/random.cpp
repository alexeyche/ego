#include "random.h"

#include <ego/solver/config.h>
#include <ego/solver/utils.h>

namespace NEgo {

	TBatchPolicyRandom::TBatchPolicyRandom(SPtr<IModel> model, const TSolverConfig& config)
		: IBatchPolicy(model, config)
		, NewBatch(false)
        , Sobol(model->GetDimSize())
	{
	}

    void TBatchPolicyRandom::InitNewBatch() {
    	NewBatch = true;
    }

    TVectorD TBatchPolicyRandom::GetNextElementInBatch() {
    	if (NewBatch) {
    		NewBatch = false;
    		return OptimizeAcquisition(Model, Config.AcqOpt, Sobol).first;
    	}
    	return NLa::UnifVec(Model->GetDimSize());
    }

} // namespace NEgo
