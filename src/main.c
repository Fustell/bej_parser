#include "decoder.h"
#include "stream_utils.h"
#include "json_writer.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Reads a file into a dynamically allocated buffer.
 *
 * @param filename The name of the file to read.
 * @param size Pointer to a variable to store the file size.
 * @return A pointer to the allocated buffer, or NULL on failure.
 */
static uint8_t *read_file(const char *filename, size_t *size)
{
    FILE *f = fopen(filename, "rb");
    if (!f)
    {
        fprintf(stderr, "Error: cannot open %s\n", filename);
        return NULL;
    }

    if (fseek(f, 0, SEEK_END) != 0)
    {
        fclose(f);
        return NULL;
    }
    long sz = ftell(f);
    if (sz < 0)
    {
        fclose(f);
        return NULL;
    }
    rewind(f);

    *size = (size_t)sz;
    uint8_t *buf = malloc(*size);
    if (!buf)
    {
        fprintf(stderr, "Error: out of memory (%zu bytes)\n", *size);
        fclose(f);
        return NULL;
    }

    size_t read_bytes = fread(buf, 1, *size, f);
    fclose(f);

    if (read_bytes != *size)
    {
        fprintf(stderr, "Error: short read in %s (expected %zu, got %zu)\n",
                filename, *size, read_bytes);
        free(buf);
        return NULL;
    }

    return buf;
}

/**
 * @brief Main function to run the BEJ parser.
 */
int main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr,
                "Usage: %s <schema_dict.bin> <payload.bin> [output.json]\n",
                argv[0]);
        return 1;
    }

    const char *schema_path = argv[1];
    const char *payload_path = argv[2];
    const char *output_path = (argc > 3) ? argv[3] : "decoded.json";

    size_t schema_size = 0, payload_size = 0;
    uint8_t *schema = read_file(schema_path, &schema_size);
    uint8_t *payload = read_file(payload_path, &payload_size);

    if (!schema || !payload)
    {
        free(schema);
        free(payload);
        return 2;
    }

    InputStream schema_is = {schema, schema_size, 0};
    InputStream payload_is = {payload, payload_size, 0};

    OutputStream out;
    output_stream_init(&out);

    if (bej_decode(&out, &payload_is, &schema_is))
    {
        printf("Decoded JSON written to %s\n", output_path);
        json_writer_flush_to_file(&out, output_path);
    }
    else
    {
        fprintf(stderr, "Decode failed\n");
    }

    free(schema);
    free(payload);

    return 0;
}