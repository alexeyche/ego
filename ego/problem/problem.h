#pragma once

#include "config.h"

#include <ego/base/la.h>

#include <ego/util/serial/proto_serial.h>
#include <ego/util/json.h>
#include <ego/util/fs.h>
#include <ego/util/any.h>
#include <ego/util/string.h>

namespace NEgo {

	template <typename T>
	double ToUnit(const TVariable& var, T val) {
        double unitVal = (val - var.Min)/(var.Max - var.Min);
        if(! (((unitVal <= 1.0) || (std::abs(unitVal - 1.0) < std::numeric_limits<double>::epsilon())) &&
        	  ((unitVal >= 0.0) || (std::abs(unitVal - 0.0) < std::numeric_limits<double>::epsilon()))) ){
    		throw TErrLogicError() << "Value of " << var.Name << " out of unit cube: " << unitVal;
        }
       	return unitVal;
    }

	template <typename T>
	T FromUnit(const TVariable& var, double unitVal);

    class TRawPoint {
    public:
        template <typename T>
        T GetValue(TString varName) const {
            auto varPtr = Variables.find(varName);
            ENSURE(varPtr != Variables.end(), "Can't find variable with name " << varName);
            return varPtr->second.GetValue<T>();
        }

        template <typename T>
        void SetValue(TString varName, T value) {
            TAny any;
            any.SetValue<T>(value);
            auto res = Variables.insert( MakePair(varName, any) );
            ENSURE(res.second, "Found duplicates of variable " << varName);
        }
        
        std::map<TString, TAny>& GetVariables() {
            return Variables;
        }
        
        const TString& GetId() const {
            return Id;
        }
        void SetId(TString id) {
            Id = id;
        }

    private:
        TString Id;

        std::map<TString, TAny> Variables;
    };


    struct TPoint {
        TPoint(TVectorD x, double y, TString id) 
            : X(x)
            , Y(y)
            , Id(id)
        {
        }

        TPoint(TString id, TVectorD x)
            : X(x)
            , Id(id)
        {}

        TVectorD X;
        double Y;
        TString Id;
    };


    class TProblem: public IProtoSerial<NEgoProto::TProblemState> {
    public:
        TProblem() {}

        void SerialProcess(TProtoSerial& serial);

        TProblem(const TProblemConfig& config);

        TPoint Remap(const TRawPoint& rawPoint);

        TRawPoint RemapBack(const TPoint& point);

        TString GetName() const;
    
        ui32 GetDimSize() const;

        const TProblemConfig& GetConfig() const;

        const std::map<TString, TVariable>& GetVariables() const;

    private:
        TProblemConfig Config;
    };


} // namespace NEgo
