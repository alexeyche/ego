#pragma once

#include <ego/base/base.h>
#include <ego/base/errors.h>
#include <ego/protos/config.pb.h>

#include <ctime>


namespace NEgo {



    struct TOptConfig {
        TOptConfig() {}

        TOptConfig(const NEgoProto::TOptConfig& config) {
            Method = config.method();
            MinimizersNum = config.minimizersnum();
            MaxEval = config.maxeval();
            Tolerance = config.tolerance();
        }

        TString Method;
        ui32 MinimizersNum;
        ui32 MaxEval;
        double Tolerance;
    };


    struct TModelConfig {
        TModelConfig()
            : Seed(std::time(0))            
        {
        }

        TModelConfig(const NEgoProto::TModelConfig &config) {
            Cov = config.cov();
            Mean = config.mean();
            Lik = config.lik();
            Inf = config.inf();
            Acq = config.acq();
            IterationsNum = config.iterationsnum();
            HyperOptFreq = config.hyperoptfreq();

            if(config.has_seed()) {
                Seed = config.seed();
            } else {
                Seed = std::time(0);
            }
            HyperOpt = TOptConfig(config.hyperopt());
            AcqOpt = TOptConfig(config.acqopt());
        }

        TString Cov;
        TString Mean;
        TString Lik;
        TString Inf;
        TString Acq;

        ui32 Seed;

        TOptConfig HyperOpt;
        TOptConfig AcqOpt;

        ui32 IterationsNum;
        ui32 HyperOptFreq;
    };

} // namespace NEgo
