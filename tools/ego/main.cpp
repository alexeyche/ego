

#include <ego/base/factory.h>
#include <ego/base/entities.h>

#include <ego/protos/config.pb.h>
#include <ego/util/proto_options.h>
#include <ego/util/string.h>

using namespace NEgo;

int main(int argc, const char** argv) {
	auto cov = Factory.CreateCov("MaternCov1", 10);
    TProtoOptions<NEgoProto::TEgoConfig> opts(argc, argv, "Ego main binary");

    NEgoProto::TEgoConfig config;
    if(!opts.Parse(config)) {
        return 0;
    }
    std::cout << config.DebugString() << "\n";

	return 0;
}