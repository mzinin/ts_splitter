#include "../error.hpp"

#include <iostream>
#include <sstream>


namespace
{
    /// @brief Type of Erro test
    enum TestType
    {
        WHAT,	///< To test what() method.
        CODE,   ///< To test code() method.
        MESSAGE ///< To test message() method.
    };

    /// @brief Run one Error unit test.
    /// @returns true if test passed, false otherwise.
    bool runTest(const std::string& testName,
                 TestType testType,
                 uint16_t errorCode,
                 const std::string& errorMsg)
    {
        std::cout << "Running Error." << testName << " ... ";

        Error err(errorCode, errorMsg);

        std::string expectedMsg = Error::codeDescription(errorCode);
        if (!errorMsg.empty())
            expectedMsg += ": " + errorMsg;

        bool result = true;
        std::ostringstream failureDescription;

        switch (testType)
        {
        case WHAT:
            result = expectedMsg == err.what();
            if (!result)
                failureDescription << "Got what '" << err.what() << "' instead of '" << expectedMsg << "'" << std::endl;
            break;

        case CODE:
            result = errorCode == err.code();
            if (!result)
                failureDescription << "Got code " << err.code() << " instead of " << errorCode << std::endl;
            break;

        case MESSAGE:
            result = expectedMsg == err.message();
            if (!result)
                failureDescription << "Got message '" << err.message() << "' instead of '" << expectedMsg << "'" << std::endl;
            break;
        }

        std::cout << (result ? "OK" : "FAIL") << std::endl;
        if (!result)
            std::cout << failureDescription.str();

        return result;
    }
}

/// @brief Run all Error unit tests.
/// @returns Number of failed tests.
uint16_t testError()
{
    uint16_t failures = 0;
    const uint16_t badErrorCode = -1;

    failures += 1 - runTest("what_NoErrorNoMessage", WHAT, Error::OK, "");
    failures += 1 - runTest("what_ErrorNoMessage", WHAT, Error::NO_OPTION_ARGUMENT, "");
    failures += 1 - runTest("what_UnknownErrorNoMessage", WHAT, badErrorCode, "");

    failures += 1 - runTest("what_NoErrorMessage", WHAT, Error::OK, "additional info");
    failures += 1 - runTest("what_ErrorMessage", WHAT, Error::NO_OPTION_ARGUMENT, "additional info");
    failures += 1 - runTest("what_UnknownErrorMessage", WHAT, badErrorCode, "additional info");

    failures += 1 - runTest("code_NoErrorNoMessage", CODE, Error::OK, "");
    failures += 1 - runTest("code_ErrorNoMessage", CODE, Error::NO_OPTION_ARGUMENT, "");
    failures += 1 - runTest("code_UnknownErrorNoMessage", CODE, badErrorCode, "");

    failures += 1 - runTest("code_NoErrorMessage", CODE, Error::OK, "additional info");
    failures += 1 - runTest("code_ErrorMessage", CODE, Error::NO_OPTION_ARGUMENT, "additional info");
    failures += 1 - runTest("code_UnknownErrorMessage", CODE, badErrorCode, "additional info");

    failures += 1 - runTest("message_NoErrorNoMessage", MESSAGE, Error::OK, "");
    failures += 1 - runTest("message_ErrorNoMessage", MESSAGE, Error::NO_OPTION_ARGUMENT, "");
    failures += 1 - runTest("message_UnknownErrorNoMessage", MESSAGE, badErrorCode, "");

    failures += 1 - runTest("message_NoErrorMessage", MESSAGE, Error::OK, "additional info");
    failures += 1 - runTest("message_ErrorMessage", MESSAGE, Error::NO_OPTION_ARGUMENT, "additional info");
    failures += 1 - runTest("message_UnknownErrorMessage", MESSAGE, badErrorCode, "additional info");

    return failures;
}
