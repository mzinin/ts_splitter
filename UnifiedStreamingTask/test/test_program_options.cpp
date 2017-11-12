#include "../error.hpp"
#include "../program_options.hpp"

#include <iostream>
#include <sstream>
#include <vector>


namespace
{
	/// @brief Expected result for ProgramOptions test.
	struct ExpectedResult
	{
		/// @brief Error code. If OK - no error expected.
		uint16_t errorCode;

		/// @brief Request for help.
		bool helpRequested;

		/// @brief Input file name.
		std::string inputName;

		/// @brief Audio output name.
		std::string audioOutputName;

		/// @brief Video output name.
		std::string videoOutputName;
	};

	/// @brief Run one Error unit test.
	/// @returns true if test passed, false otherwise.
	bool runTest(const std::string& testName,
		         const std::vector<std::string>& args,
				 const ExpectedResult& expected)
	{
		std::cout << "Running ProgramOptions." << testName << " ... ";

		if (args.empty())
		{
			std::cout << "WRONG TEST DATA" << std::endl;
			return false;
		}

		bool result = true;
		std::ostringstream failureDescription;
		uint16_t errorCode = Error::OK;

		ProgramOptions po(args.front());
		try
		{
			std::vector<const char*> argv(args.size());
			for (size_t i = 0; i < args.size(); ++i)
				argv[i] = args[i].c_str();

			po.init(argv.size(), argv.data());
		}
		catch (const Error& err)
		{
			errorCode = err.code();
		}

		if (errorCode != expected.errorCode)
		{
			result = false;
			if (expected.errorCode == Error::OK)
				failureDescription << "Unexpected exception caught: " << errorCode << std::endl;
			else
				failureDescription << "No expected exception caught" << std::endl;
		}
		if (po.helpRequested() != expected.helpRequested)
		{
			result = false;
			failureDescription << "Got help requested " << po.helpRequested() << " instead of " << expected.helpRequested << std::endl;
		}
		if (po.inputName() != expected.inputName)
		{
			result = false;
			failureDescription << "Got input name '" << po.inputName() << "' instead of '" << expected.inputName << "'" << std::endl;
		}
		if (po.audioOutputName() != expected.audioOutputName)
		{
			result = false;
			failureDescription << "Got audio output '" << po.audioOutputName() << "' instead of '" << expected.audioOutputName << "'" << std::endl;
		}
		if (po.videoOutputName() != expected.videoOutputName)
		{
			result = false;
			failureDescription << "Got video output '" << po.videoOutputName() << "' instead of '" << expected.videoOutputName << "'" << std::endl;
		}


		std::cout << (result ? "OK" : "FAIL") << std::endl;
		if (!result)
			std::cout << failureDescription.str();

		return result;
	}
}

/// @brief Run all ProgramOptions unit tests.
/// @returns Number of failed tests.
uint16_t testProgramOptions()
{
	uint16_t failures = 0;

	// test combinations of input and output names
	std::vector<std::string> args = { "ts_plitter" };
	ExpectedResult expected = { Error::OK, false, "", "audio_1.out", "video_1.out" };
	failures += 1 - runTest("NoInput_NoAudio_NoVideo", args, expected);

	args = { "ts_plitter", "-i", "input.ts" };
	expected = { Error::OK, false, "input.ts", "audio_1.out", "video_1.out" };
	failures += 1 - runTest("Input_NoAudio_NoVideo", args, expected);

	args = { "ts_plitter", "-oa", "audio.out" };
	expected = { Error::OK, false, "", "audio.out", "" };
	failures += 1 - runTest("NoInput_Audio_NoVideo", args, expected);

	args = { "ts_plitter", "-i", "intput.ts", "-ov", "video.out" };
	expected = { Error::OK, false, "intput.ts", "", "video.out" };
	failures += 1 - runTest("Input_NoAudio_Video", args, expected);

	args = { "ts_plitter", "-oa", "audio.out", "-ov", "video.out" };
	expected = { Error::OK, false, "", "audio.out", "video.out" };
	failures += 1 - runTest("NoInput_Audio_Video", args, expected);

	// test help requests
	args = { "ts_plitter", "-h" };
	expected = { Error::OK, true, "", "", "" };
	failures += 1 - runTest("ShortHelp_NoArgs", args, expected);

	args = { "ts_plitter", "--help", "-i", "intput.ts" };
	expected = { Error::OK, true, "", "", "" };
	failures += 1 - runTest("LongHelp_Args", args, expected);

	args = { "ts_plitter", "-i", "intput.ts", "-h" };
	expected = { Error::OK, true, "intput.ts", "", "" };
	failures += 1 - runTest("Args_ShortHelp", args, expected);

	args = { "ts_plitter", "-i", "intput.ts", "-h", "-oa", "audio.out" };
	expected = { Error::OK, true, "intput.ts", "", "" };
	failures += 1 - runTest("Args_ShortHelp_Args", args, expected);

	// test exceptions throwing
	args = { "ts_plitter", "-i", "-oa", "audio.out" };
	expected = { Error::NO_OPTION_ARGUMENT, true, "", "", "" };
	failures += 1 - runTest("ErrorNoArgument", args, expected);

	args = { "ts_plitter", "-i", "intput.ts", "audio.out" };
	expected = { Error::ARGUMENT_WITHOUT_OPTION, true, "intput.ts", "", "" };
	failures += 1 - runTest("ErrorNoOption", args, expected);

	// test repeated options
	args = { "ts_plitter", "-i", "intput1.ts", "-oa", "audio.out", "-i", "intput2.ts" };
	expected = { Error::OK, false, "intput2.ts", "audio.out", "" };
	failures += 1 - runTest("RepeatedInput", args, expected);

	args = { "ts_plitter", "-ov", "video_1.out", "-ov", "video_2.out" };
	expected = { Error::OK, false, "", "", "video_2.out" };
	failures += 1 - runTest("RepeatedVideo", args, expected);

	return failures;
}