#pragma once

#include "batch.h"

#include <ego/model/model.h>
#include <ego/base/factory.h>
#include <ego/util/optional.h>
#include <ego/util/sobol.h>


namespace NEgo {

	class TBatchPolicyPredictive : public IBatchPolicy {
	public:
		TBatchPolicyPredictive(SPtr<IModel> model, const TSolverConfig& config);

        void InitNewBatch() override final;

        TVectorD GetNextElementInBatch() override final;

    private:
    	SPtr<IModel> AccModel;
    	TOptional<TVectorD> LastPoint;

    	TSobolGen Sobol;
	};

	REGISTER_BATCH_POLICY(TBatchPolicyPredictive);

} // namespace NEgo
