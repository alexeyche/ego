#include "opt.h"

#include <ego/model/model.h>

namespace NEgo {
	namespace NOpt {

		std::function<double(const opt_column_vector&)> ModelCallback(std::function<double(const TModel&, const TVectorD&)> cb, const TModel &self) {
			return [&](const opt_column_vector& col) -> double {
				return cb(self, ConvertFromOptLib(col));
			};
		}

		std::function<opt_column_vector(const opt_column_vector&)> ModelCallbackDeriv(std::function<TVectorD(const TModel&, const TVectorD&)> cb, const TModel &self) {
			return [&](const opt_column_vector& col) -> opt_column_vector {
				return ConvertToOptLib(cb(self, ConvertFromOptLib(col)));
			};
		}

		opt_column_vector ConvertToOptLib(const TVectorD &egoVec) {
			opt_column_vector v(egoVec.size());
			for(size_t i=0; i<v.size(); ++i) {
				v(i) = egoVec(i);
			}
			return v;
		}

		TVectorD ConvertFromOptLib(const opt_column_vector &optVec) {
			TVectorD v(optVec.size());
			for(size_t i=0; i<optVec.size(); ++i) {
				v(i) = optVec(i);
			}
			return v;
		}

		TVectorD Minimize(
			const TModel &self
		  , std::function<double(const TModel&, const TVectorD&)> cb
		  , std::function<TVectorD(const TModel&, const TVectorD&)> cbDeriv
		  , const TVectorD &startPoint) 
		{
			auto convCb = ModelCallback(cb, self);
			auto convCbDeriv = ModelCallbackDeriv(cbDeriv, self);
		    opt_column_vector starting_point = ConvertToOptLib(startPoint);
		    dlib::find_min(
		    	dlib::cg_search_strategy()
		      , dlib::objective_delta_stop_strategy(1e-9)
		      , convCb
		      , convCbDeriv
		      , starting_point
		      , -1
		    );
		    return ConvertFromOptLib(starting_point);
		}

	} // namespace NOpt
} // namespace NEgo