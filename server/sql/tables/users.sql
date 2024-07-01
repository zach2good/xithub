CREATE TABLE users (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) NOT NULL,
    email VARCHAR(50) NOT NULL,
    created BIGINT NOT NULL,
    lastLogin BIGINT NOT NULL,
    INDEX unique_username(username),
    INDEX unique_email(email)
);
