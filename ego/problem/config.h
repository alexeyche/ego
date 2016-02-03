#pragma once

#include <ego/protos/problem.pb.h>


namespace NEgo {


	struct TVariable {
		TString Name;
		double Min;
		double Max;

		NEgoProto::EVariableType VariableType;
	};


    struct TProblemConfig {
    	TProblemConfig() {}
        TProblemConfig(const NEgoProto::TProblemConfig& config) {
        }

        ui32 DimSize;
        TVector<TVariable> Variables;
    };


} // namespace NEgo
