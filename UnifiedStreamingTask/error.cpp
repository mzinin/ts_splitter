#include "error.hpp"


Error::Error(uint16_t code, const std::string& msg)
    : code_(code)
    , message_(codeDescription(code))
{
    if (!msg.empty())
        message_ += ": " + msg;
}

const char* Error::what() const noexcept
{
    return message_.c_str();
}

uint16_t Error::code() const
{
    return code_;
}

const std::string& Error::message() const
{
    return message_;
}

std::string Error::codeDescription(uint16_t code)
{
    std::string result;
    switch (code)
    {
    case OK:
        result = "No error";
        break;
    case NO_OPTION_ARGUMENT:
        result = "Command line option's argument is missing";
        break;
    case ARGUMENT_WITHOUT_OPTION:
        result = "Command line's argument without option";
        break;
    case UNKNOWN_OPTION:
        result = "Unknown command line option";
        break;
    case CONSTRUCTION_ERROR:
        result = "Failed to create object";
        break;
    case CORRUPTED_INPUT:
        result = "Input TS stream is corrupted";
        break;
    case CORRUPTED_OUTPUT:
        result = "Output stream is corrupted";
        break;
    default:
        result = "Unknown error";
        break;
    }

    return result + " (code " + std::to_string(code) + ")";
}
