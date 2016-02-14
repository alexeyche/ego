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

        for (size_t varIdx=0; varIdx < Config.Variables.size(); ++varIdx) {
            const TVariable& var = Config.Variables[varIdx];
            switch (var.Type) {
                case EVariableType::INT:
                    Xnew(varIdx) = ToUnit(var, varSpec[var.Name].GetValue<int>());
                    break;
                case EVariableType::FLOAT:
                    Xnew(varIdx) = ToUnit(var, varSpec[var.Name].GetValue<double>());
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

} // namespace NEgo