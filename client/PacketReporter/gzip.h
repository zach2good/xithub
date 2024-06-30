#pragma once

#include <vector>
#include <cstdint>
#include <iostream>

#include <zlib.h>

std::vector<uint8_t> gzip_compress(const uint8_t* data, size_t data_size)
{
    std::vector<uint8_t> compressed_data;
    z_stream deflate_stream;
    memset(&deflate_stream, 0, sizeof(deflate_stream));
    deflate_stream.zalloc = Z_NULL;
    deflate_stream.zfree  = Z_NULL;
    deflate_stream.opaque = Z_NULL;

    deflate_stream.avail_in = static_cast<uInt>(data_size);
    deflate_stream.next_in  = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(data));

    deflateInit2(&deflate_stream, Z_BEST_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY);

    uint8_t temp_buffer[4096];
    do
    {
        deflate_stream.avail_out = sizeof(temp_buffer);
        deflate_stream.next_out  = temp_buffer;
        int deflate_status       = deflate(&deflate_stream, Z_FINISH);
        if (deflate_status != Z_OK && deflate_status != Z_STREAM_END)
        {
            // Handle compression error here
            std::cerr << "Compression error: " << deflate_status << std::endl;
            deflateEnd(&deflate_stream);
            return compressed_data; // Return whatever is compressed so far
        }
        compressed_data.insert(compressed_data.end(), temp_buffer, temp_buffer + sizeof(temp_buffer) - deflate_stream.avail_out);
    } while (deflate_stream.avail_out == 0);

    deflateEnd(&deflate_stream);
    return compressed_data;
}
