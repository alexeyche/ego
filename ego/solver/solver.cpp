#include "solver.h"    

#include <ego/util/protobuf.h>
#include <ego/util/log/log.h>
#include <ego/util/sobol.h>


namespace NEgo {


    TSolver::TSolver(const TFsPath srcFile) {
        LoadState(srcFile);
    }


    void TSolver::DumpState(const TFsPath dstFile) {
        auto mess = Serialize();
        WriteProtoTextToFile(mess, dstFile);
    }

    void TSolver::LoadState(const TFsPath srcFile) {
        NEgoProto::TSolverState solverState;
        ReadProtoTextFromFile(srcFile, solverState);
        Deserialize(solverState);
    }

    TSolver::TSolver(const TSolverSpec& spec)
        : Config(spec.SolverConfig)
        , Problem(spec.ProblemConfig)
        , StartIterationNum(0)
        , EndIterationNum(0)
        , BatchNumber(0)
    {
        Model = Factory.CreateModel(Config.ModelType, Config.ModelConfig, Problem.GetDimSize());

        InitSamples = GenerateSobolGrid(Config.InitSampleSize, Model->GetDimSize());
        BatchPolicy = Factory.CreateBatchPolicy(Config.BatchPolicy, Model, Config);
    }

    void TSolver::SerialProcess(TProtoSerial& serial) {
        serial(Config, NEgoProto::TSolverState::kSolverConfigFieldNumber);
        serial(Problem, NEgoProto::TSolverState::kProblemStateFieldNumber);
        serial(StartIterationNum, NEgoProto::TSolverState::kStartIterationNumFieldNumber);
        serial(EndIterationNum, NEgoProto::TSolverState::kEndIterationNumFieldNumber);
        serial(BatchNumber, NEgoProto::TSolverState::kBatchNumberFieldNumber);
        serial(InitSamples, NEgoProto::TSolverState::kInitSamplesFieldNumber);
        
        if (serial.IsInput()) {
            Model = Factory.CreateModel(Config.ModelType, Config.ModelConfig, Problem.GetDimSize());
            BatchPolicy = Factory.CreateBatchPolicy(Config.BatchPolicy, Model, Config);
            BatchPolicy->InitNewBatch();
        }

        serial(*Model, NEgoProto::TSolverState::kModelStateFieldNumber);
    }

    TSolver::TSolver(const TSolver& solver) {
        (*this) = solver;
    }

    TSolver& TSolver::operator=(const TSolver& solver) {
        if (this != &solver) {
            BatchNumber = solver.BatchNumber;
            StartIterationNum = solver.StartIterationNum;
            EndIterationNum = solver.EndIterationNum;
            InitSamples = solver.InitSamples;
            Config = solver.Config;
            Problem = solver.Problem;
            Model = solver.Model;
            BatchPolicy = Factory.CreateBatchPolicy(Config.BatchPolicy, Model, Config);
            BatchPolicy->InitNewBatch();
        }
        return *this;
    }

    SPtr<IModel> TSolver::GetModel() const {
        return Model;
    }
    
    TString TSolver::GetProblemName() const {
        return Problem.GetName();
    }

    const TSolverConfig& TSolver::GetConfig() const {
        return Config;
    }

    const TProblem& TSolver::GetProblem() const {
        return Problem;
    }

    template <>
    void TSolver::AddPoint(const TRawPoint& rawPoint) {
        TGuard lock(AddPointMut);
        
        TPoint point = Problem.Remap(rawPoint);

        L_DEBUG << "Got point with id " << point.Id;
        
        Model->AddPoint(point.X, point.Y);

        ++EndIterationNum;
        L_DEBUG << "Closed " << EndIterationNum << " iteration ";

        if ((EndIterationNum > 0) && (EndIterationNum > InitSamples.n_rows)) {
            // if (EndIterationNum % Config.HyperOptFreq == 0) {
            // if ((EndIterationNum - InitSamples.n_rows) % Config.BatchSize == 0) {
            //     L_DEBUG << "Updating model hyperparameters";
            //     ENSURE(Model, "Model is not set while optimizing hyperparameters");
                
            // } else {
            //     L_DEBUG << "Updating model";
            //     Model->Update();    
            // }
            Model->Update();
        }
    }

    void TSolver::CheckAvailavility() const {
        if (StartIterationNum != EndIterationNum) {
            L_DEBUG << "There are some calculation still going on (" << StartIterationNum - EndIterationNum  << " of calculations need to gather)";
            throw TErrNotAvailable() << "Ego is not available, waiting for " << StartIterationNum - EndIterationNum << " iterations to finish";
        }
    }

    template <>
    TRawPoint TSolver::GetNextPoint() {
        TGuard lock(NextPointMut);

        if (StartIterationNum < InitSamples.n_rows) {
            L_DEBUG << "Going to return random next point";

            return Problem.RemapBack(TPoint(
                NStr::TStringBuilder() << StartIterationNum << "-init",
                InitSamples.row(StartIterationNum++)
            ));
        }
        // } else
        // if (StartIterationNum == InitSamples.n_rows) {
        //     // CheckAvailavility();
        //     // L_DEBUG << "Updating model hyperparameters with init samples";
        //     // Model->OptimizeHypers(Config.HyperOpt);
        // }

        if ((StartIterationNum - InitSamples.n_rows) % Config.BatchSize == 0) {
            CheckAvailavility();
            ++BatchNumber;
            Model->OptimizeHypers(Config.HyperOpt);
            BatchPolicy->InitNewBatch();
            L_DEBUG << "Creating a new batch (# " << BatchNumber << ")";
        }

        return Problem.RemapBack(TPoint(
            NStr::TStringBuilder() << StartIterationNum++ << "-" << BatchNumber,
            BatchPolicy->GetNextElementInBatch()
        ));
    }

} // namespace NEgo