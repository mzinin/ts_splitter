#include "error.hpp"


Error::Error(uint16_t code, const std::string& msg)
	: code_(code)
	, message_(codeDescription(code))
{
	if (!msg.empty())
		message_ += ": " + msg;
}

const char* Error::what() const
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
	case Error::OK:
		result = "No error";
		break;
	case Error::NO_OPTION_ARGUMENT:
		result = "Command line option's argument is missing";
		break;
	default:
		result = "Unknown error";
		break;
	}

	return result + " (code " + std::to_string(code) + ")";
}
