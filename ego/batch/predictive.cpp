#include "predictive.h"

#include <ego/model/model.h>
#include <ego/strategy/strategy.h>
#include <ego/strategy/strategy_funcs.h>
#include <ego/strategy/config.h>

namespace NEgo {

	TBatchPolicyPredictive::TBatchPolicyPredictive(SPtr<TModel> model, const TStrategyConfig& config)
		: IBatchPolicy(model, config)
	{
	}

    void TBatchPolicyPredictive::InitNewBatch() {
    	AccModel = MakeShared(new TModel(*Model));
        LastPoint = TOptional<TVectorD>();
    }

    TVectorD TBatchPolicyPredictive::GetNextElementInBatch() {
        ENSURE(AccModel, "Batch policy was not initialized");

        if (LastPoint) {
            SPtr<IDistr> pointDistr = AccModel->GetPointPrediction(*LastPoint);
            L_DEBUG << "Predicted " << pointDistr->GetMean();
            AccModel->AddPoint(*LastPoint, pointDistr->GetMean());
            NOpt::OptimizeModelLogLik(*AccModel, AccModel->GetParameters(), Config.HyperOpt);
            AccModel->Update();
        }
    	TPair<TVectorD, double> opt = OptimizeAcquisition(*AccModel, Config.AcqOpt);
        LastPoint = opt.first;
        return opt.first;
    }

} // namespace NEgo
