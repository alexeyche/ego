#include "problem.h"

#include <ego/util/protobuf.h>

namespace NEgo {

    TProblem::TProblem(const TFsPath srcFile) {
        Model = MakeShared(new TModel());
        LoadState(srcFile);
    }

    TProblem::TProblem(const TProblemConfig& problem, const TModelConfig& config)
        : Config(problem)
    {
        Model = MakeShared(new TModel(config, problem.DimSize));
    }

    TString TProblem::GetName() const {
        return Config.Name;
    }

    void TProblem::SerialProcess(TSerializer& serial) {
        NEgoProto::TProblemConfig protoConfig = Config.ProtoConfig;

        serial(protoConfig, NEgoProto::TProblemState::kProblemConfigFieldNumber);
        serial(*Model, NEgoProto::TProblemState::kModelStateFieldNumber);

        if (serial.IsInput()) {
            Config = TProblemConfig(protoConfig);
        }
    }

    void TProblem::DumpState(const TFsPath dstFile) {
        auto mess = Serialize();
        WriteProtoTextToFile(mess, dstFile);
    }

    void TProblem::LoadState(const TFsPath srcFile) {
        NEgoProto::TProblemState problemState;
        ReadProtoTextFromFile(srcFile, problemState);
        Deserialize(problemState);
    }

    void TProblem::Accept(const TJsonDocument& inputSpec) {
        TVectorD Xnew(Config.DimSize);
        const TJsonDocument varSpec = inputSpec["Data"];
        const TJsonDocument optSpec = inputSpec["Options"];

        double Ynew = varSpec[Config.Name].GetValue<double>();

        for (const auto& namedVar: Config.Variables) {
            const TVariable& var = namedVar.second;
            switch (var.Type) {
                case EVariableType::INT:
                    Xnew(var.Id) = ToUnit(var, varSpec[var.Name].GetValue<int>());
                    break;
                case EVariableType::FLOAT:
                    Xnew(var.Id) = ToUnit(var, varSpec[var.Name].GetValue<double>());
                    break;
                case EVariableType::ENUM:
                    throw TEgoException() << "Not implemented";
            }
        }
        L_DEBUG << GetName() << " accepted new data: " << NLa::VecToStr(Xnew) << " -> " << Ynew;
        Model->AddPoint(Xnew, Ynew);
        if (!optSpec.Has("UpdateModel") || optSpec["UpdateModel"].GetValue<bool>()) {
            Model->Update();
        }
    }

    TModel& TProblem::GetModel() {
        ENSURE(Model, "Model is not set");
        return *Model;
    }

    const TProblemConfig& TProblem::GetConfig() const {
        return Config;
    }

    TJsonDocument TProblem::GetVariableSlice(const TString& varName, ui32 gridSize) {
        if (Model->Empty()) {
            return TJsonDocument::Array();
        }
        auto res = Config.Variables.find(varName);
        if (res == Config.Variables.end()) {
            throw TEgoElementNotFound() << "Variable " << varName << " is not found in problem " << GetName();
        }
        const TVariable& var = res->second;
        
        TVectorD Xbest = Model->GetMinimumX();

        TMatrixD X;
        TVectorD grid = NLa::Linspace(0.0, 1.0, gridSize);
        for (ui32 dim = 0; dim < Xbest.size(); ++dim) { 
            if (dim == var.Id) {
                X = NLa::ColBind(X, grid);
            } else {
                X = NLa::ColBind(X, NLa::VectorFromConstant(gridSize, Xbest(dim)));
            }
        }

        TDistrVec vec = Model->GetPrediction(X);

        TJsonDocument mean = TJsonDocument::Array();
        TJsonDocument leftSd = TJsonDocument::Array();
        TJsonDocument rightSd = TJsonDocument::Array();
        
        ui32 idx = 0;
        for (const auto& distr: vec) {
            mean.PushBack(TJsonDocument::Array({grid(idx), distr->GetMean()}));
            leftSd.PushBack(TJsonDocument::Array({grid(idx), distr->GetMean() - distr->GetSd()}));
            rightSd.PushBack(TJsonDocument::Array({grid(idx), distr->GetMean() + distr->GetSd()}));
            ++idx;
        }
        TJsonDocument points = TJsonDocument::Array();
        

        const TMatrixD& Xfull = Model->GetData().first;
        const TVectorD& Yfull = Model->GetData().second;

        for (size_t pi=0; pi < Yfull.size(); ++pi) {
            points.PushBack(TJsonDocument::Array({Xfull(pi, var.Id), Yfull(pi)}));
        }
        
        TJsonDocument minimum = TJsonDocument::Array();
        minimum.PushBack(TJsonDocument::Array({Xbest(var.Id), Model->GetMinimumY()}));

        TJsonDocument ret = TJsonDocument::Object();
        ret["leftBand"] = leftSd;
        ret["mean"] = mean;
        ret["rightBand"] = rightSd;
        ret["points"] = points;
        ret["minimum"] = minimum;
        return ret;
    }

} // namespace NEgo