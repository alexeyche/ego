

#include <ego/base/factory.h>
#include <ego/base/entities.h>


int main(int argc, const char** argv) {
	auto cov = Factory.CreateCov("MaternCov1", 10);

	return 0;
}