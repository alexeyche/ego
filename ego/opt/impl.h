#pragma once

#include "cg.h"

#include <ego/base/la.h>
#include <ego/util/log/log.h>
#include <ego/util/optional.h>

#include <ego/solver/config.h>

#include <functional>

#include <ego/contrib/cppoptlib/problem.h>

namespace NEgo {
	class IModel;
	class IAcq;

	namespace NOpt {

		enum EMethod {
			CG = 0,
			CG_OPTLIB = 1,
			BFGS = 2,
			LBFGS = 3,
			LBFGSB = 4
		};


		using TOptLibCallback = std::function<double(const TVectorD&, TVectorD&)>;

		class TProblem : public cppoptlib::Problem<double> {
		public:
			TProblem(TOptLibCallback cb, bool verbose)
				: Cb(cb)
				, Verbose(verbose)
			{
			}

		    double value(const TVectorD &beta) {
		    	TVectorD gr;
		    	double v = Cb(beta, gr);
		    	if (Verbose) {
		    		L_DEBUG << "Got value: " << v;	
		    	}
		    	return v;
		    }

		    void gradient(const TVectorD& beta, TVectorD& grad) {
		        grad.set_size(beta.size());
		        Cb(beta, grad);
		    }

		private:
			TOptLibCallback Cb;
			bool Verbose;
		};

		TOptional<TPair<TVectorD, TVectorD>> NoBounds();

		TPair<TVectorD, double> CppOptLibMinimize(EMethod method, const TVectorD& start, TOptLibCallback cb, TOptional<TPair<TVectorD, TVectorD>> bounds = NoBounds(), bool verbose = false);

		template <typename TSolver>
		TPair<TVectorD, double> CppOptLibMinimize(const TVectorD& start, TOptLibCallback cb, TOptional<TPair<TVectorD, TVectorD>> bounds = NoBounds(), bool verbose = false) {
			TProblem prob(cb, verbose);
			if (bounds) {
				prob.setLowerBound(bounds->first);
				prob.setUpperBound(bounds->second);
			}
			TSolver solver;
			TVectorD x = start;
			solver.minimize(prob, x);
			return MakePair(x, prob(x));
		}

	} // namespace NOpt
} // namespace NEgo