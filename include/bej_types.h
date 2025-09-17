/**
 * @file bej_types.h
 * @brief This file contains the definitions of core types and constants for the BEJ parser.
 * All BEJ_FORMAT_ constants correspond to the data types defined in the
 * Binary-encoded JSON specification.
 */

#ifndef BEJ_TYPES_H
#define BEJ_TYPES_H

#include <stdint.h>

/**
 * @defgroup bej_formats BEJ Data Formats
 * @brief Data format codes used in the BEJ protocol.
 * @{
 */

/// @brief Format for an object (SET).
#define BEJ_FORMAT_SET 0x00

/// @brief Format for an array (ARRAY).
#define BEJ_FORMAT_ARRAY 0x01

/// @brief Format for a Null value.
#define BEJ_FORMAT_NULL 0x02

/// @brief Format for an integer.
#define BEJ_FORMAT_INTEGER 0x03

/// @brief Format for an enumeration.
#define BEJ_FORMAT_ENUM 0x04

/// @brief Format for a string.
#define BEJ_FORMAT_STRING 0x05

/// @brief Format for an boolean.
#define BEJ_FORMAT_BOOLEAN 0x07

/** @} */

/**
 * @defgroup bej_selectors BEJ Dictionary Selectors
 * @brief Selector codes that specify the type of dictionary.
 * @{
 */

/// @brief Selector for the main schema dictionary.
#define BEJ_DICTIONARY_SELECTOR_MAJOR_SCHEMA 0x00

/// @brief Selector for the annotation dictionary.
#define BEJ_DICTIONARY_SELECTOR_ANNOTATION 0x01

/** @} */ // end of bej_selectors

#endif