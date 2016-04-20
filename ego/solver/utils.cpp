#include "utils.h"

#include <future>

namespace NEgo {

	TPair<TVectorD, double> OptimizeAcquisition(SPtr<IModel> model, const TOptConfig& optConfig, TSobolGen& sobol) {
		const TMatrixD x = model->GetX();
        
        ui32 searchTryLeft = 5;
        while (searchTryLeft>0) {
            TMatrixD starts = sobol.Sample(100);

            double bestAcqFun = std::numeric_limits<double>::max();
            TVectorD bestX;

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
                                L_DEBUG << "Got algebra error, ignoring: " << err.what();
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
                        bestX = r.first;
                    }
                }
            }
            ENSURE(bestX.size() > 0, "Best optimization result is not selected");
            L_DEBUG << "Found best optimization result at " << NLa::VecToStr(bestX) << " -> " << bestAcqFun;
            
            bool foundSame = false;
            for (ui32 ri=0; ri < x.n_rows; ++ri) {
                TVectorD diff = NLa::Abs(bestX - NLa::Trans(x.row(ri)));
                if (NLa::All(diff < 1e-10)) {
                    foundSame = true;
                    break;
                }
            }

            if (foundSame) {
                L_DEBUG << "Got same point as in past. Enhancing global search and trying again";
                searchTryLeft--;
                model->EnhanceGlobalSearch();
                continue;
            }
            return MakePair(bestX, bestAcqFun);
        }
        throw TErrAlgebraError() << "Failed to find good point";
	}

    double Round(double dbVal, int nPlaces /* = 2 */) {
        const double dbShift = pow(10.0, nPlaces);
        return floor(dbVal * dbShift + 0.5) / dbShift; 
    }


} // namespace NEgo