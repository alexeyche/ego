#pragma once

#include <ego/base/base.h>

#include <regex>
#include <iterator>
#include <limits>

namespace NEgo {
	namespace NStr {
	
	    TString CamelCaseToOption(TString s);

	    void Replace(TString &s, const TString &search, const TString &replace, size_t num = std::numeric_limits<size_t>::max());

	    TString Trim(const TString &strInp, TString symbols = " \t");

	    TString Strip(const TString& input);
		
		TVector<TString> SplitInto(const TString &s, char delim, TVector<TString> &elems);
		
		TVector<TString> Split(const TString &s, char delim);
	    
	    TVector<TString> Split(const TString &s_inp, const TString &delimiter, const TString &not_include = "");

	} // namespace NStr
} // namespace NEgo