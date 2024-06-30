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
    # SUBSTRING is 1-indexed
    CAST(CONV(HEX(REVERSE(SUBSTRING(RawData, 5, 4))), 16, 10) AS UNSIGNED) AS NpcId, # uint32_t
    CAST(CONV(HEX(REVERSE(SUBSTRING(RawData, 13, 2))), 16, 10) AS UNSIGNED) AS EventId # uint16_t
FROM 
    packets
WHERE 
    PacketId = LOWER('0x32')
