#include "string.h"

#include <iostream>

namespace NEgo {

    TString CamelCaseToOption(TString s) {
        std::regex e("([a-z])([A-Z])");
        std::string res = std::regex_replace(s, e, "$1-$2");
        std::transform(res.begin(), res.end(), res.begin(), ::tolower);
        return std::string("--") + res;
    }

} // namespace NEgo