#pragma once

#include "config.h"

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


	class TProblem : public ISerial<NEgoProto::TProblemState> {
	public:
		TProblem(const TFsPath srcFile);

		TProblem(const TProblemConfig& problem, const TModelConfig& config);

		TString GetName() const;

		void Accept(const TJsonDocument& inputSpec);

		void DumpState(const TFsPath dstFile);

		void LoadState(const TFsPath srcFile);

		void SerialProcess(TSerializer& serial) override;

        TModel& GetModel();

        const TProblemConfig& GetConfig() const;

        TJsonDocument GetVariableSlice(const TString& varName, ui32 gridSize);

	private:
		TProblemConfig Config;
		SPtr<TModel> Model;
	};

} // namespace NEgo