#include "error.hpp"
#include "ts_reader.hpp"

#include <cstring>


namespace
{
    const size_t tsPacketSize = 188;
    const uint8_t tsSyncByte = 0x47;
    const uint16_t nullPacketPid = 8191;

    /// @brief Parsed TS packet.
    struct TsPacket
    {
        uint16_t pid;
        uint16_t payloadOffset;
        uint8_t seqNumber;
        bool isCorrupted;
        bool newEsPacket;
        bool hasPayload;

        TsPacket(const std::vector<uint8_t>& data)
        {
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

}

TsReader::TsReader(std::istream& input, std::ostream& log, OnPayload handler)
    : input_(input)
    , log_(log)
    , handler_(handler)
    , buffer_(tsPacketSize, 0)
{
    if (!input_.good())
        throw Error(Error::CONSTRUCTION_ERROR, "TsReader, bad input");
    if (!log_.good())
        throw Error(Error::CONSTRUCTION_ERROR, "TsReader, bad log output");
    if (!handler_)
        throw Error(Error::CONSTRUCTION_ERROR, "TsReader, empty handler");
}

void TsReader::readAll()
{
    while (!input_.eof())
    {
        if (readPacket())
            processPacket();
    }
}

bool TsReader::readPacket()
{
    // read data
    if (!readFromStream(0, tsPacketSize))
        return false;

    // check sync byte
    while (true)
    {
        // buffer starts with sync byte and either next unread char is also sync byte
        // or EOF reached - most probably we got a valid packet in buffer
        if (buffer_[0] == tsSyncByte && (input_.peek() == tsSyncByte || input_.eof()))
            return true;

        // otherwise search for sync byte
        size_t offset = 1;
        while (offset < tsPacketSize && buffer_[offset] != tsSyncByte)
            ++offset;

        if (offset == tsPacketSize)
        {
            // no sync byte, that's corrupted packet, move to the next one
            log_ << "Warning: TsReader, corrupted TS packet" << std::endl;
            break;
        }
        else
        {
            // sync byte found, align it with buffer
            std::memmove(&buffer_[0], &buffer_[offset], tsPacketSize - offset);
            if (!readFromStream(tsPacketSize - offset, offset))
                return false;
        }
    }

    return false;
}

bool TsReader::readFromStream(size_t bufferOffset, size_t size)
{
    input_.read(reinterpret_cast<char*>(buffer_.data() + bufferOffset), size);

    const size_t read = input_.gcount();
    if (read != size)
    {
        if (read)
            log_ << "Warning: TsReader, corrupted TS packet" << std::endl;
        if (!input_.eof())
            throw Error(Error::CORRUPTED_INPUT, "TsReader, failed to read");
        return false;
    }
    return true;
}

void TsReader::processPacket()
{
    TsPacket pkt(buffer_);

    // check for corrupted packet
    if (pkt.isCorrupted)
    {
        log_ << "Warning: TsReader, corrupted TS packet" << std::endl;
        return;
    }

    // check for payload
    if (!pkt.hasPayload)
        return;

    // skip null packets
    if (pkt.pid == nullPacketPid)
        return;

    // check corresponding elementary stream started
    if (!checkEsStarted(pkt.pid, pkt.newEsPacket, pkt.seqNumber))
        return;

    // handle TS payload
    static TsPayload payload;
    payload.pid = pkt.pid;
    payload.data = &buffer_[pkt.payloadOffset];
    payload.size = tsPacketSize - pkt.payloadOffset;
    payload.newEsPacket = pkt.newEsPacket;

    // skip zero-length payloads
    if (payload.size)
        handler_(payload);
}

bool TsReader::checkEsStarted(uint16_t pid, bool newEsPacket, uint16_t seq)
{
    auto it = streams_.find(pid);

    // stream already started
    if (it != streams_.end())
    {
        if ((it->second + 1) % 0x10 != seq)
            log_ << "Warning: TsReader, packet sequence within PID " << pid << " is broken" << std::endl;
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
