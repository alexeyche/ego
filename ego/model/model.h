#pragma once

#include <ego/base/factory.h>
#include <ego/base/entities.h>
#include <ego/base/la.h>
#include <ego/base/opt.h>
#include <ego/protos/config.pb.h>

#include <ego/util/log/log.h>
#include <ego/util/parse_parenthesis.h>

namespace NEgo {

    struct TModelConfig {
        TModelConfig(const NEgoProto::TModelConfig &config) {
            ENSURE(config.has_input() && config.has_output(), "Need input and output data");
            ENSURE(config.has_cov(), "Need specification of covariance in model config");
            ENSURE(config.has_mean(), "Need specification of mean in model config");
            ENSURE(config.has_lik(), "Need specification of likelihood in model config");
            ENSURE(config.has_inf(), "Need specification of inference method in config");

            Input = config.input();
            Output = config.output();
            Cov = config.cov();
            Mean = config.mean();
            Lik = config.lik();
            Inf = config.inf();
        }

        TString Input;
        TString Output;
        TString Cov;
        TString Mean;
        TString Lik;
        TString Inf;
    };

    class TModel {
    public:
        TModel(TModelConfig config)
            : Config(config)
        {
            TMatrixD inputData = NLa::ReadCsv(Config.Input);
            X = NLa::HeadCols(inputData, inputData.n_cols-1);
            Y = NLa::TailCols(inputData, 1);
            L_DEBUG << "Got input values with size [" << X.n_rows << "x" << X.n_cols << "] and " << " target values with size [" << Y.n_rows << "x" << Y.n_cols << "]";

            size_t D = X.n_cols;
            
            Mean = ParseParenthesis<SPtr<IMean>>(
                Config.Mean, 
                [&](TString childName) -> SPtr<IMean> {
                    L_DEBUG << "Creating mean function \"" << childName << "\"";
                    return Factory.CreateMean(childName, D); 
                }, 
                [&](TString parentName, TVector<SPtr<IMean>> childs) -> SPtr<IMean> {
                    L_DEBUG << "Creating composite mean function \"" << parentName << "\", with childs size " << childs.size();
                    return Factory.CreateCompMean(parentName, childs);
                }
            );

            Cov = Factory.CreateCov(Config.Cov, D);
            Lik = Factory.CreateLik(Config.Lik, D);
            Inf = Factory.CreateInf(Config.Inf, Mean, Cov, Lik);
            size_t hypSize = Mean->GetHyperParametersSize() + Cov->GetHyperParametersSize() + Lik->GetHyperParametersSize();

            TVectorD best = NOpt::Minimize(
                *this, 
                [&](const TModel& self, const TVectorD &v) -> double {
                    double val = self.GetNegativeLogLik(v).GetValue();
                    L_DEBUG << "Got negative log likelihood value: " << val;
                    return val;
                }, 
                [&](const TModel& self, const TVectorD &v) -> TVectorD {
                    return self.GetNegativeLogLik(v).GetDerivative();
                },
                NLa::UnifVec(hypSize)
            );
            NLa::Print(best);
        }

        TInfRet GetNegativeLogLik(const TVectorD& v) const {
            size_t hypSizeIdx = 0;
            NLa::Print(v);
            Mean->SetHyperParameters(NLa::SubVec(v, hypSizeIdx, hypSizeIdx + Mean->GetHyperParametersSize()));
            hypSizeIdx += Mean->GetHyperParametersSize();

            Cov->SetHyperParameters(NLa::SubVec(v, hypSizeIdx, hypSizeIdx + Cov->GetHyperParametersSize()));
            hypSizeIdx += Cov->GetHyperParametersSize();
            
            Lik->SetHyperParameters(NLa::SubVec(v, hypSizeIdx, hypSizeIdx + Lik->GetHyperParametersSize()));
            hypSizeIdx += Lik->GetHyperParametersSize();

            auto infRes = Inf->CalculateNegativeLogLik(X, Y);
            return infRes;
        }
    private:
        TMatrixD X;
        TVectorD Y;

        SPtr<IMean> Mean;
        SPtr<ICov> Cov;
        SPtr<ILik> Lik;
        SPtr<IInf> Inf;

        TModelConfig Config;
    };

} // namespace NEgo


