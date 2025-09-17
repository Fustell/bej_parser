#ifndef JSON_WRITER_H
#define JSON_WRITER_H

#include "stream_utils.h"
#include <stdbool.h>

/**
 * @brief Writes indentation to the output stream for pretty-printing JSON.
 *
 * @param out Pointer to the output stream.
 * @param indent_level The current indentation level.
 */
void json_write_indent(OutputStream *stream, int indent_level);

/**
 * @brief Flushes the contents of the output stream to a file.
 *
 * @param stream Pointer to the OutputStream.
 * @param filename The name of the file to write to.
 * @return true if flushing was successful, false otherwise.
 */
bool json_writer_flush_to_file(const OutputStream *stream, const char *filename);

#endif