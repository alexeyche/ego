

#include <ego/base/factory.h>
#include <ego/base/entities.h>

#include <ego/protos/config.pb.h>
#include <ego/util/proto_options.h>
#include <ego/util/string.h>

#include <ego/base/opt.h>

#include <ego/model/model.h>


using namespace NEgo;

int main(int argc, const char** argv) {
    TProtoOptions<NEgoProto::TModelConfig> opts(argc, argv, "Ego main binary");

    NEgoProto::TModelConfig config;
    if(!opts.Parse(config)) {
        return 0;
    }
    if(config.listentities()) {
        Factory.PrintEntities();
        return 0;
    }
    if(config.verbose()) {
        TLog::Instance().SetLogLevel(TLog::DEBUG_LEVEL);
    }
    L_DEBUG << "Got model config: \n\n" << config.DebugString();
    TModel model(config);
    TVectorD v(model.GetHyperParametersSize());
    v.fill(1.0);

    NOpt::Minimize(model, v, NOpt::MMA);
    auto d = model.GetPrediction({0, 0.1, 0.2});
    L_DEBUG << d.Mean;
    L_DEBUG << d.Variance;
	return 0;
}