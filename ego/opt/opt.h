#pragma once

#include "impl.h"

namespace NEgo {
	namespace NOpt {

		enum EMethod {
			CG = 0,
			RPROP,
			GN_DIRECT ,
			GN_DIRECT_L,
			GN_DIRECT_L_RAND,
			GN_DIRECT_NOSCAL,
			GN_DIRECT_L_NOSCAL,
			GN_DIRECT_L_RAND_NOSCAL,
			GN_ORIG_DIRECT,
			GN_ORIG_DIRECT_L,
			GD_STOGO,
			GD_STOGO_RAND,
			LD_LBFGS_NOCEDAL,
			LD_LBFGS,
			LN_PRAXIS,
			LD_VAR1,
			LD_VAR2,
			LD_TNEWTON,
			LD_TNEWTON_RESTART,
			LD_TNEWTON_PRECOND,
			LD_TNEWTON_PRECOND_RESTART,
			GN_CRS2_LM,
			GN_MLSL,
			GD_MLSL,
			GN_MLSL_LDS,
			GD_MLSL_LDS,
			LD_MMA,
			LN_COBYLA,
			LN_NEWUOA,
			LN_NEWUOA_BOUND,
			LN_NELDERMEAD,
			LN_SBPLX,
			LN_AUGLAG,
			LD_AUGLAG,
			LN_AUGLAG_EQ,
			LD_AUGLAG_EQ,
			LN_BOBYQA,
			GN_ISRES,
			AUGLAG,
			AUGLAG_EQ,
			G_MLSL,
			G_MLSL_LDS,
			LD_SLSQP,
			LD_CCSAQ,
			GN_ESCH
		};

		EMethod MethodFromString(TString s);

 		TString MethodToString(EMethod m);

 		void PrintMethods();


		TPair<TVectorD, double> OptimizeModelLogLik(TModel &self, EMethod optMethod, TOptimizeConfig = TOptimizeConfig());

		TPair<TVectorD, double> OptimizeAcquisitionFunction(SPtr<IAcq> acq, EMethod optMethod, TOptimizeConfig config = TOptimizeConfig());

	} // namespace NOpt
} // namespace NEgo