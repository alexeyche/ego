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

            Input = config.input();
            Output = config.output();
            Cov = config.cov();
        }

        TString Input;
        TString Output;
        TString Cov;
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

            Covariance = Factory.CreateCov(Config.Cov, X.n_cols);

            TVectorD hypParams(X.n_cols+1);
            hypParams.fill(1.0);
            Covariance->SetHyperParameters(hypParams);
            NLa::WriteCsv(Covariance->CalculateKernel(X, X/2.0), Config.Output);
        }

    private:
        TMatrixD X;
        TVectorD Y;

        UPtr<ICov> Covariance;
        UPtr<IMean> Mean;

        TModelConfig Config;
    };

} // namespace NEgo


