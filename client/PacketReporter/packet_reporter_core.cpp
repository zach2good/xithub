#include "packet_reporter_core.h"

#include <cstdint>

#include "gzip.h"

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
    SendBuffer();

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

void PacketReporterCore::HandlePacketData(CharacterInfo charInfo, uint8_t* data, uint32_t dataSz)
{
    if (!DetectRetail())
    {
        return;
    }

    uint64_t timestamp = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

    m_charInfo = charInfo;

    InnerPacketHeader innerHeader;
    innerHeader.timestamp = timestamp;
    innerHeader.zoneId    = m_charInfo.zoneId;
    innerHeader.dataSize  = dataSz;

    m_buffer.insert(m_buffer.end(), reinterpret_cast<uint8_t*>(&innerHeader), reinterpret_cast<uint8_t*>(&innerHeader) + sizeof(innerHeader));
    m_buffer.insert(m_buffer.end(), data, data + dataSz);

    if (m_buffer.size() >= m_bufferLimit)
    {
        SendBuffer();

        // TODO: Reset send timer
    }

    // TODO: Check send timer
}

void PacketReporterCore::SendBuffer()
{
    uint64_t timestamp = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

    std::vector<uint8_t> compressed = gzip_compress(m_buffer.data(), m_buffer.size());
    const auto compressed_data      = compressed.data();
    const auto compressed_data_size = compressed.size();

    // Outer header
    OuterPacketHeader header;
    header.clientToken = m_charInfo.clientToken;
    header.timestamp   = timestamp;
    header.serverId    = m_charInfo.serverId;
    std::memcpy(header.characterName, m_charInfo.name.c_str(), std::min<size_t>(m_charInfo.name.size(), sizeof(header.characterName)));
    header.dataSize = compressed_data_size;

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
