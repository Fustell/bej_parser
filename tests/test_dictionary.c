#include "unity.h"
#include "dictionary.h"

void setUp(void) {}
void tearDown(void) {}

void test_load_dictionary_subset_into_buffer_empty(void)
{
    uint8_t buf[] = {0};
    DictionaryEntry entries[10];
    size_t count;

    bool ok = load_dictionary_subset_into_buffer(buf, sizeof(buf), 0, 0, entries, &count);

    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_UINT(0, count);
}

void test_load_dictionary_subset_into_buffer_single_entry(void)
{

    uint8_t buf[] = {
        0x10,
        0x01, 0x00,
        0x08, 0x00,
        0x00, 0x00,
        0x04,
        0x0A, 0x00,
        'N', 'a', 'm', 'e', '\0'};

    DictionaryEntry entries[1];
    size_t count;

    bool ok = load_dictionary_subset_into_buffer(buf, sizeof(buf), 0, 1, entries, &count);

    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_UINT(1, count);
    TEST_ASSERT_EQUAL_UINT16(1, entries[0].sequence_number);
    TEST_ASSERT_EQUAL_STRING("Name", entries[0].name);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_load_dictionary_subset_into_buffer_empty);
    RUN_TEST(test_load_dictionary_subset_into_buffer_single_entry);
    return UNITY_END();
}