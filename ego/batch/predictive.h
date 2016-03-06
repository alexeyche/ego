#pragma once

#include "batch.h"

#include <ego/model/model.h>
#include <ego/base/factory.h>
#include <ego/util/optional.h>


namespace NEgo {

	class TBatchPolicyPredictive : public IBatchPolicy {
	public:
		TBatchPolicyPredictive(SPtr<IModel> model, const TSolverConfig& config);

        void InitNewBatch() override final;

        TVectorD GetNextElementInBatch() override final;

    private:
    	SPtr<IModel> AccModel;
    	TOptional<TVectorD> LastPoint;
	};

	REGISTER_BATCH_POLICY(TBatchPolicyPredictive);

} // namespace NEgo
