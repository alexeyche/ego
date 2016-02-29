#pragma once

#include <ego/base/factory.h>

#include "batch.h"

namespace NEgo {

	class TBatchPolicyRandom : public IBatchPolicy {
	public:
		TBatchPolicyRandom(SPtr<TModel> model, const TStrategyConfig& config);

        void InitNewBatch() override final;

        TVectorD GetNextElementInBatch() override final;

    private:
    	bool NewBatch;

	};

	REGISTER_BATCH_POLICY(TBatchPolicyRandom);

} // namespace NEgo
