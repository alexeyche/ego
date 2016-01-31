#pragma once

#include <ego/base/base.h>

#include <ego/contrib/rapidjson/writer.h>
#include <ego/contrib/rapidjson/document.h>
#include <ego/contrib/rapidjson/prettywriter.h>

namespace NJson = rapidjson;

namespace NEgo {
	
	class TJsonDocument {
	public:
		TJsonDocument() {
		}

		TJsonDocument(const TString& json) {
			Doc.Parse(json.c_str());
		}


		template <typename T>
		NJson::Value SetValue(T v);

		template <typename T>
		T GetValue(NJson::Value& v);

		template <typename T>
		TJsonDocument& PushBack(T&& v) {
			if (!Doc.IsArray()) {
				Doc.SetArray();	
			}

			NJson::Value subv = SetValue(v);
			Doc.PushBack(subv, Doc.GetAllocator());
			return *this;
		}

		template <typename T>
		TJsonDocument& FormArray(TVector<T> arr) {
			if (!Doc.IsArray()) {
				Doc.SetArray();	
			}

			for (const auto& v: arr) {
				PushBack(v);
			}
			return *this;
		}

		TString GetPrettyString() {
			NJson::StringBuffer buffer;
			NJson::PrettyWriter<NJson::StringBuffer> writer(buffer);
			Doc.Accept(writer);
			return buffer.GetString();
		}

		template <typename T>
		T Get(TString name) {
			return GetValue<T>(Doc[name.c_str()]);
		}

	private:
		NJson::Document Doc;
	};

	



} // namespace NEgo 