#pragma once

#include <cstdint>


/// @struct TsPayload.
/// @brief Paylod of TS packet.
struct TsPayload
{
	/// @brief Start of payload data.
	const uint8_t* data;

	/// @brief Length f payload.
	uint16_t length;

	/// @brief Corresponding PID.
	uint16_t pid;
};
