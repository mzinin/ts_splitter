#pragma once

#include <string>


/// @class OutputNameGenerator
/// @brief Generates file names based on given example.
/// @details File names are generated is the form "<example>_<seq.number>.<extension>".
///          First file will not has suffix "_1", if given example has no this suffix.
///          If given example is empty, generator is uninitialized and will always generate empty names.
class OutputNameGenerator
{
public:
    /// @brief Constructor.
    /// @param[in] nameExample - Given example of output file name.
    OutputNameGenerator(const std::string& nameExample = std::string());

    /// @brief Generate name for file.
    /// @param[in] fileNumber - File sequence number.
    /// @returns File name.
    std::string name(uint16_t fileNumber) const;

private:
    /// @brief Base name for all generated file names.
    std::string baseName_;

    /// @brief Extension for all generated file names.
    std::string extension_;

    /// @brief If set, first file name has suffix "_1".
    bool firstFileWithNumber_ = false;
};