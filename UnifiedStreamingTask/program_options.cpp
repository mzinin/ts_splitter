#include "error.hpp"
#include "program_options.hpp"

#include <cstring>
#include <sstream>


namespace
{
    /// @brief Default audio output name.
    const std::string audioDefaultOutput = "audio_1.out";

    /// @brief Default video output name.
    const std::string videoDefaultOutput = "video_1.out";

    /// @brief Check if argument is an option (key).
    bool isOption(const char* arg)
    {
        return arg[0] == '-';
    }
}

ProgramOptions::ProgramOptions(const std::string& executableName)
    : executableName_(executableName)
{}

void ProgramOptions::init(int argc, const char* const * argv)
{
    int i = 1;
    while (i < argc)
    {
        const char* arg = argv[i];
        if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0)
        {
            helpRequested_ = true;
            break;
        }

        if (!isOption(arg))
        {
            helpRequested_ = true;
            throw Error(Error::ARGUMENT_WITHOUT_OPTION, arg);
        }

        if (i + 1 >= argc || isOption(argv[i + 1]))
        {
            helpRequested_ = true;
            throw Error(Error::NO_OPTION_ARGUMENT, arg);
        }

        if (strcmp(arg, "-i") == 0)
            inputName_ = argv[i + 1];
        else if (strcmp(arg, "-oa") == 0)
            audioOutputName_ = argv[i + 1];
        else if (strcmp(arg, "-ov") == 0)
            videoOutputName_ = argv[i + 1];
        else
        {
            helpRequested_ = true;
            throw Error(Error::UNKNOWN_OPTION, arg);
        }

        i += 2;
    }

    if (!helpRequested_ && audioOutputName_.empty() && videoOutputName_.empty())
    {
        audioOutputName_ = audioDefaultOutput;
        videoOutputName_ = videoDefaultOutput;
    }
}

bool ProgramOptions::helpRequested() const
{
    return helpRequested_;
}

std::string ProgramOptions::usage() const
{
    std::ostringstream buffer;

    buffer << "Usage: " << executableName_ << " [-i <input_file>] [-oa <audio_output>] [-ov <video_output>]\n"
           << "\nSplit TS file into raw audio and/or video tracks.\n\n"

           << "  -i\t\tInput file to split. If omitted, STDIN is used.\n\n"

           << "  -oa\t\tOutput file for 1st audio track. All other audio tracks are saved into files \n"
           << "\t\twith the save name and suffix:\n"
           << "\t\t-oa audio.out will produce files 'audio.out', 'audio_2.out', etc.\n"
           << "\t\t-oa audio_1.out will produce files 'audio_1.out', 'audio_2.out', etc.\n"
           << "\t\tIf omitted but video output file is set, no audio output is written. \n"
           << "\t\tIf both omitted, '" << audioDefaultOutput << "' is used by default.\n\n"

           << "  -ov\t\tOutput file for 1st video track. All other video tracks are saved into files \n"
           << "\t\twith the save name and suffix:\n"
           << "\t\t-oa video.out will produce files 'video.out', 'video_2.out', etc.\n"
           << "\t\t-oa video_1.out will produce files 'video_1.out', 'video_2.out', etc.\n"
           << "\t\tIf omitted but audio output file is set, no video output is written. \n"
           << "\t\tIf both omitted, '" << videoDefaultOutput << "' is used by default.\n\n"

           << "-h, --help\tShow this message and exit.";

    return buffer.str();
}

const std::string& ProgramOptions::inputName() const
{
    return inputName_;
}

const std::string& ProgramOptions::audioOutputName() const
{
    return audioOutputName_;
}

const std::string& ProgramOptions::videoOutputName() const
{
    return videoOutputName_;
}
