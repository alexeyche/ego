#pragma once

#include <ego/base/base.h>
#include <ego/base/errors.h>
#include <ego/protos/model.pb.h>

#include <ego/util/serial/proto_serial.h>

#include <ctime>


namespace NEgo {

    struct TModelConfig: public IProtoSerial<NEgoProto::TModelConfig> {
        void SerialProcess(TProtoSerial& serial) {
            serial(Cov);
            serial(Mean);
            serial(Inf);
            serial(Lik);
            serial(Acq);
            serial(AcqParameters);
            serial(Seed);
            if (serial.IsInput() && !serial.HasField(NEgoProto::TModelConfig::kSeedFieldNumber)) {
                Seed = std::time(0);
            }
        }
        
        TModelConfig() 
            : Seed(std::time(0)) {}

        TModelConfig(NEgoProto::TModelConfig config) {
            Deserialize(config);
        }

        TString Cov = "cExpISO";
        TString Mean = "mConst";
        TString Lik = "lGauss";
        TString Inf = "iExact";
        TString Acq = "aEI";

        TVector<double> AcqParameters;

        ui32 Seed;
    };

} // namespace NEgo
