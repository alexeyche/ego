#include "predictive.h"


#include <ego/solver/config.h>
#include <ego/solver/utils.h>

namespace NEgo {

	TBatchPolicyPredictive::TBatchPolicyPredictive(SPtr<IModel> model, const TSolverConfig& config)
		: IBatchPolicy(model, config)
	{
	}

    void TBatchPolicyPredictive::InitNewBatch() {
    	AccModel = Model->Copy();
        LastPoint = TOptional<TVectorD>();
    }

    TVectorD TBatchPolicyPredictive::GetNextElementInBatch() {
        ENSURE(AccModel, "Batch policy was not initialized");

        if (LastPoint) {
            SPtr<IDistr> pointDistr = AccModel->GetPointPrediction(*LastPoint);
            L_DEBUG << "Predicted " << pointDistr->GetMean();
            AccModel->AddPoint(*LastPoint, pointDistr->GetMean());
            NOpt::OptimizeModelLogLik(AccModel, AccModel->GetParameters(), Config.HyperOpt);
            AccModel->Update();
        }
    	TPair<TVectorD, double> opt = OptimizeAcquisition(AccModel, Config.AcqOpt);
        LastPoint = opt.first;
        return opt.first;
    }

} // namespace NEgo
