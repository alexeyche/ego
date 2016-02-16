#pragma once

#include <ego/base/base.h>
#include <ego/protos/strategy.pb.h>


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

    struct TStrategyConfig {
    	TStrategyConfig() {}

        TStrategyConfig(const NEgoProto::TStrategyConfig& config)
            : ProtoConfig(config)
        {
            InitSamplesNum = ProtoConfig.initsamplesnum();
            BatchSize = ProtoConfig.batchsize();
            IterationsNum = ProtoConfig.iterationsnum();
            HyperOptFreq = ProtoConfig.hyperoptfreq();

            HyperOpt = TOptConfig(ProtoConfig.hyperopt());
            AcqOpt = TOptConfig(ProtoConfig.acqopt());
        }

        TOptConfig HyperOpt;
        TOptConfig AcqOpt;

        ui32 InitSamplesNum;
        ui32 BatchSize;
        ui32 IterationsNum;
        ui32 HyperOptFreq;

        NEgoProto::TStrategyConfig ProtoConfig;
    };


} // namespace NEgo
