#include "error.hpp"
#include "output_writer.hpp"

#include <list>
#include <sstream>


OutputWriter::OutputWriter(std::ostream& log,
                           const OutputNameGenerator& audioNameGenerator,
                           const OutputNameGenerator& videoNameGenerator)
    : log_(log)
    , audioNameGenerator_(audioNameGenerator)
    , videoNameGenerator_(videoNameGenerator)
{
    if (!log_.good())
        throw Error(Error::CONSTRUCTION_ERROR, "OutputWriter, bad log output");
    if (audioNameGenerator_.name(0).empty() && videoNameGenerator_.name(0).empty())
        throw Error(Error::CONSTRUCTION_ERROR, "OutputWriter, both name generators are uninitialized");
}

OutputWriter::~OutputWriter()
{
    try
    {
        closeOutputs();
    }
    catch (const Error& err)
    {
        log_ << "Error: OutputWriter, failed to close output streams, " << err.message() << std::endl;
    }
}

void OutputWriter::write(const EsRawData& rawData)
{
    auto& output = chooseOutput(rawData.type, rawData.esNumber);
    if (!output.stream)
        return;

    output.stream->write(reinterpret_cast<const char*>(rawData.data), rawData.size);
    if (!output.stream->good())
        throw Error(Error::CORRUPTED_OUTPUT, "OutputWriter, failed to write into file '" + output.file + "'");
}

void OutputWriter::closeOutputs()
{
    // to collect names of failed files
    std::list<std::string> failedFiles;
    auto closeOutput = [&failedFiles](Output& output)
    {
        if (!output.stream)
            return;
        output.stream->close();
        if (!output.stream->good())
            failedFiles.push_back(output.file);
        output.stream.reset();
    };

    for (auto& pair : audioOutputs_)
        closeOutput(pair.second);
    for (auto& pair : videoOutputs_)
        closeOutput(pair.second);

    if (!failedFiles.empty())
    {
        std::ostringstream msg;
        msg << "OutputWriter, failed to close file(s) ";

        auto end = --failedFiles.cend();
        for (auto it = failedFiles.cbegin(); it != end; ++it)
            msg << "'" << *it << "',";
        msg << "'" << *end << "'";

        throw Error(Error::CORRUPTED_OUTPUT, msg.str());
    }
}

OutputWriter::Output& OutputWriter::chooseOutput(EsType type, uint16_t number)
{
    // dummy output for non-audio and non-video ES
    static Output dummyOutput{ "", nullptr };

    std::map<uint16_t, Output>* outputs = nullptr;
    const OutputNameGenerator* generator = nullptr;

    if (type == EsType::AUDIO)
    {
        outputs = &audioOutputs_;
        generator = &audioNameGenerator_;
    }
    else if (type == EsType::VIDEO)
    {
        outputs = &videoOutputs_;
        generator = &videoNameGenerator_;
    }
    else
        return dummyOutput;

    // try insert new output
    const auto insertionResult = outputs->insert(std::make_pair(number, Output{ generator->name(number), nullptr }));
    auto& output = insertionResult.first->second;

    // ES already detected or no output needed for this ES
    if (!insertionResult.second || output.file.empty())
        return insertionResult.first->second;

    // try to open new file for write
    output.stream.reset(new std::ofstream(output.file, std::fstream::out | std::fstream::binary));
    if (!output.stream->good())
    {
        output.stream.reset();
        throw Error(Error::CORRUPTED_OUTPUT, "OutputWriter, failed to open file '" + output.file + "' for writing");
    }

    return output;
}
