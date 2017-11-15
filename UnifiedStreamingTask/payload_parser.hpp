#pragma once

#include "message_types.hpp"

#include <functional>
#include <map>
#include <ostream>


/// @class PayloadParser.
/// @brief Parse TS payloads into ES raw data.
class PayloadParser
{
public:
	/// @brief Type of raw data handler.
	using OnEsRawData = std::function<void(const EsRawData&)>;

	/// @brief Constructor.
	/// @param[out] log - Stream for log messages.
	/// @param[in] handler - Raw data handler.
	/// @throws Error.
	PayloadParser(std::ostream& log, OnEsRawData handler);

	/// @brief Parse one TS payload.
	/// @details Calls handler, which may throws exceptions.
	/// @param[in] payload - TS payload.
	void parse(const TsPayload& payload);

private:
	/// @brief Parse PES header.
	/// @param[in] payload - TS payload.
	/// @param[out] offset - Raw data offset within payload.
	/// @returns true is header is successfully parsed, false otherwise.
	bool parseHeader(const TsPayload& payload, uint16_t& offset);

	/// @brief Add new stream to the set of known ones if needed.
	/// @param[in] pid - Corresponding pid in TS stream.
	/// @param[in] pid - ES stream id.
	/// @returns false if ids inconsistency detected, true otherwise.
	bool updateStreams(uint16_t pid, uint8_t streamId);

private:
	/// @brief Log output stream.
	std::ostream& log_;

	/// @brief Raw data handler.
	OnEsRawData handler_;

	/// @brief ES stream information.
	struct StreamInfo
	{
		/// @brief Type of ES stream.
		EsType type;

		/// @brief Stream number in the set of all streams of same type.
		uint16_t seqNumber;
	};

	/// @brief Set of known streams.
	std::map<uint16_t, StreamInfo> streams_;

	/// @brief Number of detected audio streams.
	uint16_t audioSeqNumber_ = 0;

	/// @brief Number of detected video streams.
	uint16_t videoSeqNumber_ = 0;
};
