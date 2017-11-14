#include "error.hpp"
#include "ts_reader.hpp"

#include <cstring>
#include <iostream>


namespace
{
	const size_t tsPacketSize = 188;
	const uint8_t tsSyncByte = 0x47;

	/// @brief Parsed TS packet.
	struct TsPacket
	{
		uint16_t pid;
		uint16_t payloadOffset;
		uint8_t seqNumber;
		uint8_t syncByte;
		bool isCorrupted;
		bool newEsPacket;
		bool hasPayload;

		TsPacket(const std::vector<uint8_t>& data)
		{
			syncByte = data[0];
			isCorrupted = data[1] & 0x80;
			newEsPacket = data[1] & 0x40;
			pid = ((data[1] & 0x1F) << 8) + data[2];
			hasPayload = data[3] & 0x10;
			seqNumber = data[3] & 0x0F;

			// check for adaptation field
			if (!(data[3] & 0x20))
				payloadOffset = 4;
			else
				payloadOffset = 5 + data[4];
		}
	};

	/// @brief Check if packet may contain elementary stream payload.
	bool mayContainEs(uint16_t pid)
	{
		return (32 <= pid && pid <= 8186) || (8188 <= pid && pid <= 8190);
	}
}

TsReader::TsReader(std::istream& input, OnPayload handler)
	: input_(input)
	, handler_(handler)
	, buffer_(2 * tsPacketSize, 0)
{
	if (!input_.good())
		throw Error(Error::CONSTRUCTION_ERROR, "TsReader, bad input");
	if (!handler_)
		throw Error(Error::CONSTRUCTION_ERROR, "TsReader, empty handler");
}

void TsReader::readAll()
{
	findFirstPacket();
	processPacket();

	while (!input_.eof())
	{
		readPacket();
		processPacket();
	}
}

void TsReader::findFirstPacket()
{
	readPacket();

	// TS stream consists of only 1 packet
	if (input_.eof())
	{
		// packet start found
		if (buffer_[0] == tsSyncByte)
		{
			buffer_.resize(tsPacketSize);
			return;
		}
		// that's not a TS stream
		throw Error(Error::CORRUPTED_INPUT);
	}

	// there is more than 1 packet
	size_t i = 0;
	while (i < tsPacketSize)
	{
		// 1st and 2nd packets are found
		if (buffer_[i] == tsSyncByte && input_.peek() == tsSyncByte)
		{
			std::memmove(&buffer_[0], &buffer_[i], tsPacketSize);
			buffer_.resize(tsPacketSize);
			return;
		}

		// read 1 byte from input and put into buffer
		buffer_[tsPacketSize + i] = input_.get();
		if (!input_.good())
			throw Error(Error::CORRUPTED_INPUT, "failed to find packet");
		++i;
	}

	// packet is not found
	throw Error(Error::CORRUPTED_INPUT, "failed to find packet");
}

void TsReader::readPacket()
{
	input_.read(reinterpret_cast<char*>(buffer_.data()), tsPacketSize);
	const auto read = input_.gcount();

	if (read != tsPacketSize)
		throw Error(Error::CORRUPTED_INPUT, "failed to read whole ts packet");

	// to check for eof
	input_.peek();
}

void TsReader::processPacket()
{
	TsPacket pkt(buffer_);
	
	// check for sync byte
	if (pkt.syncByte != tsSyncByte)
	{
		std::cerr << "Warning: TS packet has no sync byte. Skip it." << std::endl;
		return;
	}

	// check for corrupted packet
	if (pkt.isCorrupted)
	{
		std::cerr << "Warning: TS is corrupted. Skip it." << std::endl;
		return;
	}

	// check for payload
	if (!pkt.hasPayload)
		return;

	// check for useful pid
	if (!mayContainEs(pkt.pid))
		return;

	// check corresponding elementary stream started
	if (!checkEsStarted(pkt.pid, pkt.newEsPacket, pkt.seqNumber))
		return;

	// handle TS payload
	static TsPayload payload;
	payload.pid = pkt.pid;
	payload.data = &buffer_[pkt.payloadOffset];
	payload.length = tsPacketSize - pkt.payloadOffset;
	handler_(payload);
}

bool TsReader::checkEsStarted(uint16_t pid, bool newEsPacket, uint16_t seq)
{
	auto it = streams_.find(pid);

	// stream already started
	if (it != streams_.end())
	{
		if ((it->second + 1) % 0x10 != seq)
			std::cerr << "Warning: packet sequence within PID " << pid << " is broken" << std::endl;
		it->second = seq;
		return true;
	}

	// stream not started yet and is not starting now
	if (!newEsPacket)
		return false;

	// start stream
	streams_[pid] = seq;
	return true;
}
