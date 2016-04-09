#pragma once

#include <ego/base/base.h>
#include <ego/protos/problem.pb.h>
#include <ego/model/config.h>
#include <ego/util/string.h>

namespace NEgo {

    enum class EVariableType {
        FLOAT = 0,
        INT = 1,
        ENUM = 2
    };

	struct TVariable {
		TString Name;
		double Min;
		double Max;

        ui32 Id;

        EVariableType Type;
	};


    struct TProblemConfig: public IProtoSerial<NEgoProto::TProblemConfig> {
    	void SerialProcess(TProtoSerial& serial) {
            serial(Name);
            if (serial.IsInput()) {
                DeserialVariables(serial.GetMessage<NEgoProto::TProblemConfig>());
            }
            if (serial.IsOutput()) {
                SerialVariables(serial.GetMutMessage<NEgoProto::TProblemConfig>());
            }
        }
        
        void DeserialVariables(const NEgoProto::TProblemConfig& config) {
            ui32 variableId = 0;
            for (const auto& v: config.variable()) {
                if (v.type() == NEgoProto::FLOAT) {
                    TVariable var;
                    var.Name = v.name();
                    var.Type = EVariableType::FLOAT;
                    var.Min = v.min();
                    var.Max = v.max();
                    var.Id = variableId++;
                    auto res = Variables.insert(MakePair(var.Name, var));
                    ENSURE(res.second, "Found duplicates of variable name: " << var.Name);
                } else
                if (v.type() == NEgoProto::INT) {
                    TVariable var;
                    var.Name = v.name();
                    var.Type = EVariableType::INT;
                    var.Min = v.min();
                    var.Max = v.max();
                    var.Id = variableId++;
                    auto res = Variables.insert(MakePair(var.Name, var));
                    ENSURE(res.second, "Found duplicates of variable name: " << var.Name);
                } else {
                    for (const auto& vopt: v.option()) {
                        TVariable var;
                        var.Name = v.name() + "-" + vopt;
                        var.Min = 0.0;
                        var.Max = 1.0;
                        var.Type = EVariableType::ENUM;
                        var.Id = variableId++;
                        auto res = Variables.insert(MakePair(var.Name, var));
                        ENSURE(res.second, "Found duplicates of variable name: " << var.Name);
                    }
                }
                DimSize = Variables.size();
            }
        }
        void SerialVariables(NEgoProto::TProblemConfig& config) const {
            std::map<ui32, TVariable> variablesSorted;
            for (const auto& v: Variables) {
                variablesSorted.emplace(v.second.Id, v.second);
            }
            NEgoProto::TVariable* var(nullptr);
            TString enumName;
            for (const auto& vPair: variablesSorted) {
                const TVariable& v = vPair.second;
                if (v.Type == EVariableType::FLOAT) {
                    var = config.add_variable();
                    var->set_type(NEgoProto::FLOAT);
                    var->set_min(v.Min);
                    var->set_max(v.Max);
                    var->set_name(v.Name);
                } else 
                if (v.Type == EVariableType::INT) {
                    var = config.add_variable();
                    var->set_type(NEgoProto::INT);
                    var->set_min(v.Min);
                    var->set_max(v.Max);
                    var->set_name(v.Name);
                } else {
                    TVector<TString> name_spl = NStr::Split(v.Name, "-");
                    ENSURE(name_spl.size() == 2, "UB");
                    if (!var || (enumName != name_spl[0])) {
                        var = config.add_variable();
                    }
                    var->add_option(name_spl[1]);
                }
            }
        }

        TProblemConfig() {}

        TProblemConfig(const NEgoProto::TProblemConfig& config) {
            Deserialize(config);
        }

        TString Name;

        ui32 DimSize;
        std::map<TString, TVariable> Variables;
    };

} // namespace NEgo
