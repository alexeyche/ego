#include "log.h"

namespace NEgo {


    TLog& TLog::Instance() {
        static TLog _inst;
        return _inst;
    }


}
