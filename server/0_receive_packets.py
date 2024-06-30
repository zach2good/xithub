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

            # Define the outer header format
            outer_header_format = '<QQI'  # < specifies little-endian, Q for uint64_t, I for uint32_t
            outer_header_size = struct.calcsize(outer_header_format)
            if len(data) < outer_header_size:
                print("Error: Received data is smaller than outer header size.")
                continue

            # Unpack the outer header
            outer_header_data = data[:outer_header_size]
            userId, timestamp, compressed_data_size = struct.unpack(outer_header_format, outer_header_data)

            # Store the entire packet to a file
            packet_filename = os.path.join(output_dir, f"packet_{userId}_{timestamp}.bin")
            with open(packet_filename, 'wb') as packet_file:
                print(f"Writing packet to {packet_filename}")
                packet_file.write(data)


if __name__ == '__main__':
    start_server()
