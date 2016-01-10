#include "impl.h"

#include <ego/model/model.h>

#include <map>


namespace NEgo {
	namespace NOpt {


		double NLoptModelMinimizer(const std::vector<double> &x, std::vector<double> &grad, void* f_data) {
			TModel &model = *static_cast<TModel*>(f_data);
			try {
				auto ans = model.GetNegativeLogLik(NLa::StdToVec(x));
				grad = NLa::VecToStd(ans.GetDerivative());
				double nLogLik = ans.GetValue();
				L_DEBUG << "Got negative log likelihood: " << nLogLik;
				return nLogLik;
			} catch(std::exception &e) {
				L_DEBUG << "Got error while calculating likelihood: " << e.what();
				throw;
			}
		}

		TPair<TVectorD, double> NLoptModelMinimize(TModel &model, TVectorD init, nlopt::algorithm algo, TOptimizeConfig config) {
			nlopt::opt optAlg(
				algo
			  , model.GetHyperParametersSize()
			);
			optAlg.set_min_objective(NLoptModelMinimizer, static_cast<void*>(const_cast<TModel*>(&model)));
			optAlg.set_ftol_rel(config.Tol);
			optAlg.set_maxeval(config.MaxEval);
			double best = std::numeric_limits<double>::max();
			auto initStd = NLa::VecToStd(init);
			optAlg.optimize(initStd, best);
			auto res = MakePair(NLa::StdToVec(initStd), optAlg.last_optimum_value());
		    model.SetHyperParameters(res.first);
			return res;
		}
		
		double NLoptAcqMinimizer(const std::vector<double> &x, std::vector<double> &grad, void* f_data) {
			try {
				IAcq* acq = static_cast<IAcq*>(f_data);
				return acq->EvaluateCriteria(NLa::StdToVec(x)).GetValue();	
			} catch(const std::exception& e) {
				L_ERROR << "Got errors while optimization: " << e.what();
				throw;
			}
			
		}

		TPair<TVectorD, double> NLoptAcqMinimize(SPtr<IAcq> acq, nlopt::algorithm algo, TOptimizeConfig config) {
			nlopt::opt optAlg(
				algo
			  , acq->GetDimSize()
			);
			optAlg.set_min_objective(NLoptAcqMinimizer, static_cast<void*>(acq.get()));
			optAlg.set_ftol_rel(config.Tol);
			optAlg.set_maxeval(config.MaxEval);
			optAlg.set_lower_bounds(0.0);
			optAlg.set_upper_bounds(1.0);
			double best = std::numeric_limits<double>::max();
			auto initStd = NLa::VecToStd(NLa::UnifVec(acq->GetDimSize()));
			optAlg.optimize(initStd, best);
			return MakePair(NLa::StdToVec(initStd), optAlg.last_optimum_value());
		}

	} // namespace NOpt
} // namespace NEgo