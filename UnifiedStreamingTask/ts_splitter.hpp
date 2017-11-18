#pragma once

#include "program_options.hpp"

#include <fstream>
#include <memory>


/// @class TsSplitter.
/// @brief Reads TS stream and splits it according to command line arguments.
class TsSplitter
{
public:
    /// @brief Parse command line options and values.
    /// @param[in] argc - Number of arguments.
    /// @param[in] argv - Arguments.
    void init(int argc, char** argv);

    /// @brief Split TS input.
    /// @returns true if input successfully split, false other wise.
    bool run();

private:
    /// @brief Open input file if any provided.
    /// @throws Error.
    void openInput();

    /// @brief Do the splitting.
    /// @throws Error.
    void splitInput();

private:
    /// @class Program options parsed from command line.
    std::unique_ptr<ProgramOptions> programOptions_;

    /// @brief Input file stream.
    std::unique_ptr<std::ifstream> input_;
};
