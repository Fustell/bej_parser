#ifndef DECODER_H
#define DECODER_H

#include <stdbool.h>

#include "dictionary.h"
#include "json_writer.h"
#include "stream_utils.h"

/**
 * @brief Decodes a BEJ (Binary Encoded JSON) stream into a JSON output stream.
 * @param out Pointer to the output stream where the decoded JSON will be
 * written.
 * @param bej_input Pointer to the input stream containing the BEJ data.
 * @param schema_dict Pointer to the input stream for the schema dictionary.
 * @return true if decoding was successful, false otherwise.
 */
bool BejDecode(OutputStream *out, InputStream *bej_input,
                InputStream *schema_dict);

#endif