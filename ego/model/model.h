#pragma once

#include <ego/base/factory.h>
#include <ego/base/entities.h>
#include <ego/base/la.h>
#include <ego/protos/config.pb.h>

#include <ego/util/log/log.h>

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
            
            Mean = Factory.CreateMean(Config.Mean, D);
            Cov = Factory.CreateCov(Config.Cov, D);
            Lik = Factory.CreateLik(Config.Lik, D);
            Inf = Factory.CreateInf(Config.Inf, Mean, Cov, Lik);

            TVectorD meanHyps(X.n_cols);
            meanHyps.fill(1.0);
            Mean->SetHyperParameters(meanHyps);

            TVectorD hypParams(X.n_cols+1);
            hypParams.fill(1.0);
            Cov->SetHyperParameters(hypParams);

            TVectorD v(1);
            v.fill(1.0);
            Lik->SetHyperParameters(v);

            L_DEBUG << "Negative log likehood: " << Inf->NegativeLogLik(X, Y);
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


