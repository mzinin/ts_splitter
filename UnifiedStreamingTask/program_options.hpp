#pragma once

#include <string>


/// @class ProgramOptions.
/// @brief Parse command line options and values.
/// @details Supports options '-i', '-oa', '-ov' - with argument and '-h', '--help' - without one.
class ProgramOptions
{
public:
    /// @brief Constructor.
    /// @param[in] executableName - Name of current executable file.
    ProgramOptions(const std::string& executableName);

    /// @brief Parse command line arguments and init class intance.
    /// @param[in] argc - Number of arguments.
    /// @param[in] argv - Arguments.
    /// @throws Error.
    void init(int argc, const char* const * argv);

    /// @brief Check if usage help is requested.
    /// @details Help can required either explicitly, or implicitly - when error occures.
    bool helpRequested() const;

    /// @brief Get usage text;
    std::string usage() const;

    /// @brief Get input file name, can be empty.
    const std::string& inputName() const;

    /// @brief Get audio output name, can be empty.
    const std::string& audioOutputName() const;

    /// @brief Get video output name, can be empty.
    const std::string& videoOutputName() const;

private:
    /// @brief Executable file name.
    const std::string executableName_;

    /// @If set - help is required.
    bool helpRequested_ = false;

    /// @brief Parsed input name.
    std::string inputName_;

    /// @brief Parsed audio output name.
    std::string audioOutputName_;

    /// @brief Parsed video output name.
    std::string videoOutputName_;
};
