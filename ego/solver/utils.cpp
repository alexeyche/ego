#include "utils.h"

#include <ego/util/sobol.h>

#include <future>

namespace NEgo {

	TPair<TVectorD, double> OptimizeAcquisition(SPtr<IModel> model, const TOptConfig& optConfig) {
		TMatrixD starts = GenerateSobolGrid(optConfig.MinimizersNum, model->GetDimSize());

        TVector<TPair<std::future<TPair<TVectorD, double>>, TVectorD>> results;
        for (size_t minNum=0; minNum < optConfig.MinimizersNum; ++minNum) {
            TVectorD start = NLa::Trans(starts.row(minNum));

            results.push_back(std::make_pair(std::async(
                std::launch::async,
                [=]() {
                    try {
                        return NOpt::OptimizeAcquisitionFunction(model, start, optConfig);
                    } catch (const TEgoAlgebraError& err) {
                        L_DEBUG << "Got algebra error, ignoring";
                        return MakePair(TVectorD(), std::numeric_limits<double>::max());
                    }
                }
            ), start));
        }
        double bestAcqFun = std::numeric_limits<double>::max();
        TVectorD bestParams;
        for (auto& f: results) {
            auto r = f.first.get();
            L_DEBUG << "Got result from starting at " << NLa::VecToStr(f.second) << " -> " << r.second << " at " << NLa::VecToStr(r.first);
            if (r.second < bestAcqFun) {
                bestAcqFun = r.second;
                bestParams = r.first;
            }
        }
        ENSURE(bestParams.size() > 0, "Best optimization result is not selected");
        L_DEBUG << "Found best optimization result at " << NLa::VecToStr(bestParams) << " -> " << bestAcqFun;
        return MakePair(bestParams, bestAcqFun);
	}

} // namespace NEgo