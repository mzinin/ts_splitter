#pragma once

#include "message_types.hpp"

#include <functional>
#include <istream>
#include <map>
#include <vector>


/// @class TsReader.
/// @brief Reads payload from input TS stream.
class TsReader
{
public:
	/// @brief Type of payload handler.
	using OnPayload = std::function<void(const TsPayload&)>;

	/// @brief Constructor.
	/// @param[in] input - TS input.
	/// @param[in] handler - Paylod handler.
	/// @throws Error.
	TsReader(std::istream& input, OnPayload handler);

	/// @brief Read all available TS packets and produce payloads.
	/// @throws Error.
	void readAll();

private:
	/// @brief Find and read first packet in TS stream.
	/// @throws Error.
	void findFirstPacket();

	/// @brief Read packet from TS stream.
	/// @throws Error.
	void readPacket();

	/// @brief Process successfully read packet.
	/// @details Calls handler, which may throws exceptions.
	void processPacket();

	/// @brief Check if elementary stream is started, i.e. can be decoded.
	/// @param[in] pid - PID of current packet.
	/// @param[in] newEsPacket - Flag, set if current packet starts new ES packet.
	/// @param[in] seq - Sequence number of current packet.
	/// @returns true if corresponding elementary stream is started, false otherwise.
	bool checkEsStarted(uint16_t pid, bool newEsPacket, uint16_t seq);

private:
	/// @brief TS input stream.
	std::istream& input_;

	/// @brief Payload handler.
	OnPayload handler_;

	/// @brief Buffer for storing packets.
	std::vector<uint8_t> buffer_;
	
	/// @brief Set of started elementary streams as pair (PID, seq.number).
	std::map<uint16_t, uint16_t> streams_;
};
