#pragma once

#include <exception>
#include <string>

/// @class Error
/// @brief Error desribed by code and test message.
class Error : public std::exception
{
public:
	/// @brief Error codes
	enum : uint16_t
	{
		OK = 0,							///< No error.
		NO_OPTION_ARGUMENT = 1,			///< Command line argument missing.
		ARGUMENT_WITHOUT_OPTION = 2,	///< Argument with no option (key).
	};

	/// @brief Constructor.
	/// @param[in] code - Error code.
	/// @param[in] msg - Error additional description.
	Error(uint16_t code, const std::string& msg = std::string());

	/// @brief Get error message as C string.
	/// @returns C string with error description.
	const char* what() const override;

	/// @brief Get error code.
	uint16_t code() const;

	/// @brief Get error message.
	const std::string& message() const;

	/// @brief Get standard description of error code.
	/// @param[in] code - Error code.
	/// @returns Standard error message.
	static std::string codeDescription(uint16_t code);

private:
	/// @brief Error code.
	uint16_t code_;

	/// @brief Error message.
	std::string message_;
};
