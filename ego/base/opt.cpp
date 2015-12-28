#include "opt.h"

#include <ego/model/model.h>

namespace NEgo {
	namespace NOpt {

		// std::function<double(const opt_column_vector&)> ModelCallback(std::function<double(const TModel&, const TVectorD&)> cb, const TModel &self) {
		// 	return [&](const opt_column_vector& col) -> double {
		// 		return cb(self, ConvertFromOptLib(col));
		// 	};
		// }

		// std::function<opt_column_vector(const opt_column_vector&)> ModelCallbackDeriv(std::function<TVectorD(const TModel&, const TVectorD&)> cb, const TModel &self) {
		// 	return [&](const opt_column_vector& col) -> opt_column_vector {
		// 		return ConvertToOptLib(cb(self, ConvertFromOptLib(col)));
		// 	};
		// }

		// opt_column_vector ConvertToOptLib(const TVectorD &egoVec) {
		// 	opt_column_vector v(egoVec.size());
		// 	for(size_t i=0; i<v.size(); ++i) {
		// 		v(i) = egoVec(i);
		// 	}
		// 	return v;
		// }

		// TVectorD ConvertFromOptLib(const opt_column_vector &optVec) {
		// 	TVectorD v(optVec.size());
		// 	for(size_t i=0; i<optVec.size(); ++i) {
		// 		v(i) = optVec(i);
		// 	}
		// 	return v;
		// }

		// TVectorD Minimize(
		// 	const TModel &self
		//   , std::function<double(const TModel&, const TVectorD&)> cb
		//   , std::function<TVectorD(const TModel&, const TVectorD&)> cbDeriv
		//   , const TVectorD &startPoint)
		// {
		// 	auto convCb = ModelCallback(cb, self);
		// 	auto convCbDeriv = ModelCallbackDeriv(cbDeriv, self);
		//     opt_column_vector starting_point = ConvertToOptLib(startPoint);
		//     dlib::find_min(
		//     	dlib::cg_search_strategy()
		//       , dlib::objective_delta_stop_strategy(1e-9)
		//       , convCb
		//       , convCbDeriv
		//       , starting_point
		//       , -1
		//     );
		//     return ConvertFromOptLib(starting_point);
		// }

		double NLoptMinimizer(const std::vector<double> &x, std::vector<double> &grad, void* f_data) {
			const TModel &model = *static_cast<const TModel*>(f_data);
			try {
				auto ans = model.GetNegativeLogLik(NLa::VecToStd(x));
				grad = NLa::VecToStd(ans.GetDerivative());
				double nLogLik = ans.GetValue();
				L_DEBUG << "Got negative log likelihood: " << nLogLik;
				return nLogLik;
			} catch(std::exception &e) {
				L_DEBUG << "Got error while calculating likelihood: " << e.what();
				throw;
			}
		}

		TVectorD Minimize(const TModel &model, TVectorD init) {
			nlopt::opt optAlg(
				nlopt::LD_MMA
			  , model.GetHyperParametersSize()
			);
			optAlg.set_min_objective(NLoptMinimizer, static_cast<void*>(const_cast<TModel*>(&model)));
			optAlg.set_ftol_rel(1e-12);
			// optAlg.set_maxeval(100);
			double best = 0.0;
			auto initStd = NLa::VecToStd(init);
			optAlg.optimize(initStd, best);
		    return NLa::StdToVec(initStd);
		}


	} // namespace NOpt
} // namespace NEgo