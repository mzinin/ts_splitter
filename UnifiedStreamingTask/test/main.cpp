#include <cstdlib>
#include <iostream>

extern uint16_t testOutputNameGenerator();
extern uint16_t testError();
extern uint16_t testProgramOptions();

int main()
{
	uint16_t failures = 0;

	failures += testOutputNameGenerator();
	failures += testError();
	failures += testProgramOptions();

	if (failures == 0)
	{
		std::cout << "ALL TESTS PASSED" << std::endl;
		return EXIT_SUCCESS;
	}

	std::cout << failures << " TEST(S) FAILED" << std::endl;
	return EXIT_FAILURE;
}
