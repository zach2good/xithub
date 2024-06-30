#include "packet_reporter_core.h"

#include <cstdint>

#include "gzip.h"

#pragma pack(push, 1)
struct OuterPacketHeader
{
    uint64_t userId;
    uint64_t timestamp;
    uint32_t dataSize;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct InnerPacketHeader
{
    uint64_t timestamp;
    uint32_t dataSize;
};
#pragma pack(pop)

PacketReporterCore::PacketReporterCore(LoggingCallback loggingCallback)
    : m_loggingCallback(loggingCallback)
{
    if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
    {
        throw std::runtime_error("WSAStartup failed.");
    }

    m_udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_udpSocket == INVALID_SOCKET)
    {
        WSACleanup();
        throw std::runtime_error("Socket creation failed.");
    }

    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_port   = htons(5000);
    if (InetPton(AF_INET, "127.0.0.1", &m_serverAddr.sin_addr) != 1)
    {
        closesocket(m_udpSocket);
        WSACleanup();
        throw std::runtime_error("Invalid IP address.");
    }
}

PacketReporterCore::~PacketReporterCore()
{
    if (m_udpSocket != INVALID_SOCKET)
    {
        closesocket(m_udpSocket);
    }
    WSACleanup();
}

bool PacketReporterCore::DetectRetail()
{
    return GetModuleHandleA("polhook.dll") != NULL;
}

void PacketReporterCore::HandlePacketData(uint8_t* data, uint32_t dataSz)
{
    if (!DetectRetail())
    {
        return;
    }

    uint64_t timestamp = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

    InnerPacketHeader innerHeader;
    innerHeader.timestamp = timestamp;
    innerHeader.dataSize  = dataSz;

    m_buffer.insert(m_buffer.end(), reinterpret_cast<uint8_t*>(&innerHeader), reinterpret_cast<uint8_t*>(&innerHeader) + sizeof(innerHeader));
    m_buffer.insert(m_buffer.end(), data, data + dataSz);

    if (m_buffer.size() >= m_bufferLimit)
    {
        std::vector<uint8_t> compressed = gzip_compress(m_buffer.data(), m_buffer.size());
        const auto compressed_data      = compressed.data();
        const auto compressed_data_size = compressed.size();

        // Outer header
        OuterPacketHeader header;
        header.userId    = 44;
        header.timestamp = timestamp;
        header.dataSize  = compressed_data_size;

        // Allocate memory for the entire packet
        std::vector<uint8_t> packet(sizeof(OuterPacketHeader) + compressed_data_size, 0);
        std::memcpy(packet.data(), &header, sizeof(OuterPacketHeader));
        std::memcpy(packet.data() + sizeof(OuterPacketHeader), compressed_data, compressed_data_size);

        // TODO: Make access to buffer thread-safe
        // m_TaskSystem.schedule([this, packet]() {
        if (sendto(m_udpSocket, reinterpret_cast<const char*>(packet.data()), packet.size(), 0, (sockaddr*)&m_serverAddr, sizeof(m_serverAddr)) == SOCKET_ERROR)
        {
            // We won't clear the buffer here. If the send failed we'll retry next time.
            std::cerr << "sendto failed.\n";
        }
        else
        {
            // Clear buffer if send was successful
            m_buffer.clear();
        }
        // });
    }
}
