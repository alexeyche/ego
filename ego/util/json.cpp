#include "json.h"

#include <ego/util/log/log.h>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

using namespace google::protobuf;


namespace NEgo {

	template <>
    NJson::Value TJsonDocument::SetValue<TString>(TString v) {
		NJson::Value s;
		s.SetString(v.c_str(), v.size(), Doc->GetAllocator());
		return s;
	}

	template <>
    NJson::Value TJsonDocument::SetValue<TJsonDocument>(TJsonDocument v) {
		NJson::Value s;
		s.CopyFrom(v.CurrentValue(), Doc->GetAllocator());
		return s;
	}

	template <>
    NJson::Value TJsonDocument::SetValue<double>(double v) {
		NJson::Value s;
		s.SetDouble(v);
		return s;
	}

	template <>
    NJson::Value TJsonDocument::SetValue<int>(int v) {
		NJson::Value s;
		s.SetInt(v);
		return s;
	}

	template <>
	TString TJsonDocument::GetValue(const NJson::Value& v) const {
		return v.GetString();
	}

	template <>
	int TJsonDocument::GetValue(const NJson::Value& v) const {
		return v.GetInt();
	}

	template <>
	double TJsonDocument::GetValue(const NJson::Value& v) const {
		return v.GetDouble();
	}

	template <>
	bool TJsonDocument::GetValue(const NJson::Value& v) const {
		return v.GetBool();
	}


	TString TJsonDocument::GetPrettyString() const {
		NJson::StringBuffer buffer;
		NJson::PrettyWriter<NJson::StringBuffer> writer(buffer);
		CurrentValue().Accept(writer);
		return buffer.GetString();
	}

	TString TJsonDocument::GetString() const {
		NJson::StringBuffer buffer;
		NJson::Writer<NJson::StringBuffer> writer(buffer);
		CurrentValue().Accept(writer);
		return buffer.GetString();
	}

	TJsonDocument TJsonDocument::operator[](const TString& key) {
		if (!CurrentValue().IsObject()) {
			CurrentValue().SetObject();
		}
		auto ptr = CurrentValue().FindMember(key.c_str());
		if (ptr == CurrentValue().MemberEnd()) {
			NJson::Value val;
			NJson::Value keyVal(key.c_str(), key.size(), Doc->GetAllocator());
			CurrentValue().AddMember(keyVal, val, Doc->GetAllocator());
			return TJsonDocument(*this, CurrentValue().FindMember(key.c_str())->value);
		}
		return TJsonDocument(*this, ptr->value);
	}

	const TJsonDocument TJsonDocument::operator[](const TString& key) const {
		auto ptr = CurrentValue().FindMember(key.c_str());
		ENSURE(ptr != CurrentValue().MemberEnd(), "Failed to find " << key);
		return TJsonDocument(*this, ptr->value);
	}

	bool TJsonDocument::Has(const TString& key) const {
		return CurrentValue().FindMember(key.c_str()) != CurrentValue().MemberEnd();
	}

	std::ostream& operator<<(std::ostream& stream, const TJsonDocument& doc) {
		stream << doc.GetPrettyString();
		return stream;
	}

} // namespace NEgo