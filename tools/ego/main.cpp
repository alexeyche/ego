

#include <ego/base/factory.h>
#include <ego/base/entities.h>

#include <ego/protos/config.pb.h>
#include <ego/util/proto_options.h>
#include <ego/util/string.h>

#include <ego/opt/opt.h>

#include <ego/model/model.h>


using namespace NEgo;

struct test {};
using TTestTup = TTagTuple<test, double>;

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

    if(config.listmethods()) {
        NOpt::PrintMethods();
        return 0;
    }

    if(config.verbose()) {
        TLog::Instance().SetLogLevel(TLog::DEBUG_LEVEL);
    }
    L_DEBUG << "Got model config: \n\n" << config.DebugString();
    TModel model(config);

    auto res = model.GetNegativeLogLik();
    L_DEBUG << "Negative log lik: " << res.Value();
    size_t i = 0;
    for(const auto& v: res.ParamDeriv()) {
        L_DEBUG << "Param " << i << " derivative: " << v;
        ++i;
    }
    // NOpt::OptimizeModelLogLik(model, NOpt::MethodFromString(config.opt()));
    return 0;
}