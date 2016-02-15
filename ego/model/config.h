#pragma once

#include <ego/base/base.h>
#include <ego/base/errors.h>
#include <ego/protos/model.pb.h>

#include <ctime>


namespace NEgo {

    struct TModelConfig {
        TModelConfig(NEgoProto::TModelConfig config = NEgoProto::TModelConfig())
            : ProtoConfig(config)
        {
            Cov = ProtoConfig.cov();
            Mean = ProtoConfig.mean();
            Lik = ProtoConfig.lik();
            Inf = ProtoConfig.inf();
            Acq = ProtoConfig.acq();

            if(ProtoConfig.has_seed() || ProtoConfig.seed()>0) {
                Seed = ProtoConfig.seed();
            } else {
                Seed = std::time(0);
            }
        }

        TString Cov;
        TString Mean;
        TString Lik;
        TString Inf;
        TString Acq;

        ui32 Seed;

        NEgoProto::TModelConfig ProtoConfig;
    };

} // namespace NEgo
