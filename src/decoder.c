#include "decoder.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "bej_types.h"
#include "dictionary.h"
#include "json_writer.h"
#include "stream_utils.h"

/**
 * @brief Unpacks a BEJ NNInt (Non-Negative Integer) from the input stream.
 *
 * @param stream Pointer to the InputStream.
 * @return The unpacked integer value.
 */
static uint64_t BejUnpackNNInt(InputStream *stream) {
  uint8_t num_bytes = (uint8_t)StreamReadInt(stream, 1);
  return StreamReadInt(stream, num_bytes);
}

/**
 * @brief Finds a dictionary entry by its sequence number.
 *
 * @param entries Pointer to the array of dictionary entries.
 * @param count The number of entries in the array.
 * @param seq The sequence number to search for.
 * @return Pointer to the found DictionaryEntry, or NULL if not found.
 */
static DictionaryEntry *find_entry_by_seq(DictionaryEntry *entries,
                                          size_t count, uint16_t seq) {
  for (size_t i = 0; i < count; ++i) {
    if (entries[i].sequence_number == seq) {
      return &entries[i];
    }
  }
  return NULL;
}

/**
 * @brief Writes the JSON key for a given dictionary entry to the output stream.
 *
 * @param output_stream Pointer to the OutputStream.
 * @param entry Pointer to the DictionaryEntry.
 * @param is_array_item Flag indicating if the element is inside an array.
 */
static void BejDecodeName(OutputStream *output_stream,
                            const DictionaryEntry *entry, bool is_array_item,
                            bool add_name) {
  if (add_name && !is_array_item && entry && entry->name &&
      entry->name[0] != '\0') {
    OutputStreamWrite(output_stream, "\"", 1);
    OutputStreamWrite(output_stream, entry->name, strlen(entry->name));
    OutputStreamWrite(output_stream, "\": ", 3);
  }
}

/**
 * @brief Forward declaration for recursive calls.
 */
static bool BejDecode_stream(OutputStream *output_stream,
                              InputStream *input_stream,
                              InputStream *schema_dict,
                              DictionaryEntry *current_entries,
                              size_t entry_count, int prop_count,
                              int indent_level, bool add_name);

/**
 * @brief Decodes a single BEJ element (property) from the stream.
 */
static bool BejDecode_element(OutputStream *output_stream,
                               InputStream *input_stream,
                               InputStream *schema_dict,
                               DictionaryEntry *current_entries,
                               size_t entry_count, int indent_level,
                               bool is_array_item, bool add_name) {
  if (input_stream->pos >= input_stream->size) return true;

  uint64_t raw_seq = BejUnpackNNInt(input_stream);
  uint32_t seq_num = raw_seq >> 1;

  uint8_t format_byte = (uint8_t)StreamReadInt(input_stream, 1);
  uint8_t format = format_byte >> 4;

  uint64_t length = BejUnpackNNInt(input_stream);

  DictionaryEntry *entry = find_entry_by_seq(current_entries, entry_count,
                                             is_array_item ? 0 : seq_num);

  if (!entry) {
    fprintf(stderr, "Error: Dictionary entry not found for seq %u\n",
            (unsigned int)seq_num);
    return false;
  }

  if (add_name && !is_array_item) {
    JsonWriteIndent(output_stream, indent_level);
    BejDecodeName(output_stream, entry, is_array_item, add_name);
  }

  switch (format) {
    case BEJ_FORMAT_SET: {
      uint64_t count = BejUnpackNNInt(input_stream);
      OutputStreamWrite(output_stream, "{", 1);

      DictionaryEntry child_entries[MAX_DICT_ENTRIES];
      size_t child_entry_count;
      if (!LoadDictionarySubsetIntoBuffer(
              schema_dict->data, schema_dict->size, entry->offset,
              entry->child_count, child_entries, &child_entry_count)) {
        return false;
      }

      for (uint64_t i = 0; i < count; ++i) {
        if (i > 0) OutputStreamWrite(output_stream, ",", 1);

        if (!BejDecode_element(output_stream, input_stream, schema_dict,
                                child_entries, child_entry_count,
                                indent_level + 1, false, true)) {
          return false;
        }
      }

      if (count > 0) {
        JsonWriteIndent(output_stream, indent_level);
      }
      OutputStreamWrite(output_stream, "}", 1);
      return true;
    }
    case BEJ_FORMAT_ARRAY: {
      uint64_t array_member_count = BejUnpackNNInt(input_stream);
      OutputStreamWrite(output_stream, "[", 1);

      DictionaryEntry child_entries[MAX_DICT_ENTRIES];
      size_t child_entry_count;
      if (!LoadDictionarySubsetIntoBuffer(
              schema_dict->data, schema_dict->size, entry->offset,
              entry->child_count, child_entries, &child_entry_count)) {
        return false;
      }

      for (uint64_t i = 0; i < array_member_count; ++i) {
        if (i > 0) OutputStreamWrite(output_stream, ",", 1);

        JsonWriteIndent(output_stream, indent_level + 1);

        if (!BejDecode_element(output_stream, input_stream, schema_dict,
                                child_entries, child_entry_count,
                                indent_level + 1, true, false)) {
          return false;
        }
      }

      if (array_member_count > 0)
        JsonWriteIndent(output_stream, indent_level);

      OutputStreamWrite(output_stream, "]", 1);
      return true;
    }
    case BEJ_FORMAT_STRING: {
      const uint8_t *val = StreamReadBytes(input_stream, length);
      OutputStreamWrite(output_stream, "\"", 1);
      OutputStreamWrite(output_stream, (const char *)val,
                          length > 0 ? length - 1 : 0);
      OutputStreamWrite(output_stream, "\"", 1);
      return true;
    }
    case BEJ_FORMAT_INTEGER: {
      int64_t value = stream_read_sint(input_stream, length);
      char buffer[32];
      int n = snprintf(buffer, sizeof(buffer), "%" PRId64, value);
      OutputStreamWrite(output_stream, buffer, n);
      return true;
    }
    case BEJ_FORMAT_BOOLEAN: {
      uint8_t value = (uint8_t)StreamReadInt(input_stream, length);
      if (value == 0x01)
        OutputStreamWrite(output_stream, "true", 4);
      else
        OutputStreamWrite(output_stream, "false", 5);
      return true;
    }
    case BEJ_FORMAT_NULL: {
      OutputStreamWrite(output_stream, "null", 4);
      return true;
    }
    case BEJ_FORMAT_ENUM: {
      uint64_t enum_seq = BejUnpackNNInt(input_stream);
      DictionaryEntry enum_entries[MAX_DICT_ENTRIES];
      size_t enum_entry_count;
      if (!LoadDictionarySubsetIntoBuffer(
              schema_dict->data, schema_dict->size, entry->offset,
              entry->child_count, enum_entries, &enum_entry_count)) {
        return false;
      }

      DictionaryEntry *enum_val_entry =
          find_entry_by_seq(enum_entries, enum_entry_count, enum_seq);

      if (enum_val_entry && enum_val_entry->name) {
        OutputStreamWrite(output_stream, "\"", 1);
        OutputStreamWrite(output_stream, enum_val_entry->name,
                            strlen(enum_val_entry->name));
        OutputStreamWrite(output_stream, "\"", 1);
      } else {
        OutputStreamWrite(output_stream, "null", 4);
      }
      return true;
    }
    default:
      StreamReadBytes(input_stream, length);
      fprintf(stderr, "Warning: Unsupported BEJ format %02X skipped.\n",
              format);
      OutputStreamWrite(output_stream, "null", 4);
      return false;
  }
}

