#pragma once

#include <ego/base/factory.h>
#include <ego/base/entities.h>
#include <ego/base/la.h>
#include <ego/base/opt.h>
#include <ego/protos/config.pb.h>

#include <ego/util/log/log.h>

#include <ego/distr/distr.h>

#include <ctime>

namespace NEgo {

    struct TModelConfig {
        TModelConfig() {}

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
            if(config.has_seed()) {
                Seed = config.seed();
            } else {
                Seed = std::time(0);
            }
        }

        TString Input;
        TString Output;
        TString Cov;
        TString Mean;
        TString Lik;
        TString Inf;

        ui32 Seed;
    };

    class TModel {
    public:
        TModel(TModelConfig config);
        TModel();

        TInfValue GetNegativeLogLik(const TVectorD& v) const;

        size_t GetDimSize() const;

        size_t GetHyperParametersSize() const;

        TDistrVec GetPrediction(const TMatrixD &Xnew);

        void SetModel(SPtr<IMean> mean, SPtr<ICov> cov, SPtr<ILik> lik, SPtr<IInf> inf);
        void SetData(TMatrixD x, TVectorD y);
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


