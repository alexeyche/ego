#pragma once

#include "config.h"

#include <ego/model/base_model.h>
#include <ego/problem/problem.h>
#include <ego/util/fs.h>
#include <ego/util/serial/proto_serial.h>
#include <ego/protos/solver.pb.h>

namespace NEgo {

    class TSolver: public IProtoSerial<NEgoProto::TSolverState> {
	public:
        TSolver(const TSolverSpec& spec);

        TSolver(SPtr<IModel> model, const TSolverSpec& spec);

        TSolver(const TFsPath srcFile);

        TSolver(const TSolver& solver);

        TSolver& operator=(const TSolver& solver);

        /////////////////////////////////////////////////
                
        void DumpState(const TFsPath dstFile);

        void LoadState(const TFsPath srcFile);
        
        void SerialProcess(TProtoSerial& serial) override;

        /////////////////////////////////////////////////

        template <typename T>
		void AddPoint(const T& rawPoint);

        template <typename T>
        T GetNextPoint();

        template <typename T>
        void ForceAddPoint(const T& point);

        void CheckAvailavility() const;

        SPtr<IModel> GetModel() const;

        TString GetProblemName() const;

        const TSolverConfig& GetConfig() const;

        template <typename T>
        T GetVariableSlice(const TString& varName, ui32 gridSize);

        const TProblem& GetProblem() const;
        
	protected:
        TSolverConfig Config;

		ui32 BatchNumber;

        ui32 StartIterationNum;
        TMutex AddPointMut;

        ui32 EndIterationNum;
        TMutex NextPointMut;

        TMatrixD InitSamples;

		
		TProblem Problem;

		SPtr<IModel> Model;
		SPtr<IBatchPolicy> BatchPolicy;
	};


} // namespace NEgo