/**
 * @brief Decodes a stream of BEJ elements (e.g., properties of a SET).
 */
static bool BejDecode_stream(OutputStream *output_stream,
                              InputStream *input_stream,
                              InputStream *schema_dict,
                              DictionaryEntry *current_entries,
                              size_t entry_count, int prop_count,
                              int indent_level, bool add_name) {
  for (int i = 0; i < prop_count; ++i) {
    if (i > 0) {
      OutputStreamWrite(output_stream, ", ", 2);
    }
    if (!BejDecode_element(output_stream, input_stream, schema_dict,
                            current_entries, entry_count, indent_level, false,
                            add_name)) {
      return false;
    }
  }
  return true;
}

/**
 * @brief Main function to decode a BEJ stream.
 */
bool BejDecode(OutputStream *output_stream, InputStream *input_stream,
                InputStream *schema_dictionary) {
  if (input_stream->size < 7) return false;

  uint32_t version = (uint32_t)StreamReadInt(input_stream, 4);
  if (version == 0x00F0F0F1 || version == 0x00F0F1F1) {
    fprintf(stderr,
            "Error: BEJ version indicates an error payload (0x%08X). Decoding "
            "aborted.\n",
            version);
    return false;
  }

  uint16_t flags = (uint16_t)StreamReadInt(input_stream, 2);
  if (flags != 0) {
    fprintf(stderr, "Warning: Non-zero BEJ flags (0x%04X) encountered.\n",
            flags);
  }

  uint8_t schema_class = (uint8_t)StreamReadInt(input_stream, 1);
  if (schema_class != BEJ_DICTIONARY_SELECTOR_MAJOR_SCHEMA &&
      schema_class != BEJ_DICTIONARY_SELECTOR_ANNOTATION) {
    fprintf(stderr,
            "Error: Unsupported BEJ schema class (0x%02X). Only 0x00 and 0x01 "
            "are supported.\n",
            schema_class);
    return false;
  }

  DictionaryEntry root_entries[MAX_DICT_ENTRIES];
  size_t root_entry_count;
  if (!LoadDictionarySubsetIntoBuffer(schema_dictionary->data,
                                          schema_dictionary->size, 0, -1,
                                          root_entries, &root_entry_count)) {
    return false;
  }

  return BejDecode_stream(output_stream, input_stream, schema_dictionary,
                           root_entries, root_entry_count, 1, 0, false);
}