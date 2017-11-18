#pragma once

#include "message_types.hpp"
#include "output_name_generator.hpp"

#include <fstream>
#include <map>
#include <memory>


/// @class OutputWriter.
/// @brief Write ES raw data into files.
class OutputWriter
{
public:
    /// @brief Constructor.
    /// @param[out] log - Stream for log messages.
    /// @param[in] audioNameGenerator - Generator for audio output file names.
    /// @param[in] videoNameGenerator - Generator for video output file names.
    /// @throws Error.
    OutputWriter(std::ostream& log,
                 const OutputNameGenerator& audioNameGenerator,
                 const OutputNameGenerator& videoNameGenerator);

    /// @brief Desctructor.
    ~OutputWriter();

    /// @brief Write raw data.
    /// @param[in] rawData - ES raw data.
    /// @throws Error in case of corrupted output streams.
    void write(const EsRawData& rawData);

    /// @brief Close output streams.
    /// @throws Error in case of corrupted output streams.
    void closeOutputs();

private:
    /// @brief Output for every ES.
    struct Output
    {
        /// @brief Output file name.
        std::string file;

        /// @brief Output file stream.
        std::unique_ptr<std::ofstream> stream;
    };

    /// @brief Choose or open output stream for ES.
    /// @param[in] type - Type of ES.
    /// @param[in] number - Sequence number of ES.
    /// @throws Error if fails to open file stream.
    Output& chooseOutput(EsType type, uint16_t number);

private:
    /// @brief Log output stream.
    std::ostream& log_;

    /// @brief Generator for audio output file names.
    const OutputNameGenerator& audioNameGenerator_;

    /// @brief Generator for video output file names.
    const OutputNameGenerator& videoNameGenerator_;

    /// @brief Outputs for detected audio ES.
    std::map<uint16_t, Output> audioOutputs_;

    /// @brief Outputs for detected video ES.
    std::map<uint16_t, Output> videoOutputs_;
};
