#include "dictionary.h"
#include <string.h>
#include <stdio.h>

/**
 * @brief Reads an integer from a dictionary stream.
 *
 * @param stream Pointer to the DictionaryStream.
 * @param size Number of bytes to read.
 * @return The read integer value.
 */
uint64_t dict_stream_read_int(DictionaryStream *stream, size_t size)
{
    uint64_t value = 0;
    if (stream->current_index + size > stream->size)
        return 0;
    for (size_t i = 0; i < size; ++i)
    {
        value |= ((uint64_t)stream->byte_array[stream->current_index + i]) << (i * 8);
    }
    stream->current_index += size;
    return value;
}

/**
 * @brief Loads a subset of a dictionary into a provided buffer.
 *
 * This function parses dictionary data from a byte array and populates an array
 * of DictionaryEntry structures. It avoids heap usage by using a pre-allocated buffer.
 *
 * @param data Pointer to the raw dictionary byte array.
 * @param size The size of the dictionary byte array.
 * @param offset The starting byte offset for the dictionary subset.
 * @param child_count The number of dictionary entries to load. A value of -1 indicates the root entry.
 * @param out_entries A pre-allocated array where the loaded entries will be stored.
 * @param out_count Pointer to a variable to store the number of loaded entries.
 * @return true if the subset was loaded successfully, false otherwise.
 */
bool load_dictionary_subset_into_buffer(
    const uint8_t *data, size_t size,
    uint16_t offset, int16_t child_count,
    DictionaryEntry *out_entries, size_t *out_count)
{
    DictionaryStream ds = {
        .byte_array = data,
        .size = size,
        .current_index = offset};
    *out_count = 0;

    int16_t actual_child_count = child_count;
    if (child_count == -1)
    {
        if (ds.current_index + 10 > ds.size)
            return false;
        dict_stream_read_int(&ds, 1);
        dict_stream_read_int(&ds, 1);
        dict_stream_read_int(&ds, 4);
        (void)dict_stream_read_int(&ds, 2);
        dict_stream_read_int(&ds, 4);
        actual_child_count = 1;
    }

    if (actual_child_count <= 0)
        return true;
    if ((size_t)actual_child_count > MAX_DICT_ENTRIES)
    {
        fprintf(stderr, "Error: Dictionary subset exceeds max entries (%d vs %u).\n", actual_child_count, MAX_DICT_ENTRIES);
        return false;
    }

    *out_count = actual_child_count;

    for (int i = 0; i < actual_child_count; i++)
    {
        if (ds.current_index + 8 > ds.size)
        {
            fprintf(stderr, "Error: Dictionary stream read out of bounds.\n");
            return false;
        }

        uint8_t format_flags = (uint8_t)dict_stream_read_int(&ds, 1);
        out_entries[i].format = format_flags >> 4;
        out_entries[i].flags = format_flags & 0x0F;
        out_entries[i].sequence_number = (uint16_t)dict_stream_read_int(&ds, 2);
        out_entries[i].offset = (uint16_t)dict_stream_read_int(&ds, 2);
        out_entries[i].child_count = (uint16_t)dict_stream_read_int(&ds, 2);

        uint8_t name_len = (uint8_t)dict_stream_read_int(&ds, 1);
        uint16_t name_offset = (uint16_t)dict_stream_read_int(&ds, 2);

        if (name_len > 0 && name_offset + name_len <= size)
        {
            out_entries[i].name = (const char *)(data + name_offset);
        }
        else
        {
            out_entries[i].name = "";
        }
    }
    return true;
}