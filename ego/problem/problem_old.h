#pragma once

#include "config.h"

#include <ego/strategy/strategy.h>
#include <ego/model/model.h>
#include <ego/util/json.h>
#include <ego/util/fs.h>

namespace NEgo {

	template <typename T>
	double ToUnit(const TVariable& var, T val) {
        double unitVal = (val - var.Min)/(var.Max - var.Min);
        if(! (((unitVal <= 1.0) || (std::abs(unitVal - 1.0) < std::numeric_limits<double>::epsilon())) &&
        	  ((unitVal >= 0.0) || (std::abs(unitVal - 0.0) < std::numeric_limits<double>::epsilon()))) ){
    		throw TEgoLogicError() << "Value of " << var.Name << " out of unit cube: " << unitVal;
        }
       	return unitVal;
    }

	template <typename T>
	T FromUnit(const TVariable& var, double unitVal);


	class TProblem : public ISerial<NEgoProto::TProblemState> {
	public:
		struct TPoint {

		};
		
		TProblem(const TFsPath srcFile);

		TProblem(const TProblemSpec& spec);

		TString GetName() const;

		void AddPoint(const TJsonDocument& inputSpec);

		void DumpState(const TFsPath dstFile);

		void LoadState(const TFsPath srcFile);

		void SerialProcess(TSerializer& serial) override;

        SPtr<IModel> GetModel();

        const TProblemConfig& GetConfig() const;

        TJsonDocument GetVariableSlice(const TString& varName, ui32 gridSize);

        TStrategy& GetStrategy();

        TJsonDocument GetNextPoint();

	private:
		TProblemConfig Config;
		SPtr<IModel> Model;
		TString ModelType;
        
        TStrategy Strategy;
	};

} // namespace NEgo