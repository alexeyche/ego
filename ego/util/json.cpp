#include "json.h"

namespace NEgo {

	template <>
    NJson::Value TJsonDocument::SetValue<TString>(TString v) {
		NJson::Value s;
		s.SetString(v.c_str(), v.size(), Doc.GetAllocator());
		return s;
	}


	template <>
	TString TJsonDocument::GetValue(NJson::Value& v) {
		return v.GetString();
	}

	template <>
	int TJsonDocument::GetValue(NJson::Value& v) {
		return v.GetInt();
	}


} // namespace NEgo 