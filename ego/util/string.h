#pragma once

#include <ego/base/base.h>

#include <regex>
#include <iterator>
#include <limits>

namespace NEgo {

    TString CamelCaseToOption(TString s);

    void ReplaceStr(TString &s, const TString &search, const TString &replace, size_t num = std::numeric_limits<size_t>::max());
} // namespace NEgo