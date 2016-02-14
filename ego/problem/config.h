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

		EVariableType Type;
	};


    struct TProblemConfig {
    	TProblemConfig() {}

        TProblemConfig(const NEgoProto::TProblemConfig& config)
            : ProtoConfig(config)
        {
            Name = ProtoConfig.name();
            for (const auto& v: ProtoConfig.variable()) {
                if (v.type() == NEgoProto::FLOAT) {
                    TVariable var;
                    var.Name = v.name();
                    var.Type = EVariableType::FLOAT;
                    var.Min = v.min();
                    var.Max = v.max();
                    Variables.push_back(var);
                } else
                if (v.type() == NEgoProto::INT) {
                    TVariable var;
                    var.Name = v.name();
                    var.Type = EVariableType::INT;
                    var.Min = v.min();
                    var.Max = v.max();
                    Variables.push_back(var);
                } else {
                    for (const auto& vopt: v.option()) {
                        TVariable var;
                        var.Min = 0.0;
                        var.Max = 1.0;
                        var.Type = EVariableType::ENUM;
                        Variables.push_back(var);
                    }
                }
            }
            DimSize = Variables.size();
        }

        TString Name;

        ui32 DimSize;
        TVector<TVariable> Variables;

        NEgoProto::TProblemConfig ProtoConfig;
    };


} // namespace NEgo
