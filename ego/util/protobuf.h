#pragma once

#include <google/protobuf/message.h>
#include <ego/base/base.h>

namespace NEgo {

	void ReadProtoTextFromFile(const TString file, google::protobuf::Message &message);

} // namespace NEgo