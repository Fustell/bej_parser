#include "stream_utils.h"

void OutputStreamInit(OutputStream *stream) {
  stream->pos = 0;
  stream->data[0] = '\0';
}

void OutputStreamWrite(OutputStream *stream, const char *buf, size_t len) {
  if (stream->pos + len >= OUTBUF_SIZE) return;
  memcpy(stream->data + stream->pos, buf, len);
  stream->pos += len;
  stream->data[stream->pos] = '\0';
}

uint64_t StreamReadInt(InputStream *stream, size_t size) {
  uint64_t value = 0;
  if (stream->pos + size > stream->size) return 0;
  for (size_t i = 0; i < size; ++i) {
    value |= ((uint64_t)stream->data[stream->pos + i]) << (i * 8);
  }
  stream->pos += size;
  return value;
}

int64_t stream_read_sint(InputStream *stream, size_t size) {
  uint64_t u_val = StreamReadInt(stream, size);
  if (size > 0 && size <= 8) {
    int shift = (8 - size) * 8;
    return ((int64_t)(u_val << shift)) >> shift;
  }
  return u_val;
}

const uint8_t *StreamReadBytes(InputStream *stream, size_t size) {
  if (stream->pos + size > stream->size) return NULL;
  const uint8_t *ptr = &stream->data[stream->pos];
  stream->pos += size;
  return ptr;
}