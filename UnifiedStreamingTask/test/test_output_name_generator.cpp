#include "../output_name_generator.hpp"

#include <iostream>


namespace
{
	/// @brief Run one OutputNameGenerator unit test.
	/// @returns true if test passed, false otherwise.
	bool runTest(const std::string& testName,
				 const std::string& nameExample,
				 uint16_t fileNumber,
				 const std::string& expectedName)
	{
		std::cout << "Running OutputNameGenerator." << testName << " ... ";

		OutputNameGenerator ong(nameExample);
		const bool result = ong.getName(fileNumber) == expectedName;

		std::cout << (result ? "OK" : "FAIL") << std::endl;
		if (!result)
			std::cerr << "Got " << ong.getName(fileNumber) << " instead of " << expectedName << std::endl;

		return result;
	}
}

/// @brief Run all OutputNameGenerator unit tests.
/// @returns Number of failed tests.
uint16_t testOutputNameGenerator()
{
	uint16_t failures = 0;

	failures += 1 - runTest("FirstFile_Number_Extenson", "video_1.out", 1, "video_1.out");
	failures += 1 - runTest("SecondFile_Number_Extenson", "video_1.out", 2, "video_2.out");

	failures += 1 - runTest("FirstFile_NoNumber_Extenson", "audio.out", 1, "audio.out");
	failures += 1 - runTest("SecondFile_NoNumber_Extenson", "audio.out", 2, "audio_2.out");

	failures += 1 - runTest("FirstFile_Number_NoExtenson", "video_1", 1, "video_1");
	failures += 1 - runTest("SecondFile_Number_NoExtenson", "video_1", 2, "video_2");

	failures += 1 - runTest("FirstFile_NoNumber_NoExtenson", "audio", 1, "audio");
	failures += 1 - runTest("SecondFile_NoNumber_NoExtenson", "audio", 2, "audio_2");

	failures += 1 - runTest("FirstFile_Uninitialized", "", 1, "");
	failures += 1 - runTest("SecondFile_Uninitialized", "", 2, "");

	return failures;
}