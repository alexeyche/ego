#pragma once

#include "batch.h"

#include <ego/base/factory.h>
#include <ego/util/optional.h>


namespace NEgo {

	class TBatchPolicyPredictive : public IBatchPolicy {
	public:
		TBatchPolicyPredictive(SPtr<TModel> model, const TStrategyConfig& config);

        void InitNewBatch() override final;

        TVectorD GetNextElementInBatch() override final;

    private:
    	SPtr<TModel> AccModel;
    	TOptional<TVectorD> LastPoint;
	};

	REGISTER_BATCH_POLICY(TBatchPolicyPredictive);

} // namespace NEgo
