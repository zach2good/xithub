# This script is responsible for receiving packets from the client and storing them in a file.
# It also validates the user ID and throws away the packet if it isn't a registered user.
import socket
import struct
import os

def start_server(host='localhost', port=5000, output_dir='packets'):
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server_socket.bind((host, port))
    print(f"UDP server started at {host}:{port}")

    while True:
        data, addr = server_socket.recvfrom(4096)
        if data:
            print(f"Received data from {addr}")

            # Define the new outer header format
            outer_header_format = '<QQB15sI'  # < specifies little-endian, Q for uint64_t, B for uint8_t, 15s for 15-char string, I for uint32_t
            outer_header_size = struct.calcsize(outer_header_format)

            if len(data) < outer_header_size:
                print("Error: Received data is smaller than outer header size.")
                continue

            # Unpack the outer header
            outer_header_data = data[:outer_header_size]
            clientToken, timestamp, serverId, characterName, dataSize = struct.unpack(outer_header_format, outer_header_data)

            characterName = characterName.decode('utf-8', errors='ignore').rstrip('\x00')

            # TODO: Validate userId and throw away the packet if it's isn't a registered user

            # Store the entire packet to a file
            packet_filename = os.path.join(output_dir, f"packet_{clientToken}_{timestamp}.bin")
            with open(packet_filename, 'wb') as packet_file:
                print(f"{timestamp} | [{clientToken}] Writing packet to {packet_filename} (size: {len(data)})")
                packet_file.write(data)


if __name__ == '__main__':
    start_server()
