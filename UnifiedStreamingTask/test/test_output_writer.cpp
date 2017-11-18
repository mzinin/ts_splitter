#include "../error.hpp"
#include "../output_writer.hpp"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <vector>


// audio raw data
namespace
{
    const std::vector<uint8_t> audioRawData1{ 0xFF, 0xF1, 0x4C, 0x80, 0x04, 0x3F, 0xFC, 0xDE, 0x04, 0x00, 0x00, 0x6C, 0x69, 0x62, 0x66, 0x61,
                                              0x61, 0x63, 0x20, 0x31, 0x2E, 0x32, 0x38, 0x00, 0x00, 0x42, 0x00, 0x93, 0x20, 0x04, 0x32, 0x00,
                                              0x47, 0xFF, 0xF1, 0x4C, 0x80, 0x02, 0x1F, 0xFC, 0x21, 0x00, 0x49, 0x90, 0x02, 0x19, 0x00, 0x23,
                                              0x80, 0xFF, 0xF1, 0x4C, 0x80, 0x02, 0x1F, 0xFC, 0x21, 0x00, 0x49, 0x90, 0x02, 0x19, 0x00, 0x23,
                                              0x80, 0xFF, 0xF1, 0x4C, 0x80, 0x02, 0x1F, 0xFC, 0x21, 0x00, 0x49, 0x90, 0x02, 0x19, 0x00, 0x23,
                                              0x80, 0xFF, 0xF1, 0x4C, 0x80, 0x02, 0x1F, 0xFC, 0x21, 0x00, 0x49, 0x90, 0x02, 0x19, 0x00, 0x23,
                                              0x80, 0xFF, 0xF1, 0x4C, 0x80, 0x02, 0x1F, 0xFC, 0x21, 0x00, 0x49, 0x90, 0x02, 0x19, 0x00, 0x23,
                                              0x80, 0xFF, 0xF1, 0x4C, 0x80, 0x02, 0x1F, 0xFC, 0x21, 0x00, 0x49, 0x90, 0x02, 0x19, 0x00, 0x23,
                                              0x80, 0xFF, 0xF1, 0x4C, 0x80, 0x02, 0x1F, 0xFC, 0x21, 0x00, 0x49, 0x90, 0x02, 0x19, 0x00, 0x23,
                                              0x80, 0xFF, 0xF1, 0x4C, 0x80, 0x02, 0x1F, 0xFC, 0x21, 0x00, 0x49, 0x90, 0x02, 0x19, 0x00, 0x23,
                                              0x80, 0xFF, 0xF1, 0x4C, 0x80, 0x02, 0x1F, 0xFC };

    const std::vector<uint8_t> audioRawData2{ 0x21, 0x00, 0x49, 0x90, 0x02, 0x19, 0x00, 0x23, 0x80, 0xFF, 0xF1, 0x4C, 0x80, 0x02, 0x1F, 0xFC,
                                              0x21, 0x00, 0x49, 0x90, 0x02, 0x19, 0x00, 0x23, 0x80, 0xFF, 0xF1, 0x4C, 0x80, 0x02, 0x1F, 0xFC,
                                              0x21, 0x00, 0x49, 0x90, 0x02, 0x19, 0x00, 0x23, 0x80, 0xFF, 0xF1, 0x4C, 0x80, 0x02, 0x1F, 0xFC,
                                              0x21, 0x00, 0x49, 0x90, 0x02, 0x19, 0x00, 0x23, 0x80, 0xFF, 0xF1, 0x4C, 0x80, 0x02, 0x1F, 0xFC,
                                              0x21, 0x00, 0x49, 0x90, 0x02, 0x19, 0x00, 0x23, 0x80, 0xFF, 0xF1, 0x4C, 0x80, 0x02, 0x1F, 0xFC,
                                              0x21, 0x00, 0x49, 0x90, 0x02, 0x19, 0x00, 0x23, 0x80, 0xFF, 0xF1, 0x4C, 0x80, 0x02, 0x1F, 0xFC,
                                              0x21, 0x00, 0x49, 0x90, 0x02, 0x19, 0x00, 0x23, 0x80, 0xFF, 0xF1, 0x4C, 0x80, 0x02, 0x1F, 0xFC,
                                              0x21, 0x00, 0x49, 0x90, 0x02, 0x19, 0x00, 0x23, 0x80, 0xFF, 0xF1, 0x4C, 0x80, 0x02, 0x1F, 0xFC,
                                              0x21, 0x00, 0x49, 0x90, 0x02, 0x19, 0x00, 0x23, 0x80, 0xFF, 0xF1, 0x4C, 0x80, 0x02, 0x1F, 0xFC,
                                              0x21, 0x00, 0x49, 0x90, 0x02, 0x19, 0x00, 0x23, 0x80, 0xFF, 0xF1, 0x4C, 0x80, 0x02, 0x1F, 0xFC,
                                              0x21, 0x00, 0x49, 0x90, 0x02, 0x19, 0x00, 0x23, 0x80, 0xFF, 0xF1, 0x4C, 0x80, 0x02, 0x1F, 0xFC,
                                              0x21, 0x00, 0x49, 0x90, 0x02, 0x19, 0x00, 0x23 };
}

