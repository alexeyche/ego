#include "opt.h"

#include <ego/model/base_model.h>

namespace NEgo {
	namespace NOpt {
		
 		std::map<TString, EMethod> OptMethodMap = {
 			{"CG", CG},
			{"CG_OPTLIB", CG_OPTLIB},
			{"BFGS", BFGS},
			{"LBFGS", LBFGS},
			{"LBFGSB", LBFGSB}
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


		TPair<TVector<double>, double> OptimizeModelLogLik(IModel& model, const TVector<double>& start, const TOptConfig& config) {
			L_DEBUG << "Going to minimize model log likelihood with " << config.Method;
			EMethod method = MethodFromString(config.Method);
			switch(method) {
				case CG:
					{
						auto res = CgMinimize(
					        start,
					        [&] (const TVectorD &x) -> TPair<double, TVectorD> {
					            auto res = model.GetNegativeLogLik(NLa::VecToStd(x));
					            double val = res.Value();
					            if (config.Verbose) {
					            	L_DEBUG << "Got value: " << val;	
					            }
					            return MakePair(val, NLa::StdToVec(res.ParamDeriv()));
					        },
					        TCgMinimizeConfig(config)
					    );
					    TVector<double> par = NLa::VecToStd(res.first);
					    model.SetParameters(par);
					    return MakePair(par, res.second);
					}
				default:
					{
						auto res = CppOptLibMinimize(
							method,
							start,
							[&] (const TVectorD& x, TVectorD& grad) -> double {
					            auto res = model.GetNegativeLogLik(NLa::VecToStd(x));
					            grad = NLa::StdToVec(res.ParamDeriv());
					            return res.Value();
					        },
					        NoBounds(),
					        config.Verbose
						);
						TVector<double> par = NLa::VecToStd(res.first);
					    model.SetParameters(par);
					    return MakePair(par, res.second);
					}
			}
		}

		TPair<TVectorD, double> OptimizeAcquisitionFunction(IModel& model, const TVectorD& start, const TOptConfig& config) {
			switch(MethodFromString(config.Method)) {
				case CG:
				case CG_OPTLIB:
				case BFGS:
				case LBFGS:
					{
						throw TErrException() << "Can't use unconstrained method for optimization";
					}
				case LBFGSB:
					{
						return CppOptLibMinimize(
							LBFGSB,
							start,
							[&] (const TVectorD& x, TVectorD& grad) -> double {
								auto res = model.CalcCriterion(x);
								
								double val = res.Value();
								for (ui32 index=0; index < grad.size(); ++index) {
									grad(index) = res.ArgPartialDeriv(index);
								}
								return val;
					        },
					        MakePair(NLa::Zeros(model.GetDimSize()), NLa::Ones(model.GetDimSize())),
					        config.Verbose
						);
					}
			}
		}

	} // namespace NOpt
} // namespace NEgo

