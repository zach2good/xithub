SELECT 
    id,
    UserId,
    SessionId,
    CharacterName,
    ServerId,
    ZoneId,
    ServerTimestamp,
    ClientTimestamp,
    PacketId,
    HEX(RawData),
    ConvertHexToUint32(SUBSTRING(RawData, 5, 4)) AS NpcId,
    ConvertHexToUint16(SUBSTRING(RawData, 13, 2)) AS EventId
FROM 
    packets
WHERE 
    PacketId = LOWER('0x32');