// video raw data
namespace
{
    const std::vector<uint8_t> videoRawData1{ 0x00, 0x00, 0x00, 0x01, 0x09, 0xF0, 0x00, 0x00, 0x00, 0x01, 0x67, 0x42, 0xC0, 0x14, 0xDB, 0x04,
                                                0x04, 0xE8, 0x40, 0x00, 0xCB, 0x73, 0x40, 0x26, 0x25, 0xA0, 0x07, 0xC5, 0x0A, 0xB8, 0x00, 0x00,
                                              0x00, 0x01, 0x68, 0xCA, 0x8C, 0xB2, 0x00, 0x00, 0x01, 0x06, 0x05, 0xFF, 0xFF, 0x66, 0xDC, 0x45,
                                              0xE9, 0xBD, 0xE6, 0xD9, 0x48, 0xB7, 0x96, 0x2C, 0xD8, 0x20, 0xD9, 0x23, 0xEE, 0xEF, 0x78, 0x32,
                                              0x36, 0x34, 0x20, 0x2D, 0x20, 0x63, 0x6F, 0x72, 0x65, 0x20, 0x31, 0x32, 0x30, 0x20, 0x72, 0x32,
                                              0x31, 0x35, 0x31, 0x20, 0x61, 0x33, 0x66, 0x34, 0x34, 0x30, 0x37, 0x20, 0x2D, 0x20, 0x48, 0x2E,
                                              0x32, 0x36, 0x34, 0x2F, 0x4D, 0x50, 0x45, 0x47, 0x2D, 0x34, 0x20, 0x41, 0x56, 0x43, 0x20, 0x63,
                                              0x6F, 0x64, 0x65, 0x63, 0x20, 0x2D, 0x20, 0x43, 0x6F, 0x70, 0x79, 0x6C, 0x65, 0x66, 0x74, 0x20,
                                              0x32, 0x30, 0x30, 0x33, 0x2D, 0x32, 0x30, 0x31, 0x31, 0x20, 0x2D, 0x20, 0x68, 0x74, 0x74, 0x70,
                                              0x3A, 0x2F, 0x2F, 0x77, 0x77, 0x77, 0x2E, 0x76, 0x69, 0x64, 0x65, 0x6F, 0x6C, 0x61, 0x6E, 0x2E,
                                              0x6F, 0x72 };

    const std::vector<uint8_t> videoRawData2{ 0x67, 0x2F, 0x78, 0x32, 0x36, 0x34, 0x2E, 0x68, 0x74, 0x6D, 0x6C, 0x20, 0x2D, 0x20, 0x6F, 0x70,
                                              0x74, 0x69, 0x6F, 0x6E, 0x73, 0x3A, 0x20, 0x63, 0x61, 0x62, 0x61, 0x63, 0x3D, 0x30, 0x20, 0x72,
                                              0x65, 0x66, 0x3D, 0x32, 0x20, 0x64, 0x65, 0x62, 0x6C, 0x6F, 0x63, 0x6B, 0x3D, 0x31, 0x3A, 0x30,
                                              0x3A, 0x30, 0x20, 0x61, 0x6E, 0x61, 0x6C, 0x79, 0x73, 0x65, 0x3D, 0x30, 0x78, 0x31, 0x3A, 0x30,
                                              0x78, 0x31, 0x31, 0x31, 0x20, 0x6D, 0x65, 0x3D, 0x68, 0x65, 0x78, 0x20, 0x73, 0x75, 0x62, 0x6D,
                                              0x65, 0x3D, 0x36, 0x20, 0x70, 0x73, 0x79, 0x3D, 0x31, 0x20, 0x70, 0x73, 0x79, 0x5F, 0x72, 0x64,
                                              0x3D, 0x31, 0x2E, 0x30, 0x30, 0x3A, 0x30, 0x2E, 0x30, 0x30, 0x20, 0x6D, 0x69, 0x78, 0x65, 0x64,
                                              0x5F, 0x72, 0x65, 0x66, 0x3D, 0x31, 0x20, 0x6D, 0x65, 0x5F, 0x72, 0x61, 0x6E, 0x67, 0x65, 0x3D,
                                              0x31, 0x36, 0x20, 0x63, 0x68, 0x72, 0x6F, 0x6D, 0x61, 0x5F, 0x6D, 0x65, 0x3D, 0x31, 0x20, 0x74,
                                              0x72, 0x65, 0x6C, 0x6C, 0x69, 0x73, 0x3D, 0x31, 0x20, 0x38, 0x78, 0x38, 0x64, 0x63, 0x74, 0x3D,
                                              0x30, 0x20, 0x63, 0x71, 0x6D, 0x3D, 0x30, 0x20, 0x64, 0x65, 0x61, 0x64, 0x7A, 0x6F, 0x6E, 0x65,
                                              0x3D, 0x32, 0x31, 0x2C, 0x31, 0x31, 0x20, 0x66 };
}

