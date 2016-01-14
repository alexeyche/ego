#include "register.h"

#include <ego/util/log/log.h>


TTestRegister& TTestRegister::Instance() {
	static TTestRegister _inst;
	return _inst;
}

void TTestRegister::RegisterTest(std::string name, std::function<void()> cb) {
	auto res = Tests.insert(std::make_pair(name, cb));
	ENSURE(res.second, "Test with name " << name << " already exists in system");
}

TTestRegister::TCounters TTestRegister::RunTests() {
	TCounters c;
	for(const auto &t: Tests) {
		L_INFO << "Running " << t.first;
		// try {
			t.second();
			TestStatus[t.first] = std::make_pair(true, "Test Ok");
			L_INFO << "Test Ok";
			c.TestOk++;
		// } catch (const std::exception &e) {
		// 	L_ERROR << "Got error: " << e.what();
		// 	TestStatus[t.first] = std::make_pair(false, e.what());
		// 	c.TestFail++;
		// }
	}
	L_INFO << "================|  Summary  |=====================";
	L_INFO << "TestOk: " << c.TestOk;
	L_INFO << "TestFail: " << c.TestFail;
	return c;
}