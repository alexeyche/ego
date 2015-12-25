#pragma once

#include <ego/base/base.h>
#include <ego/base/errors.h>
#include <ego/util/string.h>

#include <map>

namespace NEgo {

	class ICov;
	class IMean;

	class TFactory {
		template <typename BASE>
		using TCreateMap = std::map<TString, UPtr<BASE> (*)(size_t)>;

    	template<typename BASE, typename INST> static UPtr<BASE> CreateCb(size_t dim_size) { return std::move(UPtr<BASE>(new INST(dim_size))); }

	public:
	    template <typename T>
	    void RegisterCov(TString type) {
	    	CovMap[type] = &CreateCb<ICov, T>;
	    }

		template <typename T>
	    void RegisterMean(TString type) {
	    	MeanMap[type] = &CreateCb<IMean, T>;
	    }

	    template <typename T>
	    UPtr<T> CreateEntity(TString name, size_t dim_size, const TCreateMap<T> &map) {
			auto cbPtr = map.find(name);
			ENSURE(cbPtr != map.end(), "Can't find  entity with name " << name);
			return cbPtr->second(dim_size);
	    }


	    UPtr<ICov> CreateCov(TString name, size_t dim_size);

	    UPtr<IMean> CreateMean(TString name, size_t dim_size);

	    static TFactory& Instance();

	    void PrintEntities();
	private:
		TCreateMap<ICov> CovMap;
		TCreateMap<IMean> MeanMap;
	};

	#define REGISTRATOR_CLASS(Name, SuffixToReplace, ReplaceBy) \
		template <class TObj> \
		struct T##Name##Registrator { \
			T##Name##Registrator(TString name) { \
				ReplaceStr(name, SuffixToReplace, ReplaceBy); \
				TFactory::Instance().Register##Name<TObj>(name); \
			} \
		};\

	REGISTRATOR_CLASS(Cov, "TCov", "c");
	REGISTRATOR_CLASS(Mean, "TMean", "m");

	#define REGISTER_COV(CovType) static TCovRegistrator<CovType> GENERATE_UNIQUE_ID(TCovRegistrator)(#CovType);
	#define REGISTER_MEAN(MeanType) static TMeanRegistrator<MeanType> GENERATE_UNIQUE_ID(TMeanRegistrator)(#MeanType);

} // namespace NEgo

#define Factory NEgo::TFactory::Instance()