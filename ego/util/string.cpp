#include "string.h"

#include <iostream>

namespace NEgo {

    TString CamelCaseToOption(TString s) {
        std::regex e("([a-z])([A-Z])");
        std::string res = std::regex_replace(s, e, "$1-$2");
        std::transform(res.begin(), res.end(), res.begin(), ::tolower);
        return std::string("--") + res;
    }

    void ReplaceStr(TString &s, const TString &search, const TString &replace, size_t num) {
        size_t repl_num = 0;
        for( size_t pos = 0; ; pos += replace.length() ) {
            if(repl_num>=num) break;
            // Locate the subTString to replace
            pos = s.find( search, pos );
            if( pos == TString::npos ) break;
            // Replace by erasing and inserting
            s.erase( pos, search.length() );
            s.insert( pos, replace );
            repl_num+=1;
        }
    }

} // namespace NEgo