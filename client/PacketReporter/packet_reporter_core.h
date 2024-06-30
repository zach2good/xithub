#pragma once

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include "task_system.h"

#include <string>
#include <memory>

#include <iostream>
#include <stdexcept>
#include <thread>
#include <mutex>
#include <cstring>
#include <ctime>

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // TODO: Do in CMake

template<typename T, typename U>
T& ref(U* buf, std::size_t index)
{
    return *reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(buf) + index);
}

class PacketReporterCore
{
    using LoggingCallback = std::function<void(const std::string&)>;

public:
#pragma pack(push, 1)
    struct OuterPacketHeader
    {
        uint64_t userId;
        uint64_t timestamp;
        uint8_t serverId;
        char characterName[15];
        uint32_t dataSize;
    };
#pragma pack(pop)

#pragma pack(push, 1)
    struct InnerPacketHeader
    {
        uint64_t timestamp;
        uint16_t zoneId;
        uint32_t dataSize;
    };
#pragma pack(pop)

    struct CharacterInfo
    {
        std::string name;
        uint16_t zoneId;
        uint8_t serverId;
    };

    PacketReporterCore(LoggingCallback loggingCallback);
    ~PacketReporterCore();

    PacketReporterCore(const PacketReporterCore& other)            = delete;
    PacketReporterCore(PacketReporterCore&& other)                 = delete;
    PacketReporterCore& operator=(PacketReporterCore&& other)      = delete;
    PacketReporterCore& operator=(const PacketReporterCore& other) = delete;

    bool DetectRetail();

    void HandlePacketData(CharacterInfo charInfo, uint8_t* data, uint32_t dataSz);

private:
    LoggingCallback m_loggingCallback;

    ts::task_system m_taskSystem;

    WSADATA m_wsaData;
    SOCKET m_udpSocket;
    sockaddr_in m_serverAddr;

    std::vector<uint8_t> m_buffer;
    size_t m_bufferLimit = 4096;
};
