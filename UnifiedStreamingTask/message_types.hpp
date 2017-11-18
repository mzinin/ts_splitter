#pragma once

#include <cstdint>


/// @struct TsPayload.
/// @brief Paylod of TS packet.
struct TsPayload
{
    /// @brief Start of payload data.
    const uint8_t* data;

    /// @brief Size of payload.
    uint16_t size;

    /// @brief Corresponding PID.
    uint16_t pid;

    /// @brief Flag for ES packet start.
    bool newEsPacket;
};

/// @brief Type of raw data output.
enum EsType
{
    AUDIO,
    VIDEO,
    OTHER,
};

/// @struct EsRawData.
/// @brief Elementary stream raw data.
struct EsRawData
{
    /// @brief Start of raw data.
    const uint8_t* data;

    /// @brief Size of raw data.
    uint16_t size;

    /// @brief Type of data.
    EsType type;

    /// @brief Stream number in the set of all streams of same type.
    uint16_t esNumber;
};
