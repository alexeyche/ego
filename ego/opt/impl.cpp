#include "impl.h"

#include <ego/model/model.h>

#include <map>


namespace NEgo {
	namespace NOpt {


		double NLoptModelMinimizer(const std::vector<double> &x, std::vector<double> &grad, void* f_data) {
			TModel &model = *static_cast<TModel*>(f_data);
			try {
				auto ans = model.GetNegativeLogLik(x);
				grad = ans.ParamDeriv();
				double nLogLik = ans.Value();
				L_DEBUG << "Got negative log likelihood: " << nLogLik;
				return nLogLik;
			} catch(std::exception &e) {
				L_DEBUG << "Got error while calculating likelihood: " << e.what();
				throw;
			}
		}

		TPair<TVectorD, double> NLoptModelMinimize(TModel &model, const TOptConfig& config) {
			nlopt::algorithm algo = static_cast<nlopt::algorithm>(static_cast<ui32>(MethodFromString(config.Method))-1);
			nlopt::opt optAlg(
				algo
			  , model.GetParametersSize()
			);
			optAlg.set_min_objective(NLoptModelMinimizer, static_cast<void*>(const_cast<TModel*>(&model)));
			optAlg.set_ftol_rel(config.Tolerance);
			optAlg.set_maxeval(config.MaxEval);
			double best = std::numeric_limits<double>::max();
			auto initStd = NLa::VecToStd(model.GetParameters());
			optAlg.optimize(initStd, best);
			auto res = MakePair(NLa::StdToVec(initStd), optAlg.last_optimum_value());
		    model.SetParameters(initStd);
			return res;
		}

		double NLoptAcqMinimizer(const std::vector<double>& x, std::vector<double>& grad, void* f_data) {
			try {
				IAcq* acq = static_cast<IAcq*>(f_data);
				auto res = acq->Calc(NLa::StdToVec(x));
				// grad = res.ArgDeriv();
				double val = res.Value();
				L_DEBUG << "Got acquisition function value : " << val;
				return val;
			} catch(const std::exception& e) {
				L_ERROR << "Got errors while optimization: " << e.what();
				throw;
			}

		}

		TPair<TVectorD, double> NLoptAcqMinimize(SPtr<IAcq> acq, const TOptConfig& config) {
			nlopt::algorithm algo = static_cast<nlopt::algorithm>(static_cast<ui32>(MethodFromString(config.Method))-1);
			nlopt::opt optAlg(
				algo
			  , acq->GetDimSize()
			);
			optAlg.set_min_objective(NLoptAcqMinimizer, static_cast<void*>(acq.get()));
			optAlg.set_ftol_rel(config.Tolerance);
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