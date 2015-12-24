#pragma once

#include <ego/base/base.h>

#include <map>

namespace NEgo {

	class ICov;

	class TFactory {
    	typedef std::map<TString, UPtr<ICov> (*)(size_t)> TCovCreateMap;

    	template<typename INST> static UPtr<ICov> CreateCovCb(size_t dim_size) { return std::move(UPtr<INST>(new INST(dim_size))); }

	public:
	    template <typename T>
	    void RegisterCov(TString type) {
	    	CovMap[type] = &CreateCovCb<T>;
	    }

	    UPtr<ICov> CreateCov(TString name, size_t dim_size);

	    static TFactory& Instance();

	private:
		TCovCreateMap CovMap;
	};

	#define REGISTRATOR_CLASS(Name) \
		template <class TObj> \
		struct T##Name##Registrator { \
			T##Name##Registrator(TString name) { \
				if(name[0] == 'T') { \
					name = name.substr(1); \
				} \
				TFactory::Instance().Register##Name<TObj>(name); \
			} \
		};\

	REGISTRATOR_CLASS(Cov);

	#define REGISTER_COV(CovType) static TCovRegistrator<CovType> GENERATE_UNIQUE_ID(TFactory)(#CovType);

} // namespace NEgo

#define Factory NEgo::TFactory::Instance()