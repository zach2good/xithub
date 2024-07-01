## Packet Binary Format

```cpp
#pragma pack(push, 1)
struct OuterPacketHeader
{
    uint64_t userId;
    uint64_t timestamp;
    uint8_t  serverId;
    char     characterName[15];
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

// A packet will be recieved and stored as:

OuterPacketHeader + GZipData

// `GZipData` is a gzip'd block of the following layout:

InnerPacketHeader + Data + InnerPacketHeader + Data + ...

// In all cases you'll need to unpack the header to get `dataSize` so you
// can handle whatever the next stage is.
```
