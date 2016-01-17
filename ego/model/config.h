#pragma once

#include <ego/base/base.h>
#include <ego/base/errors.h>
#include <ego/protos/config.pb.h>

#include <ctime>


namespace NEgo {


    struct TModelConfig {
        TModelConfig()
            : Seed(std::time(0))
            , AcqOptMethod("GN_DIRECT")
            , HypOptMethod("CG")
            , MaxEval(20)
            , HypOptMaxEval(20)
            , HypOptFreq(1)
        {
        }

        TModelConfig(const NEgoProto::TModelConfig &config) {
            ENSURE(config.has_input() && config.has_output(), "Need input and output data");
            ENSURE(config.has_cov(), "Need specification of covariance in model config");
            ENSURE(config.has_mean(), "Need specification of mean in model config");
            ENSURE(config.has_lik(), "Need specification of likelihood in model config");
            ENSURE(config.has_inf(), "Need specification of inference method in config");
            ENSURE(config.has_acq(), "Need specification of acquisition function in config");

            Input = config.input();
            Output = config.output();

            Cov = config.cov();
            Mean = config.mean();
            Lik = config.lik();
            Inf = config.inf();
            Acq = config.acq();
            AcqOptMethod = config.acqopt();
            HypOptMethod = config.opt();

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
        TString Acq;

        TString AcqOptMethod;
        TString HypOptMethod;
        ui32 HypOptMaxEval;
        ui32 HypOptFreq;
        ui32 Seed;

        ui32 MaxEval;
    };

} // namespace NEgo
