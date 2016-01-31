

#include <ego/base/factory.h>

#include <ego/protos/server_opts.pb.h>
#include <ego/util/proto_options.h>
#include <ego/util/log/log.h>


#include "server.h"

using namespace NEgo;



int main(int argc, const char** argv) {
    TProtoOptions<NEgoProto::TServerOpts> opts(argc, argv, "Ego server binary");

    NEgoProto::TServerOpts config;
    if(!opts.Parse(config)) {
        return 0;
    }
    if(config.verbose()) {
        TLog::Instance().SetLogLevel(TLog::DEBUG_LEVEL);
    }

    TServer(std::stoi(config.port()))
        .AddCallback(
            "GET", "/",
            [&](std::ostream &o) {
                o << "Hello\n";
            }
        )
        .MainLoop();


    return 0;
}