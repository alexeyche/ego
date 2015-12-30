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
        TModel(TModelConfig config);

        TInfValue GetNegativeLogLik(const TVectorD& v) const;

        size_t GetDimSize() const;

        size_t GetHyperParametersSize() const;

        TPredictiveDistribution GetPrediction(const TMatrixD &Xnew);

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