namespace
{
    /// @brief Expected result for OutputWriter test.
    struct ExpectedResult
    {
        /// @brief Error code. If OK - no error expected.
        uint16_t errorCode;

        /// @brief Expected output file names and contents.
        ///        If content is empty, file shouldn't exist.
        std::map<std::string, std::string> outputs;
    };

    /// @brief Check file existance (or non-existance) and content.
    void checkFile(const std::string& name, const std::string& expected)
    {
        std::ifstream file(name, std::ifstream::in | std::ifstream::binary);
        if (!file.good())
        {
            if (!expected.empty())
                throw std::logic_error("Failed to open file '" + name + "'");
            return;
        }

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        if (!file.good())
            throw std::logic_error("Failed to read from file '" + name + "'");

        if (content != expected)
            throw std::logic_error("File '" + name + "' has wrong content");
    }

    /// @brief Run one OutputWriter unit test.
    /// @returns true if test passed, false otherwise.
    bool runTest(const std::string& testName,
                 const OutputNameGenerator& audioGenerator,
                 const OutputNameGenerator& videoGenerator,
                 const std::vector<EsRawData>& input,
                 const ExpectedResult& expected)
    {
        std::cout << "Running OutputWriter." << testName << " ... ";

        bool result = true;
        Error error{ Error::OK, "" };
        std::ostringstream log;

        try
        {
            OutputWriter writer(log, audioGenerator, videoGenerator);
            for (const auto& data : input)
                writer.write(data);
            writer.closeOutputs();
        }
        catch (const Error& err)
        {
            error = err;
        }
        catch (const std::exception& e)
        {
            result = false;
            log << "Unexpected exception caught: " << e.what() << std::endl;
        }

        if (error.code() != expected.errorCode)
        {
            result = false;
            if (expected.errorCode == Error::OK)
                log << "Unexpected exception caught: " << error.message() << std::endl;
            else
                log << "No expected exception caught" << std::endl;
        }
        for (const auto& pair : expected.outputs)
        {
            try
            {
                checkFile(pair.first, pair.second);
            }
            catch (const std::exception& e)
            {
                result = false;
                log << "Wrong output: " << e.what() << std::endl;
            }
            std::remove(pair.first.c_str());
        }

        std::cout << (result ? "OK" : "FAIL") << std::endl;
        if (!result)
            std::cout << log.str();
        return result;
    }
}

