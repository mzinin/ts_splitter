#include "error.hpp"
#include "payload_parser.hpp"


namespace
{
	/// @brief Minimum size of PES header.
	const uint16_t minPesHeaderSize = 6;

	/// @brief Check if payload contains PES header.
	bool hasPesHeader(const TsPayload& payload)
	{
		return payload.size >= minPesHeaderSize &&
			   payload.data[0] == 0x00 &&
			   payload.data[1] == 0x00 &&
			   payload.data[2] == 0x01;
	}
}

PayloadParser::PayloadParser(std::ostream& log, OnEsRawData handler)
	: log_(log)
	, handler_(handler)
{
	if (!log_.good())
		throw Error(Error::CONSTRUCTION_ERROR, "PayloadParser, bad log output");
	if (!handler_)
		throw Error(Error::CONSTRUCTION_ERROR, "PayloadParser, empty handler");
}

void PayloadParser::parse(const TsPayload& payload)
{
	// check PES header only if payload has corresponding flag
	const bool isPesHeader = payload.newEsPacket && hasPesHeader(payload);

	// packet of unknown stream
	if (!isPesHeader && streams_.count(payload.pid) == 0)
	{
		log_ << "Warning: incomplete PES packet with pid " << payload.pid << ". Skip it." << std::endl;
		return;
	}

	// parse header of new ES packet - check for new stream and raw data offset
	uint16_t offset = 0;
	if (isPesHeader && !parseHeader(payload, offset))
	{
		log_ << "Warning: failed to parse PES packet header. Skip it." << std::endl;
		return;
	}
	
	// handle only audio and video data
	const auto& streamInfo = streams_.at(payload.pid);
	if (streamInfo.type == EsType::OTHER)
		return;

	static EsRawData rawData;
	rawData.type = streamInfo.type;
	rawData.esNumber = streamInfo.seqNumber;
	rawData.data = payload.data + offset;
	rawData.size = payload.size - offset;
	handler_(rawData);
}

bool PayloadParser::parseHeader(const TsPayload& payload, uint16_t& offset)
{
	if (!updateStreams(payload.pid, payload.data[3]))
		return false;

	// payload contains only PES header
	if (payload.size < minPesHeaderSize + 1)
	{
		offset = payload.size;
		return true;
	}

	const bool isOptionalHeader = (payload.data[minPesHeaderSize + 1] & 0xC0) == 0x80;
	if (!isOptionalHeader)
	{
		offset = minPesHeaderSize;
		return true;
	}

	// payload is not enought to contain optional header
	if (payload.size < minPesHeaderSize + 3)
		return false;

	offset = minPesHeaderSize + 3 + payload.data[minPesHeaderSize + 2];
	return payload.size >= offset;
}

bool PayloadParser::updateStreams(uint16_t pid, uint8_t streamId)
{
	const auto insertionResult = streams_.insert({ pid, StreamInfo() });

	// playload from some known stream
	if (!insertionResult.second)
	{
		if (insertionResult.first->first == pid)
			return true;
		// incosistency
		log_ << "Warning: different stream ids in the with the same pid." << std::endl;
		return false;
	}

	if (0xC0 <= streamId && streamId <= 0xDF)
	{
		insertionResult.first->second.type = EsType::AUDIO;
		insertionResult.first->second.seqNumber = ++audioSeqNumber_;
	}
	else if (0xE0 <= streamId && streamId <= 0xEF)
	{
		insertionResult.first->second.type = EsType::VIDEO;
		insertionResult.first->second.seqNumber = ++videoSeqNumber_;
	}
	else
		insertionResult.first->second.type = EsType::OTHER;

	return true;
}
