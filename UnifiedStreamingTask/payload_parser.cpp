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

    /// @brief Program map table id.
    const uint8_t pmTableId = 2;

    /// @brief Table name by id.
    const std::string& tableName(uint8_t id)
    {
        static const std::string pat = "PAT";
        static const std::string pmt = "PMT";
        static const std::string unknown = "Unknown";

        switch (id)
        {
        case paTableId:
            return pat;
        case pmTableId:
            return pmt;
        }
        return unknown;
    }

    /// @brief ES type by stream id from PMT.
    EsType streamTypeByPmt(uint8_t typeId)
    {
        // According https://en.wikipedia.org/wiki/Program-specific_information#Elementary_stream_types
        static std::set<uint8_t> audioStreams{ 0x03, 0x04, 0x0F, 0x11, 0x1C, 0x80, 0x81, 0x82, 0x83,
                                               0x84, 0x85, 0x86, 0x87, 0x91, 0xC1, 0xC2, 0xCF };
        static std::set<uint8_t> videoStreams{ 0x01, 0x02, 0x10, 0x1B, 0x24, 0x42, 0xD1, 0xDB, 0xEA };

        if (audioStreams.count(typeId))
            return EsType::AUDIO;
        if (videoStreams.count(typeId))
            return EsType::VIDEO;
        return EsType::OTHER;
    }

    /// @brief ES type by stream id from PES header.
    EsType streamTypeByPes(uint8_t streamId)
    {
        if (0xC0 <= streamId && streamId <= 0xDF)
            return EsType::AUDIO;
        else if (0xE0 <= streamId && streamId <= 0xEF)
            return EsType::VIDEO;
        return EsType::OTHER;
    }

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
                const auto xorArg = ((result & 0x80000000u) >> 31) * 0x04C11DB7u;
                result = (result << 1) ^ xorArg;
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
    else if (pmTablePids_.count(payload.pid))
        parsePmt(payload);
    else
        parseDataPayload(payload);
}

void PayloadParser::parsePat(const TsPayload& payload)
{
    uint16_t offset = 0, sectionSize = 0;
    if (!checkTablePayload(payload, paTableId, offset, sectionSize))
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

void PayloadParser::parsePmt(const TsPayload& payload)
{
    uint16_t offset = 0, sectionSize = 0;
    if (!checkTablePayload(payload, pmTableId, offset, sectionSize))
        return;

    const uint16_t programInfoLength = ((payload.data[offset + 10] & 0x0F) << 8) + payload.data[offset + 11];
    auto i = offset + 12 + programInfoLength;

    // section starts 4 bytes from payload start, 4 bytes for CRC
    while (i < sectionSize + 4 - 4)
    {
        const EsType type = streamTypeByPmt(payload.data[i]);
        const uint16_t pid = ((payload.data[i + 1] & 0x1F) << 8) + payload.data[i + 2];
        updateStreams(pid, type);
        i += 5 + ((payload.data[i + 3] & 0x0F) << 8) + payload.data[i + 4];
    }
}

bool PayloadParser::checkTablePayload(const TsPayload& payload, uint8_t tableId, uint16_t& offset, uint16_t& sectionSize)
{
    // offset within payload
    offset = 1 + payload.data[0];
    if (payload.size < offset)
    {
        log_ << "Warning: PayloadParser, corrupted " << tableName(tableId) << std::endl;
        return false;
    }
    if (payload.data[offset] != tableId)
    {
        log_ << "Warning: PayloadParser, " << tableName(tableId) << " has wrong table id" << std::endl;
        return false;
    }

    // check size
    sectionSize = ((payload.data[offset + 1] & 0x0F) << 8) + payload.data[offset + 2];
    if (payload.size < sectionSize + 4)
    {
        log_ << "Warning: PayloadParser, corrupted " << tableName(tableId) << std::endl;
        return false;
    }

    // check CRC
    const uint8_t* crcData = payload.data + 4 + sectionSize - 4;
    const uint32_t crc = (((((uint32_t(crcData[0]) << 8) + crcData[1]) << 8) + crcData[2]) << 8) + crcData[3];
    if (crc32(payload.data + 1, crcData - payload.data - 1) != crc)
    {
        log_ << "Warning: PayloadParser, corrupted " << tableName(tableId) << std::endl;
        return false;
    }

    // this table is not applicable
    if (!(payload.data[offset + 5] & 0x01))
        return false;

    return true;
}

void PayloadParser::parseDataPayload(const TsPayload& payload)
{
    // check PES header only if payload has corresponding flag
    const bool isPesHeader = payload.newEsPacket && hasPesHeader(payload);

    // packet of unknown stream
    if (!isPesHeader && streams_.count(payload.pid) == 0)
    {
        log_ << "Warning: PayloadParser, incomplete PES packet with pid " << payload.pid << std::endl;
        return;
    }

    // parse header of new ES packet - check for new stream and raw data offset
    uint16_t offset = 0;
    if (isPesHeader && !parseHeader(payload, offset))
    {
        log_ << "Warning: PayloadParser, failed to parse PES packet header" << std::endl;
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
    // if there was no PAT and PMT - try to detect and update streams
    if (!updateStreams(payload.pid, streamTypeByPes(payload.data[3])))
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

bool PayloadParser::updateStreams(uint16_t pid, EsType type)
{
    const auto insertionResult = streams_.insert({ pid, StreamInfo{type, 0} });

    // playload from some known stream
    if (!insertionResult.second)
    {
        if (insertionResult.first->first == pid)
            return true;
        // incosistency
        log_ << "Warning: PayloadParser, different stream ids in the with the same pid" << std::endl;
        return false;
    }

    switch (type)
    {
    case EsType::AUDIO:
        insertionResult.first->second.seqNumber = ++audioSeqNumber_;
        log_ << "Notice: PayloadParser, detected AUDIO stream with pid " << pid << std::endl;
        break;
    case EsType::VIDEO:
        insertionResult.first->second.seqNumber = ++videoSeqNumber_;
        log_ << "Notice: PayloadParser, detected VIDEO stream with pid " << pid << std::endl;
        break;
    default:
        log_ << "Notice: PayloadParser, detected unsupported stream with pid " << pid << std::endl;
        break;
    }

    return true;
}
