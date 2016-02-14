#pragma once

#include <ego/base/base.h>
#include <ego/protos/problem.pb.h>

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


    struct TProblemConfig {
    	TProblemConfig() {}

        TProblemConfig(const NEgoProto::TProblemConfig& config)
            : ProtoConfig(config)
        {
            Name = ProtoConfig.name();
            ui32 variableId = 0;
            for (const auto& v: ProtoConfig.variable()) {
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
            }
            DimSize = Variables.size();
        }

        TString Name;

        ui32 DimSize;
        std::map<TString, TVariable> Variables;

        NEgoProto::TProblemConfig ProtoConfig;
    };


} // namespace NEgo
