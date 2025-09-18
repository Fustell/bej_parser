#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_DICT_ENTRIES 512

/**
 * @struct DictionaryEntry
 * @brief Represents a single entry in a BEJ dictionary.
 */
typedef struct {
  uint8_t format;
  uint8_t flags;
  uint16_t sequence_number;
  uint16_t offset;
  uint16_t child_count;
  const char *name;
} DictionaryEntry;

/**
 * @struct DictionaryStream
 * @brief A utility structure for reading data from a dictionary byte array.
 */
typedef struct {
  const uint8_t *byte_array;
  size_t size;
  size_t current_index;
} DictionaryStream;

/**
 * @struct DecodedSeq
 * @brief Represents a decoded sequence number and selector from the BEJ stream.
 */
typedef struct {
  uint32_t seq_num;
  uint8_t selector;
} DecodedSeq;

/**
 * @brief Reads an integer from a dictionary stream.
 *
 * @param stream Pointer to the DictionaryStream.
 * @param size Number of bytes to read.
 * @return The read integer value.
 */
uint64_t DictStreamReadInt(DictionaryStream *stream, size_t size);

/**
 * @brief Loads a subset of a dictionary into a provided buffer.
 *
 * This function parses dictionary data from a byte array and populates an array
 * of DictionaryEntry structures. It avoids heap usage by using a pre-allocated
 * buffer.
 *
 * @param data Pointer to the raw dictionary byte array.
 * @param size The size of the dictionary byte array.
 * @param offset The starting byte offset for the dictionary subset.
 * @param child_count The number of dictionary entries to load. A value of -1
 * indicates the root entry.
 * @param out_entries A pre-allocated array where the loaded entries will be
 * stored.
 * @param out_count Pointer to a variable to store the number of loaded entries.
 * @return true if the subset was loaded successfully, false otherwise.
 */
bool LoadDictionarySubsetIntoBuffer(const uint8_t *data, size_t size,
                                        uint16_t offset, int16_t child_count,
                                        DictionaryEntry *out_entries,
                                        size_t *out_count);

#endif