#include "opt.h"

#include <ego/model/model.h>

namespace NEgo {
	namespace NOpt {

 		std::map<TString, EMethod> OptMethodMap = {
 			{"CG", CG},
 		  	{"GN_DIRECT", GN_DIRECT},
			{"GN_DIRECT_L", GN_DIRECT_L},
			{"GN_DIRECT_L_RAND", GN_DIRECT_L_RAND},
			{"GN_DIRECT_NOSCAL", GN_DIRECT_NOSCAL},
			{"GN_DIRECT_L_NOSCAL", GN_DIRECT_L_NOSCAL},
			{"GN_DIRECT_L_RAND_NOSCAL", GN_DIRECT_L_RAND_NOSCAL},
			{"GN_ORIG_DIRECT", GN_ORIG_DIRECT},
			{"GN_ORIG_DIRECT_L", GN_ORIG_DIRECT_L},
			{"GD_STOGO", GD_STOGO},
			{"GD_STOGO_RAND", GD_STOGO_RAND},
			{"LD_LBFGS_NOCEDAL", LD_LBFGS_NOCEDAL},
			{"LD_LBFGS", LD_LBFGS},
			{"LN_PRAXIS", LN_PRAXIS},
			{"LD_VAR1", LD_VAR1},
			{"LD_VAR2", LD_VAR2},
			{"LD_TNEWTON", LD_TNEWTON},
			{"LD_TNEWTON_RESTART", LD_TNEWTON_RESTART},
			{"LD_TNEWTON_PRECOND", LD_TNEWTON_PRECOND},
			{"LD_TNEWTON_PRECOND_RESTART", LD_TNEWTON_PRECOND_RESTART},
			{"GN_CRS2_LM", GN_CRS2_LM},
			{"GN_MLSL", GN_MLSL},
			{"GD_MLSL", GD_MLSL},
			{"GN_MLSL_LDS", GN_MLSL_LDS},
			{"GD_MLSL_LDS", GD_MLSL_LDS},
			{"LD_MMA", LD_MMA},
			{"LN_COBYLA", LN_COBYLA},
			{"LN_NEWUOA", LN_NEWUOA},
			{"LN_NEWUOA_BOUND", LN_NEWUOA_BOUND},
			{"LN_NELDERMEAD", LN_NELDERMEAD},
			{"LN_SBPLX", LN_SBPLX},
			{"LN_AUGLAG", LN_AUGLAG},
			{"LD_AUGLAG", LD_AUGLAG},
			{"LN_AUGLAG_EQ", LN_AUGLAG_EQ},
			{"LD_AUGLAG_EQ", LD_AUGLAG_EQ},
			{"LN_BOBYQA", LN_BOBYQA},
			{"GN_ISRES", GN_ISRES},
			{"AUGLAG", AUGLAG},
			{"AUGLAG_EQ", AUGLAG_EQ},
			{"G_MLSL", G_MLSL},
			{"G_MLSL_LDS", G_MLSL_LDS},
			{"LD_SLSQP", LD_SLSQP},
			{"LD_CCSAQ", LD_CCSAQ},
			{"GN_ESCH", GN_ESCH}
 		};
 		
 		EMethod MethodFromString(TString s) {
 			auto ptrOpt = OptMethodMap.find(s);
 			ENSURE(ptrOpt != OptMethodMap.end(), "Can't find method " << s);
 			return ptrOpt->second;
 		}
 		
 		TString MethodToString(EMethod m) {
 			static std::map<EMethod, TString> optMethodMapRev;
 			for(const auto& p: OptMethodMap) {
 				optMethodMapRev.insert(MakePair(p.second, p.first));
 			}
 			auto ptrOpt = optMethodMapRev.find(m);
 			ENSURE(ptrOpt != optMethodMapRev.end(), "Can't find method " << m);
 			return ptrOpt->second;
 		}

 		void PrintMethods() {
 			for(const auto& p: OptMethodMap) {
 				std::cout << p.first << "\n"; 
 			}
 		}


		TPair<TVectorD, double> OptimizeModelLogLik(TModel &model, EMethod optMethod, TOptimizeConfig config) {
			L_DEBUG << "Going to minimize model log likelihood with " << MethodToString(optMethod); 
			switch(optMethod) {
				case CG:
					{
						auto res = CgMinimize(
					        model.GetHyperParameters(),
					        [&] (const TVectorD &x) -> TPair<double, TVectorD> {
					            auto res = model.GetNegativeLogLik(NLa::VecToStd(x));
					            return MakePair(res.Value(), NLa::StdToVec(res.ParamDeriv()));
					        },
					        TCgMinimizeConfig(config)
					    );
					    model.SetHyperParameters(NLa::VecToStd(res.first));
					    return res;
					}
				default:
					{
						nlopt::algorithm algo = static_cast<nlopt::algorithm>(static_cast<ui32>(optMethod)-1);
						return NLoptModelMinimize(model, model.GetHyperParameters(), algo, config);
					}
			}
		}

		TPair<TVectorD, double> OptimizeAcquisitionFunction(SPtr<IAcq> acq, EMethod optMethod, TOptimizeConfig config) {
			switch(optMethod) {
				case CG:
					throw TEgoException() << "Need to use derivative free methods\n";
				default:
					{
						nlopt::algorithm algo = static_cast<nlopt::algorithm>(static_cast<ui32>(optMethod)-1);
						return NLoptAcqMinimize(acq, algo, config);
					}
			}	
		}

	} // namespace NOpt
} // namespace NEgo