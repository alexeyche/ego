#include "problem.h"

#include <ego/util/protobuf.h>
#include <ego/util/log/log.h>

namespace NEgo {

    template <>
    double FromUnit(const TVariable& var, double unitVal) {
        if(! (((unitVal <= 1.0) || (std::abs(unitVal - 1.0) < std::numeric_limits<double>::epsilon())) &&
              ((unitVal >= 0.0) || (std::abs(unitVal - 0.0) < std::numeric_limits<double>::epsilon()))) ){
            throw TErrLogicError() << "Value of " << var.Name << " out of unit cube: " << unitVal;
        }
        return var.Min + unitVal * (var.Max - var.Min);
    }

    template <>
    int FromUnit(const TVariable& var, double unitVal) {
        return std::round(FromUnit<double>(var, unitVal));
    }

    TProblem::TProblem(const TProblemConfig& config) 
        : Config(config) 
    {
    }

    void TProblem::SerialProcess(TProtoSerial& serial) {
        serial(Config, NEgoProto::TProblemState::kProblemConfigFieldNumber);
    }

    TPoint TProblem::Remap(const TRawPoint& rawPoint) {
        TVectorD x(Config.DimSize);
        double y = rawPoint.GetValue<double>(Config.Name);

        for (const auto& namedVar: Config.Variables) {
            const TVariable& var = namedVar.second;
            switch (var.Type) {
                case EVariableType::INT:
                    x(var.Id) = ToUnit(var, rawPoint.GetValue<int>(var.Name));
                    break;
                case EVariableType::FLOAT:
                    x(var.Id) = ToUnit(var, rawPoint.GetValue<double>(var.Name));
                    break;
                case EVariableType::ENUM:
                    throw TErrException() << "Not implemented";
            }
        }
        L_DEBUG << GetName() << " accepted new data: " << NLa::VecToStr(x) << " -> " << y;
        return TPoint(x, y, rawPoint.GetId());
    }


    TRawPoint TProblem::RemapBack(const TPoint& point) {
        TRawPoint rPoint;
        rPoint.SetId(point.Id);
        for (const auto& namedVar: Config.Variables) {
            const TVariable& var = namedVar.second;

            switch (var.Type) {
                case EVariableType::INT:
                    rPoint.SetValue<int>(
                        var.Name, 
                        FromUnit<int>(var, point.X(var.Id))
                    );
                    break;
                case EVariableType::FLOAT:
                    rPoint.SetValue<double>(
                        var.Name, 
                        FromUnit<double>(var, point.X(var.Id))
                    );
                    break;
                case EVariableType::ENUM:
                    throw TErrException() << "Not implemented";
            }
        }
        return rPoint;
    }

    TString TProblem::GetName() const {
        return Config.Name;
    }
    
    ui32 TProblem::GetDimSize() const {
        return Config.DimSize;
    }    

    const TProblemConfig& TProblem::GetConfig() const {
        return Config;
    }

    const std::map<TString, TVariable>& TProblem::GetVariablesMap() const {
        std::map<TString, TVariable> varMap;
        for (const auto& var: Config.Variables) {
            varMap.emplace(var.second.Name, var.second);
        }
        return varMap;
    }


    TVector<TVariable> TProblem::GetVariables() const {
        TVector<TVariable> vars;
        vars.reserve(varsSorted.size());
        for (const auto& var: Config.Variables) {
            vars.emplace_back(var.second);
        }
        return vars;
    }

} // namespace NEgo