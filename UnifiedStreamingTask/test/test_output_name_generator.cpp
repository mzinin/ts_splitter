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
        const bool result = ong.name(fileNumber) == expectedName;

        std::cout << (result ? "OK" : "FAIL") << std::endl;
        if (!result)
            std::cerr << "Got " << ong.name(fileNumber) << " instead of " << expectedName << std::endl;

        return result;
    }
}

/// @brief Run all OutputNameGenerator unit tests.
/// @returns Number of failed tests.
uint16_t testOutputNameGenerator()
{
    uint16_t failures = 0;

    failures += 1 - runTest("name_NumberExtenson_File1", "video_1.out", 1, "video_1.out");
    failures += 1 - runTest("name_NumberExtenson_File2", "video_1.out", 2, "video_2.out");

    failures += 1 - runTest("name_NoNumberExtenson_File1", "audio.out", 1, "audio.out");
    failures += 1 - runTest("name_NoNumberExtenson_File2", "audio.out", 2, "audio_2.out");

    failures += 1 - runTest("name_NumberNoExtenson_File1", "video_1", 1, "video_1");
    failures += 1 - runTest("name_NumberNoExtenson_File2", "video_1", 2, "video_2");

    failures += 1 - runTest("name_NoNumberNoExtenson_File1", "audio", 1, "audio");
    failures += 1 - runTest("name_NoNumberNoExtenson_File2", "audio", 2, "audio_2");

    failures += 1 - runTest("name_Uninitialized_File1", "", 1, "");
    failures += 1 - runTest("name_Uninitialized_File2", "", 2, "");

    return failures;
}