/// @brief Run all OutputWriter unit tests.
/// @returns Number of failed tests.
uint16_t testOutputWriter()
{
    uint16_t failures = 0;

    // no output names
    {
        OutputNameGenerator audioNamer;
        OutputNameGenerator videoNamer;
        std::vector<EsRawData> rawData;
        ExpectedResult expected{ Error::CONSTRUCTION_ERROR };
        failures += 1 - runTest("ctor_UninitialisedNames_Exception", audioNamer, videoNamer, rawData, expected);
    }

    // 1 video raw data
    {
        OutputNameGenerator audioNamer;
        OutputNameGenerator videoNamer("video_1.out");
        std::vector<EsRawData> rawData;
        rawData.push_back({ videoRawData1.data(), static_cast<uint16_t>(videoRawData1.size()), EsType::VIDEO, 1 });
        ExpectedResult expected{ Error::OK };
        expected.outputs[videoNamer.name(1)] = std::string(videoRawData1.begin(), videoRawData1.end());
        failures += 1 - runTest("write_Video1RawData_OK", audioNamer, videoNamer, rawData, expected);
    }

    // 2 video raw data
    {
        OutputNameGenerator audioNamer;
        OutputNameGenerator videoNamer("video_1.out");
        std::vector<EsRawData> rawData;
        rawData.push_back({ videoRawData1.data(), static_cast<uint16_t>(videoRawData1.size()), EsType::VIDEO, 1 });
        rawData.push_back({ videoRawData2.data(), static_cast<uint16_t>(videoRawData2.size()), EsType::VIDEO, 1 });
        ExpectedResult expected{ Error::OK };
        expected.outputs[videoNamer.name(1)] = std::string(videoRawData1.begin(), videoRawData1.end()) +
                                                  std::string(videoRawData2.begin(), videoRawData2.end());
        failures += 1 - runTest("write_Video2RawData_OK", audioNamer, videoNamer, rawData, expected);
    }

    // audio and video raw data
    {
        OutputNameGenerator audioNamer("audio_1.out");
        OutputNameGenerator videoNamer("video_1.out");
        std::vector<EsRawData> rawData;
        rawData.push_back({ videoRawData1.data(), static_cast<uint16_t>(videoRawData1.size()), EsType::VIDEO, 1 });
        rawData.push_back({ videoRawData2.data(), static_cast<uint16_t>(videoRawData2.size()), EsType::VIDEO, 1 });
        rawData.push_back({ audioRawData1.data(), static_cast<uint16_t>(audioRawData1.size()), EsType::AUDIO, 1 });
        rawData.push_back({ audioRawData2.data(), static_cast<uint16_t>(audioRawData2.size()), EsType::AUDIO, 1 });
        ExpectedResult expected{ Error::OK };
        expected.outputs[videoNamer.name(1)] = std::string(videoRawData1.begin(), videoRawData1.end()) +
                                               std::string(videoRawData2.begin(), videoRawData2.end());
        expected.outputs[audioNamer.name(1)] = std::string(audioRawData1.begin(), audioRawData1.end()) +
                                               std::string(audioRawData2.begin(), audioRawData2.end());
        failures += 1 - runTest("write_AudioAndVideoRawData_OK", audioNamer, videoNamer, rawData, expected);
    }

    // 2 video outputs
    {
        OutputNameGenerator audioNamer;
        OutputNameGenerator videoNamer("video_1.out");
        std::vector<EsRawData> rawData;
        rawData.push_back({ videoRawData1.data(), static_cast<uint16_t>(videoRawData1.size()), EsType::VIDEO, 1 });
        rawData.push_back({ videoRawData2.data(), static_cast<uint16_t>(videoRawData2.size()), EsType::VIDEO, 2 });
        ExpectedResult expected{ Error::OK };
        expected.outputs[videoNamer.name(1)] = std::string(videoRawData1.begin(), videoRawData1.end());
        expected.outputs[videoNamer.name(2)] = std::string(videoRawData2.begin(), videoRawData2.end());
        failures += 1 - runTest("write_Video2Outputs_OK", audioNamer, videoNamer, rawData, expected);
    }

    // discard audio
    {
        OutputNameGenerator audioNamer;
        OutputNameGenerator videoNamer("video_1.out");
        std::vector<EsRawData> rawData;
        rawData.push_back({ videoRawData1.data(), static_cast<uint16_t>(videoRawData1.size()), EsType::VIDEO, 1 });
        rawData.push_back({ videoRawData2.data(), static_cast<uint16_t>(videoRawData2.size()), EsType::VIDEO, 1 });
        rawData.push_back({ audioRawData1.data(), static_cast<uint16_t>(audioRawData1.size()), EsType::AUDIO, 1 });
        rawData.push_back({ audioRawData2.data(), static_cast<uint16_t>(audioRawData2.size()), EsType::AUDIO, 1 });
        ExpectedResult expected{ Error::OK };
        expected.outputs[videoNamer.name(1)] = std::string(videoRawData1.begin(), videoRawData1.end()) +
                                               std::string(videoRawData2.begin(), videoRawData2.end());
        expected.outputs[audioNamer.name(1)] = std::string();
        failures += 1 - runTest("write_DiscardAudio_OK", audioNamer, videoNamer, rawData, expected);
    }

    return failures;
}
