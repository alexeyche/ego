#include "pbjson.hpp"

namespace NEgo {
    namespace NPbJson {

        TString ProtobufToJson(const google::protobuf::Message& message) {
            NJson::Value* json = pbjson::pb2jsonobject(&message);
            NJson::StringBuffer buffer;
            NJson::PrettyWriter<NJson::StringBuffer> writer(buffer);
            json->Accept(writer);
            TString ret = buffer.GetString();
            delete json;
            return ret;
        }

    } // namespace NPbJson
} // namespace NEgo

