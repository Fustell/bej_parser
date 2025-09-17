#include "json_writer.h"
#include <stdio.h>

void json_write_indent(OutputStream *out, int indent_level)
{
    output_stream_write(out, "\n", 1);

    for (int i = 0; i < indent_level; ++i)
    {
        output_stream_write(out, "    ", 4);
    }
}

bool json_writer_flush_to_file(const OutputStream *stream, const char *filename)
{
    if (!stream || stream->pos == 0)
        return false;

    FILE *f = fopen(filename, "w");
    if (!f)
        return false;

    size_t written = fwrite(stream->data, 1, stream->pos, f);
    fclose(f);

    return written == stream->pos;
}