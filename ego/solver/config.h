#pragma once

#include <ego/base/base.h>
#include <ego/model/config.h>
#include <ego/problem/config.h>
#include <ego/protos/solver.pb.h>


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

    struct TSolverConfig {
        TSolverConfig(const NEgoProto::TSolverConfig& config = NEgoProto::TSolverConfig())
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
            ModelType = ProtoConfig.modeltype();

            ModelConfig = TModelConfig(ProtoConfig.modelconfig());
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
        TString ModelType;

        TModelConfig ModelConfig;

        NEgoProto::TSolverConfig ProtoConfig;
    };


    struct TSolverSpec {
        TSolverSpec(const NEgoProto::TSolverSpec& solverSpec) {
            SolverConfig = TSolverConfig(solverSpec.solverconfig());
            ProblemConfig = TProblemConfig(solverSpec.problemconfig());
        }


        TSolverConfig SolverConfig;
        TProblemConfig ProblemConfig;
    };

} // namespace NEgo
