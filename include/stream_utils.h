#ifndef STREAM_UTILS_H
#define STREAM_UTILS_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define OUTBUF_SIZE (256 * 1024)

/**
 * @struct InputStream
 * @brief A utility structure for reading data from a byte array.
 */
typedef struct
{
    const uint8_t *data;
    size_t size;
    size_t pos;
} InputStream;

/**
 * @struct OutputStream
 * @brief A utility structure for writing data to a fixed-size buffer.
 */
typedef struct
{
    char data[OUTBUF_SIZE];
    size_t pos;
} OutputStream;

/**
 * @brief Initializes an OutputStream.
 *
 * @param stream Pointer to the OutputStream.
 */
void output_stream_init(OutputStream *stream);

/**
 * @brief Writes a buffer to the OutputStream.
 *
 * @param stream Pointer to the OutputStream.
 * @param buf Pointer to the data to write.
 * @param len The number of bytes to write.
 */
void output_stream_write(OutputStream *stream, const char *buf, size_t len);

/**
 * @brief Reads an integer from an InputStream.
 *
 * @param stream Pointer to the InputStream.
 * @param size The number of bytes to read.
 * @return The read integer value.
 */
uint64_t stream_read_int(InputStream *stream, size_t size);

/**
 * @brief Reads a signed integer from an InputStream.
 *
 * @param stream Pointer to the InputStream.
 * @param size The number of bytes to read.
 * @return The read signed integer value.
 */
int64_t stream_read_sint(InputStream *stream, size_t size);

/**
 * @brief Reads a sequence of bytes from an InputStream.
 *
 * @param stream Pointer to the InputStream.
 * @param size The number of bytes to read.
 * @return A constant pointer to the read bytes, or NULL if out of bounds.
 */
const uint8_t *stream_read_bytes(InputStream *stream, size_t size);

#endif