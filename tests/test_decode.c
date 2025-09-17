
#include <stdlib.h>

#include "unity.h"
#include "decoder.h"
#include "stream_utils.h"
#include "dictionary.h"

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    TEST_ASSERT_NOT_NULL_MESSAGE(f, "Failed to open file");

    if (fseek(f, 0, SEEK_END) != 0)
    {
        fclose(f);
        TEST_FAIL_MESSAGE("fseek failed");
        return NULL;
    }

    long sz = ftell(f);
    if (sz < 0)
    {
        fclose(f);
        TEST_FAIL_MESSAGE("ftell failed");
        return NULL;
    }
    rewind(f);

    if ((unsigned long)sz > SIZE_MAX)
    {
        fclose(f);
        TEST_FAIL_MESSAGE("file too large to fit into memory");
        return NULL;
    }

    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    TEST_ASSERT_NOT_NULL_MESSAGE(buf, "malloc failed");

    size_t nread = fread(buf, 1, (size_t)sz, f);
    fclose(f);

    TEST_ASSERT_EQUAL_size_t((size_t)sz, nread);

    *out_size = (size_t)sz;
    return buf;
}

void setUp(void) {}
void tearDown(void) {}

void test_decoder_real_payload(void)
{

    const char expected_memory_dict[] = "{\n"
                                        "    \"CapacityMiB\": 65536,\n"
                                        "    \"DataWidthBits\": 64,\n"
                                        "    \"AllowedSpeedsMHz\": [\n"
                                        "        2400,\n"
                                        "        3200\n"
                                        "    ],\n"
                                        "    \"ErrorCorrection\": \"NoECC\",\n"
                                        "    \"MemoryLocation\": {\n"
                                        "        \"Channel\": 0,\n"
                                        "        \"Slot\": 0\n"
                                        "    },\n"
                                        "    \"IsRankSpareEnabled\": true,\n"
                                        "    \"PartNumber\": null,\n"
                                        "    \"Manufacturer\": \"Some\"\n"
                                        "}";

    const char expected_message_dict[] = "{\n"
                                         "    \"MessageId\": \"Base.1.8.PropertyValueError\",\n"
                                         "    \"Message\": \"The value for the property MemorySize is not valid.\",\n"
                                         "    \"MessageArgs\": [\n"
                                         "        \"MemorySize\",\n"
                                         "        \"4096\"\n"
                                         "    ],\n"
                                         "    \"Severity\": \"Warning\",\n"
                                         "    \"Resolution\": \"Correct the property value and retry the operation.\",\n"
                                         "    \"RelatedProperties\": [\n"
                                         "        \"/Systems/1/Memory/1/CapacityMiB\",\n"
                                         "        \"/Systems/1/Memory/1/SpeedMHz\"\n"
                                         "    ]\n"
                                         "}";

    size_t dict_memory_sz, bej_memory_sz;
    size_t dict_message_sz, bej_message_sz;

    uint8_t *dict_memory_buf = read_file("dummy_dictionaries/Memory_v1.bin", &dict_memory_sz);
    uint8_t *dict_message_buf = read_file("dummy_dictionaries/Message_v1.bin", &dict_message_sz);

    uint8_t *bej_memory_buf = read_file("dummy_data/memory_bej.bin", &bej_memory_sz);
    uint8_t *bej_message_buf = read_file("dummy_data/message_bej.bin", &bej_message_sz);

    TEST_ASSERT_NOT_NULL(dict_memory_buf);
    TEST_ASSERT_NOT_NULL(bej_memory_buf);
    TEST_ASSERT_NOT_NULL(dict_message_buf);
    TEST_ASSERT_NOT_NULL(bej_message_buf);

    InputStream dict_memory = {dict_memory_buf, dict_memory_sz, 0};
    InputStream bej_memory = {bej_memory_buf, bej_memory_sz, 0};

    InputStream dict_message = {dict_message_buf, dict_message_sz, 0};
    InputStream bej_message = {bej_message_buf, bej_message_sz, 0};

    OutputStream json_memory_output, json_message_output;
    output_stream_init(&json_memory_output);
    output_stream_init(&json_message_output);

    bool success_memory = bej_decode(&json_memory_output, &bej_memory, &dict_memory);
    bool success_message = bej_decode(&json_message_output, &bej_message, &dict_message);

    TEST_ASSERT_TRUE(success_memory);
    TEST_ASSERT_TRUE(success_message);

    TEST_ASSERT_EQUAL_STRING(expected_memory_dict, (char *)json_memory_output.data);
    TEST_ASSERT_EQUAL_STRING(expected_message_dict, (char *)json_message_output.data);

    free(dict_memory_buf);
    free(bej_memory_buf);
    free(dict_message_buf);
    free(bej_message_buf);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_decoder_real_payload);
    return UNITY_END();
}