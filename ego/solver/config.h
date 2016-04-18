#pragma once

#include <ego/base/base.h>
#include <ego/model/config.h>
#include <ego/problem/config.h>
#include <ego/protos/solver.pb.h>

#include <ego/util/serial/proto_serial.h>


namespace NEgo {

    struct TOptConfig: public IProtoSerial<NEgoProto::TOptConfig> {
        TOptConfig() {}
        TOptConfig(const TString& method)
            : Method(method)
        {}

        void SerialProcess(TProtoSerial& serial) {
            serial(Method);
            serial(MinimizersNum);
            serial(MaxEval);
            serial(Tolerance);
            serial(Verbose);
        }
    
        TString Method;
        ui32 MinimizersNum = 100;
        ui32 MaxEval = 100;
        double Tolerance = 1e-10;
        bool Verbose = false;
    };

    struct TSolverConfig: public IProtoSerial<NEgoProto::TSolverConfig> {
        void SerialProcess(TProtoSerial& serial) {
            serial(HyperOpt);
            serial(AcqOpt);
            serial(InitSampleSize);
            serial(BatchSize);
            serial(IterationsNum);
            serial(HyperOptFreq);
            serial(ModelType);
            serial(ModelConfig);
            serial(BatchPolicy);
        }


        TSolverConfig(const NEgoProto::TSolverConfig& config = NEgoProto::TSolverConfig())
            : HyperOpt("CG")
            , AcqOpt("LBFGSB")
        {
        }

        TOptConfig HyperOpt;
        TOptConfig AcqOpt;

        ui32 InitSampleSize = 10;
        ui32 BatchSize = 1;
        ui32 IterationsNum = 100;
        ui32 HyperOptFreq = 1;
        
        TString BatchPolicy = "bpPredictive";
        TString ModelType = "Model";

        TModelConfig ModelConfig;
    };


    struct TSolverSpec: public IProtoSerial<NEgoProto::TSolverSpec> {
        void SerialProcess(TProtoSerial& serial) {
            serial(SolverConfig);
            serial(ProblemConfig);
        }

        TSolverSpec(const NEgoProto::TSolverSpec& solverSpec) {
            Deserialize(solverSpec);
        }
        TSolverSpec() {}

        TSolverConfig SolverConfig;
        TProblemConfig ProblemConfig;
    };

} // namespace NEgo
