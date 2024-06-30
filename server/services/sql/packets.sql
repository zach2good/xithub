DROP TABLE IF EXISTS packets;
CREATE TABLE packets (
    id INT AUTO_INCREMENT PRIMARY KEY,
    UserId BIGINT NOT NULL,
    SessionId VARCHAR(50) NOT NULL,
    CharacterName VARCHAR(15) NOT NULL,
    ServerId INT NOT NULL,
    ZoneId INT NOT NULL,
    ServerTimestamp BIGINT NOT NULL,
    ClientTimestamp BIGINT NOT NULL,
    PacketId VARCHAR(50) NOT NULL,
    PacketHash VARCHAR(64) NOT NULL,
    RawData LONGBLOB NOT NULL,
    INDEX unique_user_session_hash(UserId, PacketHash)
);
