#pragma once

#include <ego/base/base.h>

#include <map>
#include <functional>

using namespace NEgo;

class TTestRegister {
public:
	struct TCounters {
		ui32 TestOk = 0;
		ui32 TestFail = 0;
	};

	static TTestRegister& Instance();

	void RegisterTest(std::string name, std::function<void()> cb);

	TCounters RunTests();
private:
	std::vector<std::string> TestNames;
	std::map<std::string, std::function<void()>> Tests;
	std::map<std::string, std::pair<bool, std::string>> TestStatus;
};


struct TTestRegistrator {
	TTestRegistrator(TString name, std::function<void()> cb) {
		TTestRegister::Instance().RegisterTest(name, cb);
	}
};


#define TEST(Name) \
	void Name(); \
	static TTestRegistrator JOIN(TTestRegistrator, Name)(#Name, Name);\
	void Name()

