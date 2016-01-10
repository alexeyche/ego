
#include "register.h"


int main(int argc, const char** argv) {
	TTestRegister::TCounters c = TTestRegister::Instance().RunTests();
	if(c.TestFail > 0) {
		throw TEgoException() << "Found failed tests";
	}
	return 0;
}