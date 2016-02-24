

#include <ego/base/factory.h>
#include <ego/base/entities.h>

#include <ego/protos/ego_options.pb.h>
#include <ego/util/proto_options.h>
#include <ego/util/string.h>
#include <ego/util/protobuf.h>

#include <ego/opt/opt.h>

#include <ego/model/model.h>


using namespace NEgo;

struct test {};
using TTestTup = TTagTuple<test, double>;

int main(int argc, const char** argv) {
    TProtoOptions<NEgoProto::TEgoOptions> opts(argc, argv, "Ego main binary");

    NEgoProto::TEgoOptions config;
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
    L_DEBUG << "Got ego options: \n\n" << config.DebugString();
    NEgoProto::TModelConfigMessage modelConfig;
    ReadProtoTextFromFile(config.config(), modelConfig);

    L_DEBUG << "Got ego model config: \n\n" << modelConfig.DebugString();

    TMatrixD inputData = NLa::ReadCsv(config.input());
    TModel model(modelConfig.modelconfig(), NLa::HeadCols(inputData, inputData.n_cols-1), NLa::TailCols(inputData, 1));

    auto res = model.GetNegativeLogLik();
    L_DEBUG << "Negative log lik: " << res.Value();
    size_t i = 0;
    for(const auto& v: res.ParamDeriv()) {
        L_DEBUG << "Param " << i << " derivative: " << v;
        ++i;
    }
    return 0;
}