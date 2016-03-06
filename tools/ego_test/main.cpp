
#include "register.h"

#include <ego/util/log/log.h>

int main(int argc, const char** argv) {
    TLog::Instance().SetLogLevel(TLog::DEBUG_LEVEL);
	TTestRegister::TCounters c = TTestRegister::Instance().RunTests();
	if(c.TestFail > 0) {
		throw TEgoException() << "Found failed tests";
	}
	return 0;
}