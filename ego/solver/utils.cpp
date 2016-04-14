#include "utils.h"

#include <future>

namespace NEgo {

	TPair<TVectorD, double> OptimizeAcquisition(SPtr<IModel> model, const TOptConfig& optConfig, TSobolGen& sobol) {
		TMatrixD starts = sobol.Sample(100);

        double bestAcqFun = std::numeric_limits<double>::max();
        TVectorD bestParams;

        for (size_t iter=0; iter < 100; iter +=10) {
            TVector<TPair<std::future<TPair<TVectorD, double>>, TVectorD>> results;
            for (size_t minNum=0; minNum < 10; ++minNum) {
                TVectorD start = NLa::Trans(starts.row(iter + minNum));

                results.push_back(std::make_pair(std::async(
                    std::launch::async,
                    [=]() {
                        try {
                            return NOpt::OptimizeAcquisitionFunction(*model.get(), start, optConfig);
                        } catch (const TErrAlgebraError& err) {
                            L_DEBUG << "Got algebra error, ignoring";
                            return MakePair(TVectorD(), std::numeric_limits<double>::max());
                        }
                    }
                ), start));
            }
        
            for (auto& f: results) {
                auto r = f.first.get();
                L_DEBUG << "Got result from starting at " << NLa::VecToStr(f.second) << " -> " << r.second << " at " << NLa::VecToStr(r.first);
                if (r.second < bestAcqFun) {
                    bestAcqFun = r.second;
                    bestParams = r.first;
                }
            }
        }
        ENSURE(bestParams.size() > 0, "Best optimization result is not selected");
        L_DEBUG << "Found best optimization result at " << NLa::VecToStr(bestParams) << " -> " << bestAcqFun;
        return MakePair(bestParams, bestAcqFun);
	}

    double Round(double dbVal, int nPlaces /* = 2 */) {
        const double dbShift = pow(10.0, nPlaces);
        return floor(dbVal * dbShift + 0.5) / dbShift; 
    }


} // namespace NEgo