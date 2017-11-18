#include "error.hpp"
#include "payload_parser.hpp"


namespace
{
	/// @brief Minimum size of PES header.
	const uint16_t minPesHeaderSize = 6;

	/// @brief Program association table pid.
	const uint16_t paTablePid = 0;

	/// @brief PAT id.
	const uint8_t paTableId = 0;

	/// @brief Minimum size of PAT.
	const uint16_t minPaTableSize = 8;

	/// @brief Check if payload contains PES header.
	bool hasPesHeader(const TsPayload& payload)
	{
		return payload.size >= minPesHeaderSize &&
			   payload.data[0] == 0x00 &&
			   payload.data[1] == 0x00 &&
			   payload.data[2] == 0x01;
	}

	/// @brief Calculate CRC-32/MPEG-2.
	uint32_t crc32(const uint8_t* data, size_t size)
	{
		const uint8_t* const end = data + size;
		uint32_t result = 0xFFFFFFFFu;
		while (data != end)
		{
			result ^= uint32_t(*data++) << 24;
			for (int i = 0; i < 8; ++i)
			{
				const auto xor = ((result & 0x80000000u) >> 31) * 0x04C11DB7u;
				result = (result << 1) ^ xor;
			}
		}
		return result;
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
	if (payload.pid == paTablePid)
		parsePat(payload);
	else
		parseDataPayload(payload);
}

void PayloadParser::parsePat(const TsPayload& payload)
{
	// PAT offset within payload
	const uint16_t offset = 1 + payload.data[0];
	if (payload.size < offset)
	{
		log_ << "Warning: PayloadParser, corrupted PAT." << std::endl;
		return;
	}
	if (payload.data[offset] != paTablePid)
	{
		log_ << "Warning: PayloadParser, PAT has wrong table id." << std::endl;
		return;
	}

	const uint16_t sectionSize = ((payload.data[offset + 1] & 0x0F) << 8) + payload.data[offset + 2];
	if (payload.size < sectionSize + 4)
	{
		log_ << "Warning: PayloadParser, corrupted PAT." << std::endl;
		return;
	}

	// check CRC
	const uint8_t* crcData = payload.data + 4 + sectionSize - 4;
	const uint32_t crc = (((((uint32_t(crcData[0]) << 8) + crcData[1]) << 8) + crcData[2]) << 8) + crcData[3];
	if (crc32(payload.data + 1, crcData - payload.data - 1) != crc)
	{
		log_ << "Warning: PayloadParser, corrupted PAT." << std::endl;
		return;
	}

	// this table is not applicable
	if (!(payload.data[offset + 5] & 0x01))
		return;

	// section starts 4 bytes from payload start, 4 bytes for CRC
	for (auto i = offset + 8; i < sectionSize + 4 - 4; i += 4)
	{
		const uint16_t program = (payload.data[i] << 8) + payload.data[i + 1];
		if (!detectedPrograms_.count(program))
		{
			log_ << "Notice: PayloadParser, detected program " << program << std::endl;
			detectedPrograms_.insert(program);
			if (program)
				pmTablePids_.insert(((payload.data[i + 2] & 0x1F) << 8) + payload.data[i + 3]);
		}
	}
}

void PayloadParser::parseDataPayload(const TsPayload& payload)
{
	// check PES header only if payload has corresponding flag
	const bool isPesHeader = payload.newEsPacket && hasPesHeader(payload);

	// packet of unknown stream
	if (!isPesHeader && streams_.count(payload.pid) == 0)
	{
		log_ << "Warning: PayloadParser, incomplete PES packet with pid " << payload.pid << ". Skip it." << std::endl;
		return;
	}

	// parse header of new ES packet - check for new stream and raw data offset
	uint16_t offset = 0;
	if (isPesHeader && !parseHeader(payload, offset))
	{
		log_ << "Warning: PayloadParser, failed to parse PES packet header. Skip it." << std::endl;
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
		log_ << "Warning: PayloadParser, different stream ids in the with the same pid." << std::endl;
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
