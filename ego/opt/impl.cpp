#include "impl.h"

#include <ego/model/model.h>

#include <ego/contrib/cppoptlib/solver/conjugatedgradientdescentsolver.h>
#include <ego/contrib/cppoptlib/solver/bfgssolver.h>
#include <ego/contrib/cppoptlib/solver/lbfgssolver.h>
#include <ego/contrib/cppoptlib/solver/lbfgsbsolver.h>

#include <map>


namespace NEgo {
	namespace NOpt {


		TPair<TVectorD, double> CppOptLibMinimize(EMethod method, const TVectorD& start, TOptLibCallback cb, TOptional<TPair<TVectorD, TVectorD>> bounds) {
			switch(method) {
				case CG:
					throw TEgoException() << "Method is not in cppoptlib";
				case CG_OPTLIB:
					return CppOptLibMinimize<cppoptlib::ConjugatedGradientDescentSolver<double>>(start, cb);
				case BFGS:
					return CppOptLibMinimize<cppoptlib::BfgsSolver<double>>(start, cb);
				case LBFGS:
					return CppOptLibMinimize<cppoptlib::LbfgsSolver<double>>(start, cb);
				case LBFGSB:
					return CppOptLibMinimize<cppoptlib::LbfgsbSolver<double>>(start, cb, bounds);
				default:
					throw TEgoException() << "Method is not implemented";
			}
		}

	} // namespace NOpt
} // namespace NEgo