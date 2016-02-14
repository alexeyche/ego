#pragma once

#include <ego/base/base.h>
#include <ego/base/errors.h>
#include <ego/protos/model.pb.h>

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
        TModelConfig(NEgoProto::TModelConfig config = NEgoProto::TModelConfig())
            : ProtoConfig(config)
        {
            Cov = ProtoConfig.cov();
            Mean = ProtoConfig.mean();
            Lik = ProtoConfig.lik();
            Inf = ProtoConfig.inf();
            Acq = ProtoConfig.acq();
            IterationsNum = ProtoConfig.iterationsnum();
            HyperOptFreq = ProtoConfig.hyperoptfreq();

            if(ProtoConfig.has_seed() || ProtoConfig.seed()>0) {
                Seed = ProtoConfig.seed();
            } else {
                Seed = std::time(0);
            }
            HyperOpt = TOptConfig(ProtoConfig.hyperopt());
            AcqOpt = TOptConfig(ProtoConfig.acqopt());
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

        NEgoProto::TModelConfig ProtoConfig;
    };

} // namespace NEgo
