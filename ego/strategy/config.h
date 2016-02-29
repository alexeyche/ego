#pragma once

#include <ego/base/base.h>
#include <ego/protos/strategy.pb.h>


namespace NEgo {

    struct TOptConfig {
        TOptConfig() {}

        TOptConfig(const NEgoProto::TOptConfig& config, TString defaultMethod) {
            if (config.has_method()) {
                Method = config.method();    
            } else {
                Method = defaultMethod;
            }
            
            MinimizersNum = config.minimizersnum();
            MaxEval = config.maxeval();
            Tolerance = config.tolerance();
            Verbose = config.verbose();
        }

        TString Method;
        ui32 MinimizersNum;
        ui32 MaxEval;
        double Tolerance;
        bool Verbose;
    };

    struct TStrategyConfig {
        TStrategyConfig(const NEgoProto::TStrategyConfig& config = NEgoProto::TStrategyConfig())
            : ProtoConfig(config)
        {
            InitSampleSize = ProtoConfig.initsamplesize();
            BatchSize = ProtoConfig.batchsize();
            IterationsNum = ProtoConfig.iterationsnum();
            HyperOptFreq = ProtoConfig.hyperoptfreq();

            HyperLowerBound = ProtoConfig.hyperlowerbound();
            HyperUpperBound = ProtoConfig.hyperupperbound();

            HyperOpt = TOptConfig(ProtoConfig.hyperopt(), "LBFGS");
            AcqOpt = TOptConfig(ProtoConfig.acqopt(), "LBFGSB");

            BatchPolicy = ProtoConfig.batchpolicy();
        }

        TOptConfig HyperOpt;
        TOptConfig AcqOpt;

        ui32 InitSampleSize;
        ui32 BatchSize;
        ui32 IterationsNum;
        ui32 HyperOptFreq;
        double HyperLowerBound;
        double HyperUpperBound;

        TString BatchPolicy;

        NEgoProto::TStrategyConfig ProtoConfig;
    };


} // namespace NEgo
