#pragma once

#include <iostream>

#include <ego/base/base.h>
#include <ego/util/optional.h>

#include <ego/contrib/rapidjson/writer.h>
#include <ego/contrib/rapidjson/document.h>
#include <ego/contrib/rapidjson/prettywriter.h>

namespace NJson = rapidjson;

namespace NEgo {

	class TJsonDocument {
	// 	friend class TJsonDocumentWrap;
	public:
		template <typename T>
		static TJsonDocument Array(T array) {
			TJsonDocument doc;
			doc.Doc->SetArray();
			doc.FormArray(array);
			return doc;
		}

		static TJsonDocument Array() {
			TJsonDocument doc;
			doc.Doc->SetArray();
			return doc;
		}

		static TJsonDocument Object() {
			TJsonDocument doc;
			doc.Doc->SetObject();
			return doc;
		}

		TJsonDocument() {
			Doc = std::make_shared<NJson::Document>();
		}

		TJsonDocument(const TString& json) {
			Doc = std::make_shared<NJson::Document>();
			Doc->Parse(json.c_str());
		}

		TJsonDocument(const TJsonDocument& anotherDoc, NJson::Value& leaf) {
			Doc = anotherDoc.Doc;
			Leaf.emplace(leaf);
		}

		TJsonDocument(const TJsonDocument& anotherDoc, const NJson::Value& cleaf) {
			Doc = anotherDoc.Doc;
			CLeaf.emplace(cleaf);
		}

		template <typename T>
		NJson::Value SetValue(T v);

		template <typename T>
		T GetValue(const NJson::Value& v) const;

		template <typename T>
		T GetValue() const {
			return GetValue<T>(CurrentValue());
		}

		template <typename T>
		TJsonDocument& PushBack(T v) {
			if (!CurrentValue().IsArray()) {
				CurrentValue().SetArray();
			}
			CurrentValue().PushBack(SetValue(v), Doc->GetAllocator());
			return *this;
		}

		template <typename T>
		TJsonDocument& FormArray(T arr) {
			if (!CurrentValue().IsArray()) {
				CurrentValue().SetArray();
			}
			for (const auto& v: arr) {
				PushBack(v);
			}
			return *this;
		}

		TString GetPrettyString() const;

		TString GetString() const;

		TJsonDocument operator[](const TString& key);

		const TJsonDocument operator[](const TString& key) const;

		bool Has(const TString& key) const;

		TJsonDocument& operator=(const TJsonDocument& anotherVal) {
			CurrentValue().CopyFrom(*anotherVal.Doc, Doc->GetAllocator());
			return *this;
		}

		template <typename T>
		TJsonDocument& operator=(const T& anotherVal) {
			CurrentValue().CopyFrom(SetValue(anotherVal), Doc->GetAllocator());
			return *this;
		}

		template <typename T>
		TJsonDocument& operator=(const TVector<T>& anotherVal) {
			FormArray(anotherVal);
			return *this;
		}

        friend std::ostream& operator<<(std::ostream& stream, const TJsonDocument& doc);

	private:
		NJson::Value& CurrentValue() {
			if (Leaf) {
				return *Leaf;
			}
			return *Doc;
		}

		const NJson::Value& CurrentValue() const {
			if (CLeaf) {
				return *CLeaf;
			}
			return *Doc;
		}

		SPtr<NJson::Document> Doc;

		TOptional<NJson::Value&> Leaf;
		TOptional<const NJson::Value&> CLeaf;
	};

} // namespace NEgo